# Name of the final executable
MAIN = main
LIB = yamalloc
STATIC_LIB_NAME = lib$(LIB).a
DYNAMIC_LIB_NAME = lib$(LIB).so

# Source directory
SRC_DIR = src

# Include directory
INC_DIR = include

# Build type. Values: debug, release
BUILD = debug

# Target directory for the final executable
TARGET_DIR = target
# Destination of the final executable
OUT_DIR = $(TARGET_DIR)/$(BUILD)

# Compiler directory
LIB_SRC_DIR = $(SRC_DIR)/lib
LIB_INC_DIR = $(INC_DIR)/lib
LIB_OUT_DIR = $(OUT_DIR)/$(LIB_SRC_DIR)
OUT_STATIC_LIB_DIR = $(LIB_OUT_DIR)/static_lib
OUT_DYNAMIC_LIB_DIR = $(LIB_OUT_DIR)/dynamic_lib
LIB_CFILES = $(wildcard $(LIB_SRC_DIR)/*.c)
LIB_HFILES = $(wildcard $(LIB_INC_DIR)/*.h)
LIB_OFILES = $(patsubst $(LIB_SRC_DIR)/%.c, $(LIB_OUT_DIR)/%.o, $(LIB_CFILES))

# CLI directory
CLI_SRC_DIR = $(SRC_DIR)/cli
CLI_INC_DIR = $(INC_DIR)/cli
CLI_OUT_DIR = $(OUT_DIR)/$(CLI_SRC_DIR)
CLI_OUT_STATIC_DIR = $(CLI_OUT_DIR)/static
CLI_OUT_DYNAMIC_DIR = $(CLI_OUT_DIR)/dynamic
CLI_CFILES = $(wildcard $(CLI_SRC_DIR)/*.c)
CLI_HFILES = $(wildcard $(CLI_INC_DIR)/*.h)
CLI_OFILES = $(patsubst $(CLI_SRC_DIR)/%.c, $(CLI_OUT_DIR)/%.o, $(CLI_CFILES))

# Compiler
CC = gcc
# Compiler flags
CFLAGS = -Wall -Wextra -Werror -Wpedantic -std=c99
# Linker flags
LDFLAGS = -lm

# Compiler flags for debug
CFLAGS_DEBUG = -g -O0 -DDEBUG
# Compiler flags for release
CFLAGS_RELEASE = -O3 -DNDEBUG

# Set the compiler flags according to the build type
ifeq ($(BUILD), debug)
	CFLAGS += $(CFLAGS_DEBUG)
else ifeq ($(BUILD), release)
	CFLAGS += $(CFLAGS_RELEASE)
endif

# ============================================================================
# Link with .o files
all: comp_lib comp_cli link_cli

comp_lib: $(LIB_OFILES)
comp_cli: $(CLI_OFILES)
link_cli: $(CLI_OUT_DIR)/$(MAIN)
# =========== Compile Lib ==========
$(LIB_OUT_DIR):
	mkdir -p $(LIB_OUT_DIR)

$(LIB_OUT_DIR)%.o: $(LIB_SRC_DIR)/%.c $(LIB_HFILES) | $(LIB_OUT_DIR)
	$(CC) $(CFLAGS) -I$(LIB_INC_DIR) -c $< -o $@
# =========== Compile CLI ==========
$(CLI_OUT_DIR):
	mkdir -p $(CLI_OUT_DIR)

$(CLI_OUT_DIR)%.o: $(CLI_SRC_DIR)/%.c $(CLI_HFILES) | $(CLI_OUT_DIR)
	$(CC) $(CFLAGS) -I$(LIB_INC_DIR) -I$(CLI_INC_DIR) -c $< -o $@
# =========== Link CLI ==========
$(CLI_OUT_DIR)/$(MAIN): $(CLI_OFILES) $(LIB_OFILES)
	$(CC) $(CFLAGS) -o $@ $(CLI_OFILES) $(LIB_OFILES) $(LDFLAGS)
# ============================================================================

# ============================================================================
# ========== Link with dynamic library ==========
# You need to export the path of the dynamic library:
# export LD_LIBRARY_PATH=$(OUT_DYNAMIC_LIB_DIR):$LD_LIBRARY_PATH
comp_cli_dynamic: comp_lib comp_as_dyn_lib link_cli_dyn

comp_as_dyn_lib: $(OUT_DYNAMIC_LIB_DIR)/$(DYNAMIC_LIB_NAME)
link_cli_dyn: $(CLI_OUT_DYNAMIC_DIR)/$(MAIN)

# ========= Compile Library ==========
$(OUT_DYNAMIC_LIB_DIR):
	mkdir -p $(OUT_DYNAMIC_LIB_DIR)

$(OUT_DYNAMIC_LIB_DIR)/$(DYNAMIC_LIB_NAME): $(LIB_OFILES) | $(OUT_DYNAMIC_LIB_DIR)
	$(CC) -shared -o $@ $^

# ========= Link CLI with dynamic library ==========
$(CLI_OUT_DYNAMIC_DIR):
	mkdir -p $(CLI_OUT_DYNAMIC_DIR)

$(CLI_OUT_DYNAMIC_DIR)/$(MAIN): $(OUT_DYNAMIC_LIB_DIR)/$(DYNAMIC_LIB_NAME) $(CLI_OFILES) | $(CLI_OUT_DYNAMIC_DIR)
	$(CC) $(CFLAGS) -I$(LIB_INC_DIR) -I$(CLI_INC_DIR) -o $@ $(CLI_CFILES) $(LDFLAGS) -L$(OUT_DYNAMIC_LIB_DIR) -l$(LIB)

# ============================================================================

# ============================================================================
# ========== Link with static library ==========
comp_cli_static: comp_lib comp_as_static_lib link_cli_static

comp_as_static_lib: $(OUT_STATIC_LIB_DIR)/$(STATIC_LIB_NAME)
link_cli_static: $(CLI_OUT_STATIC_DIR)/$(MAIN)

# ========= Compile Library ==========
$(OUT_STATIC_LIB_DIR):
	mkdir -p $(OUT_STATIC_LIB_DIR)

$(OUT_STATIC_LIB_DIR)/$(STATIC_LIB_NAME): $(LIB_OFILES) | $(OUT_STATIC_LIB_DIR)
	ar rcs $@ $^

# ========= Link CLI with static library ==========
$(CLI_OUT_STATIC_DIR):
	mkdir -p $(CLI_OUT_STATIC_DIR)

$(CLI_OUT_STATIC_DIR)/$(MAIN): $(OUT_STATIC_LIB_DIR)/$(STATIC_LIB_NAME) $(CLI_OFILES) | $(CLI_OUT_STATIC_DIR)
	$(CC) $(CFLAGS) -I$(LIB_INC_DIR) -I$(CLI_INC_DIR) -o $@ $(CLI_CFILES) $(LDFLAGS) -L$(OUT_STATIC_LIB_DIR) -l$(LIB)

# $(OUT_DIR)/$(STATIC_LIB_NAME): $(LIB_OFILES)
# 	ar rcs $@ $^

# $(CLI_OUT_STATIC_DIR)/$(LIB): $(OUT_DIR)/$(STATIC_LIB_NAME) $(CLI_OFILES) | $(CLI_OUT_STATIC_DIR)
# 	$(CC) $(CFLAGS) -I$(LIB_INC_DIR) -I$(CLI_INC_DIR) -o $@ $(CLI_CFILES) $(LDFLAGS) -L$(OUT_DIR) -l$(LIB)
# ============================================================================

clean:
	rm -rf $(TARGET_DIR)

