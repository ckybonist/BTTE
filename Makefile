REL=$(notdir $(CURDIR))
BETA=$(REL)g

SRC=src
INC=include
OBJ=obj/bin
DBG=obj/dbg
LIB=lib
OUT=bin

INCLUDES=-I. -I$(INC)
LIBS=
#LIBS=-lstdc++

#CC=gcc
CXX = g++

CFLAGS = -std=c++11 -Wall -Wextra $(3RD_CFLAGS)
CDEBUG = -std=c++11 -g -Wall -Wextra $(3RD_CFLAGS)

LDFLAG = -L$(LIB)

SOURCES = $(wildcard $(SRC)/*.cpp)
OBJS = $(notdir $(patsubst %.cpp,%.o,$(SOURCES)))
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
.SUFFIXES: .o .cpp

# Create object files
$(OBJ)/%.o: $(SRC)/%.cpp $(INC)/%.h
	$(CXX) $(CDEBUG) $(INCLUDES) -c $< -o $@

$(DBG)/%.o: $(SRC)/%.cpp $(INC)/%.h
	$(CXX) -o $@ -c $< $(CFLAGS) $(INCLUDES)

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
