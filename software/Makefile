
all: monitor.txt

MONITOR_OBJS = \
	src/libc/strlen.rel \
	src/libc/strcmp.rel \
	src/libc/atoi.rel \
	src/libc/itoa.rel \
	src/libc/strtol.rel \
	src/libc/printf.rel \
	src/libc/vfprintf.rel \
	src/libc/vsnprintf.rel \
	src/monitor/monitor.rel \
	src/monitor/uart.rel \
	src/monitor/crt0.rel

monitor.ihx: $(MONITOR_OBJS)

welcome.ihx: src/welcome.rel


%.txt: %.bin
	hexdump -v -e '/1 "0x%02x, "' $< > $@

%.bin: %.ihx
	objcopy --input-target=ihex --output-target=binary $*.ihx $@
	z80dasm -alt --origin=0 -o $*.dis $@

%.ihx:
	sdcc -Wl-u -mz80 --no-std-crt0 --code-loc 0x0100 --data-loc 0x17F0 --stack-loc 0xF000 -o $@ $^

%.rel: %.c
	sdcc -Iinclude -mz80 --code-loc 0x0100 --data-loc 0x17F0 --stack-loc 0xF000 -c -o $@ $<

%.rel: %.asm
	sdasz80 -Iinclude -l -p -o -s $<

clean:
	find -regex ".*\.\(ihx\|bin\|rel\|sym\|lst\|rst\|lk\|noi\|map\)" -delete -print

.SECONDARY:

