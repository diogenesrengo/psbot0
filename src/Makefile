
PSBOTlib = psbot0
PSBOTinc = psbot0.h

src = $(wildcard *.c)
obj = $(src:.c=.o)
dep = $(obj:.o=.d)  

mcflags = `pkg-config --cflags MagickWand`
mlflags = `pkg-config --libs MagickWand`
CFLAGS = -g -O2 -Wall -fPIC 
LDFLAGS= -lX11 -lxdo $(mlflags) ../gocr/src/libPgm2asc.a
gocrinc= -I../gocr/src/ -I../gocr/include/

all: lib$(PSBOTlib).so #lib$(PSBOTlib).a 

-include $(dep) 

mocr.o: mocr.c 
	$(CC) $(CFLAGS) $(gocrinc) -c -o $@ $<
mocr.d: mocr.c
	@$(CC) $(CFLAGS) $(gocrinc) $< -MM -MT $(@:.d=.o) >$@

capt.o: capt.c
	$(CC) $(CFLAGS) $(mcflags) -c -o $@ $<
%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

#lib$(PSBOTlib).a: $(obj) Makefile
#	$(AR) cru $@ $<
#	$(RANLIB) $@

lib$(PSBOTlib).so: $(obj) Makefile
	$(CC) $(CFLAGS) $(mcflags) -fPIC -shared -Wl,-h$@ -o $@ $(obj) $(LDFLAGS)

.PHONY: clean install uninstall
clean:
	rm -f $(obj) lib$(PSBOTlib).*
	rm -f $(dep) 
	
PREFIX = /usr/local
install: all
	mkdir -p $(DESTDIR)$(PREFIX)/lib
	mkdir -p $(DESTDIR)$(PREFIX)/include
	cp lib$(PSBOTlib).* $(DESTDIR)$(PREFIX)/lib/
	cp $(PSBOTinc) $(DESTDIR)$(PREFIX)/include/

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/lib$(PSBOTlib).*
	rm -f $(DESTDIR)$(PREFIX)/include/$(PSBOTinc)

