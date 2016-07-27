EXE0 := igzip0
EXE0C := igzip0c
EXE1 := igzip1
EXE1C := igzip1c
LIB0 := libigzip0.a
LIB0C := libigzip0c.a
LIB1 := libigzip1.a
LIB1C := libigzip1c.a
SLIB0 := libigzip0.so
SLIB0C := libigzip0c.so
SLIB1 := libigzip1.so
SLIB1C := libigzip1c.so
#YASM := /nfs/site/disks/iots4/engineering/cai_isa/tools/yasm-1.1.0-Install/bin/yasm
#YASM := /nfs/site/disks/iots4/engineering/cai_isa/tools/yasm-hsw/yasm
YASM := yasm
OBJ_DIR0 := obj0
OBJ_DIR0C := obj0c
OBJ_DIR1 := obj1
OBJ_DIR1C := obj1c
SOBJ_DIR0 := sobj0
SOBJ_DIR0C := sobj0c
SOBJ_DIR1 := sobj1
SOBJ_DIR1C := sobj1c

REL_DIR := ../igzip_lib

INCLUDE_DIRS := . c_code ../include

INCLUDES := $(foreach i,$(INCLUDE_DIRS),-I $i)

CXX := g++
CC  := gcc
CXXFLAGS := -g -D LINUX $(INCLUDES) -O2
ASMFLAGS := -f x64 -f elf64 -X gnu -g dwarf2 -D LINUX $(INCLUDES)
LDFLAGS += -g

lib_obj_files := \
	common.o \
	crc.o \
	hufftables_c.o \
	crc_utils.o \
	hufftables.o \
	igzip0c_body.o \
	igzip0c_finish.o \
	igzip1c_body.o \
	igzip1c_finish.o \
	init_stream.o \
#	igzip0_body.o \
#	igzip0_finish.o \
#	igzip1_body.o \
#	igzip1_finish.o \
#	asm_all.o \
#	huffman.o \

obj_files := $(lib_obj_files) \
	main.o \
	common2.o \
	time_test.o \
	validate.o \
	warmup.o \
	utils_asm.o \
#	igzip0_2.o \
#	igzip0c_2.o \
#	igzip1_2.o \
#	igzip1c_2.o \
#	igzip0c_body_test.o \

obj0_files := $(obj_files:%=$(OBJ_DIR0)/%)
obj0c_files := $(obj_files:%=$(OBJ_DIR0C)/%)
obj1_files := $(obj_files:%=$(OBJ_DIR1)/%)
obj1c_files := $(obj_files:%=$(OBJ_DIR1C)/%)

lib_sobj0_files := $(lib_obj_files:%=$(SOBJ_DIR0)/%)
lib_sobj0c_files := $(lib_obj_files:%=$(SOBJ_DIR0C)/%)
lib_sobj1_files := $(lib_obj_files:%=$(SOBJ_DIR1)/%)
lib_sobj1c_files := $(lib_obj_files:%=$(SOBJ_DIR1C)/%)

lib_obj0_files := $(lib_obj_files:%=$(OBJ_DIR0)/%)
lib_obj0c_files := $(lib_obj_files:%=$(OBJ_DIR0C)/%)
lib_obj1_files := $(lib_obj_files:%=$(OBJ_DIR1)/%)
lib_obj1c_files := $(lib_obj_files:%=$(OBJ_DIR1C)/%)

release_files := types.h igzip_lib.h internal_state_size.h \
	$(LIB0) $(LIB0C) $(LIB1) $(LIB1C)
release2_files := $(release_files:%=$(REL_DIR)/%)

.PHONY: all exe0 exe0c exe1 exe1c clean release

all: lib0c lib1c slib0c slib1c

exes: exe0 exe0c exe1 exe1c

libs: lib0 lib0c lib1 lib1c

slibs: slib0 slib0c slib1 slib1c

exe0: $(EXE0)

exe0c: $(EXE0C)

exe1: $(EXE1)

exe1c: $(EXE1C)

lib0: $(LIB0)

lib0c: $(LIB0C)

lib1: $(LIB1)

lib1c: $(LIB1C)

slib0: $(SLIB0)

slib0c: $(SLIB0C)

slib1: $(SLIB1)

slib1c: $(SLIB1C)

release: $(release2_files)

options.h: options.inc
	perl asm2h.pm < options.inc > options.h

$(EXE0): $(obj0_files)
	$(CXX) $(LDFLAGS) -o $@ $^

$(EXE0C): $(obj0c_files)
	$(CXX) $(LDFLAGS) -o $@ $^

$(EXE1): $(obj1_files)
	$(CXX) $(LDFLAGS) -o $@ $^

$(EXE1C): $(obj1c_files)
	$(CXX) $(LDFLAGS) -o $@ $^


$(LIB0): $(lib_obj0_files)
	ar -qcs $@ $^

$(LIB0C): $(lib_obj0c_files)
	ar -qcs $@ $^

$(LIB1): $(lib_obj1_files)
	ar -qcs $@ $^

$(LIB1C): $(lib_obj1c_files)
	ar -qcs $@ $^


$(SLIB0): $(lib_sobj0_files)
	$(CXX) -shared -o $@ $^

$(SLIB0C): $(lib_sobj0c_files)
	$(CXX) -shared -o $@ $^

$(SLIB1): $(lib_sobj1_files)
	$(CXX) -shared -o $@ $^

$(SLIB1C): $(lib_sobj1c_files)
	$(CXX) -shared -o $@ $^

################

$(obj0_files): | $(OBJ_DIR0)

$(obj0c_files): | $(OBJ_DIR0C)

$(obj1_files): | $(OBJ_DIR1)

$(obj1c_files): | $(OBJ_DIR1C)

$(lib_sobj0_files): | $(SOBJ_DIR0)

$(lib_sobj0c_files): | $(SOBJ_DIR0C)

$(lib_sobj1_files): | $(SOBJ_DIR1)

$(lib_sobj1c_files): | $(SOBJ_DIR1C)

$(release2_files): | $(REL_DIR)

$(OBJ_DIR0):
	mkdir $(OBJ_DIR0)

$(OBJ_DIR0C):
	mkdir $(OBJ_DIR0C)

$(OBJ_DIR1):
	mkdir $(OBJ_DIR1)

$(OBJ_DIR1C):
	mkdir $(OBJ_DIR1C)

$(SOBJ_DIR0):
	mkdir $(SOBJ_DIR0)

$(SOBJ_DIR0C):
	mkdir $(SOBJ_DIR0C)

$(SOBJ_DIR1):
	mkdir $(SOBJ_DIR1)

$(SOBJ_DIR1C):
	mkdir $(SOBJ_DIR1C)

$(REL_DIR):
	mkdir $(REL_DIR)

########################################################################

$(OBJ_DIR0)/%.o:%.cpp options.h
	@ echo "Making object file $@ "
	$(CXX) -c $(CXXFLAGS) -fPIC -D MAJOR_VERSION=IGZIP0 $< -o $@
	@ echo "--------------------------------------------------------------"

$(OBJ_DIR0)/%.o:c_code/%.cpp options.h
	@ echo "Making object file $@ "
	$(CXX) -c $(CXXFLAGS) -fPIC -D MAJOR_VERSION=IGZIP0 $< -o $@
	@ echo "--------------------------------------------------------------"

$(OBJ_DIR0)/%.o:%.asm options.inc
	@ echo "Making object file $@ "
	$(YASM) $(ASMFLAGS) -D MAJOR_VERSION=IGZIP0 $< -o $@
	@ echo "--------------------------------------------------------------"

$(OBJ_DIR0)/%.o:c_code/%.asm options.inc
	@ echo "Making object file $@ "
	$(YASM) $(ASMFLAGS) -D MAJOR_VERSION=IGZIP0 $< -o $@
	@ echo "--------------------------------------------------------------"

########################################################################

$(OBJ_DIR0C)/%.o:%.cpp options.h
	@ echo "Making object file $@ "
	$(CXX) -c $(CXXFLAGS) -fPIC -D MAJOR_VERSION=IGZIP0C $< -o $@
	@ echo "--------------------------------------------------------------"

$(OBJ_DIR0C)/%.o:c_code/%.cpp options.h
	@ echo "Making object file $@ "
	$(CXX) -c $(CXXFLAGS) -fPIC -D MAJOR_VERSION=IGZIP0C $< -o $@
	@ echo "--------------------------------------------------------------"

$(OBJ_DIR0C)/%.o:%.asm options.inc
	@ echo "Making object file $@ "
	$(YASM) $(ASMFLAGS) -D MAJOR_VERSION=IGZIP0C $< -o $@
	@ echo "--------------------------------------------------------------"

$(OBJ_DIR0C)/%.o:c_code/%.asm options.inc
	@ echo "Making object file $@ "
	$(YASM) $(ASMFLAGS) -D MAJOR_VERSION=IGZIP0C $< -o $@
	@ echo "--------------------------------------------------------------"

########################################################################

$(OBJ_DIR1)/%.o:%.cpp options.h
	@ echo "Making object file $@ "
	$(CXX) -c $(CXXFLAGS) -fPIC -D MAJOR_VERSION=IGZIP1 $< -o $@
	@ echo "--------------------------------------------------------------"

$(OBJ_DIR1)/%.o:c_code/%.cpp options.h
	@ echo "Making object file $@ "
	$(CXX) -c $(CXXFLAGS) -fPIC -D MAJOR_VERSION=IGZIP1 $< -o $@
	@ echo "--------------------------------------------------------------"

$(OBJ_DIR1)/%.o:%.asm options.inc
	@ echo "Making object file $@ "
	$(YASM) $(ASMFLAGS) -D MAJOR_VERSION=IGZIP1 $< -o $@
	@ echo "--------------------------------------------------------------"

$(OBJ_DIR1)/%.o:c_code/%.asm options.inc
	@ echo "Making object file $@ "
	$(YASM) $(ASMFLAGS) -D MAJOR_VERSION=IGZIP1 $< -o $@
	@ echo "--------------------------------------------------------------"

########################################################################

$(OBJ_DIR1C)/%.o:%.cpp options.h
	@ echo "Making object file $@ "
	$(CXX) -c $(CXXFLAGS) -fPIC -D MAJOR_VERSION=IGZIP1C $< -o $@
	@ echo "--------------------------------------------------------------"

$(OBJ_DIR1C)/%.o:c_code/%.cpp options.h
	@ echo "Making object file $@ "
	$(CXX) -c $(CXXFLAGS) -fPIC -D MAJOR_VERSION=IGZIP1C $< -o $@
	@ echo "--------------------------------------------------------------"

$(OBJ_DIR1C)/%.o:%.asm options.inc
	@ echo "Making object file $@ "
	$(YASM) $(ASMFLAGS) -D MAJOR_VERSION=IGZIP1C $< -o $@
	@ echo "--------------------------------------------------------------"

$(OBJ_DIR1C)/%.o:c_code/%.asm options.inc
	@ echo "Making object file $@ "
	$(YASM) $(ASMFLAGS) -D MAJOR_VERSION=IGZIP1C $< -o $@
	@ echo "--------------------------------------------------------------"

########################################################################

$(SOBJ_DIR0)/%.o:%.cpp options.h
	@ echo "Making object file $@ "
	$(CXX) -c $(CXXFLAGS) -fPIC -D MAJOR_VERSION=IGZIP0 $< -o $@
	@ echo "--------------------------------------------------------------"

$(SOBJ_DIR0)/%.o:c_code/%.cpp options.h
	@ echo "Making object file $@ "
	$(CXX) -c $(CXXFLAGS) -fPIC -D MAJOR_VERSION=IGZIP0 $< -o $@
	@ echo "--------------------------------------------------------------"

$(SOBJ_DIR0)/%.o:%.asm options.inc
	@ echo "Making object file $@ "
	$(YASM) $(ASMFLAGS) -D MAJOR_VERSION=IGZIP0 $< -o $@
	@ echo "--------------------------------------------------------------"

$(SOBJ_DIR0)/%.o:c_code/%.asm options.inc
	@ echo "Making object file $@ "
	$(YASM) $(ASMFLAGS) -D MAJOR_VERSION=IGZIP0 $< -o $@
	@ echo "--------------------------------------------------------------"

########################################################################

$(SOBJ_DIR0C)/%.o:%.cpp options.h
	@ echo "Making object file $@ "
	$(CXX) -c $(CXXFLAGS) -fPIC -D MAJOR_VERSION=IGZIP0C $< -o $@
	@ echo "--------------------------------------------------------------"

$(SOBJ_DIR0C)/%.o:c_code/%.cpp options.h
	@ echo "Making object file $@ "
	$(CXX) -c $(CXXFLAGS) -fPIC -D MAJOR_VERSION=IGZIP0C $< -o $@
	@ echo "--------------------------------------------------------------"

$(SOBJ_DIR0C)/%.o:%.asm options.inc
	@ echo "Making object file $@ "
	$(YASM) $(ASMFLAGS) -D MAJOR_VERSION=IGZIP0C $< -o $@
	@ echo "--------------------------------------------------------------"

$(SOBJ_DIR0C)/%.o:c_code/%.asm options.inc
	@ echo "Making object file $@ "
	$(YASM) $(ASMFLAGS) -D MAJOR_VERSION=IGZIP0C $< -o $@
	@ echo "--------------------------------------------------------------"

########################################################################

$(SOBJ_DIR1)/%.o:%.cpp options.h
	@ echo "Making object file $@ "
	$(CXX) -c $(CXXFLAGS) -fPIC -D MAJOR_VERSION=IGZIP1 $< -o $@
	@ echo "--------------------------------------------------------------"

$(SOBJ_DIR1)/%.o:c_code/%.cpp options.h
	@ echo "Making object file $@ "
	$(CXX) -c $(CXXFLAGS) -fPIC -D MAJOR_VERSION=IGZIP1 $< -o $@
	@ echo "--------------------------------------------------------------"

$(SOBJ_DIR1)/%.o:%.asm options.inc
	@ echo "Making object file $@ "
	$(YASM) $(ASMFLAGS) -D MAJOR_VERSION=IGZIP1 $< -o $@
	@ echo "--------------------------------------------------------------"

$(SOBJ_DIR1)/%.o:c_code/%.asm options.inc
	@ echo "Making object file $@ "
	$(YASM) $(ASMFLAGS) -D MAJOR_VERSION=IGZIP1 $< -o $@
	@ echo "--------------------------------------------------------------"

########################################################################

$(SOBJ_DIR1C)/%.o:%.cpp options.h
	@ echo "Making object file $@ "
	$(CXX) -c $(CXXFLAGS) -fPIC -D MAJOR_VERSION=IGZIP1C $< -o $@
	@ echo "--------------------------------------------------------------"

$(SOBJ_DIR1C)/%.o:c_code/%.cpp options.h
	@ echo "Making object file $@ "
	$(CXX) -c $(CXXFLAGS) -fPIC -D MAJOR_VERSION=IGZIP1C $< -o $@
	@ echo "--------------------------------------------------------------"

$(SOBJ_DIR1C)/%.o:%.asm options.inc
	@ echo "Making object file $@ "
	$(YASM) $(ASMFLAGS) -D MAJOR_VERSION=IGZIP1C $< -o $@
	@ echo "--------------------------------------------------------------"

$(SOBJ_DIR1C)/%.o:c_code/%.asm options.inc
	@ echo "Making object file $@ "
	$(YASM) $(ASMFLAGS) -D MAJOR_VERSION=IGZIP1C $< -o $@
	@ echo "--------------------------------------------------------------"

########################################################################

$(REL_DIR)/%.h:%.h
	cp $< $@

$(REL_DIR)/%.h:../include/%.h
	cp $< $@

$(REL_DIR)/%.h:c_code/%.h
	cp $< $@

$(REL_DIR)/%.a:%.a
	cp $< $@

########################################################################

clean:
	rm -f $(EXE0) $(EXE0C) $(EXE1) $(EXE1C)
	rm -f $(LIB0) $(LIB0C) $(LIB1) $(LIB1C)
	rm -f $(SLIB0) $(SLIB0C) $(SLIB1) $(SLIB1C)
	rm -f $(obj0_files)
	rm -f $(obj0c_files)
	rm -f $(obj1_files)
	rm -f $(obj1c_files)
	rm -f $(lib_sobj0_files)
	rm -f $(lib_sobj0c_files)
	rm -f $(lib_sobj1_files)
	rm -f $(lib_sobj1c_files)
