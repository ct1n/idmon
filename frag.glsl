#version 120

varying		vec4	color;

void main()
{
	float tr = floor(mod(gl_FragCoord.y, 2.0));
	gl_FragColor = vec4(color.r, color.g, color.b, 1.0);
}

