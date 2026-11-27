# target.mk — shared RP2040 target build for every module project.
# Each project Makefile sets PROJECT, APP, DEPTH then includes this file.
# Produces build-target/$(PROJECT).uf2/.hex/.bin — UF2 flashable via BOOTSEL drag-drop.
TCROOT  := C:/ST/STM32CubeCLT_1.22.0/GNU-tools-for-STM32/bin
CC      := "$(TCROOT)/arm-none-eabi-gcc.exe"
OBJCOPY := "$(TCROOT)/arm-none-eabi-objcopy.exe"
MCUFLAGS := -mcpu=cortex-m0plus -mthumb

# DEPTH — relative path from the project dir back up to projects/
DEPTH   ?= ../
# ROOT — repo root (one above projects/); TGT — shared target infra dir
ROOT    := $(DEPTH)/..
TGT     := $(DEPTH)/target
CFLAGS  := $(MCUFLAGS) -O1 -Wall -fno-common -g \
           -I. -I$(TGT) -I$(ROOT)/drivers -I$(DEPTH)/lib
DEVSRC := $(ROOT)/drivers/devices/dig_in/dig_in.c $(ROOT)/drivers/devices/analog_in/analog_in.c \
          $(ROOT)/drivers/devices/dig_out/dig_out.c $(ROOT)/drivers/devices/pwm_out/pwm_out.c \
          $(ROOT)/drivers/devices/quad_enc/quad_enc.c $(ROOT)/drivers/devices/onewire/onewire.c \
          $(ROOT)/drivers/devices/kit/kit.c \
          $(ROOT)/drivers/peripherals/gpio/rp_gpio.c $(ROOT)/drivers/peripherals/adc/rp_adc.c \
          $(ROOT)/drivers/peripherals/pwm/rp_pwm.c $(ROOT)/drivers/peripherals/timer/rp_timer.c
LIBSRC := $(DEPTH)/lib/blink.c $(DEPTH)/lib/edge_count.c $(DEPTH)/lib/hysteresis.c \
          $(DEPTH)/lib/colorwheel.c $(DEPTH)/lib/sequencer.c $(DEPTH)/lib/peak_detect.c
BOOTSRC := $(TGT)/startup_rp2040.c $(TGT)/boot2/boot2_padded.S
TDIR    := build-target

all: $(TDIR)/$(PROJECT).hex $(TDIR)/$(PROJECT).bin $(TDIR)/$(PROJECT).uf2

$(TDIR):
	mkdir -p $(TDIR)

$(TDIR)/$(PROJECT).elf: main.c $(APP).c $(DEVSRC) $(LIBSRC) $(BOOTSRC) | $(TDIR)
	$(CC) $(CFLAGS) -o $@ main.c $(APP).c $(DEVSRC) $(LIBSRC) $(BOOTSRC) \
	      -T$(TGT)/rp2040_flash.ld -Wl,-Map=$(TDIR)/$(PROJECT).map --specs=nosys.specs

$(TDIR)/$(PROJECT).hex: $(TDIR)/$(PROJECT).elf
	$(OBJCOPY) -O ihex $< $@
$(TDIR)/$(PROJECT).bin: $(TDIR)/$(PROJECT).elf
	$(OBJCOPY) -O binary $< $@
$(TDIR)/$(PROJECT).uf2: $(TDIR)/$(PROJECT).bin
	py -3 $(TGT)/bin2uf2.py $< $@ 0x10000000

clean:
	rm -rf $(TDIR)

.PHONY: all clean
