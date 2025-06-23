# Detect OS
ifeq ($(OS),Windows_NT)
	RM = del /F /Q
	RMDIR = rmdir /S /Q
	MKDIR = mkdir
	EXE = .exe
else
	RM = rm -f
	RMDIR = rm -rf
	MKDIR = mkdir -p
	EXE =
endif

CXX = g++
CXXFLAGS = -Wall -std=c++11 -Iinclude

# Directories
OBJ_DIR := obj
BIN_DIR := bin
# Ensure directories exist
$(shell $(MKDIR) $(OBJ_DIR) $(BIN_DIR))

# Source files
COMMON_SRCS  := src/tlb.cpp src/memory_manager.cpp
SINGLE_SRCS  := src/io.cpp src/task.cpp
MULTI_SRCS   := src/iomulti.cpp src/taskmulti.cpp
TEST_SRC     := test.cpp

# Object files
COMMON_OBJS  := $(COMMON_SRCS:src/%.cpp=$(OBJ_DIR)/%.o)
SINGLE_OBJS  := $(SINGLE_SRCS:src/%.cpp=$(OBJ_DIR)/%.o)
MULTI_OBJS   := $(MULTI_SRCS:src/%.cpp=$(OBJ_DIR)/%.o)
TEST_OBJ     := $(OBJ_DIR)/test.o

# Executables in bin/
SINGLE_EXE   := $(BIN_DIR)/single_pagetable$(EXE)
MULTI_EXE    := $(BIN_DIR)/multilevel_pagetable$(EXE)
TEST_EXE     := $(BIN_DIR)/test$(EXE)

# Default target (multilevel)
default: $(MULTI_EXE)

# Build single-level executable
single: $(SINGLE_EXE)

$(SINGLE_EXE): $(COMMON_OBJS) $(SINGLE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build multilevel executable
$(MULTI_EXE): $(COMMON_OBJS) $(MULTI_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build test trace generator
$(TEST_EXE): $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile rule for project sources
$(OBJ_DIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile rule for test.cpp
$(OBJ_DIR)/test.o: $(TEST_SRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Trace generation (runs test executable)
trace: $(TEST_EXE)
	@echo "Generating new trace file..."
	@$(TEST_EXE)

# Clean up
clean:
	@$(RMDIR) $(OBJ_DIR)
	@$(RMDIR) $(BIN_DIR)
	@$(RM) $(SINGLE_EXE) $(MULTI_EXE) $(TEST_EXE)

# Help
help:
	@echo "Available targets:"
	@echo "  default          - Build multilevel page table (default)"
	@echo "  single           - Build single-level page table"
	@echo "  trace            - Generate new trace file"
	@echo "  clean            - Remove obj/bin directories and executables"
	@echo "  help             - Show this help message"

.PHONY: default single trace clean help
