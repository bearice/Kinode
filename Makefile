COFFEE=coffee
OUT_DIR=build

SUBDIRS=binding lib example

TARGETS=$(addprefix $(OUT_DIR)/,$(SUBDIRS))
CLAEN_TARGETS=$(addprefix clean_$(OUT_DIR)/,$(SUBDIRS))

	
all: $(TARGETS) $(OUT_DIR)

$(OUT_DIR)/%: %
	make -C $<

clean_$(OUT_DIR)/%: %
	make -C $< clean

$(OUT_DIR):
	-mkdir -p $(OUT_DIR)

clean: $(CLAEN_TARGETS)
	rm -rf $(OUT_DIR)



