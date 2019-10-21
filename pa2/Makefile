# Makefile for building
#   - amr executable
#   - lab report

SRC_DIR     = ./src
BUILD_DIR   = ./build
INCLUDE_DIR = ./include

C_COMPILER  = icc
C_FLAGS     = -O3 \
			  -Wall \
			  -Wextra \
			  -Wshadow \
			  -pedantic
DEBUG_FLAGS = -DDEBUG -g
LD_FLAGS    = -lrt -pthread

OBJECTS = $(BUILD_DIR)/common.o
HEADERS = $(INCLUDE_DIR)/amr.h \
          $(INCLUDE_DIR)/common.h
TARGETS = disposable persistent


all: $(TARGETS)

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: $(TARGETS) report

make_build:
	mkdir -p $(BUILD_DIR)

disposable: make_build $(BUILD_DIR)/lehman_caleb_disposable.o $(OBJECTS) $(HEADERS)
	$(C_COMPILER) $(C_FLAGS) -o $@ $(BUILD_DIR)/lehman_caleb_disposable.o $(OBJECTS) $(LD_FLAGS)

persistent: make_build $(BUILD_DIR)/lehman_caleb_persistent.o $(OBJECTS) $(HEADERS)
	$(C_COMPILER) $(C_FLAGS) -o $@ $(BUILD_DIR)/lehman_caleb_persistent.o $(OBJECTS) $(LD_FLAGS)

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
