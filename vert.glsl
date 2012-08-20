#version 120

uniform		mat4	mvp;
attribute	vec3	pos;
attribute	vec3	col;
varying		vec4	color;

void main()
{
	gl_Position = mvp * vec4(pos, 1.0);
	color = vec4(col, 1.0);
}

