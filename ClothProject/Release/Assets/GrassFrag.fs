#version 330 core

out vec4 fragColor;
uniform sampler2D tex;

in vec3 cColor;
in vec2 texCoords;

void main(void)
{
	//fragColor = vec4(cColor, 1.0f);
	fragColor = texture(tex, texCoords).rgba;
	
}