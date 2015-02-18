// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_GUI_STRINGFIELD_HPP
#define XPCC_GUI_STRINGFIELD_HPP

#include "widget.hpp"
#include "label.hpp"

namespace xpcc
{

namespace gui
{

/**
 * @ingroup	gui
 * @author	Daniel Krebs
 */
class StringField : public Widget
{
public:
	StringField(const char* value, Dimension d) :
		Widget(d, false),
		value(value)
	{
	}

	void
	render(View* view);

	void
	setValue(const char* value)
	{
		if(this->value == value)
			return;
		this->value = value;
		this->markDirty();
	}

	const char*
	getValue()
	{
		return this->value;
	}

private:
	const char* value;
};

}	// namespace gui

}	// namespace xpcc

#endif  // XPCC_GUI_STRINGFIELD_HPP
