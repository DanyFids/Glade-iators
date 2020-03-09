#version 450 core

layout (location = 1) in vec2 texCoord;

layout (location = 0) out vec4 fragColor;

uniform sampler2D INPUT_0;

void main()
{
	//https://en.wikipedia.org/wiki/Grayscale#Luma_coding_in_video_systems

	//vec4 Colour = texture(INPUT_0, texCoord);
	//float Brightness = ((Colour.r *0.2126) + (Colour.g * 0.7152) + (Colour.b * 0.0722));
	//
	//if (Brightness > 0.9)
	//{
	//	fragColor = Colour;
	//}
	//else{
	//	fragColor = vec4(0.0);
	//}

	vec3 sampled = texture2D(INPUT_0, texCoord).rgb;    
	// Determine our luminance, based on perceived brightness of colors    
	float luminance = dot(sampled, vec3(0.299, 0.587, 0.114));    
	// Step will return 0 if luminance < threshold, and 1 if otherwise    
	fragColor.rgb = sampled * step(0.3, luminance);    
	fragColor.a = 1.0; 
}