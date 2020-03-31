#version 410 core
const int MAX_PARTICLES = 512;

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

//uniform sampler2D tex;
uniform vec3 cam_r;
uniform vec3 cam_u;

uniform mat4 VP_mat;

uniform int num_particles;

layout (location = 0) in vec2[1] size;
layout (location = 0) out vec4 FragPos;
layout (location = 1) out vec2 outCoord;

void main(){
	vec4 vertPos = gl_in[0].gl_Position;
	//first tri
	FragPos = VP_mat * vec4(vertPos.xyz + (cam_r * 0.5 * size[0].x) + (cam_u * 0.5 * size[0].y), 1.0);
	gl_Position = FragPos;
	outCoord = vec2(1.0, 1.0);
	EmitVertex();
	//first vert
	FragPos = VP_mat * vec4(vertPos.xyz + (cam_r * -0.5 * size[0].x) + (cam_u * 0.5 * size[0].y), 1.0);
	gl_Position = FragPos;
	outCoord = vec2(0.0, 1.0);
	EmitVertex();
	//second vert
	FragPos = VP_mat * vec4(vertPos.xyz + (cam_r * 0.5 * size[0].x) + (cam_u * -0.5 * size[0].y), 1.0);
	gl_Position = FragPos;
	outCoord = vec2(1.0, 0.0);
	EmitVertex();
	//third vert
	FragPos = VP_mat * vec4(vertPos.xyz + (cam_r * -0.5 * size[0].x) + (cam_u * -0.5 * size[0].y), 1.0);
	gl_Position = FragPos;
	outCoord = vec2(0.0, 0.0);
	EmitVertex();
	// end first tri
	EndPrimitive();

	////second tri
	////first vert
	//FragPos = VP_mat * vec4(vertPos.xyz + (cam_r * -0.5 * size[0].x) + (cam_u * 0.5 * size[0].y), 1.0);
	//gl_Position = FragPos;
	//outCoord = vec2(0.0, 1.0);
	//EmitVertex();
	////second vert
	//FragPos = VP_mat * vec4(vertPos.xyz + (cam_r * 0.5 * size[0].x) + (cam_u * 0.5 * size[0].y), 1.0);
	//gl_Position = FragPos;
	//outCoord = vec2(1.0, 1.0);
	//EmitVertex();
	////third vert
	//FragPos = VP_mat * vec4(vertPos.xyz + (cam_r * 0.5 * size[0].x) + (cam_u * -0.5 * size[0].y), 1.0);
	//gl_Position = FragPos;
	//outCoord = vec2(0.0, 0.0);
	//EmitVertex();
	//// end second tri
	//EndPrimitive();
}