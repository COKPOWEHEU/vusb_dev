obj = main usbdrv usbdrvasm
mcu = atmega8
F_CPU = 12000000

CFLAGS = -mmcu=$(mcu) -Wall -gdwarf-2 -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -DF_CPU=$(F_CPU)UL -std=gnu99
CFLAGS += -MD -MP -MT $(*F).o -MF build/dep/$(@F).d

CXXFLAGS = $(CFLAGS)

ASMFLAGS = $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

LDFLAGS = -mmcu=$(mcu)
LDFLAGS +=  -Wl,-Map=build/firmware.map

HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings

INCLUDES =

objects = $(addprefix build/,$(addsuffix .o,$(obj)))

all: res/firmware.hex res/firmware.eep res/firmware.lss size

res/%.elf: $(objects)
	mkdir -p res
	avr-gcc $(objects) $(LDFLAGS) -o res/firmware.elf
res/%.hex: res/%.elf
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@
res/%.eep: res/%.elf
	avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0
res/%.lss: res/%.elf
	avr-objdump -h -S $< > $@
size: res/firmware.elf
	avr-size -C --mcu=$(mcu) $<
clean:
	rm -f $(objects)
	rm -rf res
	rm -rf build
build/%.o: src/%.c
	mkdir -p build
	avr-gcc -c $(CFLAGS) $< -o $@
build/%.o: src/%.cpp
	mkdir -p build
	avr-g++ -c $(CXXFLAGS) $< -o $@
build/%.o: src/%.S
	mkdir -p build
	avr-gcc -c $(ASMFLAGS) $< -o $@
prog: res/firmware.hex
	avrdude -c stk500v2 -P avrdoper -p $(mcu) -V -U flash:w:res/firmware.hex:i
prog2: res/firmware.hex
	../bootloader/software/res/prog64 -t /dev/ttyUSB0 -b 115200 -i res/firmware.hex
reset:
	avrdude -c stk500v2 -P avrdoper -p $(mcu) -U lfuse:r:/dev/null:i

-include $(shell mkdir -p build/dep) $(wildcard build/dep/*)
