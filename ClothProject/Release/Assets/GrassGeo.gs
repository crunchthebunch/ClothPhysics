#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 mvp;

in VS_OUT{
	in vec3 bColor;
}gs_in[];

out vec3 cColor;
out vec2 texCoords;

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(void)
{
	cColor = gs_in[0].bColor;
	
	float offsetX = rand(gl_in[0].gl_Position.xy) * 1.0f;
	float offsetY = rand(cColor.xz) * -0.5f;
	float offsetZ = rand(cColor.yz) * 1.0f;
	
	if (cColor.r >= 1.0f)
	{
		gl_Position = mvp * gl_in[0].gl_Position + mvp * vec4(-1.0f + offsetX, 0.0f+ offsetY, 0.0f+ offsetZ,0.0f);
		texCoords =  vec2(0.0f, 1.0f);
		EmitVertex();
		gl_Position = mvp * gl_in[0].gl_Position + mvp * vec4(1.0f+ offsetX, 0.0f+ offsetY, 0.0f+ offsetZ,0.0f);
		texCoords =  vec2(1.0f, 1.0f);
		EmitVertex();
		gl_Position = mvp * gl_in[0].gl_Position + mvp * vec4(-1.0f+ offsetX, 2.0f+ offsetY, 0.0f+ offsetZ,0.0f);
		texCoords =  vec2(0.0f, 0.0f);
		EmitVertex();
		gl_Position = mvp * gl_in[0].gl_Position + mvp * vec4(1.0f+ offsetX, 2.0f + offsetY, 0.0f+ offsetZ,0.0f);
		texCoords =  vec2(1.0f, 0.0f);
		EmitVertex();
		EndPrimitive();
	}
}