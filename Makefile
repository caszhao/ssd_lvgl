#
# Makefile
#
PWD=`pwd`
SDK_LIBS=../project/release/nvr/i2m/common/glibc/8.2.1/mi_libs/dynamic
SDK_LIBS_STATIC=$(patsubst %dynamic,%static,$(SDK_LIBS))
PLAT_DEPENDENT_LIB=-lmi_gfx -lmi_sys -lmi_common -ldl 
PLAT_DEPENDENT_LIB_EX= -lmi_disp -lmi_panel 
MINIGUI_LIBS= -ljpeg -lz 
SDK_INCS=../project/release/include
UAPI_INCS=../project/kbuild/4.9.84/i2m/include/uapi/mstar
INCLUDES=./common
LDFLAGS+=-L$(SDK_LIBS)  -L$(SDK_LIBS_STATIC)
LDFLAGS+=-L$(PWD)/lib -L/usr/local/lib
STDLIBS:=  -lstdc++ -ldl  -lpthread -lm 
LIBS:=-Wl,-Bdynamic ${MINIGUI_LIBS} ${PLAT_DEPENDENT_LIB_EX} ${PLAT_DEPENDENT_LIB} -Wl,-Bdynamic $(STDLIBS)
#CFLAGS=-rdynamic -funwind-tables -I. -I$(SDK_INCS) -I$(PWD)/inc -I$(UAPI_INCS) -I$(INCLUDES)
#-I$(PWD)/library/include -I$(PWD)/library/include/ctrl -I$(PWD)/library/include/libpng16/ -I/usr/local/include -I$(UAPI_INCS)

CC := arm-linux-gnueabihf-gcc
#CC ?= gcc
LVGL_DIR_NAME ?= lvgl
LVGL_DIR ?= ${shell pwd}
CFLAGS += -rdynamic -funwind-tables -I. -I$(SDK_INCS) -I$(PWD)/inc -I$(UAPI_INCS) -I$(INCLUDES) -O3 -g0 -I$(LVGL_DIR)/ -Wall -Wshadow -Wundef -Wmaybe-uninitialized -Wmissing-prototypes -Wall -Wextra -Wno-unused-function -Wundef -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wno-switch-enum -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body -Wtype-limits -Wstack-usage=1024 -Wno-unused-value -Wno-unused-parameter -Wno-missing-field-initializers -Wuninitialized -Wmaybe-uninitialized -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wpointer-arith -Wno-cast-qual -Wmissing-prototypes -Wunreachable-code -Wno-switch-default -Wswitch-enum -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-sign-compare 
LDFLAGS += $(LIBS)

BIN = demossd


#Collect the files to compile
MAINSRC = ./main.c

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk
include $(LVGL_DIR)/lv_examples/lv_examples.mk

OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))

MAINOBJ = $(MAINSRC:.c=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS) $(MAINSRC)
OBJS = $(AOBJS) $(COBJS)

## MAINOBJ -> OBJFILES

all: default

%.o: %.c
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"
    
default: $(AOBJS) $(COBJS) $(MAINOBJ)
	$(CC) -o $(BIN) $(MAINOBJ) $(AOBJS) $(COBJS) $(LDFLAGS)

clean: 
	rm -f $(BIN) $(AOBJS) $(COBJS) $(MAINOBJ)

