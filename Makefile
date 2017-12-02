# COPIED FROM https://gist.github.com/rehno-lindeque/36e1e5720ca05f89e985fc7b62f54158

ifdef VERBOSE
	Q =
	E = @true 
else
	Q = @
	E = @echo 
endif

CFILES := $(shell find src -mindepth 1 -maxdepth 4 -name "*.c")
CXXFILES := $(shell find src -mindepth 1 -maxdepth 4 -name "*.cpp")

INFILES := $(CFILES) $(CXXFILES)

OBJFILES := $(CXXFILES:src/%.cpp=%) $(CFILES:src/%.c=%)
DEPFILES := $(CXXFILES:src/%.cpp=%) $(CFILES:src/%.c=%)
OFILES := $(OBJFILES:%=obj/%.o)

BINFILE = drunt

COMMONFLAGS = -Wall -Wextra -pedantic
LDFLAGS =

ifdef DEBUG
	COMMONFLAGS := $(COMMONFLAGS) -g
endif
CFLAGS = $(COMMONFLAGS) --std=c99
CXXFLAGS = $(COMMONFLAGS) --std=c++0x
DEPDIR = deps
all: $(BINFILE)
ifeq ($(MAKECMDGOALS),)
-include Makefile.dep
endif
ifneq ($(filter-out clean, $(MAKECMDGOALS)),)
-include Makefile.dep
endif

CC = gcc
CXX = g++


-include Makefile.local

.PHONY: clean all depend
.SUFFIXES:
obj/%.o: src/%.c
	$(E)C-compiling $<
	$(Q)if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(Q)$(CC) -o $@ -c $< $(CFLAGS)
obj/%.o: src/%.cpp
	$(E)C++-compiling $<
	$(Q)if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(Q)$(CXX) -o $@ -c $< $(CXXFLAGS)
Makefile.dep: $(CFILES) $(CXXFILES)
	$(E)Depend
	$(Q)for i in $(^); do $(CXX) $(CXXFLAGS) -MM "$${i}" -MT obj/`basename $${i%.*}`.o; done > $@

	
$(BINFILE): $(OFILES)
	$(E)Linking $@
	$(Q)$(CXX) -o $@ $(OFILES) $(LDFLAGS)
clean:
<<<<<<< HEAD
	$(E)Removing files
	$(Q)rm -f $(BINFILE) obj/* Makefile.dep
=======
	rm -vf drunt
	rm -vf *.o
	rm -vf newcal.ics
>>>>>>> master
