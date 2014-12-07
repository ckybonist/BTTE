REL=$(notdir $(CURDIR))
BETA=$(REL)g

EXT = cpp

SRC = src
CUSTOM_ALGO = src/custom

SRC_SUBDIRS = \
          $(SRC)/algorithm \
		  $(SRC)/utils \
		  $(CUSTOM_ALGO)

OBJ = obj/bin
DBG = obj/dbg
LIB = lib
OUT = bin

INCLUDES=-I. -I$(wildcard $(SRC)/%.h) $(foreach d,$(SRC_SUBDIRS),-I$d)
LIBS=
#LIBS=-lstdc++


#CC=gcc
CXX = g++
CFLAGS = -std=c++11 -Wall -Wextra $(3RD_CFLAGS)
CDEBUG = -std=c++11 -g -Wall -Wextra $(3RD_CFLAGS)
LDFLAG = -L$(LIB)


SOURCES = $(shell find $(SRC) -name "*.$(EXT)")
OBJS = $(notdir $(SOURCES:.$(EXT)=.o))
REL_OBJS = $(addprefix $(OBJ)/,$(OBJS))
DBG_OBJS = $(addprefix $(DBG)/,$(OBJS))

# pkg-config --cflags (get 3rd lib's pre-processing info.)
#3RD_CFLAGS=$(shell pkg-config --cflags 3rd-lib)

# pkg-config --libs (get 3rd lib's linking info.)
#3RD_LIBS= $(shell pkg-config --libs 3rd-lib)


# implementation
# delete the default suffixes
.SUFFIXES:
# define our suffix list
.SUFFIXES: .o .$(EXT) .tpp

# Create object files
# src/
$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(DBG)/%.o: $(SRC)/%.cpp
	$(CXX) $(CDEBUG) $(INCLUDES) -c $< -o $@

# src/algorithm
$(OBJ)/%.o: $(SRC)/algorithm/%.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(DBG)/%.o: $(SRC)/algorithm/%.cpp
	$(CXX) $(CDEBUG) $(INCLUDES) -c $< -o $@

# src/utils
$(OBJ)/%.o: $(SRC)/utils/%.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(DBG)/%.o: $(SRC)/utils/%.cpp
	$(CXX) $(CDEBUG) $(INCLUDES) -c $< -o $@

# custom/
$(OBJ)/%.o: $(SRC)/custom/%.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(DBG)/%.o: $(SRC)/custom/%.cpp
	$(CXX) $(CDEBUG) $(INCLUDES) -c $< -o $@

all: release debug

# Create binaries
release: $(REL_OBJS)
	$(CXX) -o $(OUT)/$(REL) $(REL_OBJS) $(LDFLAG) $(LIBS)

debug: $(DBG_OBJS)
	$(CXX) -o $(OUT)/$(BETA) $(DBG_OBJS) $(LDFLAG) $(LIBS)


.PHONY: clean

clean:
	rm -f $(REL_OBJS) $(DBG_OBJS) $(OUT)/* core
# @rm -f $(REL_OBJS) $(DBG_OBJS) $(OUT)/* core (silence mode)
