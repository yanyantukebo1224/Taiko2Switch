# Taiko2Switch - Nintendo Switch homebrew
# Build with devkitPro / devkitA64.

TARGET      := Taiko2Switch
BUILD       := build
SOURCES     := source
DATA        := data
INCLUDES    := include

ARCH        := -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS      := -g -Wall -Wextra -O2 -ffunction-sections $(ARCH) $(DEFINES)
CXXFLAGS    := $(CFLAGS) -std=gnu++17 -fno-rtti -fno-exceptions
ASFLAGS     := $(ARCH)
LDFLAGS     := -specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*).map
LIBS        := -lnx

# Include the devkitPro libnx rules.
include $(DEVKITPRO)/libnx/switch_rules
