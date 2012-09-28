// Kanvas: a canvas API for Kindle
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <string>
#include <algorithm>

#include <v8.h>
#include <node.h>
#include <node_buffer.h>

#include "kanvas.h"
#include "font.h"

using namespace std;
using namespace v8;

#include "common.h"
void Kanvas::Init(Handle<Object> target) {
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(SYM("Kanvas"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
#define BIND(name,func) tpl->PrototypeTemplate()->Set(SYM(#name),FUNC(func));
    BIND(fillRect  ,FillRect);
    BIND(strokeRect,StrokeRect);
    BIND(drawLine  ,DrawLine);

    BIND(drawString,DrawString);
    BIND(sizeString,SizeString);
    
    BIND(getPixel  ,GetPixel);
    BIND(setPixel  ,SetPixel);
#undef BIND

#define RWATTR(name) tpl->InstanceTemplate()->SetAccessor(SYM(#name), GetProp, SetProp)
#define ROATTR(name) tpl->InstanceTemplate()->SetAccessor(SYM(#name), GetProp, RoSetProp )

    ROATTR(width);
    ROATTR(height);
    ROATTR(buffer);
    ROATTR(pitch);
    RWATTR(color);
    RWATTR(font);

#undef ROATTR
#undef RWATTR

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(SYM("Kanvas"), constructor);
}


Handle<Value> Kanvas::New(const Arguments& args) {
    HandleScope scope;

    int width = args[0]->Int32Value();
    int rows  = args[1]->Int32Value();

    Kanvas* obj = Kanvas::New(width,rows);
    obj->Wrap(args.This());
    return args.This();
}

#define align(x,a) (((x)+(a)-1) & (~((a)-1)))
Kanvas* Kanvas::New(int width,int rows){
    int pitch = align(width,4);
    size_t size = pitch*rows;
    node::Buffer* buf = node::Buffer::New(size);
    Kanvas* ret = new Kanvas();
    ret->width = width;
    ret->rows = rows;
    ret->pitch = pitch;
    ret->buffer = Persistent<Object>::New(buf->handle_);
    ret->reloadBuffer();
  
    return ret;
}

bool Kanvas::reloadBuffer(){
    assert(!buffer.IsEmpty());
    assert(node::Buffer::HasInstance(buffer));
    buf = node::Buffer::Data(buffer);
    buflen = node::Buffer::Length(buffer);
    if(!buf){
        string wtf = ObjectToString(buffer->ToString());
        printf("WTF? %s\n",wtf.c_str());
        return false;
    }
    return true;
}

Handle<Value> Kanvas::GetProp(Local<String> name,const AccessorInfo &info) {
    string key = ObjectToString(name);
    Kanvas* obj = ObjectWrap::Unwrap<Kanvas>(info.Holder());

#define ATTR(name,val) if(key==#name) return val ; else
#define SATTR(name,val) ATTR(name, String::New(obj->val))
#define IATTR(name,val) ATTR(name, Integer::New(obj->val))

	ATTR(buffer,obj->buffer)
	ATTR(font,obj->font)
	IATTR(width,width)
	IATTR(height,rows)
	IATTR(pitch,pitch)
	IATTR(color,color)

#undef IATTR
#undef SATTR
#undef ATTR

    return Handle<Value>();
}

void Kanvas::SetProp(Local<String> name, Local<Value> value, const AccessorInfo& info){
    string key = ObjectToString(name);
    Kanvas* obj = ObjectWrap::Unwrap<Kanvas>(info.Holder());

	//printf("update prop %s = %s\n",key.c_str(),ObjectToString(value->ToDetailString()).c_str());
    if(key=="color"){
        obj->color = (char)value->Uint32Value();
    }else if(key=="font"){
		Font* f = ObjectWrap::Unwrap<Font>(value->ToObject());
		assert(f != NULL);
		obj->font = Persistent<Object>::New( f->handle_ );
	}
}

Handle<Value> Kanvas::GetPixel(const Arguments& args) {
    HandleScope scope;
    Kanvas* obj = ObjectWrap::Unwrap<Kanvas>(args.This());
    
    int x=0,y=0;
    if(args.Length()>=2){
        x = args[0]->Uint32Value();
        y = args[1]->Uint32Value();
    }else{
        return THROW("bad arguments count");
    }

    obj->reloadBuffer();
    char ret;
    if(obj->getPixel(x,y,&ret)){
        return scope.Close(Integer::New(ret));
    }

    return scope.Close(Handle<Value>());
}

Handle<Value> Kanvas::SetPixel(const Arguments& args) {
    HandleScope scope;
    Kanvas* obj = ObjectWrap::Unwrap<Kanvas>(args.This());
    
    int x=0,y=0;
    char c;
    if(args.Length()>=3){
        x = args[0]->Uint32Value();
        y = args[1]->Uint32Value();
        c = (char)args[2]->Uint32Value();
    }else{
        return THROW("bad arguments count");
    }

    obj->reloadBuffer();
    bool ret = obj->setPixel(x,y,c);

    return scope.Close(Boolean::New(ret));
}

Handle<Value> Kanvas::FillRect(const Arguments& args) {
    HandleScope scope;

    Kanvas* obj = ObjectWrap::Unwrap<Kanvas>(args.This());

    int x=0,y=0,w=obj->width,h=obj->rows;
    if(args.Length()>=4){
        x = args[0]->Int32Value();
        y = args[1]->Int32Value();
        w = args[2]->Int32Value();
        h = args[3]->Int32Value();
    }else if(args.Length()){
        return THROW("bad arguments count");
    }

    obj->reloadBuffer();
    obj->fillRect(x,y,w,h);

    return scope.Close(Handle<Value>());
}

bool Kanvas::fillRect(int x,int y,int w,int h){

    if(y<0)y=0;
    if(x>=width || y>=rows)return false;
    //printf("fill x=%d,y=%d,w=%d,h=%d,c=%d,width=%d,rows=%d\n",x,y,w,h,color,width,rows);
    if((x==0)&&(y==0)&&(w>=width)&&(h>=rows)){
        //printf("buf=%p,len=%d\n",buf,buflen);
        memset(buf,color,buflen);
        return true;
    }
    int endx = min(x+w,width);
    int endy = min(y+h,rows);
    //printf("endx=%d,endy=%d,pitch=%d,buflen=%d\n",endx,endy,pitch,buflen);
    assert((endy-1)*pitch+endx-1 < buflen);
    for(;y<endy;y++){
        memset(buf+(y*pitch+x),color,endx-x);
    }
    return true;
}

Handle<Value> Kanvas::StrokeRect(const Arguments& args){
    HandleScope scope;

    Kanvas* obj = ObjectWrap::Unwrap<Kanvas>(args.This());
    int argc = args.Length();
    int x=0,y=0,w=obj->width,h=obj->rows,W=1;
    if(argc>=4){
        x = args[0]->Int32Value();
        y = args[1]->Int32Value();
        w = args[2]->Int32Value();
        h = args[3]->Int32Value();
        if(argc>=5){
            W=args[4]->Uint32Value();
        }
    }else if(args.Length()){
        return THROW("bad arguments count");
    }

    obj->reloadBuffer();
    obj->strokeRect(x,y,w,h,W);

    return scope.Close(Handle<Value>());
}

void Kanvas::strokeRect(int x,int y,int w,int h,int W){
    if(W>=w/2 || W>=h/2)fillRect(x,y,w,h);
    fillRect(x,y,w,W);
    fillRect(x,y+h-W,w,W);
    fillRect(x,y+W,W,h-W*2);
    fillRect(x+w-W,y+W,W,h-W*2);
}

Handle<Value> Kanvas::DrawLine(const Arguments& args) {
    HandleScope scope;

    Kanvas* obj = ObjectWrap::Unwrap<Kanvas>(args.This());

    int x1=0,y1=0,x2=0,y2=0;
    if(args.Length()>=4){
        x1 = args[0]->Int32Value();
        y1 = args[1]->Int32Value();
        x2 = args[2]->Int32Value();
        y2 = args[3]->Int32Value();
    }else{
        return THROW("bad arguments count");
    }

    obj->reloadBuffer();
    obj->drawLine(x1,y1,x2,y2);

    return scope.Close(Handle<Value>());
}

//Bresenham algorithm
#define abs(x) ((x)>=0?(x):-(x))
void Kanvas::drawLine(int x1,int y1,int x2,int y2){

#define _setPixel(_x,_y,_c) \
do{\
    int offset = (_y)*pitch+(_x); \
    if(offset<buflen) \
        buf[offset]=(_c); \
}while(0)

    int dx = x2 - x1;
    int dy = y2 - y1;
    int ux = ((dx > 0) << 1) - 1;
    int uy = ((dy > 0) << 1) - 1;
    int x = x1, y = y1, eps;

    eps = 0;dx = abs(dx); dy = abs(dy);
    printf("drawLine dx=%d,dy=%d,ux=%x,uy=%d \n",dx,dy,ux,uy); 
    if (dx > dy) {
        for (x = x1; x != x2+1; x += ux){
            _setPixel(x, y, color);
            eps += dy;
            if ((eps << 1) >= dx){
                y += uy; eps -= dx;
            }
        }
    }else{
        for (y = y1; y != y2+1; y += uy){
            _setPixel(x, y, color);
            eps += dx;
            if ((eps << 1) >= dy){
                x += ux; eps -= dy;
            }
        }
    }
#undef _setPixel
}

//Wu's line algorithm
/*
#define ipart(x) (floor(x))
#define fpart(x) (x-floor(x))
#define rfpart(x) (1-fpart(x))
#define plot(x,y,c) setPixel(x,y,floor(c*color))

void Kanvas::drawLine(int x1,int y1,int x2,int y2,float width){
    float dx = x2 - x1;
    float dy = y2 - y1;
    if( fabs(dx) < fabs(dy) ){                 
      swap(x1, y1);
      swap(x2, y2);
      swap(dx, dy);
    }

    if( x2 < x1){
      swap(x1, x2);
      swap(y1, y2);
    }
    float gradient = dy / dx;
    
    // handle first endpoint
    float xend = x1;
    float yend = y1 + gradient * (xend - x1);
    float xgap = rfpart(x1 + 0.5);
    int xpxl1 = x1;  // this will be used in the main loop
    int ypxl1 = ipart(yend);
    plot(xpxl1, ypxl1, rfpart(yend) * xgap);
    plot(xpxl1, ypxl1 + 1, fpart(yend) * xgap);
    float intery = yend + gradient; // first y-intersection for the main loop
    
    // handle second endpoint
    xend = x2;
    yend = y2 + gradient * (xend - x2);
    xgap = fpart(x2 + 0.5);
    int xpxl2 = x2;  // this will be used in the main loop
    int ypxl2 = ipart (yend);
    plot (xpxl2, ypxl2, rfpart (yend) * xgap);
    plot (xpxl2, ypxl2 + 1, fpart (yend) * xgap);
    
    // main loop
    for (int x = xpxl1 + 1; x < xpxl2 - 1; x++){
        plot (x, ipart (intery), rfpart (intery));
        plot (x, ipart (intery) + 1, fpart (intery));
        intery = intery + gradient;
    }
}
*/

Handle<Value> Kanvas::DrawString(const Arguments& args) {
    HandleScope scope;

    Kanvas* obj = ObjectWrap::Unwrap<Kanvas>(args.This());

	string str = ObjectToString(args[0]);
	int x=0,y=0;
	x = args[1]->Int32Value();
    y = args[2]->Int32Value();

    obj->reloadBuffer();
	obj->drawString(str,x,y);
    return scope.Close(Handle<Value>());
}


void Kanvas::drawString(string str,int x,int y){
    Font* f = ObjectWrap::Unwrap<Font>(font);
	assert(f != NULL);
	f->drawString(this,str,x,y);	
}

Handle<Value> Kanvas::SizeString(const Arguments& args) {
    HandleScope scope;

    Kanvas* obj = ObjectWrap::Unwrap<Kanvas>(args.This());

	string str = ObjectToString(args[0]);
	int w,h;
	obj->sizeString(str,&w,&h);

    return scope.Close(Integer::New(w));
}

void Kanvas::sizeString(string str,int* width,int* height){
	Font* f = ObjectWrap::Unwrap<Font>(font);
	assert(f != NULL);
	int ret = f->sizeString(str,height);
	if(width)*width=ret;
}

NODE_MODULE(kanvas,Kanvas::Init)

