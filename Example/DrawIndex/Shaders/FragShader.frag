#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;

//specify your own output variable for each framebuffer 
//where the layout(location = 0) modifier specifies 
//the index of the framebuffer. 
//The color red is written to this outColor variable 
//that is linked to the first (and only) framebuffer at index 0.

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = vec4(fragColor,1.0);
}