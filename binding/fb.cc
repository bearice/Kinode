#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/einkfb.h>

#include <string>
#include <algorithm>

#include <v8.h>
#include <node.h>
#include <node_buffer.h>

#include "fb.h"

using namespace std;
using namespace v8;

#define SHMLEN finfo->smem_len
#define XRES vinfo->xres
#define YRES vinfo->yres

FBDev::FBDev(){
    fbfd = open("/dev/fb0", O_RDWR);
	vinfo = new struct fb_var_screeninfo;
	finfo = new struct fb_fix_screeninfo;
	fbioctl(FBIOGET_FSCREENINFO, finfo);
    fbioctl(FBIOGET_VSCREENINFO, vinfo);
    fbp = (char*)mmap(0, SHMLEN, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
}

FBDev::~FBDev(){
	if(vinfo) delete (vinfo);
	if(finfo) delete (finfo);
    if(fbp)munmap(fbp,SHMLEN);
    if(fbfd)close(fbfd);
}


#define SYM(x)  String::NewSymbol(x)
#define FUNC(x) FunctionTemplate::New(x)->GetFunction()

void FBDev::Init(Handle<Object> target) {
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(SYM("FBDev"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
#define BIND(name,func) tpl->PrototypeTemplate()->Set(SYM(#name),FUNC(func));    
    BIND(clear,Clear);
    BIND(flush,Flush);
    BIND(splash,Splash);
    BIND(update,Update);
#undef BIND

    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(String::NewSymbol("FBDev"), constructor);
}

Handle<Value> FBDev::New(const Arguments& args) {
    HandleScope scope;

    FBDev* obj = new FBDev();
    obj->Wrap(args.This());
    
    return args.This();
}

Handle<Value> FBDev::Clear(const Arguments& args) {
    HandleScope scope;

    FBDev* obj = ObjectWrap::Unwrap<FBDev>(args.This());

    int ret = obj->fbioctl(FBIO_EINK_SCREEN_CLEAR);

    return scope.Close(Number::New(ret));
}

Handle<Value> FBDev::Flush(const Arguments& args) {
    HandleScope scope;

    FBDev* obj = ObjectWrap::Unwrap<FBDev>(args.This());

    int ret = obj->fbioctl(FBIO_EINK_UPDATE_DISPLAY, fx_update_full);

    return scope.Close(Number::New(ret));
}

Handle<Value> FBDev::Splash(const Arguments& args) {
    HandleScope scope;

    FBDev* obj = ObjectWrap::Unwrap<FBDev>(args.This());
    int val = args[0]->IsUndefined() ? 0 : args[0]->Int32Value();
    int ret = obj->fbioctl(FBIO_EINK_SPLASH_SCREEN, val);

    return scope.Close(Number::New(ret));
}

Handle<Value> FBDev::Update(const Arguments& args) {
    HandleScope scope;

    FBDev* obj = ObjectWrap::Unwrap<FBDev>(args.This());
    int argc = args.Length();
    Local<Object> buf = args[0]->ToObject();

    int bufx,bufy,bufw,bufh,bufpitch;
    bufx = buf->Get(SYM("x"))->Uint32Value();
    bufy = buf->Get(SYM("y"))->Uint32Value();
    bufw = buf->Get(SYM("width"))->Uint32Value();
    bufh = buf->Get(SYM("height"))->Uint32Value();
    bufpitch = buf->Get(SYM("pitch"))->Uint32Value();

    if(bufx >= bufw || bufy >= bufh)
		return scope.Close(Boolean::New(false));

    Local<Object> pbuf = buf->Get(SYM("buffer"))->ToObject();
    char* bufptr = node::Buffer::Data(pbuf);
    size_t buflen = node::Buffer::Length(pbuf);

    int x=0,y=0,w=bufw,h=bufh;
    if(argc >= 3){
        x = args[1]->Int32Value();
        y = args[2]->Int32Value();
    }
    if(argc >= 5){
        w = args[3]->Uint32Value();
        h = args[4]->Uint32Value();
    }

    w = min(w,bufw);
    h = min(h,bufh);

    if(x<0){
        w += x;
        x = 0;
    }
    if(y<0){
        h += y;
        y = 0;
    }

    if(w <=0 || h <=0 || x >= obj->XRES || y>=obj->YRES )
		return scope.Close(Boolean::New(false));

    int x_max = min((uint32_t)x+w,obj->XRES),
        y_max = min((uint32_t)y+h,obj->YRES);

    printf("x=%d,y=%d,x_max=%d,y_max=%d\n",x,y,x_max,y_max);

    int i,j,p,q;
    for ( j = y, q = bufy; j < y_max; j++, q++ )
    {
        for ( i = x, p = bufx; i < x_max; i++, p++ )
        {
            char pix = bufptr[q * bufpitch + p];
            obj->update_pixel(i,j,pix);
        }
    }

    update_area_t ua = {
	    .x1 = x,
		.y1 = y,
		.x2 = x_max,
		.y2 = y_max,
		.which_fx = fx_update_partial,
		.buffer = NULL,
    };

    int ret = obj->fbioctl(FBIO_EINK_UPDATE_DISPLAY_AREA, &ua);

    printf("fbupdate: %d\n",ret);
    return scope.Close(Boolean::New(true));
}

NODE_MODULE(fb,  FBDev::Init)
