#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment.")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

TARGET      := Taiko2Switch
BUILD       := build
SOURCES     := source
DATA        := data
INCLUDES    := include

APP_TITLE   := Taiko2Switch
APP_AUTHOR  := Taiko2Switch contributors
APP_VERSION := 0.1.0

ARCH        := -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS      := -g -Wall -Wextra -O2 -ffunction-sections $(ARCH) $(DEFINES)
CFLAGS      += $(INCLUDE) -D__SWITCH__
CXXFLAGS    := $(CFLAGS) -std=gnu++17 -fno-rtti -fno-exceptions
ASFLAGS     := -g $(ARCH)
LDFLAGS     = -specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*).map

LIBS        := -lnx
LIBDIRS     := $(PORTLIBS) $(LIBNX)
