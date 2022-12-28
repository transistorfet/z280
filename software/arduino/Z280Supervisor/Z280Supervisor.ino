
#define SERIAL_SPEED	115200

#define MEM_ADDR	0x100000
#define MEM_SIZE	4096

#define FLASH_ADDR	0x200000

#define ZBUS_XADDR_PORT	PORTF
#define ZBUS_XADDR_PIN	PINF
#define ZBUS_XADDR_DDR	DDRF

#define ZBUS_HADDR_PORT	PORTC
#define ZBUS_HADDR_PIN	PINC
#define ZBUS_HADDR_DDR	DDRC

#define ZBUS_LADDR_PORT	PORTL
#define ZBUS_LADDR_PIN	PINL
#define ZBUS_LADDR_DDR	DDRL
#define ZBUS_A0		9

#define ZBUS_LDATA_PORT	PORTA
#define ZBUS_LDATA_PIN	PINA
#define ZBUS_LDATA_DDR	DDRA

#define ZBUS_HDATA_PORT	PORTK
#define ZBUS_HDATA_PIN	PINK
#define ZBUS_HDATA_DDR	DDRK

#define ZBUS_WAIT	38 // PD7
#define ZBUS_BUSACK	39 // PG2
//#define ZBUS_BGACK	40 // PG1
#define ZBUS_BUSREQ	41 // PG0

#define ZBUS_DS		8
#define ZBUS_AS		50 // PB3
#define ZBUS_UDS	51 // PB2
#define ZBUS_LDS	52 // PB1
#define ZBUS_RW		53 // PB0

#define ZBUS_BW		7

#define ZBUS_WAIT_RESET 6

#define ZBUS_RESET	10 // PB4
#define ZBUS_BERR	11 // PB5

#define ZBUS_IS_AS()	(!(PINB & 0x08))
#define ZBUS_IS_UDS()	(!(PINB & 0x04))
#define ZBUS_IS_LDS()	(!(PINB & 0x02))
#define ZBUS_IS_WRITE()	(!(PINB & 0x01))
#define ZBUS_IS_READ()	(PINB & 0x01)

#define ZBUS_INIT_WAIT()	{ digitalWrite(ZBUS_WAIT, 0); pinMode(ZBUS_WAIT, INPUT); }
#define ZBUS_ASSERT_WAIT()	(DDRD |= 0x80)
#define ZBUS_UNASSERT_WAIT()	(DDRD &= ~(0x80))

#define ZBUS_INIT_RESET()	{ digitalWrite(ZBUS_RESET, 0); pinMode(ZBUS_RESET, INPUT); }
#define ZBUS_ASSERT_RESET()	(pinMode(ZBUS_RESET, OUTPUT))
#define ZBUS_UNASSERT_RESET()	(pinMode(ZBUS_RESET, INPUT))

#define ZBUS_RESET_WAIT()	{ PORTH |= 0x08; PORTH &= ~0x08; }
#define ZBUS_CLEAR_WAIT()	{ PORTH &= ~0x08; }

#undef INLINE_NOP
#define INLINE_NOP		__asm__("nop\n\t");

#define BUS_DEVICE	0
#define BUS_CONTROLLER	1

byte bus_request = 0;
byte bus_mode = BUS_DEVICE;

#define TTY_COMMAND	0
#define TTY_PASS	1

byte tty_mode = TTY_COMMAND;

void cpu_stop();

void enable_bus_interrupt(byte pin)
{
	*digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
	PCIFR |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
	PCICR |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

void disable_bus_interrupt(byte pin)
{
	PCICR &= ~(bit (digitalPinToPCICRbit(pin))); // disable interrupt for the group
}

void init_bus()
{
	pinMode(ZBUS_BUSREQ, OUTPUT);
	digitalWrite(ZBUS_BUSREQ, 1);
	pinMode(ZBUS_BUSACK, INPUT);
	// WARNING don't use with CPU in, will drive BUSACK to turn off the latches
	//pinMode(ZBUS_BUSACK, OUTPUT);
	//digitalWrite(ZBUS_BUSACK, 0);

	pinMode(ZBUS_UDS, INPUT);
	pinMode(ZBUS_LDS, INPUT);

	pinMode(ZBUS_WAIT_RESET, OUTPUT);
	digitalWrite(ZBUS_WAIT_RESET, 0);
}

void take_bus()
{
	digitalWrite(ZBUS_BUSREQ, 0);
	while (digitalRead(ZBUS_BUSACK) && !digitalRead(ZBUS_AS)) { }
}

void release_bus()
{
	digitalWrite(ZBUS_BUSREQ, 1);
}

void set_bus_mode_controller()
{
	disable_bus_interrupt(ZBUS_AS);

	take_bus();

	// Controls
	pinMode(ZBUS_AS, OUTPUT);
	pinMode(ZBUS_DS, OUTPUT);
	pinMode(ZBUS_RW, OUTPUT);
	pinMode(ZBUS_BW, OUTPUT);
	pinMode(ZBUS_WAIT, INPUT);
	digitalWrite(ZBUS_AS, 1);
	digitalWrite(ZBUS_RW, 1);
	digitalWrite(ZBUS_BW, 0);

	// Address Bus
	ZBUS_XADDR_PORT = 0x00;	// A17 - A23
	ZBUS_XADDR_DDR = 0x7F;
	ZBUS_HADDR_PORT = 0x00;	// A9 - A16
	ZBUS_HADDR_DDR = 0xFF;
	ZBUS_LADDR_PORT = 0x00;	// A1 - A8
	ZBUS_LADDR_DDR = 0xFF;
	pinMode(ZBUS_A0, OUTPUT);

	// Data Bus
	ZBUS_LDATA_PORT = 0x00;	// D0 - D7
	ZBUS_LDATA_DDR = 0x00;
	ZBUS_HDATA_PORT = 0x00;	// D8 - D15
	ZBUS_HDATA_DDR = 0x00;

	tty_mode = TTY_COMMAND;
	bus_mode = BUS_CONTROLLER;
}

void set_bus_mode_device()
{
	// Controls
	pinMode(ZBUS_AS, INPUT);
	pinMode(ZBUS_DS, INPUT);
	pinMode(ZBUS_RW, INPUT);
	pinMode(ZBUS_BW, INPUT);

	ZBUS_INIT_WAIT();

	// Address Bus
	ZBUS_XADDR_PORT = 0x00;	// A17 - A23
	ZBUS_XADDR_DDR = 0x00;
	ZBUS_HADDR_PORT = 0x00;	// A9 - A16
	ZBUS_HADDR_DDR = 0x00;
	ZBUS_LADDR_PORT = 0x00;	// A1 - A8
	ZBUS_LADDR_DDR = 0x00;
	pinMode(ZBUS_A0, INPUT);

	// Data Bus
	ZBUS_LDATA_PORT = 0x00;	// D0 - D7
	ZBUS_LDATA_DDR = 0x00;
	ZBUS_HDATA_PORT = 0x00;	// D8 - D15
	ZBUS_HDATA_DDR = 0x00;

	// TODO remove after testing, disable the interrupt
	//enable_bus_interrupt(ZBUS_AS);
	ZBUS_CLEAR_WAIT();
	bus_mode = BUS_DEVICE;
}

/******************************
 * Serial Communications Code *
 ******************************/

#define SERIAL_SIZE	64

char serial_read_tail = 0;
char serial_read_head = 0;
char serial_avail = 0;
char serial_rb[SERIAL_SIZE];
char serial_write_head = 0;
char serial_write_tail = 0;
char serial_wb[SERIAL_SIZE];

byte read_serial()
{
	register char b;

	b = Serial.read();
	if (b == -1)
		return 0;

	if (tty_mode == TTY_PASS) {
		if (b == '`') {
			cpu_stop();
			set_bus_mode_controller();
			clear_read_buffer();
			return 0;
		}

		noInterrupts();
		//if (serial_read_head == serial_read_tail) {
		//	serial_read_head = 0;
		//	serial_read_tail = 0;
		//}

		serial_rb[serial_read_tail++] = b;
		if (serial_read_tail >= SERIAL_SIZE)
			serial_read_tail = 0;
		interrupts();
		//Serial.write(serial_get_byte());
		return 0;
	}
	else if (tty_mode == TTY_COMMAND) {
		noInterrupts();
		serial_rb[serial_read_tail] = b;
		if (b == '\n' || b == '\r') {
			serial_rb[serial_read_tail] = '\0';
			serial_avail = 1;
		}
		if (serial_read_tail < SERIAL_SIZE)
			serial_read_tail++;
		interrupts();
		return serial_avail;
	}
}

inline byte serial_get_byte()
{
	register byte value;

	noInterrupts();
	if (serial_read_head == serial_read_tail) {
		//clear_read_buffer();
		value = -1;
	} else {
		value = serial_rb[serial_read_head++];
		if (serial_read_head >= SERIAL_SIZE)
			serial_read_head = 0;
	}
	interrupts();
	return value;
}

inline byte serial_add_byte(byte data)
{
	noInterrupts();
	serial_wb[serial_write_tail++] = data;
	interrupts();
}

void clear_read_buffer()
{
	noInterrupts();
	serial_avail = 0;
	serial_read_head = 0;
	serial_read_tail = 0;
	interrupts();
}

void flush_write_buffer()
{
	if (serial_write_tail > 0) {
		byte is_running = bus_mode == BUS_DEVICE;
		take_bus();
		noInterrupts();
		for (byte i = 0; i < serial_write_tail; i++) {
			Serial.write(serial_wb[i]);
		}
		serial_write_head = 0;
		serial_write_tail = 0;
		interrupts();
		if (is_running)
			release_bus();
	}
}


#define ROM_SIZE	0x1800
#define ROM_MEM_SIZE	0x1800
word mem_size = ROM_MEM_SIZE;
byte mem[ROM_MEM_SIZE] = {
//// Welcome - Test Stack, No External Ram: 512
//0xc3, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x00, 0x02, 0xcd, 0x87, 0x00, 0x76, 0x3e, 0x01, 0xd3, 0xff, 0x11, 0x9b, 0x00, 0x1a, 0xfe, 0x00, 0xca, 0x9a, 0x00, 0xd3, 0xfe, 0x13, 0xc3, 0x8e, 0x00, 0xc9, 0x57, 0x65, 0x6c, 0x63, 0x6f, 0x6d, 0x65, 0x20, 0x54, 0x6f, 0x20, 0x42, 0x72, 0x65, 0x61, 0x64, 0x38, 0x30, 0x21, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#include "/media/work/z280/software/welcome.txt"
};

#define READ_BUS_DEBUG	1
#define WRITE_BUS_DEBUG	1
#define WRITE_PROTECT	1
#define IO_SPACE_HADDR	(0x20 >> 1)	// address 0x2000

char trace = 0;

ISR (PCINT0_vect) __attribute__ ((naked));
ISR (PCINT0_vect) {
	// The request handling has been moved out of the interrput so that the serial port interrupt is
	// not blocked for the entirety of the request, which is the bulk of the work the ardunio is doing
	// when the CPU is running
	if (ZBUS_IS_AS()) {
		//ZBUS_ASSERT_WAIT();
		bus_request = 1;
	}

	/*
	asm(
	"push	r0\n"
	"in	r0, 0x3f\n"
	"push	r0\n"
	"push	r1\n"

	"in	r0, %0\n"
	"in	r1, %1\n"
	"bst	r0, 0x3\n"
	"bld	r1, 0x7\n"
	"out	%1, r1\n"
	"sts	%2, r1\n"

	"pop	r1\n"
	"pop	r0\n"
	"out	0x3f, r0\n"
	"pop	r0\n"
	"reti\n"
	: : "I" (_SFR_IO_ADDR(PINB)), "I" (_SFR_IO_ADDR(DDRD)), "m" (bus_request) :
	);
	*/
}

void check_bus_cycle()
{
	if (!bus_request)
		return;

	//noInterrupts();
	PCICR = 0x00;
	PCIFR = 0x00;
	bus_request = 0;

	register uint32_t addr = ((((ZBUS_XADDR_PIN & 0x7F) << 16) | (ZBUS_HADDR_PIN << 8) | ZBUS_LADDR_PIN) << 1);
	if (addr < 0x4000) {
		noInterrupts();

		// Read Operation
		if (ZBUS_IS_READ()) {
			ZBUS_HDATA_DDR = 0xFF;
			ZBUS_LDATA_DDR = 0xFF;

			//#if READ_BUS_DEBUG
			if (READ_BUS_DEBUG || (trace && addr >= ROM_SIZE)) {
				Serial.write('R');
				Serial.print(addr, HEX);
				if (addr < mem_size) {
					Serial.write('|');
					Serial.print((mem[addr] << 8) | mem[addr + 1], HEX);
				}
				Serial.write('\n');
			}
			//#endif

			if (ZBUS_HADDR_PIN == IO_SPACE_HADDR) {
				//Serial.write('S');
				//Serial.write('\n');
				switch (ZBUS_LADDR_PIN) {
				    case 0x01:
					ZBUS_LDATA_PORT = ((serial_read_head != serial_read_tail) ? 0x01 : 0x00) | ((serial_write_head == serial_write_tail) ? 0x04 : 0x00);
					break;
				    case 0x03:
					register byte val = serial_get_byte();
					//Serial.write(val);
					ZBUS_LDATA_PORT = val;
					break;
				    case 0x0e:
					ZBUS_LDATA_PORT = digitalRead(13);
					break;
				    default:
					break;
				}
			}
			else {
				if (ZBUS_IS_LDS())
					ZBUS_LDATA_PORT = (addr < mem_size) ? mem[addr] : 0x00;
				if (ZBUS_IS_UDS())
					ZBUS_HDATA_PORT = (addr < mem_size) ? mem[addr + 1] : 0x00;
			}
		}
		// Write Operation
		else {
			//#if WRITE_BUS_DEBUG
			if (WRITE_BUS_DEBUG || (trace && addr >= ROM_SIZE)) {
				Serial.write('W');
				Serial.print(addr, HEX);
				Serial.write('|');
				register word data = (ZBUS_HDATA_PIN << 8) | ZBUS_LDATA_PIN;
				Serial.print(data, HEX);
				Serial.write('\n');
			}
			//#endif

			if (ZBUS_HADDR_PIN == IO_SPACE_HADDR) {
				//Serial.write('X');
				//Serial.write('\n');
				switch (ZBUS_LADDR_PIN) {
				    case 0x03:
					Serial.write(ZBUS_LDATA_PIN);
					//serial_add_byte(ZBUS_LDATA_PIN);
					break;
				    case 0x0c:
					trace = (ZBUS_LDATA_PIN & 0x01);
					Serial.println(trace ? "Trace On" : "Trace Off");
					break;
				    case 0x0e:
					digitalWrite(13, (ZBUS_LDATA_PIN & 0x01));
					break;
				    default:
					break;
				}
			}
			else if (addr < mem_size) {
				if (!WRITE_PROTECT || addr >= ROM_SIZE) {
					if (ZBUS_IS_LDS())
						mem[addr] = ZBUS_LDATA_PIN;
					if (ZBUS_IS_UDS())
						mem[addr + 1] = ZBUS_HDATA_PIN;
				}
			}
		}


		ZBUS_LDATA_DDR = 0x00;
		ZBUS_HDATA_DDR = 0x00;
		interrupts();
	}

	ZBUS_UNASSERT_WAIT();
	INLINE_NOP;
	INLINE_NOP;
	PCICR = 0x01;
}

//
// Print debug information on the bus cycles and provide WAIT signal without responding to the request
//
// This is used when the computer jumpers are set to run the program from the onboard Flash memory, but allows
// each memory access to be observed
//
void snoop_bus_cycle()
{
	if (!bus_request)
		return;

	bus_request = 0;
	PCICR = 0x00;
	PCIFR = 0x00;

	register uint32_t addr = ((ZBUS_XADDR_PIN & 0x7F) << 17) | (ZBUS_HADDR_PIN << 9) | (ZBUS_LADDR_PIN << 1);
	if (ZBUS_IS_READ()) {
		Serial.write('R');
	}
	else {
		Serial.write('W');
	}

	Serial.print(addr, HEX);
	Serial.write('|');
	register uint16_t data = (ZBUS_HDATA_PIN << 8) | ZBUS_LDATA_PIN;
	Serial.print(data, HEX);
	//Serial.write(type ? 'w' : 'b');
	Serial.write('\n');

	PCICR = 0x01;
	//interrupts();
	ZBUS_UNASSERT_WAIT();
	//ZBUS_RESET_WAIT();
}


inline void write_data(uint32_t addr, uint16_t data)
{
	ZBUS_HDATA_DDR = 0xFF;
	ZBUS_LDATA_DDR = 0xFF;

	ZBUS_HDATA_PORT = (uint8_t) (data >> 8);
	ZBUS_LDATA_PORT = (uint8_t) data;

	ZBUS_XADDR_PORT = (uint8_t) (addr >> 17) & 0x7F;
	ZBUS_HADDR_PORT = (uint8_t) (addr >> 9);
	ZBUS_LADDR_PORT = (uint8_t) (addr >> 1);
	digitalWrite(ZBUS_A0, addr & 0x01);

	digitalWrite(ZBUS_BW, 0);
	digitalWrite(ZBUS_RW, 0);
	digitalWrite(ZBUS_DS, 0);
	//delayMicroseconds(1);
	INLINE_NOP;
	INLINE_NOP;
	digitalWrite(ZBUS_DS, 1);
	digitalWrite(ZBUS_RW, 1);
	INLINE_NOP;

	ZBUS_HDATA_PORT = 0x00;
	ZBUS_LDATA_PORT = 0x00;
	ZBUS_XADDR_PORT = 0x00;
	ZBUS_HADDR_PORT = 0x00;
	ZBUS_LADDR_PORT = 0x00;

	ZBUS_HDATA_DDR = 0x00;
	ZBUS_LDATA_DDR = 0x00;
}

inline uint16_t read_data(uint32_t addr)
{
	uint8_t hvalue = 0;
	uint8_t lvalue = 0;

	ZBUS_HDATA_DDR = 0x00;
	ZBUS_LDATA_DDR = 0x00;

	ZBUS_XADDR_PORT = (uint8_t) (addr >> 17) & 0x7F;
	ZBUS_HADDR_PORT = (uint8_t) (addr >> 9);
	ZBUS_LADDR_PORT = (uint8_t) (addr >> 1);
	digitalWrite(ZBUS_A0, addr & 0x01);

	digitalWrite(ZBUS_BW, 0);
	digitalWrite(ZBUS_RW, 1);
	digitalWrite(ZBUS_DS, 0);
	//delayMicroseconds(1);
	INLINE_NOP;
	hvalue = ZBUS_HDATA_PIN;
	lvalue = ZBUS_LDATA_PIN;
	digitalWrite(ZBUS_DS, 1);
	INLINE_NOP;

	ZBUS_HDATA_PORT = 0x00;
	ZBUS_LDATA_PORT = 0x00;
	ZBUS_XADDR_PORT = 0x00;
	ZBUS_HADDR_PORT = 0x00;
	ZBUS_LADDR_PORT = 0x00;

	return ((uint16_t) hvalue << 8) | (uint16_t) lvalue;
}

void run_read_test(int argc, char **argv)
{
	int errors = 0;

	uint32_t start_addr = (argc >= 2) ? strtol(argv[1], NULL, 16) : MEM_ADDR;
	uint16_t size = (argc >= 3) ? strtol(argv[2], NULL, 16) : MEM_SIZE;

	set_bus_mode_controller();
	Serial.print("Running Read Test\n");

	for (uint32_t i = 0; i < size / 2; i++) {
		uint32_t addr = start_addr + (i << 1);
		uint16_t data = read_data(addr);

		if (data != i)
			errors += 1;

		if (i % 32 == 0) {
			Serial.print("\n");
			Serial.print(addr, HEX);
			Serial.print(" ");
		}

		if (data < 0x10) Serial.print("000");
		else if (data < 0x100) Serial.print("00");
		else if (data < 0x1000) Serial.print("0");
		Serial.print(data, HEX);
		Serial.print(" ");
	}

	Serial.print("\n");
	Serial.print("Errors: ");
	Serial.print(errors, DEC);
	Serial.print("\n");
}

void run_write_test(int argc, char **argv)
{
	uint32_t start_addr = (argc >= 2) ? strtol(argv[1], NULL, 16) : MEM_ADDR;
	uint16_t size = (argc >= 3) ? strtol(argv[2], NULL, 16) : MEM_SIZE;

	set_bus_mode_controller();
	Serial.print("Running Write Test\n");

	digitalWrite(ZBUS_RW, 1);
	digitalWrite(ZBUS_AS, 1);

	for (uint32_t i = 0; i < size / 2; i++) {
		write_data(start_addr + (i << 1), i);

		delayMicroseconds(1);

		if ((i % 64) == 63) {
			delayMicroseconds(10000);
		}
	}

	Serial.print("Complete\n");
}


void run_erase_flash(int argc, char **argv)
{
	uint32_t flash_addr = (argc >= 2) ? strtol(argv[1], NULL, 16) : FLASH_ADDR;

	Serial.println("Erasing flash");
	write_data(flash_addr + (0x555 << 1), 0xAAAA);
	write_data(flash_addr + (0x2AA << 1), 0x5555);
	write_data(flash_addr + (0x555 << 1), 0x8080);
	write_data(flash_addr + (0x555 << 1), 0xAAAA);
	write_data(flash_addr + (0x2AA << 1), 0x5555);
	write_data(flash_addr + 0x00, 0x3030);
}


void program_flash_data(uint32_t flash_addr, uint32_t addr, uint16_t data)
{
	write_data(flash_addr + (0x555 << 1), 0xAAAA);
	write_data(flash_addr + (0x2AA << 1), 0x5555);
	write_data(flash_addr + (0x555 << 1), 0xA0A0);
	write_data(addr, data);
}

void run_send_mem(int argc, char **argv)
{
	uint16_t i;
	uint32_t addr;

	uint32_t flash_addr = (argc >= 2) ? strtol(argv[1], NULL, 16) : FLASH_ADDR;

	set_bus_mode_controller();
	Serial.print("Writing data to flash to ");
	Serial.print(flash_addr, HEX);
	Serial.print("\n");

	for (addr = flash_addr; addr < flash_addr + mem_size; addr += 2) {
		uint16_t data = read_data(addr);
		if (data != 0xFFFF) {
			Serial.print("Flash not erased at ");
			Serial.print(addr, HEX);
			Serial.print(" (");
			Serial.print(data, HEX);
			Serial.print(")\n");
			return;
		}
	}

	for (i = 0, addr = flash_addr; i < mem_size; i += 2, addr += 2) {
		word data = (mem[i] << 8) | mem[i + 1];
		program_flash_data(flash_addr, addr, data);
		Serial.print(data, HEX);
		Serial.print(" ");

		if ((i % 64) == 62) {
			Serial.print("\n");
		}
	}

	Serial.print("Sending complete\n");
}

void run_verify_mem(int argc, char **argv)
{
	word i;
	long addr;

	uint32_t flash_addr = (argc >= 2) ? strtol(argv[1], NULL, 16) : FLASH_ADDR;

	set_bus_mode_controller();
	Serial.print("Verify flash data\n");

	for (i = 0, addr = flash_addr; i < mem_size; i += 2, addr += 2) {
		word data = (mem[i] << 8) | mem[i + 1];
		word actual_data = read_data(addr);
		if (data != actual_data) {
			Serial.print("Expected ");
			Serial.print(data, HEX);
			Serial.print(" but found ");
			Serial.print(actual_data, HEX);
			Serial.print("\n");
		}
	}

	Serial.print("Verify complete\n");
}


void run_flash_test()
{
	word value;

	write_data(0x555 << 1, 0xAAAA);
	write_data(0x2AA << 1, 0x5555);
	write_data(0x555 << 1, 0x9090);
	value = read_data(0x01 << 1);
	Serial.print(value, HEX);
	Serial.print("\n");
}


void cpu_start()
{
	set_bus_mode_device();
	tty_mode = TTY_PASS;
	release_bus();
	Serial.print("Running\n\n");
}

void cpu_stop()
{
	tty_mode = TTY_COMMAND;
	take_bus();
	set_bus_mode_controller();
	Serial.print("\nStopped");
}

void cpu_reset()
{
	ZBUS_ASSERT_RESET();
	delay(10);

	ZBUS_LDATA_DDR = 0xFF;
	ZBUS_LDATA_PORT = 0x8E;
	ZBUS_INIT_WAIT();
	ZBUS_ASSERT_WAIT();
	INLINE_NOP;
	INLINE_NOP;
	ZBUS_UNASSERT_RESET();
	INLINE_NOP;
	INLINE_NOP;
	INLINE_NOP;
	INLINE_NOP;
	ZBUS_ASSERT_WAIT();
	ZBUS_LDATA_DDR = 0x00;
}

int parseline(char *input, char **vargs)
{
	short j = 0;

	while (*input == ' ')
		input++;

	vargs[j++] = input;
	while (*input != '\0' && *input != '\n' && *input != '\r') {
		if (*input == ' ') {
			*input = '\0';
			input++;
			while (*input == ' ')
				input++;
			vargs[j++] = input;
		}
		else
 			input++;
	}

	*input = '\0';
	if (*vargs[j - 1] == '\0')
		j -= 1;
	vargs[j] = NULL;

	return j;
}

void do_command(String line)
{
	short argc;
	char *argv[10];
	argc = parseline(line.c_str(), argv);

	if (!strcmp(argv[0], "read")) {
		run_read_test(argc, argv);
	}
	else if (!strcmp(argv[0], "write")) {
		run_write_test(argc, argv);
	}
	else if (!strcmp(argv[0], "send")) {
		run_send_mem(argc, argv);
	}
	else if (!strcmp(argv[0], "verify")) {
		run_verify_mem(argc, argv);
	}
	else if (!strcmp(argv[0], "erase")) {
		run_erase_flash(argc, argv);
	}
	else if (!strcmp(argv[0], "testflash")) {
		run_flash_test();
	}
	else if (!strcmp(argv[0], "run")) {
		cpu_reset();
		cpu_start();
	}
	else if (!strcmp(argv[0], "stop")) {
		cpu_stop();
	}
	else if (!strcmp(argv[0], "reset")) {
		set_bus_mode_device();
		cpu_reset();
	}
}

void setup()
{
	pinMode(13, OUTPUT);
	digitalWrite(13, 1);

	Serial.begin(SERIAL_SPEED);

	ZBUS_INIT_RESET();

	init_bus();

	set_bus_mode_controller();
	//set_bus_mode_device();

	digitalWrite(13, 0);
	Serial.print("> ");

	cpu_reset();
	cpu_stop();
	//cpu_start();
}

void loop()
{
	if (read_serial() && tty_mode == TTY_COMMAND) {
		String line = String(serial_rb);
		clear_read_buffer();
		Serial.print(line);
		Serial.print("\n");
		do_command(line);
		if (tty_mode == TTY_COMMAND)
			Serial.print("\n> ");
	}

	//check_bus_cycle();
	snoop_bus_cycle();
}

