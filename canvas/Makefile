OUT_DIR=../build/node_modules/canvas

SUBDIRS=src lib

TARGETS=$(addprefix $(OUT_DIR)/,$(SUBDIRS)) 
TARGETS+=$(OUT_DIR)/index.js
CLAEN_TARGETS=$(addprefix clean_$(OUT_DIR)/,$(SUBDIRS))

	
all: $(TARGETS) $(OUT_DIR)

$(OUT_DIR)/index.js: index.js
	cp -f index.js $(OUT_DIR)/index.js

$(OUT_DIR)/%: %
	make -C $<

clean_$(OUT_DIR)/%: %
	make -C $< clean

$(OUT_DIR):
	-mkdir -p $(OUT_DIR)

clean: $(CLAEN_TARGETS)
	rm -rf $(OUT_DIR)




