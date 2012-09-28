#include <string>

#include <v8.h>
#include <node.h>
#include <node_buffer.h>

#include "fb.h"
#include "font.h"
#include "kanvas.h"

void init(v8::Handle<v8::Object> target) {
	FBDev::Init(target);
	Font::Init(target);
	Kanvas::Init(target);
}

NODE_MODULE(binding,init)

