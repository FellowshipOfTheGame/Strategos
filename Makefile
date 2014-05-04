# CHANGE THIS

SDL2_INCLUDE_DIR = /usr/include/SDL2
SDL2_TTF_INCLUDE_DIR =
SDL2_IMAGE_INCLUDE_DIR =

# _WINDOWS_ or _UNIX_
OS =_UNIX_

# Compiler
CC=gcc
CXX=g++

# Remove
RM=rm -f
# Move --> Windows: move,	Linux: mv
MV=mv

#####

include_project = -Isource/include/ -Isource/
include_lib = -I$(SDL2_INCLUDE_DIR) -I$(SDL2_TTF_INCLUDE_DIR) -I$(SDL2_IMAGE_INCLUDE_DIR)
includes = $(include_project) $(include_lib)

#####

EXECUTABLE = strategos

CPPFLAGS= $(includes) -std=c++11 -D$(OS) -g
LDFLAGS= -g
LDLIBS= -lSDL2 -lSDL2_image -lSDL2_ttf

SRCS_CPP = $(wildcard source/src/*.cpp) $(wildcard source/SDL2_gfx/*.cpp)
SRCS_C = $(wildcard source/SDL2_gfx/*.c)
SRCS =  $(SRCS_CPP) $(SRCS_C)
OBJS = $(subst .cpp,.o,$(SRCS_CPP)) $(subst .c,.o,$(SRCS_C))

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	g++ $(LDFLAGS) -o $(EXECUTABLE) $(OBJS) $(LDLIBS)
	$(MV) $(EXECUTABLE) bin/Release/

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .dependtool *.o

include .depend
