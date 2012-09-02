COFFEE=coffee

TARGETS=$(patsubst %.coffee,%.js,$(wildcard *.coffee))

%.js: %.coffee
	$(COFFEE) -c -o "$@" "$<"

all: $(TARGETS)
	make -C binding

clean: 
	rm -rf *.js
	make -C binding clean
