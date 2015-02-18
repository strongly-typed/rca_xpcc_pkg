// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_GUI_BUTTON_HPP
#define XPCC_GUI_BUTTON_HPP

#include <xpcc/ui/display/font.hpp>
#include "widget.hpp"

namespace xpcc
{

namespace gui
{

/**
 * @ingroup	gui
 * @author	Daniel Krebs
 */
class ButtonWidget : public Widget
{
public:
	ButtonWidget(const char* lbl, Dimension d) :
		Widget(d, true),
		label(lbl)
	{
	}

	void
	render(View* view);

	void
	setLabel(char* lbl)
	{
		this->label = lbl;
	}

private:
	const char *label;
};

/**
 * @ingroup	gui
 * @author	Daniel Krebs
 */
class ArrowButton : public Widget
{
public:
	ArrowButton(bool orientation, Dimension d) :
		Widget(d, true),
		orientation(orientation)
	{
	}

	void
	render(View* view);

private:
	// Orientation of the arrow. false: left | true: right
	bool orientation;
};

/**
 * @ingroup	gui
 * @author	Daniel Krebs
 */
class FilledAreaButton : public Widget
{
public:
	FilledAreaButton(xpcc::glcd::Color color, Dimension d) :
		Widget(d, true),
		color(color)
	{
	}

	void
	setBackgroundColor(xpcc::glcd::Color color)
	{
		this->color = color;
	}

	void
	render(View* view);

private:
	xpcc::glcd::Color color;
};

}	// namespace gui

}	// namespace xpcc

#endif  // XPCC_GUI_BUTTON_HPP
