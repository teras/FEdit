## Makefile for Dokimh

include $(shell pwd)/config.make


all: $(TARGET).prc


$(TARGET).prc: everything

everything:
	cd $(SRC); make
	cd $(RSC); make
	$(BUILDPRC) $(TARGET).prc $(APPNAME) $(APPID) $(SRC)/*.grc $(RSC)/*.bin



send: $(TARGET).prc
	$(PILOTXFER) -i $(TARGET).prc

compact:distclean

clean:
	cd $(SRC); make clean
	cd $(RSC); make clean

distclean: clean
	rm -f $(TARGET).prc pilot.ram pilot.scratch

tar: clean
	mkdir /tmp/$(TARGET)-$(VERSION)
	cp -ra $(TARGET).prc Makefile config.make Src Rsc doc /tmp/$(TARGET)-$(VERSION)
	cd /tmp; tar zcvf $(TARGET)-$(VERSION).tar.gz $(TARGET)-$(VERSION)
	rm -r /tmp/$(TARGET)-$(VERSION)
	mv /tmp/$(TARGET)-$(VERSION).tar.gz .

run: all
	-pose -load_apps fedit.prc,others/FontHack123-32.prc,others/RsrcEdit.prc,others/X-Master.prc,others/FEdit-FontCache.pdb -run_app FEdit

rr: clean run
