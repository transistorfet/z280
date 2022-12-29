

all: rom


rom: monitor.txt

%.txt: %.bin
	hexdump -v -e '/1 "0x%02x, "' $< > ../$@

#%.bin: %.asm
#	#z80asm --list=$<.lst $<
#	z80asm -o $@ $<

%.bin: %.rel
	sdldz80 -iu $* -e
	objcopy --input-target=ihex --output-target=binary $*.ihx $@


image: image.hex
	objcopy -I ihex $< -O binary image.bin
	#hexdump -v -e '/1 "0x%02x, "' image.bin; echo ""
	hexdump -v -e '/1 "0x%02x, "' image.bin > ../output.txt
	z80dasm -at image.bin > image.bin.lst

image.hex: main.rel crt0.o 
	sdcc -mz80 --no-std-crt0 --code-loc 0x0100 --data-loc 0x800 --stack-loc 0x0C00 -o $@ main.rel crt0.rel

%.rel: %.c
	sdcc -mz80 --no-std-crt0 --code-loc 0x0100 --data-loc 0x800 --stack-loc 0x0C00 -c -o $@ $<

%.rel: %.asm
	sdasz80 -l -p -o -s $<

clean:
	rm *.o *.sym *.lst *.rel *.lk *.map

.SECONDARY:

