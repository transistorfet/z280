

all: rom


rom: monitor.txt

%.txt: %.bin
	hexdump -v -e '/1 "0x%02x, "' $< > ../$@

#%.bin: %.asm
#	#z80asm --list=$<.lst $<
#	z80asm -o $@ $<

#%.bin: %.rel
#	sdldz80 -iu $* -e
#	objcopy --input-target=ihex --output-target=binary $*.ihx $@


#image: image.hex
#	objcopy -I ihex $< -O binary image.bin
#	#hexdump -v -e '/1 "0x%02x, "' image.bin; echo ""
#	hexdump -v -e '/1 "0x%02x, "' image.bin > ../output.txt
#	z80dasm -at image.bin > image.bin.lst

MONITOR_OBJS = \
	src/monitor/monitor.rel \
	src/monitor/crt0.rel

monitor.ihx: $(MONITOR_OBJS)

welcome.ihx: src/welcome.rel

%.bin: %.ihx
	objcopy --input-target=ihex --output-target=binary $*.ihx $@

%.ihx:
	sdcc -mz80 --no-std-crt0 --code-loc 0x0100 --data-loc 0x800 --stack-loc 0xF000 -o $@ $^

%.rel: %.c
	sdcc -Iinclude -mz80 --no-std-crt0 --code-loc 0x0100 --data-loc 0x800 --stack-loc 0xF000 -c -o $@ $<

%.rel: %.asm
	sdasz80 -Iinclude -l -p -o -s $<

clean:
	find -regex ".*\.\(ihx\|bin\|rel\|sym\|lst\|rst\|lk\|noi\|map\)" -delete -print

.SECONDARY:

