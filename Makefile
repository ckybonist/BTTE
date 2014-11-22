REL=$(notdir $(CURDIR))
BETA=$(REL)g

SRC = src
ALGORITHM = algorithm

OBJ = obj/bin
DBG = obj/dbg
LIB = lib
OUT = bin

INCLUDES=-I. -I$(wildcard $(SRC)/%.h) -I$(ALGORITHM)
LIBS=
#LIBS=-lstdc++

#CC=gcc
CXX = g++

CFLAGS = -std=c++11 -Wall -Wextra $(3RD_CFLAGS)
CDEBUG = -std=c++11 -g -Wall -Wextra $(3RD_CFLAGS)

LDFLAG = -L$(LIB)

SOURCES = $(wildcard $(SRC)/*.cpp $(ALGORITHM)/*.cpp)
OBJS = $(notdir $(patsubst %.cpp,%.o,*.$(SOURCES)))
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
.SUFFIXES: .o .cpp .tpp

# Create object files
$(OBJ)/%.o: $(SRC)/%.cpp $(SRC)/%.h
	$(CXX) $(CDEBUG) $(INCLUDES) -c $< -o $@

$(DBG)/%.o: $(SRC)/%.cpp $(SRC)/%.h
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
