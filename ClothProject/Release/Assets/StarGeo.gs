#version 430 core

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

out vec3 outColor;

in VS_GS_VERTEX{
	in vec4 vertex;
	in vec3 color;
	in mat4 mvp;
}gs_in[];

void main(void)
{
	outColor = gs_in[0].color;
	
	gl_Position = gs_in[0].vertex + gs_in[0].mvp * vec4(-2.0f,0.0f,0.0f,0.0f); EmitVertex();
	gl_Position = gs_in[0].vertex + gs_in[0].mvp * vec4(2.0f,0.0f,0.0f,0.0f); EmitVertex();
	gl_Position = gs_in[0].vertex + gs_in[0].mvp * vec4(0.0f,3.0f,0.0f,0.0f); EmitVertex();
	EndPrimitive();
	
	gl_Position = gs_in[0].vertex + gs_in[0].mvp * vec4(2.0f,1.5f,0.0f,0.0f); EmitVertex();
	gl_Position = gs_in[0].vertex + gs_in[0].mvp * vec4(-2.0f,1.5f,0.0f,0.0f); EmitVertex();
	gl_Position = gs_in[0].vertex + gs_in[0].mvp * vec4(0.0f,-1.5f,0.0f,0.0f); EmitVertex();
	EndPrimitive();
}