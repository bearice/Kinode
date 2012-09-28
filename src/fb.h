#ifndef _FB_H
#define _FB_H

#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/einkfb.h>

class FBDev : public node::ObjectWrap{
public:
    static void Init(v8::Handle<v8::Object> target);

private:
    FBDev();
    ~FBDev();
    
    static v8::Handle<v8::Value> New(const v8::Arguments& args);
    static v8::Handle<v8::Value> Clear(const v8::Arguments& args);
    static v8::Handle<v8::Value> Flush(const v8::Arguments& args);
    static v8::Handle<v8::Value> Splash(const v8::Arguments& args);
    static v8::Handle<v8::Value> Update(const v8::Arguments& args);

    char*  fbp;
    int    fbfd;
    struct fb_var_screeninfo *vinfo;
    struct fb_fix_screeninfo *finfo;

	fx_type update_mode;

#define SHMLEN finfo->smem_len
#define XRES vinfo->xres
#define YRES vinfo->yres
	inline void update_pixel(int x,int y,char pix){
		assert( x >= 0 && y >= 0 && x < XRES && y< YRES);
		int offset = XRES * y/2 + (x/2);
		assert( offset < SHMLEN );
		pix &= 0x0F;
		//pix >>= 4;
		if(x%2){
			fbp[offset] = (fbp[offset] & 0xf0) | pix;
		}else{
			fbp[offset] = (fbp[offset] & 0x0f) | (pix << 4); 
		}
	}
#undef SHMLEN
#undef XRES
#undef YRES

	inline int fbioctl(int cmd,int arg1){
		return ioctl(fbfd,cmd,arg1);
	}

	inline int fbioctl(int cmd,void* arg1){
		return ioctl(fbfd,cmd,arg1);
	}

	inline int fbioctl(int cmd,int arg1,void* arg2){
		return ioctl(fbfd,cmd,arg1,arg2);
	}

};

#endif
