LDFLAGS=-lftdi -lusb
CFLAGS=-Wall
all: 96boardctl

%: %.c
	gcc $< $(CFLAGS) $(LDFLAGS) -o $@
