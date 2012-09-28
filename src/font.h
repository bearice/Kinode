#ifndef _FONT_H
#define _FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_STROKER_H

#include "lru_cache.h"

struct FTGlyphDelete {
    void operator()( const FT_ULong &k, const FT_Glyph &v ) {
        FT_Done_Glyph(v);
    }
};
class Kanvas;

class Font : public node::ObjectWrap {
public:
    static void Init(v8::Handle<v8::Object> target);

	static v8::Handle<v8::Value> New(const v8::Arguments& args);
	static v8::Handle<v8::Value> SizeString(const v8::Arguments& args);
    
	static v8::Handle<v8::Value> GetProp(v8::Local<v8::String> property, 
			const v8::AccessorInfo &info);
    static void SetProp(v8::Local<v8::String> property, 
			v8::Local<v8::Value> value, const v8::AccessorInfo& info);

	Font();
	~Font();

    std::string filename;
    FT_Face face;
    FT_Error error;
	uint16_t size; //in pixel

	int sizeString(std::string str,int* height = NULL);

    int drawChar(Kanvas* kanvas,FT_ULong chr,int &x,int &y);
    int drawString(Kanvas* kanvas,std::string ,int &x,int &y);
private:

    //LRUCache< FT_ULong , FT_Glyph , Countfn< FT_Glyph >, FTGlyphDelete > cache;

    FT_Glyph loadGlyph(FT_ULong chr);
};

#endif
