#ifndef _COMMON_H
#define _COMMON_H

static inline string ObjectToString(Local<Value> value) {
    String::Utf8Value utf8_value(value);
    return string(*utf8_value);
}

static void RoSetProp(Local<String> name, Local<Value> value, const AccessorInfo& info){
    THROW("updating read-only property");
}

static inline FT_ULong decodeUTF8(string cp,size_t &idx){
    unsigned char c = cp[idx];
    int len = cp.length() - idx;
    FT_ULong ret = -1;
    if(c<0x80 && len >= 1) {
        ret = c;
        idx++; 
    } else 
    if(c<0xE0 && len >= 2 ) {
        ret =  ((c & 0x1F) << 6) |  
        (cp[idx+1] & 0x3F);
        idx+=2; 
    } else 
    if(c<0xF0 && len >= 3 ) {
        ret =  ((c & 0x0F) << 12) |
       ((cp[idx+1] & 0x3F) <<  6) |  
        (cp[idx+2] & 0x3F);
        idx+=3; 
    } else
    if(c<0xF8 && len >= 4 ) {
        ret =  ((c & 0x07) << 18) |
       ((cp[idx+1] & 0x3F) << 12) | 
       ((cp[idx+2] & 0x3F) <<  6) |  
        (cp[idx+3] & 0x3F);
        idx+=4; 
    } else
    if(c<0xFC && len >= 5 ) {
        ret =  ((c & 0x03) << 24) |
       ((cp[idx+1] & 0x3F) << 18) |
       ((cp[idx+2] & 0x3F) << 12) |
       ((cp[idx+3] & 0x3F) <<  6) | 
        (cp[idx+4] & 0x3F);
        idx+=5; 
    } else 
    if(len >= 6){
        ret =  ((c & 0x01) << 30) | 
       ((cp[idx+1] & 0x3F) << 24) | 
       ((cp[idx+2] & 0x3F) << 18) | 
       ((cp[idx+3] & 0x3F) << 12) | 
       ((cp[idx+4] & 0x3F) <<  6) | 
        (cp[idx+5] & 0x3F);
        idx+=6;
    }
    return ret;
}


#endif
