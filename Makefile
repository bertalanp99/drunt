# [inspired by Nicholas Hamilton] #

#################
# CONFIGURATION #
#################

# compiler and Linker
CC          := gcc

# target binary
TARGET      := drunt

# directories
SRCDIR      := src
INCDIR      := inc
BUILDDIR    := obj
TARGETDIR   := bin

# extensions of sources, includes and objects
SRCEXT      := c
DEPEXT      := h
OBJEXT      := o

# flags, libraries, includes
CFLAGS      := -Wall -Wextra -Wno-sign-compare -std=c99 -g 
#LIB         := -fopenmp -lm -larmadillo
#INC         := -I$(INCDIR) -I/usr/local/include
#INCDEP      := -I$(INCDIR)

######################
# DO NOT EDIT BELOW  # ##############################################################
######################

SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

# default
#all: resources $(TARGET)
all: $(TARGET)

# remake
remake: purge all

# copy resources from resources directory to target directory
#resources: directories
#	@cp $(RESDIR)/* $(TARGETDIR)/

# make the directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

# clean only objects
clean:
	@$(RM) -rf $(BUILDDIR)

# full clean: cleans objects and binaries
purge: clean
	@$(RM) -rf $(TARGETDIR)

# pull in dependency info for *existing* .o files
#-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

# linking
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(TARGET) $^ $(LIB)

# compilation
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

#Non-File Targets
#.PHONY: all remake clean cleaner resources
