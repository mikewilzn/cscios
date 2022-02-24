########################################################
# Mike Wilson Makefile for Shell
########################################################

########################################################
# Variable definitions
########################################################
# C compiler
CC      = gcc

# C compiler flags
#CFLAGS = -g -Wall -Wextra -Werror 
#CFLAGS = -O3 -Wall -Wextra -Werror 
CFLAGS = -Wall

# Linker flags
LDFLAGS  := 

# Libraries we're using, prefaced with "-l"
LDLIBS   := 

#############################################################
# Rules
#   Rules have the form
#   target : prerequisites
#   	  recipe
#############################################################

Shell : Shell.o
	$(LINK.o) $(LDFLAGS) $^ $(LDLIBS) -o $@

Shell.o : Shell.c

#############################################################

.PHONY : clean

clean :
	@$(RM) Shell a.out core
	@$(RM) *.o *~ *.d

#############################################################
