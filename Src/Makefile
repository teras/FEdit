## Makefile for Dokimh

include $(shell pwd)/../config.make

COBJS=$(shell ls *.c 2>/dev/null)
OBJS=$(COBJS:.c=.o)

all: $(TARGET) code0000.$(TARGET).grc code0001.$(TARGET).grc data0000.$(TARGET).grc pref0000.$(TARGET).grc rloc0000.$(TARGET).grc

.h.c:
	touch $@

.S.o:
	$(CC) $(TARGETFLAGS) -c $<

.c.s:
	$(CC) $(CSFLAGS) $<

code0000.$(TARGET).grc: $(TARGET)
	$(OBJRES) $(TARGET)

code0001.$(TARGET).grc: code0000.$(TARGET).grc

data0000.$(TARGET).grc: code0000.$(TARGET).grc

pref0000.$(TARGET).grc: code0000.$(TARGET).grc

rloc0000.$(TARGET).grc: code0000.$(TARGET).grc


$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS)
	! $(NM) -u $(TARGET) | grep .


clean:
	-rm -f *.[oa] $(TARGET) *.bin bin.res *.grc Makefile.bak

distclean: clean
	-rm -f $(TARGET).prc pilot.ram pilot.scratch
