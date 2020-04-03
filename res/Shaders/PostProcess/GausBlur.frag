#version 430
uniform bool  isHorizontal;
const float weights[9] = float[](0.000088	,0.002289,	0.023205,	0.092566,	0.146634,	0.092566,	0.023205,	0.002289,	0.000088);

layout (location = 1) in vec2 texCoord;

layout (location = 0) out vec4 fragColor;

uniform sampler2D INPUT_0;

void main() 
{
vec2 off = 1.0 / textureSize(INPUT_0, 0);
if (isHorizontal) 
{ 
off = off * vec2(1, 0); 
}
else 
{ 
off = off * vec2(0, 1); 
}
vec4 result = texture(INPUT_0, texCoord) * weights[0];
for(int i = 1; i < 5; i++) 
{ 
result += texture(INPUT_0, texCoord + off * i) * weights[i]; 
result += texture(INPUT_0, texCoord - off * i) * weights[i];    
}    
fragColor = vec4(result.rgb, 1);
}