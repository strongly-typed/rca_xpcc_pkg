// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_STROBE_HPP
#define XPCC_UI_STROBE_HPP

#include <stdint.h>
#include "key_frame.hpp"
#include <xpcc/utils/arithmetic_traits.hpp>

namespace xpcc
{

namespace ui
{

/**
 * This class provided two smooth on and off phases.
 *
 * @code
 *   |--------------------------------- period -|
 *   |-- dc1 --||-- dc2 --||-- dc3 --|
 *    _________             _________             ____
 *  _/         \___________/         \___________/     ...
 *   ^ onFade  ^ offFade   ^ onFade  ^ offFade
 * @endcode
 *
 * @author	Niklas Hauser
 * @ingroup animation
 */
template< typename T = uint8_t >
class Strobe
{
	typedef typename Animation<T>::TimeType TimeType;
public:
	Strobe(Animation<T> &animator)
	:	animator(animator), frames{
			xpcc::ui::KeyFrame<T>(60, xpcc::ArithmeticTraits<T>::max),
			xpcc::ui::KeyFrame<T>(40, xpcc::ArithmeticTraits<T>::max),
			xpcc::ui::KeyFrame<T>(90, xpcc::ArithmeticTraits<T>::min),
			xpcc::ui::KeyFrame<T>(110, xpcc::ArithmeticTraits<T>::min),
			xpcc::ui::KeyFrame<T>(60, xpcc::ArithmeticTraits<T>::max),
			xpcc::ui::KeyFrame<T>(40, xpcc::ArithmeticTraits<T>::max),
			xpcc::ui::KeyFrame<T>(90, xpcc::ArithmeticTraits<T>::min),
			xpcc::ui::KeyFrame<T>(510, xpcc::ArithmeticTraits<T>::min),
		}
	{
		this->animator.setKeyFrames(frames, 8);
		this->animator.setMode(xpcc::ui::KeyFrameAnimationMode::Repeat);
	}

	/// @param	period		in ms
	/// @param	dutyCycle	in percent <= 100
	/// @return	`true` if inputs were correct, `false` otherwise
	inline bool
	setPeriod(TimeType period,
			uint8_t dutyCycle1=7, uint8_t dutyCycle2=10, uint8_t dutyCycle3=10)
	{
		uint8_t remaining = 100;
		if (dutyCycle1 > 100) dutyCycle1 = 100;
		remaining -= dutyCycle1;
		frames[1].length = static_cast<uint32_t>(period) * dutyCycle1 / 100;
		// check if enough length for fading
		uint32_t fadeLength = (frames[0].length + frames[2].length);
		if (frames[1].length <= fadeLength) frames[1].length = 0;
		else frames[1].length -= fadeLength;

		if (dutyCycle2 > remaining) return false;
		remaining -= dutyCycle2;
		frames[3].length = static_cast<uint32_t>(period) * dutyCycle2 / 100;

		if (dutyCycle3 > remaining) return false;
		remaining -= dutyCycle3;
		frames[5].length = static_cast<uint32_t>(period) * dutyCycle3 / 100;
		if (frames[5].length <= fadeLength) frames[1].length = 0;
		else frames[5].length -= fadeLength;

		frames[7].length = period - fadeLength * 2 - frames[1].length - frames[5].length;
		return true;
	}

	/// @param	onFade		the time is takes to turn on in ms
	/// @param	offFade		the time is takes to turn off in ms
	inline void
	setFadeTimes(TimeType onFade, TimeType offFade)
	{
		frames[1].length += (frames[0].length + frames[2].length);
		frames[0].length = onFade;
		frames[2].length = offFade;
		uint32_t fadeLength = (onFade + offFade);
		if (frames[1].length <= fadeLength) frames[1].length = 0;
		else frames[1].length -= fadeLength;

		frames[5].length += (frames[4].length + frames[6].length);
		frames[4].length = onFade;
		frames[6].length = offFade;
		if (frames[5].length <= fadeLength) frames[5].length = 0;
		else frames[5].length -= fadeLength;
	}

	void inline
	setRange(T minValue, T maxValue)
	{
		frames[0].value[0] = maxValue;
		frames[1].value[0] = maxValue;
		frames[2].value[0] = minValue;
		frames[3].value[0] = minValue;
		frames[4].value[0] = maxValue;
		frames[5].value[0] = maxValue;
		frames[6].value[0] = minValue;
		frames[7].value[0] = minValue;
	}

	/// start indicating for ever
	inline void
	start(uint8_t repeat=0)
	{
		animator.start(repeat);
	}

	/// Stops indicating after finishing the current cycle
	inline void
	stop()
	{
		animator.stop();
	}

	inline void
	cancel()
	{
		animator.cancel();
	}

	inline bool
	isAnimating() const
	{
		return animator.isAnimating();
	}

	/// Must be called at least every ms
	void inline
	update()
	{
		animator.update();
	}

private:
	xpcc::ui::KeyFrameAnimation<T> animator;
	// 1. fade up to max
	// 2. stay at max (on1)
	// 3. fade down to min
	// 4. stay at min (pause)
	// 5. fade up to max
	// 6. stay at max (on2)
	// 7. fade down to min
	// 8. stay at min
	// (cycle)
	xpcc::ui::KeyFrame<T> frames[8];
};

}

}

#endif	// XPCC_UI_STROBE_HPP
