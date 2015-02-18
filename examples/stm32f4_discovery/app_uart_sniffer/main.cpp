#include <xpcc/architecture.hpp>
#include "../stm32f4_discovery.hpp"
#include <xpcc/debug/logger.hpp>

Usart2 uart;
xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice(uart);

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG



enum class Direction {
	Init,
	Host2Node,
	Node2Host,
};

Direction direction = Direction::Init;

void
setDirection(Direction dir)
{
	if (direction == dir) {
		// no change
	} else {
		direction = dir;
		static uint16_t counter = 0;
		static xpcc::Timestamp lastTimestamp = xpcc::Clock::now();

		xpcc::Timestamp timestamp = xpcc::Clock::now();

		XPCC_LOG_INFO.printf("\e[39m\n%04d %02d:%03d +%01d:%03d ",
				counter,
				timestamp.getTime() / 1000,
				timestamp.getTime() % 1000,
				(timestamp.getTime() - lastTimestamp.getTime()) / 1000,
				(timestamp.getTime() - lastTimestamp.getTime()) % 1000);

		switch (direction)
		{
		case Direction::Host2Node:
			XPCC_LOG_INFO.printf("\e[91m");
			break;
		case Direction::Node2Host:
			XPCC_LOG_INFO.printf("\e[92m");
			break;
		}

		lastTimestamp = timestamp;
		++counter;
	}
}

// ----------------------------------------------------------------------------
/**
 *
 */
MAIN_FUNCTION
{
	defaultSystemClock::enable();
	xpcc::cortex::SysTickTimer::enable();

	LedOrange::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::High);

	// Enable USART 2: To / from PC
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<defaultSystemClock, 115200>(12);

	// Enable USART 1 Host To Node
	GpioInputA10::connect(Usart1::Rx, Gpio::InputType::PullUp);
	Usart1::initialize<defaultSystemClock, 115200>(12);

	// Enable USART 3 Node to Host
	GpioInputD9::connect(Usart3::Rx, Gpio::InputType::PullUp);
	Usart3::initialize<defaultSystemClock, 115200>(12);

	XPCC_LOG_INFO.printf("\e[H\e[J\e[39m");
	XPCC_LOG_INFO.printf("Welcome to XPCC Bidirectional UART Sniffer.\n\n");
	XPCC_LOG_INFO.printf("\e[91mRed PD9    \e[92mGreen PA10\n\n\e[39m");
	XPCC_LOG_INFO.printf("ctr   time  relati data\n");
	XPCC_LOG_INFO.printf("==== ====== ====== ===== ...\n");

	while (1)
	{
		uint8_t c;
		while (Usart3::read(c)) {
			setDirection(Direction::Node2Host);
			XPCC_LOG_INFO.printf("%02x ", c);
			LedRed::toggle();
		}
		while (Usart1::read(c)) {
			setDirection(Direction::Host2Node);
			XPCC_LOG_INFO.printf("%02x ", c);
			LedGreen::toggle();
		}

		xpcc::delayMicroseconds(100);
	}

	return 0;
}
