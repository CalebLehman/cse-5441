# Makefile for building
#   - amr executable
#   - lab report

SRC_DIR     = ./src
BUILD_DIR   = ./build
INCLUDE_DIR = ./include

C_COMPILER  = icc
C_FLAGS     = -O3 \
              -DCACHE_LINE=$(shell getconf LEVEL1_DCACHE_LINESIZE) \
              -Wall \
              -Wextra \
              -Wshadow \
              -pedantic
DEBUG_FLAGS = -g
LD_FLAGS    = -lrt -lcuda -lcudart
OTHER_INCS  = -I${CUDA_HOME}/include
OTHER_LIBS  = -L${CUDA_HOME}/lib64

CUDA_COMPILER = nvcc
CUDA_FLAGS    = -O3

OBJECTS = $(BUILD_DIR)/amr.o \
          $(BUILD_DIR)/common.o
HEADERS = $(INCLUDE_DIR)/amr.h \
          $(INCLUDE_DIR)/common.h \
          $(INCLUDE_DIR)/kernel.h
TARGETS = cuda-disposable cuda-persistent


all: $(TARGETS)

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: $(TARGETS)

make_build:
	mkdir -p $(BUILD_DIR)

cuda-disposable: make_build $(BUILD_DIR)/kernel_disposable.co $(OBJECTS) $(HEADERS)
	$(C_COMPILER) $(C_FLAGS) -o $@ $(BUILD_DIR)/kernel_disposable.co $(OBJECTS) $(OTHER_LIBS) $(LD_FLAGS)

cuda-persistent: make_build $(BUILD_DIR)/kernel_persistent.co $(OBJECTS) $(HEADERS)
	$(C_COMPILER) $(C_FLAGS) -o $@ $(BUILD_DIR)/kernel_persistent.co $(OBJECTS) $(OTHER_LIBS) $(LD_FLAGS)

.PHONY: report
report: make_build $(SRC_DIR)/report.tex
	cp $(SRC_DIR)/report.tex $(BUILD_DIR)/.;
	cd $(BUILD_DIR) && \
	pdflatex report.tex && \
	mv report.pdf ../.

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(C_COMPILER) $(C_FLAGS) -I$(INCLUDE_DIR) $(OTHER_INCS) -o $@ -c $<

$(BUILD_DIR)/%.co: $(SRC_DIR)/%.cu $(HEADERS)
	$(CUDA_COMPILER) $(CUDA_FLAGS) -I$(INCLUDE_DIR) $(OTHER_INCS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -f  $(TARGETS) report.pdf
