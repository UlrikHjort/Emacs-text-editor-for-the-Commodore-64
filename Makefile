#
# Makefile for C64 Emacs Editor
# Requires: cc65 toolchain: https://cc65.github.io/
#          c1541 from VICE: https://vice-emu.sourceforge.io/
#
# Usage:
#   make          - build emacs.prg and emacs_disk.d64
#   make clean    - remove build artefacts
#   make run      - build + launch in VICE (x64sc)
#

CC65_HOME ?= <ADD PATH TO cc65>
CL65      = $(CC65_HOME)/bin/cl65
TARGET    = c64
PROG      = emacs
DISK      = $(PROG)_disk.d64

CFLAGS = -t $(TARGET) -O --static-locals -I src

SRCS = src/main.c    \
       src/editor.c  \
       src/display.c \
       src/fileio.c

.PHONY: all clean run

all: $(PROG).prg $(DISK)

$(PROG).prg: $(SRCS) src/editor.h
	$(CL65) $(CFLAGS) -o $@ $(SRCS)

$(DISK): $(PROG).prg
	rm -f $(DISK)
	c1541 -format "c64 emacs,em" d64 $(DISK) \
	      -attach $(DISK) \
	      -write $(PROG).prg $(PROG)

run: $(DISK)
	x64sc -autostart $(DISK)

clean:
	rm -f $(PROG).prg $(PROG).map $(DISK)
