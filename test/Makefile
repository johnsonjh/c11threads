obj = test.o
bin = test

CFLAGS  = -std=gnu11 -Wall -O3 -flto=auto -g -I..
LDFLAGS = -lpthread -flto=auto

#CFLAGS += -DTESTING
#CFLAGS += -DUSE_MONOTONIC

$(bin): $(obj)
	$(CC) -o $@ $(obj) $(LDFLAGS)

test.o: test.c ../c11threads.h

.PHONY: clean
clean:
	$(RM) $(obj) $(bin) *.wo ../*.wo *.exe

test.exe: test.wo ../c11threads_win32.wo
	$(CC) -o $@ $^

.SUFFIXES: .wo
.c.wo:
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: cross
cross:
	+$(MAKE) CC=x86_64-w64-mingw32-gcc test.exe
