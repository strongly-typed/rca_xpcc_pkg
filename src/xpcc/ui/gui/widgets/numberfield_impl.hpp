// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_GUI_NUMBERFIELD_HPP
#   error	"Don't include this file directly, use 'numberfield.hpp' instead!"
#endif

template<typename T>
void
xpcc::gui::NumberField<T>::render(View* view)
{
	if(view == 0)
		return;

	// output device of view
	xpcc::GraphicDisplay* out = &view->display();

	// color palette of view
	ColorPalette cp = this->color_palette;

	const uint16_t box_width = this->dimension.width - 4;
	const uint16_t box_height = this->dimension.height;
	const uint16_t box_x = this->getPosition().x + 2;
	const uint16_t box_y = this->getPosition().y;

	// clear background
	out->setColor(cp[Color::BACKGROUND]);
	out->fillRectangle(this->getPosition(), this->getWidth(), this->getHeight());

	// draw box
	out->setColor(cp[Color::BORDER]);
	out->drawLine(box_x, box_y, box_x + box_width - 1, box_y);
	out->drawLine(box_x, box_y, box_x, box_y + box_height - 1);
	out->drawLine(box_x + box_width - 1, box_y + box_height - 1, box_x + box_width - 1, box_y);
	out->drawLine(box_x + box_width, box_y + box_height - 1, box_x, box_y + box_height - 1);

	// draw number
	const uint16_t stringHeight = out->getFontHeight();

	out->setColor(cp[Color::TEXT]);
	out->setCursor(box_x + 10, box_y + (box_height - stringHeight) / 2);
	*out << this->value;
}
