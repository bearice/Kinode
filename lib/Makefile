COFFEE=coffee
OUT_DIR=../build/node_modules

TARGETS=$(patsubst %.coffee,$(OUT_DIR)/%.js,$(wildcard *.coffee))

all: $(TARGETS) $(OUT_DIR)

$(OUT_DIR)/%.js: %.coffee
	$(COFFEE) -o $(OUT_DIR) -c "$<"

$(OUT_DIR):
	-mkdir -p $(OUT_DIR)

clean:
	rm -rf $(TARGETS) 
	rm -rf $(OUT_DIR)



