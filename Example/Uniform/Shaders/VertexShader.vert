#version 450

#extension GL_ARB_separate_shader_objects : enable

//resource descriptors. 
//A descriptor is a way for shaders to 
//	freely access resources like buffers and images. 
//We're going to set up a buffer 
//	that contains the transformation matrices 
//	and have the vertex shader access them through a descriptor. 

layout(binding =0)	uniform UniformBufferObject
{
	mat4 model;
	mat4 view;
	mat4 proj;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main()
{
	//gl_VertexIndex:the index of the current vertex. 
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
	fragColor = inColor;
}