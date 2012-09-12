COFFEE=coffee

TARGETS=$(patsubst %.coffee,%.js,$(wildcard *.coffee))

%.js: %.coffee
	$(COFFEE) -c "$<"

all: $(TARGETS)
	make -C binding

clean: 
	rm -rf $(TARGETS)
	make -C binding clean

rsync: all
	rsync -rv --exclude=.git --exclude=.DS_Store . k3usb:/mnt/us/kinode


