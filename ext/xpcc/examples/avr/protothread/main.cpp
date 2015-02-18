
#include <xpcc/architecture/platform.hpp>
#include <xpcc/processing/protothread.hpp>
#include <xpcc/processing/timer.hpp>

using namespace xpcc::atmega;

typedef GpioOutputB0 LedGreen;
typedef GpioOutputB1 LedRed;

class BlinkingLightGreen : public xpcc::pt::Protothread
{
public:
	bool
	run()
	{
		PT_BEGIN();
		
		// set everything up
		LedGreen::setOutput();
		LedGreen::set();
		
		while (true)
		{
			LedGreen::set();
			
			this->timeout.restart(100);
			PT_WAIT_UNTIL(this->timeout.isExpired());
			
			LedGreen::reset();
			
			this->timeout.restart(600);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}
		
		PT_END();
	}

private:
	xpcc::ShortTimeout timeout;
};

class BlinkingLightRed : public xpcc::pt::Protothread
{
public:
	bool
	run()
	{
		PT_BEGIN();
		
		// set everything up
		LedRed::setOutput();
		LedRed::set();
		
		while (true)
		{
			LedRed::set();
			
			this->timeout.restart(200);
			PT_WAIT_UNTIL(this->timeout.isExpired());
			
			LedRed::reset();
			
			this->timeout.restart(300);
			PT_WAIT_UNTIL(this->timeout.isExpired());
			
			LedRed::set();
			
			this->timeout.restart(200);
			PT_WAIT_UNTIL(this->timeout.isExpired());
			
			LedRed::reset();
			
			this->timeout.restart(1000);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}
		
		PT_END();
	}

private:
	xpcc::ShortTimeout timeout;
};

// timer interrupt routine
ISR(TIMER2_COMPA_vect)
{
	xpcc::Clock::increment();
}

int
main()
{
	// timeout initialization
	// compare-match-interrupt every 1 ms at 14.7456 MHz
	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22);
	TIMSK2 = (1 << OCIE2A);
	OCR2A = 230;
	
	enableInterrupts();
	
	BlinkingLightGreen greenLight;
	BlinkingLightRed redLight;
	while (1)
	{
		greenLight.run();
		redLight.run();
	}
}
