#version 460 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 VertexTexCoord;

layout (location = 0) out vec2 FragCoord;

out gl_PerVertex
{
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[1];
};


void main()
{
	FragCoord = VertexTexCoord;
	gl_Position = vec4(VertexPosition, 1.0);
}
