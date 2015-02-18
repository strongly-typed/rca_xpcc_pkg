// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically from robot_packets.hpp.tpl.
 * Do not edit! Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

#ifndef	{{ namespace | upper }}_PACKETS_HPP
#define	{{ namespace | upper }}_PACKETS_HPP

#include <stdint.h>
#include <cstdlib>
#include <xpcc/io/iostream.hpp>

namespace {{ namespace }}
{
	namespace packet
	{
{%- for packet in packets %}
{%- if packet.isBuiltIn %}{% continue %}{% endif %}
		{%- if packet.description %}
		/** {{ packet.description | xpcc.wordwrap(67) | xpcc.indent(2, " * ") }} */
		{%- endif %}
	{%- if packet.isEnum %}
		enum {%- if packet.isStronglyTyped %} class{%- endif %}
		{{ packet.name | typeName }}{%- if packet.underlyingType != None %} : {{ packet.underlyingType }}{%- endif %}
		{
			{%- for element in packet.iter() %}
				{%- if element.description %}
			/** {{ element.description | xpcc.wordwrap(63) | xpcc.indent(3, " * ") }} */
				{%- endif %}
				{%- if packet.isStronglyTyped %}
			{{ element.name | enumElementStrong }} = {{ element.value }},
				{%- else %}
			{{ element.name | enumElement }} = {{ element.value }},
				{%- endif %}
			{%- endfor %}
		} {%- if packet.underlyingType == None %} __attribute__((packed)){%- endif %};
		{% if packet.isStronglyTyped %}
		constexpr {{ packet.underlyingType }}
		value({{ packet.name | typeName }} e) {
			return {{ packet.underlyingType }}(e);
		}
		{%- endif %}

		static constexpr int {{ packet.name | typeName }}NumberOfElements = {{ packet.numberOfElements }};

		inline const char*
		enumToString({{ packet.name | typeName }} e)
		{
			switch (e)
			{
		{%- for element in packet.iter() %}
			{%- if packet.isStronglyTyped %}
				{%- set enumName = element.name | enumElementStrong %}
				{%- set prefix = (packet.name | typeName) ~ "::" %}
			{%- else %}
				{%- set enumName = element.name | enumElement %}
				{%- set prefix = "" %}
			{%- endif %}
			{%- if element.string %}
				case {{ prefix ~ enumName }}: return "{{ element.string }}";
			{%- else %}
				case {{ prefix ~ enumName }}: return "{{ prefix ~ enumName }}";
			{%- endif %}
		{%- endfor %}
		{%- if packet.isStronglyTyped %}
				default: return "{{ packet.name | typeName }}::Unknown";
		{%- else %}
				default: return "__UNKNOWN_ENUM__";
		{%- endif %}
			}
		}

		xpcc::IOStream&
		operator << (xpcc::IOStream& s, const {{ packet.name | typeName }} e);

	{% elif packet.isStruct %}
		struct {{ packet.name | typeName }}
		{
			{{ packet.flattened() | generateConstructor }};

			{% if packet.flattened().size > 0 -%}
			{{ packet.flattened() | generateConstructor(default=False) }};
			{%- endif %}
			{% for element in packet.flattened().iter() %}
			{%- if element.description %}
			/** {{ element.description | xpcc.wordwrap(63) | xpcc.indent(3, " * ") }} */
			{%- endif %}
			{{ element | subtype }};
			{%- endfor %}
		} __attribute__((packed));
	{% elif packet.isTypedef %}
		typedef {{ packet.subtype.name | typeName }} {{ packet.name | typeName }};
	{% endif %}
{%- endfor -%}
	} // namespace packet
} // namespace {{ namespace }}

#endif	// {{ namespace | upper }}_PACKETS_HPP
