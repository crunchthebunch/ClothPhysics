#version 450 core

in vec3 fragColor;
in vec3 fragVertex;
in vec2 fragTexCoord;

out vec4 color;

uniform vec3 camPos;
uniform float currentTime;
uniform sampler2D tex;

void main(void)
{
	//Fog
	float d = distance(fragVertex.xyz, camPos);
	float lerp = (d - 10.0f)/20.0f;
	lerp = clamp(lerp, 0.0f, 1.0f);
	vec4 fogColor = vec4(0.5f,0.5f,0.5f,1.0f);
	
	color = mix(texture(tex, fragTexCoord).rgba,fogColor,lerp);
}