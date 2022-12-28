
Z280 Computer
=============

###### *Started October 29, 2022*

A [series of] computers using the Z280, similar to the [Computie
project](https://jabberwocky.ca/projects/computie/).  The first board is a prototyping board with
the CPU's basic needs (clock/pullup resistors/etc), a 1MB Static RAM, a 1MB Flash chip, plus an
expansion bus that an Arduino can be connected to.  The bus pin assignments match Computie's
Supervisor shield, so the onboard flash chip can be programmed using the same harness as Computie.

Z280-SBC Revision 1
-------------------

I designed the first board quickly with the idea of it being rough.  Rather than prototype the CPU
using solderless breadboards, which are prone to electrical faults, I thought I'd design a minimal
board instead, with footprints for various additional components, like memory, a compact flash card
slot, and a CPLD.  I made the mistake of using vias that were 0.4 mm total diameter, which made
the boards more expensive at JLC.  If I redesign the board, I'll probably try to use 0.45 mm vias
to avoid that.

Building the board was pretty quick and easy.  I used the supervisor shield from Computie, along
with the Arduino program here to try to feed data to the CPU live, without the memory.  I couldn't
find documentation on the frequency range of the CPU, so I used a 12 MHz crystal, which clocks
the CPU at 3 MHz.  That's still too fast for the Arduino without a flip flop to latch in the WAIT
signal.

I added the memory to board to see if I could get the CPU executing from there, but it turns out
the footprint for the flash chip is upside down, and I installed it right way up.  I didn't notice
at first, and tried reading from the RAM which didn't quite work because the A4 address line was
being held low.  The flash chip was also getting extremely hot when programming the Arduino, but
not when idle, oddly enough.  Eventually I clued into the fact that the chip needed to be rotated,
and was able to do that using a hot air rework station to remove the chip.  I reused the same
chip, and it seems to work, but there are other issues with the board, and I'm a bit suspicious of
it still.

Using a logic analyzer, I looked at the ST0-ST3 pins of the CPU which has a number corresponding
to the transaction type.  It seems to be as expected, accessing memory either cacheable (0x8) or
non-cacheable (0x9), and refreshing memory (0x1).  When the CPU halts, it issues a halt bus cycle
(0x3) and then only performs refresh cycles after that.  Beyond that though, it didn't seem to
correspond to the program it was supposed to be running.

I programmed the halt instruction into each byte in the beginning of the flash and ran it to make
sure it halted pretty much right away.  It accessed memory twice before halting instead of just
once, but that might be the instruction pre-fetch (not sure yet).  I was able to used the /DS line
wired to the /WAIT line to pause the CPU on the first instruction, and then inspect the address
and data buses.  The address bus had the halt instruction on it, as did the data bus, but the
address demultiplexing should have taken care of that.  Well, I got the logic backwards on the
latch enable pin, so it was was latching when the /AS line *wasn't* asserted (low).

I was able to use one of the inverter gates for the backup clock, which wasn't populated yet, to
invert the /AS line before going into the latch enabled pin (positive-logic).  That fixed the
latching issue, and it seems to execute code now, but I can't get much visibility into it yet.

