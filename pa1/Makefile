# Makefile created for users without a working CMake installation
# Building using CMake is preferred

SRC_DIR     = ./src
BUILD_DIR   = ./build
INCLUDE_DIR = ./include

C_COMPILER  = icc
PRINT_DSVS ?= 0
C_FLAGS     = -O3 \
              -DPRINT_DSVS=${PRINT_DSVS} \
			  -Wall \
			  -Wextra \
			  -Wshadow \
			  -pedantic
DEBUG_FLAGS = -DDEBUG -g
LD_FLAGS    = -lrt

OBJECTS = $(BUILD_DIR)/amr.o \
          $(BUILD_DIR)/common.o
HEADERS = $(INCLUDE_DIR)/amr.h \
          $(INCLUDE_DIR)/common.h
TARGETS = amr


all: $(TARGETS)

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: $(TARGETS) report

make_build:
	mkdir -p $(BUILD_DIR)

amr: make_build $(OBJECTS) $(HEADERS)
	$(C_COMPILER) $(C_FLAGS) -o amr $(OBJECTS) $(LD_FLAGS)

.PHONY: report
report: make_build $(SRC_DIR)/report.tex
	cp $(SRC_DIR)/report.tex $(BUILD_DIR)/.;
	cd $(BUILD_DIR) && \
	pdflatex report.tex && \
	mv report.pdf ../.

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(C_COMPILER) $(C_FLAGS) -I$(INCLUDE_DIR) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -f  $(TARGETS) report.pdf
