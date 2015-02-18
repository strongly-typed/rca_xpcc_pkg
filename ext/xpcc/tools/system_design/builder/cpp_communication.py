﻿#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright (c) 2009, Roboterclub Aachen e.V.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of the Roboterclub Aachen e.V. nor the
#    names of its contributors may be used to endorse or promote products
#    derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# -----------------------------------------------------------------------------

import os
import builder_base
import filter.cpp as filter

# -----------------------------------------------------------------------------
def filter_lower(value):
	return value.lower().replace(" ", "_")

# -----------------------------------------------------------------------------
class CppCommunicationBuilder(builder_base.Builder):
	"""
	Generate the whole communication tree. The Output is a class named
	Communication, which contains all concrete Components. They all contain
	methods, which are corresponding to Actions of given Components.
	
	A common call would be like:
	$python java_communication.py  --outpath source/rca/robot --package rca.robot robot.xml;
	"""
	
	
	VERSION = "0.1"
	
	def setup(self, optparser):
		optparser.add_option(
				"--namespace",
				dest = "namespace",
				default = "robot",
				help = "Namespace of the generated identifiers.")

	def generate(self):
		# check the commandline options
		if not self.options.outpath:
			raise builder_base.BuilderException("You need to provide an output path!")

		if self.options.namespace:
			namespace = self.options.namespace
		else:
			raise builder_base.BuilderException("You need to provide a namespace!")

		cppFilter = {
			'camelcase': filter_lower,
			'camelCase': filter.variableName,
			'CamelCase': filter.typeName,
			'CAMELCASE': filter.enumElement,
		}
		template = self.template('templates/cpp_communication.tpl',
								filter = cppFilter)
		
		# Bool has a special status because its primitive but user generated
		# and the only not numerical type
		components = self.tree.components
		
		substitutions = {
			'components': components,
			'events': self.tree.events,
			'namespace': namespace
		}
		
		file = os.path.join(self.options.outpath, 'communication.hpp')
		self.write(file, template.render(substitutions))

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	CppCommunicationBuilder().run()
