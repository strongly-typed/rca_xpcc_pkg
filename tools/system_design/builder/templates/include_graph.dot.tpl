digraph include {
	rankdir=LR;
	graph [label="Includes relative to '{{ label_path }}'", splines=ortho, nodesep=0.8]
	node [style="rounded,filled", width=0, height=0, shape=box, fillcolor="#E5E5E5"]
	{%- for e in edges %}
	"{{ e[0] }}" -> "{{ e[1] }}";
	{%- endfor %}
	{% if edges|length < 1 %}
	"No Includes"
	{% endif %}
}
