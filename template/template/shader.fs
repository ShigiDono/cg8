#version 330 core

in vec3 gl_Color;
// Ouput data
out vec3 color;

void main()
{

	// Output color = red 
	color = gl_Color;//vec3(1,0,0);

}