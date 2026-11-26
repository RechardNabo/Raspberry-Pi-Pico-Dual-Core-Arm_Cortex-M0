# Makefile — native gcc mirror of the Ceedling suite. No Ruby needed.
CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -std=c99 -g -DUNIT_TEST_HOST
UNITY   := vendor/unity/unity.c
SUPPORT := unit/support/mock_sfr.c
INC     := -Ivendor/unity -Iunit/support -I..

MODULES := gpio uart spi i2c pwm adc
DEVICES := dig_in analog_in dig_out
BINS    := $(MODULES:%=build/test_rp_%) $(DEVICES:%=build/test_%) build/test_pwm_out build/test_quad_enc build/test_timer build/test_onewire build/test_kit build/test_climate_station build/test_security_alarm

all: $(BINS) run

build:
	mkdir -p build

build/test_rp_%: unit/test_rp_%.c $(UNITY) $(SUPPORT) | build
	$(CC) $(CFLAGS) $(INC) -o $@ $< ../peripherals/$*/rp_$*.c $(UNITY) $(SUPPORT)

build/test_dig_in: unit/test_dig_in.c $(UNITY) $(SUPPORT) | build
	$(CC) $(CFLAGS) $(INC) -o $@ $< ../devices/dig_in/dig_in.c ../peripherals/gpio/rp_gpio.c $(UNITY) $(SUPPORT)

build/test_analog_in: unit/test_analog_in.c $(UNITY) $(SUPPORT) | build
	$(CC) $(CFLAGS) $(INC) -o $@ $< ../devices/analog_in/analog_in.c ../peripherals/adc/rp_adc.c $(UNITY) $(SUPPORT)

build/test_dig_out: unit/test_dig_out.c $(UNITY) $(SUPPORT) | build
	$(CC) $(CFLAGS) $(INC) -o $@ $< ../devices/dig_out/dig_out.c ../peripherals/gpio/rp_gpio.c $(UNITY) $(SUPPORT)


build/test_pwm_out: unit/test_pwm_out.c $(UNITY) $(SUPPORT) | build
	$(CC) $(CFLAGS) $(INC) -o $@ $< ../devices/pwm_out/pwm_out.c ../peripherals/pwm/rp_pwm.c $(UNITY) $(SUPPORT)

build/test_quad_enc: unit/test_quad_enc.c $(UNITY) $(SUPPORT) | build
	$(CC) $(CFLAGS) $(INC) -o $@ $< ../devices/quad_enc/quad_enc.c ../peripherals/gpio/rp_gpio.c $(UNITY) $(SUPPORT)

build/test_timer: unit/test_timer.c $(UNITY) $(SUPPORT) | build
	$(CC) $(CFLAGS) $(INC) -o $@ $< ../peripherals/timer/rp_timer.c $(UNITY) $(SUPPORT)

build/test_onewire: unit/test_onewire.c $(UNITY) $(SUPPORT) | build
	$(CC) $(CFLAGS) $(INC) -o $@ $< ../devices/onewire/onewire.c ../peripherals/gpio/rp_gpio.c ../peripherals/timer/rp_timer.c $(UNITY) $(SUPPORT)


build/test_kit: unit/test_kit.c $(UNITY) $(SUPPORT) | build
	$(CC) $(CFLAGS) $(INC) -o $@ $< ../devices/kit/kit.c ../devices/dig_in/dig_in.c ../devices/analog_in/analog_in.c ../devices/dig_out/dig_out.c ../devices/pwm_out/pwm_out.c ../devices/quad_enc/quad_enc.c ../devices/onewire/onewire.c ../peripherals/gpio/rp_gpio.c ../peripherals/adc/rp_adc.c ../peripherals/pwm/rp_pwm.c ../peripherals/timer/rp_timer.c $(UNITY) $(SUPPORT)

build/test_climate_station: unit/test_climate_station.c $(UNITY) $(SUPPORT) | build
	$(CC) $(CFLAGS) $(INC) -o $@ $< ../apps/climate_station/climate_station.c ../devices/kit/kit.c ../devices/dig_in/dig_in.c ../devices/analog_in/analog_in.c ../devices/dig_out/dig_out.c ../devices/pwm_out/pwm_out.c ../devices/quad_enc/quad_enc.c ../devices/onewire/onewire.c ../peripherals/gpio/rp_gpio.c ../peripherals/adc/rp_adc.c ../peripherals/pwm/rp_pwm.c ../peripherals/timer/rp_timer.c $(UNITY) $(SUPPORT)

build/test_security_alarm: unit/test_security_alarm.c $(UNITY) $(SUPPORT) | build
	$(CC) $(CFLAGS) $(INC) -o $@ $< ../apps/security_alarm/security_alarm.c $(UNITY) $(SUPPORT)

run: $(BINS)
	@total=0; fail=0; \
	for b in $(BINS); do \
		out=$$(./$$b); echo "$$out" | tail -1; \
		n=$$(echo "$$out" | grep -oE '^[0-9]+ Tests' | grep -oE '^[0-9]+'); \
		f=$$(echo "$$out" | grep -oE '[0-9]+ Failures' | grep -oE '^[0-9]+'); \
		total=$$((total+n)); fail=$$((fail+f)); \
	done; \
	echo "== mirror total: $$total tests, $$fail failures =="; \
	test $$fail -eq 0

clean:
	rm -rf build

.PHONY: all run clean
