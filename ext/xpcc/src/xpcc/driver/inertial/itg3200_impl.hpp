// coding: utf-8
/* Copyright (c) 2012, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ITG3200_HPP
#	error "Don't include this file directly, use 'itg3200.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Itg3200<I2cMaster>::Itg3200(Data &data, uint8_t address)
:	data(data), i2cTask(I2cTask::Idle), i2cSuccess(0),
	adapter(address, i2cTask, i2cSuccess),
	rawBuffer{0x00, 0x00, 0x00, 0,0,0,0,0,0,0,0, 0x00}
{
}

// MARK: - i2cTasks
// MARK: ping
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Itg3200<I2cMaster>::ping()
{
	CO_BEGIN();

	CO_WAIT_UNTIL(adapter.configurePing() &&
			(i2cTask = I2cTask::Ping, this->startTransaction(&adapter)));

	CO_WAIT_WHILE(i2cTask == I2cTask::Ping);

	CO_END_RETURN(i2cSuccess == I2cTask::Ping);
}

template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Itg3200<I2cMaster>::configure(LowPassFilter filter, uint8_t divider)
{
	CO_BEGIN();

	rawBuffer[4] = divider;
	rawBuffer[5] = rawBuffer[0] = LowPassFilter_t(filter).value | uint8_t(Filter::FullScale);
	rawBuffer[6] = rawBuffer[1] = 0;

	CO_END_RETURN_CALL(write(Register::SMPLRT_DIV, rawBuffer, 3, false));
}

template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Itg3200<I2cMaster>::readRotationRate()
{
	CO_BEGIN();

	if (CO_CALL(read(Register::INT_STATUS, rawBuffer+2, 9)))
	{
		std::memcpy(data.data, rawBuffer+3, 8);
		CO_RETURN(true);
	}

	CO_END_RETURN(false);
}

template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Itg3200<I2cMaster>::setSampleRateDivider(uint8_t divider)
{
	CO_BEGIN();

	rawBuffer[4] = divider;

	CO_END_RETURN_CALL(write(Register::SMPLRT_DIV, rawBuffer+4, 1, false));
}

// ----------------------------------------------------------------------------
// MARK: - register access
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Itg3200<I2cMaster>::readStatus()
{
	return read(Register::INT_STATUS, rawBuffer[2]);
}

// MARK: update register
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Itg3200<I2cMaster>::updateRegister(uint8_t index, uint8_t setMask, uint8_t clearMask)
{
	CO_BEGIN();

	rawBuffer[index] = (rawBuffer[index] & ~clearMask) | setMask;

	CO_END_RETURN_CALL(write(Register(index), rawBuffer[index]));
}

// MARK: write multilength register
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Itg3200<I2cMaster>::write(Register reg, uint8_t *buffer, uint8_t length, bool copyBuffer)
{
	CO_BEGIN();

	if (length > 7)
		CO_RETURN(false);

	rawBuffer[3] = uint8_t(reg);
	if (copyBuffer) std::memcpy(rawBuffer+4, buffer, length);

	CO_WAIT_UNTIL(
			adapter.configureWrite(rawBuffer+3, length+1) and
					(i2cTask = I2cTask::WriteRegister, this->startTransaction(&adapter))
	);

	CO_WAIT_WHILE(i2cTask == I2cTask::WriteRegister);

	CO_END_RETURN(i2cSuccess == I2cTask::WriteRegister);
}

// MARK: read multilength register
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Itg3200<I2cMaster>::read(Register reg, uint8_t *buffer, uint8_t length)
{
	CO_BEGIN();

	rawBuffer[3] = uint8_t(reg);

	CO_WAIT_UNTIL(
			adapter.configureWriteRead(rawBuffer+3, 1, buffer, length) and
					(i2cTask = I2cTask::ReadRegister, this->startTransaction(&adapter))
	);

	CO_WAIT_WHILE(i2cTask == I2cTask::ReadRegister);

	CO_END_RETURN(i2cSuccess == I2cTask::ReadRegister);
}
