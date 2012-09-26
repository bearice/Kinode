#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <string>
#include <algorithm>

#include <v8.h>
#include <node.h>
#include <node_buffer.h>


#include "font.h"
#include "kanvas.h"

#define SYM(x) String::NewSymbol(x)
#define FUNC(x) FunctionTemplate::New(x)->GetFunction()
#define THROW(x) ThrowException(Exception::Error(String::New(x)))

using namespace std;
using namespace v8;

#include "common.h"
static FT_Library library;

void Font::Init(Handle<Object> target){
    FT_Error error;
    error = FT_Init_FreeType( &library );              /* initialize library */
    assert(error==0);
    int ftmajor,ftminor,ftpatch;
    FT_Library_Version(library,&ftmajor,&ftminor,&ftpatch);
    printf("loaded freetype version %d.%d.%d\n",ftmajor,ftminor,ftpatch);

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(SYM("Font"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype

#define BIND(name,func) tpl->PrototypeTemplate()->Set(SYM(#name),FUNC(func));
    BIND(sizeString,SizeString);
#undef BIND

#define RWATTR(name) tpl->InstanceTemplate()->SetAccessor(SYM(#name), GetProp, SetProp)
#define ROATTR(name) tpl->InstanceTemplate()->SetAccessor(SYM(#name), GetProp, RoSetProp )
    RWATTR(size);
//    RWATTR(bold);
//    RWATTR(italic);
//    RWATTR(matrix);
    ROATTR(style_name);
    ROATTR(family_name);
    ROATTR(num_faces);
    ROATTR(face_index);
    ROATTR(face_flags);
    ROATTR(style_flags);
    ROATTR(num_glyphs);
    ROATTR(file_name);

#undef RWATTR
#undef ROATTR

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(String::NewSymbol("Font"), constructor);
}

#define CHK_FT_ERROR(err,ret) \
do{\
    int __error_id = (err);\
    if(__error_id){\
        ThrowException(Exception::Error(\
            String::Concat(\
                SYM("FreeType2 Error: (" #err ") => "),\
                Integer::New(__error_id)->ToString()\
            )\
        ));\
        ret;\
    }\
}while(0)

Handle<Value> Font::New(const Arguments& args){
    string filename = ObjectToString(args[0]);
    int index =  args[0]->IsUndefined() ? 0 : args[0]->Int32Value();

    FT_Face face;
    CHK_FT_ERROR( FT_New_Face(library, filename.c_str(), index, &face ) ,
        return Handle<Value>()
    );

    Font* obj = new Font();
    obj->face = face;
    obj->filename = filename;
    obj->Wrap(args.This());
    
    args.This()->Set(SYM("size"),Integer::New(12));

    return args.This();    
}

Handle<Value> Font::GetProp(Local<String> name,const AccessorInfo &info) {
    string key = ObjectToString(name);
    Font* obj = ObjectWrap::Unwrap<Font>(info.Holder());

#define ATTR(name,val) if(key==#name) return (val) ; else
#define SATTR(name,val) ATTR(name, String::New(obj->val))
#define IATTR(name,val) ATTR(name, Integer::New(obj->val))


    IATTR(size,size)
    SATTR(style_name,face->style_name);
    SATTR(family_name,face->family_name);
    IATTR(num_faces,face->num_faces);
    IATTR(face_index,face->face_index);
    IATTR(face_flags,face->face_flags);
    IATTR(style_flags,face->style_flags);
    IATTR(num_glyphs,face->num_glyphs);
    SATTR(file_name,filename.c_str());

#undef IATTR
#undef SATTR
#undef ATTR

    return Handle<Value>();
}

void Font::SetProp(Local<String> name, Local<Value> value, const AccessorInfo& info){
    string key = ObjectToString(name);
    Font* obj = ObjectWrap::Unwrap<Font>(info.Holder());

    if(key=="size"){
        obj->size = (uint16_t)value->Uint32Value();
        if(obj->size==0){
            THROW("font size must > 0");
            return;
        }
        CHK_FT_ERROR(FT_Set_Pixel_Sizes(obj->face,obj->size,0),
            return
        );
    }
}

Font::Font() {
    face = NULL;
    size = 0;
}

Font::~Font(){
    //cache.clear();
    if(face)FT_Done_Face(face);
}


v8::Handle<v8::Value> Font::SizeString(const v8::Arguments& args){
    HandleScope scope;
    
    Font* obj = ObjectWrap::Unwrap<Font>(args.This());    
    string str = ObjectToString(args[0]);
    int ret = obj->sizeString(str);

    return scope.Close(Integer::New(ret));
}

int Font::sizeString(string str,int* height){
    size_t w = 0,h = 0, idx = 0;
    FT_ULong chr = 0;
    FT_GlyphSlot slot = NULL;
    while((chr=decodeUTF8(str,idx)) != -1){
        error = FT_Load_Char( face, chr, FT_LOAD_DEFAULT );
        slot = face->glyph;
        if(slot){
            w += slot->advance.x >>6;
            h = max(h,(size_t)slot->advance.y >>6);
        }else{
            fprintf(stderr,"Unable to load glyph of char %d\n", chr);
        }
    }
    if(height) *height = h;
    return w;
}

typedef struct Raster_Callback_Param {
    Kanvas* kanvas;
    int x,y;
    int baseline;    
} Param;

static void Raster_Callback(
        const int y,
        const int count,
        const FT_Span * const spans,
        void * const user) {

    Param *p = (Param*)user;
    int Y = p->y + (p->baseline - y);
    int X = 0;
    //printf("Raster_Callback Y=%d\n",Y);
    for (int i = 0; i < count; ++i) {
        const FT_Span* span = spans+i;
        for(int j = 0; j < span->len; j++){
            X = p->x + span->x + j;
            bool ret = p->kanvas->setPixel(X,Y,(span->coverage * p->kanvas->color) >> 8);
            if(!ret)return;
        }
        
        //X = p->x + span->x;
        //p->kanvas->fillRect(X,Y,span->len,1);
    }
}

int Font::drawChar(Kanvas* kanvas,FT_ULong chr,int &x,int &y){
    FT_GlyphSlot slot = NULL;
    error = FT_Load_Char( face, chr, FT_LOAD_DEFAULT );
    slot = face->glyph;

    
    //printf("drawChar: chr=%04x, x=%d, y=%d w=%d, h=%d bx=%d, by=%d \n",chr,x,y,
    //        slot->metrics.width >>6,
    //        slot->metrics.height >>6,
    //        slot->metrics.horiBearingX >>6,
    //        slot->metrics.horiBearingY >>6
    //);

    FT_Size_Metrics*  metrics = &face->size->metrics; /* shortcut */
    double            pixels_x, pixels_y;
    double            em_size, x_scale, y_scale;


    /* compute floating point scale factors */
    em_size = 1.0 * face->units_per_EM;
    x_scale = metrics->x_ppem / em_size;
    y_scale = metrics->y_ppem / em_size;

    int bl = face->bbox.yMax * x_scale;

    Param p = { .kanvas=kanvas, .x=x, .y=y, .baseline=bl};
    
    FT_Raster_Params params = {0};
    params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
    params.gray_spans = Raster_Callback;
    params.user = &p;

    FT_Outline_Render(library, &slot->outline, &params);
    
    return x+= slot->advance.x >>6;
}

int Font::drawString(Kanvas* kanvas,string str,int &x,int &y){
    size_t idx=0;
    FT_ULong chr;
    while((chr=decodeUTF8(str,idx)) != -1){
        drawChar(kanvas,chr,x,y);
    }
    return idx;
}

FT_Glyph Font::loadGlyph(FT_ULong chr){
    //if(cache.exists(chr))return cache.fetch(chr);
    FT_Glyph glyph = NULL;
    error = FT_Load_Char( face, chr, FT_LOAD_DEFAULT );
    if ( error ) return NULL;
    error = FT_Get_Glyph( face->glyph, &glyph );
    if ( error ) return NULL;
    //cache.insert(chr,glyph);
    return glyph;
}

NODE_MODULE(font,Font::Init)

