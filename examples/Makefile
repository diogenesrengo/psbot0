
src = $(wildcard *.c)
progs = $(patsubst %.c,%,$(src))
CFLAGS = -g -O2 -Wall  
LDFLAGS = -lpsbot0 

all: $(progs)
%: %.c 
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

.PHONY: clean ctmps
clean:
	rm -f $(progs) 
ctmps:
	rm -r /tmp/psbot0-*

