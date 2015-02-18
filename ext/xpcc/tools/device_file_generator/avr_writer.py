# -*- coding: utf-8 -*-
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
#
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

from writer import XMLDeviceWriter, XMLElement
import avr_io

import os, sys
# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger

class AVRDeviceWriter(XMLDeviceWriter):
	""" AVRDeviceWriter
	Translates the Device to a XPCC specific format.
	"""
	def __init__(self, device, logger=None):
		XMLDeviceWriter.__init__(self, device, logger)

		self.log.info(("Generating Device File for '%s'." % self.device.ids.string))

		self.types = self.device.ids.getAttribute('type')
		self.pin_ids = self.device.ids.getAttribute('pin_id')
		self.names = self.device.ids.getAttribute('name')
		self.family = self.device.ids.intersection.family

		# search the io dictionary for this device
		# we only need one pin name to identify the device group
		pin_name = self.device.getProperty('pin-name').values[0].value
		self.io = [a for a in avr_io.pins if pin_name in a['devices']]
		if len(self.io) > 0:
			self.io = self.io[0]
		else:
			self.io = {}
			if self.device.id.family != 'xmega':
				self.log.warn("AvrWriter: IO not found for device '%s' with pin-name: '%s'" % (self.device.id.string, pin_name))

		self.addDeviceAttributesToNode(self.root, 'flash')
		self.addDeviceAttributesToNode(self.root, 'ram')
		self.addDeviceAttributesToNode(self.root, 'eeprom')
		self.addDeviceAttributesToNode(self.root, 'core')
		self.addDeviceAttributesToNode(self.root, 'mcu')

		pin_count_child = self.root.addChild('pin-count')
		if self.family == 'xmega':
			# the int in the type is the package id
			# ie. A1, B1 = 100 pins, A3, C3 = 64 pins, etc...
			pins = [0, 100, 0, 64, 44, 32]
			pin_count_child.setValue(pins[int(self.types[0][1:])])
		else:
			# the AT90, ATtiny and ATmega have very weird pin counts, with so many different packages
			pin_count_child.setValue(0)

		for header in ['avr/io.h', 'avr/interrupt.h']:
			header_child = self.root.addChild('header')
			header_child.setValue(header)

		# self.addDeviceAttributesToNode(self.root, 'define')

		core_child = self.root.addChild('driver')
		core_child.setAttributes({'type': 'core', 'name': 'avr'})
		ram_sizes = self.device.getProperty('ram')
		for ram_size in ram_sizes.values:
			size = ram_size.value
			# for large RAM sizes, reserve 1kB for stack
			# for small RAM sizes, reserve half of entire size for stack
			if size > 2048:
				size -= 1024
			else:
				size /= 2
			for id in ram_size.ids.differenceFromIds(self.device.ids):
				attr = self._getAttributeDictionaryFromId(id)
				attr['name'] = 'ram_length'
				ram_size_child = core_child.addChild('parameter')
				ram_size_child.setAttributes(attr)
				ram_size_child.setValue(size)

				attr2 = self._getAttributeDictionaryFromId(id)
				attr2['name'] = 'ram_block_length'
				block_size = 4
				while (size / block_size > 127):
					block_size *= 2

				ram_block_child = core_child.addChild('parameter')
				ram_block_child.setAttributes(attr2)
				ram_block_child.setValue(block_size)

		# ADC
		self.addAdcToNode(self.root)
		# Clock
		clock_child = self.root.addChild('driver')
		clock_child.setAttributes({'type': 'clock', 'name': 'avr'})
		# DAC
		self.addDacToNode(self.root)
		# I2C aka TWI
		self.addI2cToNode(self.root)
		# SPI
		self.addSpiToNode(self.root)
		# Timer
		self.addTimerToNode(self.root)
		# UART
		self.addUartToNode(self.root)
		# USI can be used to emulate UART, SPI and I2C, so there should not be a seperate driver for it.
		# self.addUsiToNode(self.root)
		# GPIO
		self.addGpioToNode(self.root)

	def addDeviceAttributesToNode(self, node, name):
		properties = self.device.getProperty(name)

		if properties == None:
			return

		for prop in properties.values:
			for id in prop.ids.differenceFromIds(self.device.ids):
				attr = self._getAttributeDictionaryFromId(id)
				child = node.addChild(name)
				child.setAttributes(attr)
				child.setValue(prop.value)

	def addModuleAttributesToNode(self, node, peripheral, name, family=None):
		if family == None:
			family = self.family
		modules = self.device.getProperty('modules')

		for prop in modules.values:
			if any(m for m in prop.value if m.startswith(peripheral)):
				for id in prop.ids.differenceFromIds(self.device.ids):
					attr = self._getAttributeDictionaryFromId(id)
					driver = node.addChild('driver')
					driver.setAttributes(attr)
					driver.setAttributes({'type': name, 'name': family})

	def addModuleInstancesAttributesToNode(self, node, peripheral, name, family=None):
		if family == None:
			family = self.family
		modules = self.device.getProperty('modules')

		for prop in modules.values:
			instances = []
			for module in [m for m in prop.value if m.startswith(peripheral)]:
				instances.append(module[len(peripheral):])

			if len(instances) == 0:
				continue
			instances.sort()

			for id in prop.ids.differenceFromIds(self.device.ids):
				attr = self._getAttributeDictionaryFromId(id)

				driver = node.addChild('driver')
				driver.setAttributes(attr)
				driver.setAttributes({'type': name, 'name': family})

				if len(instances) > 0:
					driver.setAttribute('instances', ",".join(instances))

				if name in self.io:
					for io in self.io[name]:
						ch = driver.addChild('gpio')
						ch.setAttributes(io)

	def addI2cToNode(self, node):
		family = 'at90_tiny_mega' if (self.family in ['at90', 'attiny', 'atmega']) else self.family
		if self.family == 'xmega':
			self.addModuleInstancesAttributesToNode(node, 'TWI', 'i2c', family)
		else:
			self.addModuleAttributesToNode(node, 'TWI', 'i2c', family)

	def addSpiToNode(self, node):
		family = 'at90_tiny_mega' if (self.family in ['at90', 'attiny', 'atmega']) else self.family
		if self.family == 'xmega':
			self.addModuleInstancesAttributesToNode(node, 'SPI', 'spi', family)
		else:
			self.addModuleAttributesToNode(node, 'SPI', 'spi', family)

	def addAdcToNode(self, node):
		family = 'at90_tiny_mega' if (self.family in ['at90', 'attiny', 'atmega']) else self.family
		if self.family == 'xmega':
			self.addModuleInstancesAttributesToNode(node, 'ADC', 'adc', family)
		else:
			self.addModuleAttributesToNode(node, 'AD_CONVERTER', 'adc', family)

	def addDacToNode(self, node):
		if self.family == 'xmega':
			self.addModuleInstancesAttributesToNode(node, 'DAC', 'dac')
		else:
			self.addModuleAttributesToNode(node, 'DA_CONVERTER', 'dac')

	def addUsiToNode(self, node):
		if self.family != 'xmega':
			family = 'at90_tiny_mega' if (self.family in ['at90', 'attiny', 'atmega']) else self.family
			self.addModuleAttributesToNode(node, 'USI', 'usi', family)

	def addTimerToNode(self, node):
		if self.family == 'xmega':
			self.addModuleInstancesAttributesToNode(node, 'TC', 'timer')
		else:
			self.addModuleInstancesAttributesToNode(node, 'TIMER_COUNTER_', 'timer')

	def addUartToNode(self, node):
		family = 'at90_tiny_mega' if (self.family in ['at90', 'attiny', 'atmega']) else self.family
		# this is special, some AT90_Tiny_Megas can put their USART into SPI mode
		# we have to parse this specially.
		uartSpi = 'uartspi' in self.io or self.family == 'xmega'
		modules = self.device.getProperty('modules')

		for prop in modules.values:
			instances = []
			for module in [m for m in prop.value if m.startswith('USART')]:
				if self.family == 'xmega':
					instances.append(module[5:7])
				else:
					# some device only have a 'USART', but we want 'USART0'
					mod = module + '0'
					instances.append(mod[5:6])

			if instances != []:
				instances = list(set(instances))
				instances.sort()

				for id in prop.ids.differenceFromIds(self.device.ids):
					attr = self._getAttributeDictionaryFromId(id)

					driver = node.addChild('driver')
					driver.setAttributes(attr)
					driver.setAttributes({'type': 'uart', 'name': family})
					if uartSpi:
						spiDriver = node.addChild('driver')
						spiDriver.setAttributes(attr)
						spiDriver.setAttributes({'type': 'spi', 'name': family + "_uart"})

					driver.setAttribute('instances', ",".join(instances))
					if uartSpi:
						spiDriver.setAttribute('instances', ",".join(instances))

					ram_sizes = self.device.getProperty('ram')
					for ram_size in ram_sizes.values:
						size = ram_size.value
						# for small RAM sizes, reserve only 16 bytes for the tx buffer
						if size < 1024 or size > 1024*4:
							for ram_id in ram_size.ids.differenceFromIds(self.device.ids):
								attr = self._getAttributeDictionaryFromId(ram_id)
								attr['name'] = 'tx_buffer'
								ram_size_child = driver.addChild('parameter')
								ram_size_child.setAttributes(attr)
								ram_size_child.setValue(16 if size < 1024 else 250)

	def addGpioToNode(self, node):
		family = 'at90_tiny_mega' if (self.family in ['at90', 'attiny', 'atmega']) else self.family
		props = self.device.getProperty('gpios')

		driver = node.addChild('driver')
		driver.setAttributes({'type': 'gpio', 'name': family})

		for prop in props.values:
			gpios = prop.value
			gpios.sort(key=lambda k: (k['port'], k['id']))
			for id in prop.ids.differenceFromIds(self.device.ids):
				dict = self._getAttributeDictionaryFromId(id)
				for gpio in gpios:
					gpio_child = driver.addChild('gpio')
					gpio_child.setAttributes(dict)
					for name in ['port', 'id', 'pcint', 'extint']:
						if name in gpio:
							gpio_child.setAttribute(name, gpio[name])
					for af in gpio['af']:
						af_child = gpio_child.addChild('af')
						af_child.setAttributes(af)

	def _getAttributeDictionaryFromId(self, id):
		target = id.properties
		dict = {}
		for attr in target:
			if target[attr] != None:
				if attr == 'type':
					dict['device-type'] = target[attr]
				if attr == 'name':
					dict['device-name'] = target[attr]
				if attr == 'pin_id':
					dict['device-pin-id'] = target[attr]
		return dict

	def write(self, folder):
		names = self.names
		names.sort(key=int)
		types = self.types
		name = self.family + "-".join(["_".join(names), "_".join(types)]) + ".xml"
		if self.family == 'xmega':
			name = name[:-4] + "-" + "_".join(self.pin_ids) + ".xml"
		self.writeToFolder(folder, name)

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return "AVRDeviceWriter(\n" + self.toString() + ")"
