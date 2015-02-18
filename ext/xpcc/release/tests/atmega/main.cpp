
#include <xpcc/architecture.hpp>

using namespace xpcc::atmega;
typedef xpcc::avr::SystemClock clock;

typedef Uart0 uart;

typedef GpioOutputB0 Out;
typedef GpioInputB1  In;
typedef GpioB2       Io;

int
main(void)
{
	Out::setOutput();
	Out::setOutput(true);
	Out::set();
	Out::reset();	
	Out::toggle();
	
	In::setInput();
	In::read();
	
	Io::setOutput();
	Io::set();
	Io::setInput();
	Io::read();
	
#if defined(__AVR_ATmega64__)   || \
	defined(__AVR_ATmega645__)  || \
	defined(__AVR_ATmega6490__) || \
	defined(__AVR_ATmega128__) 
	typedef Uart0 uart;
	GpioOutputE1::connect(Uart0::Tx);
	GpioInputE0::connect(Uart0::Rx);
#elif defined(__AVR_ATmega640__) || \
	defined(__AVR_ATmega1280__)  || \
	defined(__AVR_ATmega1281__)
	typedef Uart1 uart;
	GpioOutputD3::connect(Uart1::Tx);
	GpioInputD2::connect(Uart1::Rx);	
#else
	typedef Uart0 uart;
	GpioOutputD1::connect(Uart0::Tx);
	GpioInputD0::connect(Uart0::Rx);
#endif

	uart::initialize<clock, 9600>();
	uart::write('x');
	
	while (1)
	{
	}
}
