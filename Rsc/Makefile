## Makefile for Dokimh

include $(shell pwd)/../config.make

all: bin.res


bin.res: $(TARGET).rcp *.bmp
	$(PILRC) -q $(TARGET).rcp .
	touch bin.res

clean:
	-rm -f *.[oa] $(TARGET) *.bin bin.res *.grc Makefile.bak

distclean: clean
	-rm -f $(TARGET).prc pilot.ram pilot.scratch
