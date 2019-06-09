#version 330 core

out vec4 color;
  
in vec2 fragTexCoord;

uniform sampler2D frameTexture;
uniform vec2 resolution;
uniform float timeMS;
uniform int fxMode;

void main()
{ 
	if (fxMode == 0)
	{
		color = texture(frameTexture, fragTexCoord);
	}
	else
	{
		// distance from center of image, used to adjust blur
		vec2 uv = fragTexCoord.xy;
		float d = length(uv - vec2(0.5,0.5));
		
		// blur amount
		float blur = 0.0;	
		blur = (1.0 + sin(timeMS*6.0)) * 0.5;
		blur *= 1.0 + sin(timeMS*16.0) * 0.5;
		blur = pow(blur, 3.0);
		blur *= 0.05;
		// reduce blur towards center
		blur *= d;
		
		// final color
		vec3 col;
		col.r = texture( frameTexture, vec2(uv.x+blur,uv.y) ).r;
		col.g = texture( frameTexture, uv ).g;
		col.b = texture( frameTexture, vec2(uv.x-blur,uv.y) ).b;
		
		// scanline
		float scanline = sin(uv.y*1200.0)*0.04;
		col -= scanline;
		
		// vignette
		col *= 1.0 - (d * 0.5);
		
		color = vec4(col,1.0);
	}
}