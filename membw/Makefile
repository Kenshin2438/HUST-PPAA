CC = gcc
CFLAGS = -O2
LDFLAGS = 
LIBS = -lm -lpthread

EXTS = 

MEMBENCH = membw
TARGETS = $(MEMBENCH)

MEMBENCHOBJS = main.o util.o x86-asm.o
OBJS = $(MEMBENCHOBJS) $(EXTS) 

.PHONY:all
all: $(TARGETS) 

$(MEMBENCH): $(MEMBENCHOBJS) $(EXTS)
	$(CC) $(CFLAGS) $(LDFLAGS)  $^ $(LIBS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.S.o:
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -f *.o $(OBJS) $(TARGETS) 

