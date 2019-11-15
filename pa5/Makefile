# Makefile for building

SRC_DIR     = ./src
BUILD_DIR   = ./build
INCLUDE_DIR = ./include

C_COMPILER  = icc
C_FLAGS     = -O3 \
              -qopenmp \
              -DCACHE_LINE=$(shell getconf LEVEL1_DCACHE_LINESIZE) \
              -Wall \
              -Wextra \
              -Wshadow \
              -pedantic
DEBUG_FLAGS = -g
LD_FLAGS    = -lrt -qopenmp

MPI_COMPILER = mpicc
MPI_FLAGS    = $(C_FLAGS)

OBJECTS = $(BUILD_DIR)/common.o \
          $(BUILD_DIR)/amr.o
HEADERS = $(INCLUDE_DIR)/amr.h \
          $(INCLUDE_DIR)/common.h
TARGETS = lab5_mpi


all: $(TARGETS)

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: $(TARGETS)

make_build:
	mkdir -p $(BUILD_DIR)

lab5_mpi: make_build $(OBJECTS) $(HEADERS)
	$(MPI_COMPILER) $(MPI_FLAGS) -o $@ $(OBJECTS) $(LD_FLAGS)

.PHONY: report
report: make_build $(SRC_DIR)/report.tex
	cp $(SRC_DIR)/report.tex $(BUILD_DIR)/.;
	cd $(BUILD_DIR) && \
	pdflatex report.tex && \
	mv report.pdf ../.

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(MPI_COMPILER) $(MPI_FLAGS) -I$(INCLUDE_DIR) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -f  $(TARGETS) report.pdf
