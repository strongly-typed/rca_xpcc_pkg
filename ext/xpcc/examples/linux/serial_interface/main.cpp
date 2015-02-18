
#include <xpcc/debug/logger.hpp>
#include <xpcc/architecture.hpp>

int
main()
{
	xpcc::hosted::SerialInterface port("/dev/ttyUSB0", 115200);
	
	if (!port.open()) {
		XPCC_LOG_ERROR << "Could not open port: " << port.getDeviceName().c_str() << xpcc::endl;
	}

	while(true)
	{
		uint8_t a;
		port.readBytes(&a, 1);
		XPCC_LOG_DEBUG << "Read: " << a << xpcc::endl; 

		/*char a;
		if (port.read(a))
		{
			std::cout << "Read: " << a << std::endl; 
		}*/
	}

	port.close();
}
