COFFEE=coffee
OUT_DIR=build

TARGETS=$(patsubst lib/%.coffee,$(OUT_DIR)/%.js,$(wildcard lib/*.coffee))

$(OUT_DIR)/%.js: lib/%.coffee
	$(COFFEE) -o $(OUT_DIR) -c "$<"

all: $(TARGETS) $(OUT_DIR)
	make -C binding

$(OUT_DIR):
	-mkdir -p $(OUT_DIR)

clean: 
	make -C binding clean
	rm -rf $(OUT_DIR)



