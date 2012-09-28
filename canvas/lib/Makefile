OUT_DIR=../../build/node_modules/canvas

TARGETS=$(addprefix $(OUT_DIR)/,$(wildcard *.js))

all: $(TARGETS)

$(OUT_DIR):
	-mkdir -p $(OUT_DIR)

$(OUT_DIR)/%.js: %.js $(OUT_DIR)
	cp "$<" "$@"

clean: $(CLAEN_TARGETS)
	rm -rf $(OUT_DIR)



