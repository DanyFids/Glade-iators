#version 330 core
const int MAX_PARTICLES = 2048;

layout (triangles) in;
layout (triangle_strip, max_vertices = 6144) out;

uniform sampler2D tex;
uniform mat4[MAX_PARTICLES] MVP;

int num_particles;

out vec4 FragPos;
in vec2 texCoord;
out vec2 outCoord;

void main(){
	
	for(int p = 0; p < num_particles && p < MAX_PARTICLES; p++){
		for(int i = 0; i < 3; i++){
			FragPos = gl_in[i].gl_Position;
			gl_Position = MVP[p] * FragPos;

			vec2 texSize = textureSize(tex, 0);
			vec2 frmScl = frame_size / texSize;

			vec2 offset = frame_position * (1.0/texSize);

			vec2 uv = (TexCoords * frmScl) + offset;

			outCoord = texCoord;
			EmitVertex();
		}
		EndPrimitive();
	}
}