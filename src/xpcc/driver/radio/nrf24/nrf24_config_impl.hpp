// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__NRF24_CONFIG_HPP
#   error "Don't include this file directly, use 'nrf24_config.hpp' instead!"
#endif

#include "nrf24_config.hpp"
#include "nrf24_definitions.hpp"

#include <stdint.h>

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::powerUp()
{
	Nrf24Phy::setBits(NrfRegister::CONFIG, Config::PWR_UP);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::powerDown()
{
	Nrf24Phy::clearBits(NrfRegister::CONFIG, Config::PWR_UP);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::setChannel(uint8_t channel)
{
	Nrf24Phy::writeRegister(NrfRegister::RF_CH, channel);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::setMode(Mode mode)
{
	if(mode == Mode::Rx)
	{
		Nrf24Phy::flushRxFifo();
		Nrf24Phy::setBits(NrfRegister::CONFIG, Config::PRIM_RX);
	} else
	{
		Nrf24Phy::setBits(NrfRegister::STATUS, Status::MAX_RT);
		Nrf24Phy::setBits(NrfRegister::STATUS, Status::TX_DS);
		Nrf24Phy::clearBits(NrfRegister::CONFIG, Config::PRIM_RX);

		// pulsing CE seems to be neccessary to enter TX mode
		Nrf24Phy::pulseCe();
	}

	// don't go to standby
	Nrf24Phy::setCe();
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::setSpeed(Speed speed)
{
	if(speed == Speed::kBps250)
	{
		Nrf24Phy::clearBits(NrfRegister::RF_SETUP, RfSetup::RF_DR_HIGH);
		Nrf24Phy::setBits(NrfRegister::RF_SETUP, RfSetup::RF_DR_LOW);
	}
	else if(speed == Speed::MBps1)
	{
		Nrf24Phy::clearBits(NrfRegister::RF_SETUP, RfSetup::RF_DR_LOW);
		Nrf24Phy::clearBits(NrfRegister::RF_SETUP, RfSetup::RF_DR_HIGH);
	}
	else if(speed == Speed::MBps1)
	{
		Nrf24Phy::setBits(NrfRegister::RF_SETUP, RfSetup::RF_DR_HIGH);
		Nrf24Phy::clearBits(NrfRegister::RF_SETUP, RfSetup::RF_DR_LOW);
	}
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void xpcc::Nrf24Config<Nrf24Phy>::setCrc(Crc crc)
{
	if(crc == Crc::NoCrc)
	{
		Nrf24Phy::clearBits(NrfRegister::CONFIG, Config::EN_CRC);

	} else
	{
		Nrf24Phy::setBits(NrfRegister::CONFIG, Config::EN_CRC);

		if (crc == Crc::Crc1Byte)
		{
			Nrf24Phy::clearBits(NrfRegister::CONFIG, Config::CRC0);
		}
		else if (crc == Crc::Crc2Byte)
		{
			Nrf24Phy::setBits(NrfRegister::CONFIG, Config::CRC0);
		}
	}
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::setAddressWidth(AddressWidth width)
{
	Nrf24Phy::writeRegister(NrfRegister::SETUP_AW, static_cast<uint8_t>(width));
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::setRfPower(RfPower power)
{
	Nrf24Phy::writeRegister(NrfRegister::RF_SETUP, static_cast<uint8_t>(power) << 1);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::setAutoRetransmitDelay(AutoRetransmitDelay delay)
{
	Nrf24Phy::writeRegister(NrfRegister::SETUP_RETR, static_cast<uint8_t>(delay) << 4);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::setAutoRetransmitCount(AutoRetransmitCount count)
{
	Nrf24Phy::writeRegister(NrfRegister::SETUP_RETR, static_cast<uint8_t>(count));
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::enableFeatureNoAck()
{
	Nrf24Phy::setBits(NrfRegister::FEATURE, Feature::EN_DYN_ACK);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::disableFeatureNoAck()
{
	Nrf24Phy::clearBits(NrfRegister::FEATURE, Feature::EN_DYN_ACK);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::enablePipe(Pipe_t pipe, bool enableAutoAck)
{

	uint16_t payload_length = Nrf24Phy::getPayloadLength();

	NrfRegister_t reg = NrfRegister::RX_PW_P0;
	reg.value += pipe.value;

	/* Set payload width for pipe */
	Nrf24Phy::writeRegister(reg, payload_length);


	Flags_t pipe_flag = static_cast<Flags_t>(1 << pipe.value);

	/* Enable or disable auto acknowledgement for this pipe */
	if(enableAutoAck)
	{
		Nrf24Phy::setBits(NrfRegister::EN_AA, pipe_flag);
	} else
	{
		Nrf24Phy::clearBits(NrfRegister::EN_AA, pipe_flag);
	}

	/* enable pipe */
	Nrf24Phy::setBits(NrfRegister::EN_RX_ADDR, pipe_flag);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
void
xpcc::Nrf24Config<Nrf24Phy>::disablePipe(Pipe_t pipe)
{
	/* DISABLE pipe */
	Nrf24Phy::clearBits(NrfRegister::EN_RX_ADDR, (1 << pipe.value));
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Nrf24Phy>
typename xpcc::Nrf24Config<Nrf24Phy>::Pipe_t
xpcc::Nrf24Config<Nrf24Phy>::getPayloadPipe()
{
	uint8_t status = Nrf24Phy::readStatus();

	return static_cast<Pipe_t>((status & (uint8_t)Status::RX_P_NO) >> 1);
}
