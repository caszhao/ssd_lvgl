#
# Makefile
#
# LVGL 7.11 + SSD202X
#
# Default:
#   make
#
# Build:
#   liblvgl.so
#   test_lvgl
#
# Other:
#   make lvgl
#   make test
#   make app
#   make clean
#

PWD := $(shell pwd)


# ============================================================
# Build
# ============================================================

BUILD_DIR ?= build_lvgl

# Original application
BIN := $(BUILD_DIR)/demossd

# LVGL shared library
LVGL_SO := $(BUILD_DIR)/liblvgl.so

# LVGL test application
TEST_BIN := $(BUILD_DIR)/test_lvgl
TEST_SRC := $(PWD)/test/main.c
TEST_OBJ := $(BUILD_DIR)/test/main.o


# ============================================================
# Compiler
# ============================================================

CC := arm-linux-gnueabihf-gcc


# ============================================================
# SDK
# ============================================================

SDK_LIBS := ../project/release/nvr/i2m/common/glibc/8.2.1/mi_libs/dynamic

SDK_LIBS_STATIC := $(patsubst %dynamic,%static,$(SDK_LIBS))


# ============================================================
# Platform libraries
# ============================================================

PLAT_DEPENDENT_LIB := \
	-lmi_gfx \
	-lmi_sys \
	-lmi_common \
	-ldl

PLAT_DEPENDENT_LIB_EX := \
	-lmi_disp \
	-lmi_panel


# ============================================================
# MiniGUI libraries
# ============================================================

MINIGUI_LIBS := \
	-ljpeg \
	-lz


# ============================================================
# SDK include
# ============================================================

SDK_INCS := ../project/release/include

UAPI_INCS := \
	../project/kbuild/4.9.84/i2m/include/uapi/mstar

INCLUDES := ./common


# ============================================================
# Library search paths
# ============================================================

LDFLAGS += -L$(SDK_LIBS)
LDFLAGS += -L$(SDK_LIBS_STATIC)
LDFLAGS += -L$(PWD)/lib
LDFLAGS += -L/usr/local/lib


# ============================================================
# Standard libraries
# ============================================================

STDLIBS := \
	-lstdc++ \
	-ldl \
	-lpthread \
	-lm


# ============================================================
# Application libraries
# ============================================================

LIBS := \
	-Wl,-Bdynamic \
	$(MINIGUI_LIBS) \
	$(PLAT_DEPENDENT_LIB_EX) \
	$(PLAT_DEPENDENT_LIB) \
	$(STDLIBS)

LDFLAGS += $(LIBS)


# ============================================================
# Source directories
# ============================================================

LVGL_DIR := $(PWD)/lvgl

DRIVER_DIR := $(PWD)/lv_drivers

EXAMPLE_DIR := $(PWD)/lv_examples


# ============================================================
# Include paths
# ============================================================

CFLAGS += -I.
CFLAGS += -I$(PWD)
CFLAGS += -I$(PWD)/inc
CFLAGS += -I$(INCLUDES)

CFLAGS += -I$(SDK_INCS)
CFLAGS += -I$(UAPI_INCS)

CFLAGS += -I$(LVGL_DIR)
CFLAGS += -I$(DRIVER_DIR)
CFLAGS += -I$(EXAMPLE_DIR)


# ============================================================
# Compiler flags
# ============================================================

CFLAGS += -rdynamic
CFLAGS += -funwind-tables

CFLAGS += -O3
CFLAGS += -g0

CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wshadow
CFLAGS += -Wundef
CFLAGS += -Wmaybe-uninitialized
CFLAGS += -Wmissing-prototypes
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-error=strict-prototypes
CFLAGS += -Wpointer-arith
CFLAGS += -fno-strict-aliasing
CFLAGS += -Wno-error=cpp
CFLAGS += -Wuninitialized
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-missing-field-initializers
CFLAGS += -Wtype-limits
CFLAGS += -Wsizeof-pointer-memaccess
CFLAGS += -Wno-format-nonliteral
CFLAGS += -Wno-cast-qual
CFLAGS += -Wunreachable-code
CFLAGS += -Wno-switch-default
CFLAGS += -Wno-switch-enum
CFLAGS += -Wreturn-type
CFLAGS += -Wmultichar
CFLAGS += -Wformat-security
CFLAGS += -Wno-ignored-qualifiers
CFLAGS += -Wno-error=pedantic
CFLAGS += -Wno-sign-compare
CFLAGS += -Wdouble-promotion
CFLAGS += -Wclobbered
CFLAGS += -Wdeprecated
CFLAGS += -Wempty-body
CFLAGS += -Wstack-usage=1024
CFLAGS += -Wno-unused-value


# ============================================================
# Source files
# ============================================================

MAINSRC := $(PWD)/main.c


# ============================================================
# LVGL sources
# ============================================================

LVGL_SRCS := $(shell \
	find $(LVGL_DIR) \
	-type f \
	-name '*.c')


# ============================================================
# LVGL Driver sources
# ============================================================

DRIVER_SRCS := $(shell \
	find $(DRIVER_DIR) \
	-type f \
	-name '*.c')


# ============================================================
# LVGL Example sources
# ============================================================

EXAMPLE_SRCS := $(shell \
	find $(EXAMPLE_DIR) \
	-type f \
	-name '*.c')


# ============================================================
# Normal object files
# ============================================================

LVGL_OBJS := \
	$(patsubst $(PWD)/%.c,$(BUILD_DIR)/%.o,$(LVGL_SRCS))

DRIVER_OBJS := \
	$(patsubst $(PWD)/%.c,$(BUILD_DIR)/%.o,$(DRIVER_SRCS))

EXAMPLE_OBJS := \
	$(patsubst $(PWD)/%.c,$(BUILD_DIR)/%.o,$(EXAMPLE_SRCS))

MAINOBJ := $(BUILD_DIR)/main.o


# ============================================================
# Original application objects
# ============================================================

APP_OBJS := \
	$(LVGL_OBJS) \
	$(DRIVER_OBJS) \
	$(EXAMPLE_OBJS) \
	$(MAINOBJ)


# ============================================================
# LVGL PIC objects
#
# Used to build liblvgl.so
# ============================================================

LVGL_PIC_OBJS := \
	$(patsubst $(PWD)/%.c,$(BUILD_DIR)/pic/%.o,$(LVGL_SRCS))


# ============================================================
# Targets
# ============================================================

.PHONY: all
.PHONY: lvgl
.PHONY: test
.PHONY: app
.PHONY: clean


# ============================================================
# Default target
#
# make
#
# will build:
#
#   liblvgl.so
#   test_lvgl
#
# ============================================================

all: lvgl test


# ============================================================
# Build LVGL shared library
# ============================================================

lvgl: $(LVGL_SO)


# ============================================================
# Build LVGL test
#
# This automatically depends on liblvgl.so
# ============================================================

test: $(TEST_BIN)


# ============================================================
# Build original application
# ============================================================

app: $(BIN)


# ============================================================
# Original application
# ============================================================

$(BIN): $(APP_OBJS)

	@mkdir -p $(dir $@)

	@echo ""
	@echo "=========================================="
	@echo " LINK APPLICATION"
	@echo " $@"
	@echo "=========================================="

	@$(CC) \
		-o $@ \
		$(APP_OBJS) \
		$(LDFLAGS)


# ============================================================
# Compile normal C
#
# Example:
#
# lvgl/src/xxx.c
#
# ->
#
# build_lvgl/lvgl/src/xxx.o
#
# ============================================================

$(BUILD_DIR)/%.o: $(PWD)/%.c

	@mkdir -p $(dir $@)

	@echo "CC  $<"

	@$(CC) \
		$(CFLAGS) \
		-c $< \
		-o $@


# ============================================================
# Compile original main.c
# ============================================================

$(MAINOBJ): $(MAINSRC)

	@mkdir -p $(dir $@)

	@echo "CC  $<"

	@$(CC) \
		$(CFLAGS) \
		-c $< \
		-o $@


# ============================================================
# Compile LVGL PIC
#
# Example:
#
# lvgl/src/xxx.c
#
# ->
#
# build_lvgl/pic/lvgl/src/xxx.o
#
# ============================================================

$(BUILD_DIR)/pic/%.o: $(PWD)/%.c

	@mkdir -p $(dir $@)

	@echo "CC-PIC  $<"

	@$(CC) \
		$(CFLAGS) \
		-fPIC \
		-c $< \
		-o $@


# ============================================================
# Build liblvgl.so
# ============================================================

$(LVGL_SO): $(LVGL_PIC_OBJS)

	@mkdir -p $(dir $@)

	@echo ""
	@echo "=========================================="
	@echo " LINK LVGL SHARED LIBRARY"
	@echo " $@"
	@echo "=========================================="

	@$(CC) \
		-shared \
		-Wl,-soname,liblvgl.so \
		-o $@ \
		$(LVGL_PIC_OBJS) \
		-lm \
		-ldl \
		-lpthread


# ============================================================
# Compile LVGL test application
#
# test/main.c
#
# ->
#
# build_lvgl/test/main.o
#
# ============================================================

$(TEST_OBJ): $(TEST_SRC)

	@mkdir -p $(dir $@)

	@echo "CC  $<"

	@$(CC) \
		$(CFLAGS) \
		-c $< \
		-o $@


# ============================================================
# Link LVGL test application
#
# test_lvgl
#     |
#     +---- liblvgl.so
#                |
#                +---- LVGL 7.11
#
# custom_tick_get()
# is implemented in test/main.c
#
# ============================================================

$(TEST_BIN): $(TEST_OBJ) $(LVGL_SO)

	@mkdir -p $(dir $@)

	@echo ""
	@echo "=========================================="
	@echo " LINK LVGL TEST APPLICATION"
	@echo " $@"
	@echo "=========================================="

	@$(CC) \
		-o $@ \
		$(TEST_OBJ) \
		-L$(BUILD_DIR) \
		-Wl,-rpath,'$$ORIGIN' \
		-Wl,--no-as-needed \
		-llvgl \
		-lm \
		-ldl \
		-lpthread


# ============================================================
# Clean
# ============================================================

clean:

	@echo "CLEAN $(BUILD_DIR)"

	@rm -rf $(BUILD_DIR)
