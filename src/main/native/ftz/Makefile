CPPFLAGS=-g -O3 -mavx -fPIC -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I$(JAVA_HOME)/include/Darwin

OUTPUT_OPTION=-MMD -MP -o $@
LDLIBS=

TARGET = ftz.o

SRC = $(wildcard *.cc)
OBJ = $(SRC:.cc=.o)
DEP = $(SRC:.cc=.d)

all: javah $(TARGET)

javah:
	javah -o ftz.h -cp ../../java com.intel.gkl.ftz.FTZ

-include $(DEP)

clean:
	rm -f $(OBJ) $(DEP) $(TARGET)
