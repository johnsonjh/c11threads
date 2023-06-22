##########################################################################

obj = test.o
bin = test

##########################################################################

ifdef USE_C11THREADS_H
  THREAD = -DUSE_C11THREADS_H=1
  LPTHREAD = -lpthread
endif

##########################################################################

ifdef USE_C11THREADS
  THREAD = -DUSE_C11THREADS_H=1
  USE_CTHREADS_H = 1
  LPTHREAD = -lpthread
endif

##########################################################################

ifdef USE_THREADS_H
  THREAD = -DUSE_THREADS_H=1
endif

##########################################################################

ifdef USE_THREADS
  THREAD = -DUSE_THREADS_H=1
  USE_THREADS_H = 1
endif

##########################################################################

ifdef C11THREADS_PTHREAD_WIN32
  THREAD += -DC11THREADS_PTHREAD_WIN32=1
endif

##########################################################################

ifdef PTHREAD_WIN32
  THREAD += -DC11THREADS_PTHREAD_WIN32=1
endif

##########################################################################

LTO ?= -flto=auto

##########################################################################

CFLAGS  = $(THREAD) -std=gnu11 -Wall -O3 $(LTO) -g -I.. -I../../libsir
LDFLAGS = $(LPTHREAD) $(LTO) ../../libsir/build/obj/*.lo

##########################################################################

#CFLAGS += -DTESTING
#CFLAGS += -DUSE_MONOTONIC

##########################################################################

$(bin): $(obj)
	$(CC) -o $@ $(obj) $(LDFLAGS)

##########################################################################

ifdef USE_C11THREADS_H
test.o: test.c ../c11threads.h
endif
ifdef USE_CTHREADS_H
test.o: test.c
endif

##########################################################################

.PHONY: clean
clean:
	$(RM) $(obj) $(bin) *.wo ../*.wo *.exe

##########################################################################

test.exe: test.wo ../c11threads_win32.wo
	$(CC) -o $@ $^ $(LDFLAGS)

##########################################################################

.SUFFIXES: .wo
.c.wo:
	$(CC) -o $@ -c -D_WIN32=1 $< $(CFLAGS)

##########################################################################

.PHONY: cross
cross: ../c11threads.h
	+$(MAKE) CC=x86_64-w64-mingw32-gcc test.exe

##########################################################################
