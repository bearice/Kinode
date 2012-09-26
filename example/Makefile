COFFEE=coffee
SED=gsed
OUT_DIR=../build

TARGETS=$(patsubst %.coffee,$(OUT_DIR)/%.js,$(wildcard *.coffee))

all: $(TARGETS) $(OUT_DIR)

$(OUT_DIR)/%.js: %.coffee
	$(COFFEE) -c -s < $< | $(SED) -e'1i#!/usr/bin/env node' > $@
	chmod +x $@

$(OUT_DIR):
	-mkdir -p $(OUT_DIR)

clean:
	rm -rf $(TARGETS) 
	rm -rf $(OUT_DIR)



