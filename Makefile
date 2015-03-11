#   ----------------------------------------------------------------------------
#  @file   Makefile
#
#  @path   
#
#  @desc   Makefile for screenshot (Debug and Release versions)
#
#  @ver    1.0
#   ----------------------------------------------------------------------------
#  Copyright (c) Texas Instruments Incorporated 2002-2009
#
#  Use of this software is controlled by the terms and conditions found in the
#  license agreement under which this software has been supplied or provided.
#

#   ----------------------------------------------------------------------------
#   Included defined variables
#   ----------------------------------------------------------------------------
-include ../../Rules.make

#   ----------------------------------------------------------------------------
#   Variables passed in externally
#   ----------------------------------------------------------------------------
CFLAGS ?= -march=armv5te
CROSS_COMPILE ?=

#   ----------------------------------------------------------------------------
#   Name of the Linux compiler
#   ----------------------------------------------------------------------------
CC := $(CROSS_COMPILE)gcc

#   ----------------------------------------------------------------------------
#   General options, sources and libraries
#   ----------------------------------------------------------------------------
NAME := screenshot
SRCS := screenshot.c
HDRS := 
OBJS :=
DEBUG :=
BIN := screenshot

#   ----------------------------------------------------------------------------
#   Compiler and Linker flags for Debug
#   ----------------------------------------------------------------------------
OBJDIR_D := Debug
BINDIR_D := $(OBJDIR_D)
LIBS_D := $(LIBS)
OBJS_D := $(SRCS:%.c=$(OBJDIR_D)/%.o)
ALL_DEBUG := -g3 -gdwarf-2

#   ----------------------------------------------------------------------------
#   Compiler and Linker flags for Release
#   ----------------------------------------------------------------------------
OBJDIR_R := Release
BINDIR_R := $(OBJDIR_R)
LIBS_R := $(LIBS)
OBJS_R := $(SRCS:%.c=$(OBJDIR_R)/%.o)
ALL_RELEASE :=

#   ----------------------------------------------------------------------------
#   Compiler include directories 
#   ----------------------------------------------------------------------------
INCLUDES := 

#   ----------------------------------------------------------------------------
#   All compiler options to be passed to the command line
#   ----------------------------------------------------------------------------
ALL_CFLAGS := $(INCLUDES)                   \
              -c                            \
              $(CFLAGS)

LDFLAGS :=

#   ----------------------------------------------------------------------------
#   Compiler symbol definitions 
#   ----------------------------------------------------------------------------
DEFS :=

#   ----------------------------------------------------------------------------
#   Compiler and Linker procedure
#   From this point and on changes are very unlikely.
#   ----------------------------------------------------------------------------
.PHONY: all
all: first debug release

#   ----------------------------------------------------------------------------
#   Building Debug... 
#   ----------------------------------------------------------------------------
.PHONY: first
first:
	@mkdir -p $(OBJDIR_D)
	@mkdir -p $(OBJDIR_R)

.PHONY: debug
debug: first $(BINDIR_D)/$(BIN) 

$(BINDIR_D)/$(BIN): $(OBJS_D)
	@echo Compiling Debug...
	$(CC) -o $@ $(OBJS_D) $(LIBS_D) $(LDFLAGS)  -Wl,-Map,$(BINDIR_D)/$(NAME).map

$(OBJDIR_D)/%.o : %.c $(HDRS)
	$(CC) $(ALL_DEBUG) $(DEFS) $(ALL_CFLAGS) -o$@ $<

#   ----------------------------------------------------------------------------
#   Building Release... 
#   ----------------------------------------------------------------------------
.PHONY: release
release: first $(BINDIR_R)/$(BIN)

$(BINDIR_R)/$(BIN): $(OBJS_R)
	@echo Compiling Release...
	$(CC) -o $@ $(OBJS_R) $(LIBS_R) $(LDFLAGS) -Wl,-Map,$(BINDIR_R)/$(NAME).map


$(OBJDIR_R)/%.o : %.c $(HDRS)
	$(CC) $(DEFS) $(ALL_RELEASE) $(ALL_CFLAGS) -o$@ $<

.PHONY: clean
clean:
	@rm -f $(OBJDIR_D)/*
	@rm -f $(OBJDIR_R)/*

install:
	@install -d $(DESTDIR)/usr/bin
	@if [ -e Release/screenshot ] ; then \
		install Release/screenshot $(DESTDIR)/usr/bin ; \
		echo "screenshot release version installed."; \
	else \
		echo "screenshot release version not compiled."; \
	fi

install_debug:
	@install -d $(DESTDIR)/usr/bin
	@if [ -e Debug/screenshot ] ; then \
		install Debug/screenshot $(DESTDIR)/usr/bin ; \
		echo "screenshot debug version installed."; \
	else \
		echo "screenshot debug version not compiled."; \
	fi
