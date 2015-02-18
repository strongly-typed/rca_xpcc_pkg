// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_GUI_NUMBERROCKER_HPP
#define XPCC_GUI_NUMBERROCKER_HPP

#include "widget.hpp"
#include "button.hpp"
#include "numberfield.hpp"

namespace xpcc
{

namespace gui
{

/**
 * @ingroup	gui
 * @author	Daniel Krebs
 */
template<typename T = int16_t>
class NumberRocker : public WidgetGroup
{
public:
	NumberRocker(T default_value, T step, Dimension d) :
		WidgetGroup(d),
		value(default_value),
		step(step),
		button_increase(true, Dimension(d.height, d.height)),
		button_decrease(false, Dimension(d.height, d.height)),
		num_field(default_value, Dimension(d.width - 2*d.height, d.height))
	{
		button_decrease.cb_activate = &decrease_cb;
		button_increase.cb_activate = &increase_cb;

		this->pack(&button_decrease, xpcc::glcd::Point(0,0));
		this->pack(&button_increase, xpcc::glcd::Point(d.width - d.height,0));
		this->pack(&num_field, xpcc::glcd::Point(d.height, 0));
	}

	void
	increase();

	void
	decrease();

	void
	activate(const InputEvent& ev, void* data);

	void
	deactivate(const InputEvent& ev, void* data);

	T
	getValue()
	{
		return this->value;
	}

private:
	static void
	increase_cb(const InputEvent& ev, Widget* w, void* data);

	static void
	decrease_cb(const InputEvent& ev, Widget* w, void* data);

private:
	T value;
	T step;
	ArrowButton button_increase, button_decrease;
	NumberField<T> num_field;
};

typedef NumberRocker<int16_t> IntegerRocker;
typedef NumberRocker<float> FloatRocker;


}	// namespace gui

}	// namespace xpcc

#include "numberrocker_impl.hpp"

#endif  // XPCC_GUI_NUMBERROCKER_HPP
