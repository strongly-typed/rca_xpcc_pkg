// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_INTERFACE_SPI_DEVICE_HPP
#define XPCC_INTERFACE_SPI_DEVICE_HPP

#include "spi.hpp"
#include "spi_master.hpp"

namespace xpcc
{

/**
 * Base class of an SPI Device.
 *
 * This class provides functions for configuring the peripheral.
 *
 * @author	Georgi Grinshpun
 * @author	Niklas Hauser
 * @ingroup spi
 */
template < class SpiMaster >
class SpiDevice
{
	Spi::ConfigurationHandler configuration;

public:
	SpiDevice()
	:	configuration(nullptr)
	{
	}

	void inline
	attachConfigurationHandler(Spi::ConfigurationHandler handler)
	{
		this->configuration = handler;
	}

protected:
	bool inline
	aquireMaster()
	{
		uint_fast8_t response = SpiMaster::aquire(this);

		// another context is using it
		if (response == 0)
			return false;

		// call configure only on the first aquire
		if (response == 1 && this->configuration)
			this->configuration();

		// we have successfully aquired the master
		return true;
	}

	bool inline
	releaseMaster()
	{
		return (SpiMaster::release(this) == 0);
	}
};

}	// namespace xpcc

#endif // XPCC_INTERFACE_SPI_DEVICE_HPP
