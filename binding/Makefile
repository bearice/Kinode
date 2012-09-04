HOST=/Volumes/XTools/arm-kindle-linux-gnueabi/bin/arm-kindle-linux-gnueabi
NODE=/Users/bearice/Working/node

CC=$(HOST)-gcc
CXX=$(HOST)-g++
LD=$(HOST)-gcc

CFLAGS=-c -pipe
CXXFLAGS=-pipe -shared
LDFLAGS=

TARGET=fb.node kanvas.node

NODE_INCLUDES=-I$(NODE)/src -I$(NODE)/deps/v8/include -I$(NODE)/deps/uv/include

all: $(TARGET)

%.node: %.cc
	$(CXX) $(CXXFLAGS) $(NODE_INCLUDES) -o "$@" "$<"

clean:
	rm -rf $(TARGET) *.o

.PHONY: all clean 

