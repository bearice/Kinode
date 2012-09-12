#ifndef _KANVAS_H
#define _KANVAS_H

class Kanvas : public node::ObjectWrap{
public:
    static void Init(v8::Handle<v8::Object> target);

    static v8::Handle<v8::Value> New(const v8::Arguments& args);
    static Kanvas* New(const node::Buffer buf,int width,int rows,int pitch);
    static Kanvas* New(int width,int rows);

    static v8::Handle<v8::Value>   FillRect(const v8::Arguments& args);
    static v8::Handle<v8::Value> StrokeRect(const v8::Arguments& args);
    static v8::Handle<v8::Value>   DrawLine(const v8::Arguments& args);
    static v8::Handle<v8::Value> DrawString(const v8::Arguments& args);
    static v8::Handle<v8::Value> SizeString(const v8::Arguments& args);

    static v8::Handle<v8::Value> GetPixel(const v8::Arguments& args) ;
    static v8::Handle<v8::Value> SetPixel(const v8::Arguments& args) ;
    
    static v8::Handle<v8::Value> GetProp(v8::Local<v8::String> property, 
			const v8::AccessorInfo &info);
    static void SetProp(v8::Local<v8::String> property, 
			v8::Local<v8::Value> value, const v8::AccessorInfo& info);

	v8::Persistent<v8::Object> buffer;
	v8::Persistent<v8::Object> font;

    int  width,rows,pitch,perpend;
    char color;

    void   fillRect(int x ,int y ,int w ,int h );
    void strokeRect(int x ,int y ,int w ,int h ,int width);
    void   drawLine(int x1,int y1,int x2,int y2);
    void drawString(std::string str, int x ,int y);
    void sizeString(std::string str, int* width, int* height);

    bool getPixel(int x, int y, char* c);
    bool setPixel(int x, int y, char c);

	inline int offset(int x,int y){
		return y*pitch+x;
	};
};

#endif
