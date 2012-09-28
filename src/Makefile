HOST=/Volumes/XTools/arm-kindle-linux-gnueabi/bin/arm-kindle-linux-gnueabi
NODE=/Users/bearice/Working/node

PKG_CONFIG_PATH=/Volumes/XTools/arm-kindle-linux-gnueabi/arm-kindle-linux-gnueabi/sysroot/usr/local/lib/pkgconfig

CC=$(HOST)-gcc
CXX=$(HOST)-g++
LD=$(HOST)-g++

CXXFLAGS=-c -pipe $(NODE_INCLUDES) $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags freetype2) -g3
LDFLAGS=-shared -g3
LIBS=-static-libstdc++ \
	 $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs freetype2) \
	 -Wl,-Bdynamic

OBJECTS=$(patsubst %.cc,$(OUT_DIR)/%.obj,$(wildcard *.cc))
TARGET=$(OUT_DIR)/binding.node $(OUT_DIR)/index.js

NODE_INCLUDES=-I$(NODE)/src -I$(NODE)/deps/v8/include -I$(NODE)/deps/uv/include

OUT_DIR=../build/node_modules/binding

HEADERS=$(wildcard *.h)

all: $(OUT_DIR) $(TARGET)

$(OUT_DIR)/%.obj: %.cc $(HEADERS) 
	$(CXX) $(CXXFLAGS) -o "$@" "$<"

$(OUT_DIR)/%.node: $(OBJECTS)
	$(LD) $(LDFLAGS) -o "$@" $(OBJECTS) $(LIBS) 

$(OUT_DIR)/%.js: %.coffee
	coffee -o $(OUT_DIR) -c "$<"

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

clean:
	rm -rf $(TARGET) $(OBJECTS)
	rm -rf $(OUT_DIR)

.PHONY: all clean 

