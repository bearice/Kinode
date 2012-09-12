HOST=/Volumes/XTools/arm-kindle-linux-gnueabi/bin/arm-kindle-linux-gnueabi
NODE=/Users/bearice/Working/node

PKG_CONFIG_PATH=/Volumes/XTools/arm-kindle-linux-gnueabi/arm-kindle-linux-gnueabi/sysroot/usr/local/lib/pkgconfig

CC=$(HOST)-gcc
CXX=$(HOST)-g++
LD=$(HOST)-g++

CXXFLAGS=-c -pipe $(NODE_INCLUDES) $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags freetype2) -g3
LDFLAGS=-shared -g3
LIBS=-static-libstdc++ -Wl,-static $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs freetype2) -Wl,-Bdynamic

OBJECTS=$(patsubst %.cc,%.obj,$(wildcard *.cc))
TARGET=binding.node index.js

NODE_INCLUDES=-I$(NODE)/src -I$(NODE)/deps/v8/include -I$(NODE)/deps/uv/include

HEADERS=$(wildcard *.h)

all: $(TARGET)

%.obj: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -o "$@" "$<"

binding.node: $(OBJECTS)
	$(LD) $(LDFLAGS) -o "$@" $(OBJECTS) $(LIBS) 

%.js: %.coffee
	coffee -c "$<"

clean:
	rm -rf $(TARGET) 

.PHONY: all clean 

