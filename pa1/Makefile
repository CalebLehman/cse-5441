# Makefile created for users without a working CMake installation
# Building using CMake is preferred

SRC_DIR     = ./src
BUILD_DIR   = ./build
INCLUDE_DIR = ./include

C_COMPILER  = icc
C_FLAGS     = -O3
DEBUG_FLAGS = -DDEBUG -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic
LD_FLAGS    = 

OBJECTS = $(BUILD_DIR)/amr.o $(BUILD_DIR)/common.o
HEADERS = $(INCLUDE_DIR)/amr.h $(INCLUDE_DIR)/common.h
TARGET  = amr


all: $(TARGET)

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

make_build:
	mkdir -p $(BUILD_DIR)

$(TARGET): make_build $(OBJECTS) $(HEADERS)
	$(C_COMPILER) $(C_FLAGS) -o $(TARGET) $(OBJECTS) $(LD_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(C_COMPILER) $(C_FLAGS) -I$(INCLUDE_DIR) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -f  $(TARGET)
