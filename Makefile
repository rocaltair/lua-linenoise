CC = gcc
LIB = linenoise.so
OBJS = linenoise.o \
       llinenoise.o

LLIBS = -llua
CFLAGS = -c -fPIC -Wall
LDFLAGS = --shared

all : $(LIB)

$(LIB): $(OBJS)
	$(CC) -o $@ $^ $(LLIBS) $(LDFLAGS) 

$(OBJS) : %.o : %.c
	$(CC) -o $@ $(CFLAGS) $<

clean : 
	rm -f $(OBJS) $(LIB)

.PHONY : clean

