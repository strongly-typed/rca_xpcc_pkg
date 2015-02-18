// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_GUI_COLORPALETTE_HPP
#define XPCC_GUI_COLORPALETTE_HPP

#include <xpcc/ui/display/graphic_display.hpp>


namespace xpcc
{

namespace gui
{

class ColorPalette;
extern ColorPalette DefaultColorPalette;

/*
 * TODO: find a way so that color options can be defined in user program
 */
enum
Color {
	BLACK,
	WHITE,
	GRAY,
	RED,
	GREEN,
	BLUE,
	YELLOW,
	BORDER,
	TEXT,
	BACKGROUND,
	ACTIVATED,
	DEACTIVATED,
	PALETTE_SIZE
};

/// @author	Niklas Hauser
/// @ingroup	gui
class ColorPalette
{
public:
	ColorPalette(xpcc::glcd::Color colors[Color::PALETTE_SIZE]) :
		colors(colors)
	{
	}

	ColorPalette(ColorPalette &rhs = DefaultColorPalette) :
		colors(rhs.colors)
	{
	}

	ColorPalette&
	operator=(ColorPalette &rhs)
	{
		colors = rhs.colors;
		return *this;
	}

	void
	setColor(Color name, xpcc::glcd::Color color)
	{
		if (name < Color::PALETTE_SIZE)
		{
			colors[name] = color;
		}
	}

	const xpcc::glcd::Color
	getColor(Color name) const
	{
		if (name >= Color::PALETTE_SIZE)
			return xpcc::glcd::Color(0xffff);
		return colors[name];
	}

	const xpcc::glcd::Color
	operator[](Color name)
	{
		return getColor(name);
	}

	const xpcc::glcd::Color*
	getPointer() const
	{
		return colors;
	}

private:
	xpcc::glcd::Color *colors;
};

}	// namespace gui

}	// namespace xpcc

#endif  // XPCC_GUI_COLORPALETTE_HPP
