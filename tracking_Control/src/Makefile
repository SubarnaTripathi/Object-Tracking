################################################################################
# Makefile.sun
# Copyright (C) 2000, by A. Borneo - STMicroelectronics
#
# Put here all the modifications.
# If it is not strickly required, DO NOT MODIFY "Makefile.all".
# If you have usefull proposals for this file, please let me know.
################################################################################

### include debug information 
#ADD_DEBUG_FLAGS= 1


################################################################################
# This section defines the C compiler used by this Makefile
# and the compiling flags.
################################################################################


MY_MKFILE = Makefile.dir

CC       = gcc
WARNINGS = -Wall -ansi -pedantic

ifdef ADD_DEBUG_FLAGS
DEBUG_FLAGS := -g -DDEBUG
else
DEBUG_FLAGS := -O3
endif


PENTIUM4_FLAGS :=  -std=c89 -pipe -fforce-addr -fomit-frame-pointer -funroll-loops -frerun-loop-opt -frerun-cse-after-loop -falign-functions=4
CFLAGS   = $(DEBUG_FLAGS) $(PENTIUM4_FLAGS) -D_GNU_SOURCE -DAST_VC1_Linux  -ffloat-store $(WARNINGS) -I../inc -I../common_hdr
LINK     = -lm


################################################################################
# This section defines the variables exported to the next Makefile
################################################################################


export EXE_NAME = Xcoder 
  
export EXT_DIR =
export COMMON_SRC = ../common_src
 
export CMD_COMP = $(CC) $(CFLAGS)
export EXT_COMP =
export CMD_LINK = $(CC) $(CFLAGS) $(LINK)
export EXT_LINK = $(LIBS)


export CMD_DEP = $(CC)  $(CFLAGS) -MM
export EXT_DEP =
export CMD_DEF = $(CC)  $(CFLAGS) -E -dM
export EXT_DEF =

export CMD_TOUCH = /bin/touch -t `date '+%y%m%d%H%M.%S'`
#export CMD_TOUCH =


################################################################################
# The following lines, until the end of this file, are in the form
#	<target>: <dependencies>
#	<TAB>command ...
# each target can be addressed using the UNIX command:
#	make <target>
#
# The first target (default) is the default one and is addressed when "make" is
# called without explicit target
#
# The optionally .PHONY directive defines internal targets NOT related with file
# names. It avoids both loosing time in unusefull checks and execution errors.
################################################################################

.PHONY : default all clean def defines deps

all default:
	$(MAKE) -f $(MY_MKFILE) all

clean:
	$(MAKE) -f $(MY_MKFILE) clean

def defines:
	$(MAKE) -f $(MY_MKFILE) def

deps:
	$(MAKE) -f $(MY_MKFILE) deps

