COFFEE=coffee
OUT_DIR=build

SUBDIRS=src lib canvas example

TARGETS=$(addprefix build_,$(SUBDIRS))
CLAEN_TARGETS=$(addprefix clean_,$(SUBDIRS))

	
all: $(TARGETS) $(OUT_DIR)

build_%: %
	make -C $<

clean_%: %
	make -C $< clean

$(OUT_DIR):
	-mkdir -p $(OUT_DIR)

clean: $(CLAEN_TARGETS)



