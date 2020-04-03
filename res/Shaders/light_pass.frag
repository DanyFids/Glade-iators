#version 430
in vec2 texCoord;

out vec4 outColor;

struct PointLight{
	vec3 position;

	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	samplerCube depthMap;
};

struct Camera{
	vec3 position;
	vec3 dir;

	mat4 iView;
	mat4 iProj;
	mat4 iVP;
};

const int MAX_LIGHTS = 28;
const int MAX_CAMS = 2;

uniform PointLight lights[MAX_LIGHTS];
uniform int num_lights;
uniform float far_plane;
uniform Camera cam[MAX_CAMS];
uniform int num_cams;

uniform sampler2D normals;
uniform	sampler2D depth;
uniform	sampler2D spec;

//float shadow_bias = 0.005;

float ShadowCalc(vec3 fragPos, int light_id){
	float shadowPen = 0.0001;

	vec3 fragToLight = fragPos - lights[light_id].position;
	fragToLight = fragToLight * vec3(1, -1, -1);
	float closestDepth = texture(lights[light_id].depthMap, normalize(fragToLight)).r;
	closestDepth *= far_plane;

	float curDepth = length(fragToLight);

	float shadow = 0.0;



	if ((curDepth + shadowPen) < closestDepth)
	    shadow = 1.0;
	else
	    shadow = 0.0;

	return shadow;
}

// Calculates a world position from the main camera's depth buffer
vec4 GetWorldPos(vec2 uv) {
	int c;

	if(uv.x < 1.0 / num_cams) c = 0;
	else c = 1;
	// Get the depth buffer value at this pixel.    
	float zOverW = texture(depth, uv).r * 2 - 1; // Convert depth from normal to clip space
	// Our clip space position (in the [-1,1] range)
	vec4 currentPos = vec4(uv.xy * 2 - 1, zOverW, 1); 
	// Transform by the view-projection inverse.    
	vec4 D = cam[c].iVP * currentPos; 
	// Divide by w to get the world position.    
	vec4 worldPos = D / D.w;
	return worldPos;
}

void main(){
	int cid;
	
	if(texCoord.x <= 1.0 / num_cams) cid = 0;
	else cid = 1;
	
	if(texture(normals, texCoord).xyz == vec3(0.0,0.0,0.0)){
		outColor = vec4(1.0, 1.0, 1.0, 1.0);
		return;
	}

	vec3 fragPos = GetWorldPos(texCoord).xyz;
	vec3 viewDir = normalize(cam[cid].position - fragPos.xyz);
	vec3 norm = (texture(normals, texCoord).xyz - 0.5) * 2;
	vec4 specTex = texture(spec, texCoord);
	vec3 specVal = specTex.xyz;
	float matVal = specTex.w * 256;
	
	vec3 result = vec3(0.0);
	
	for(int c = 0; c < num_lights; c++){
		vec3 lightDir = normalize(lights[c].position - fragPos);
		vec3 halfWay = normalize(viewDir + lightDir);
	
		float dist = length(lights[c].position - fragPos);
		float attenuation = 1.0 / (1.0 + lights[c].linear * dist + lights[c].quadratic * (dist * dist));
	
		// Ambient Light
		vec3 p_ambient = lights[c].ambient;
	
		// Diffuse Light
		float p_diff = max(dot(norm, lightDir), 0.0);
		vec3 p_diffuse = lights[c].diffuse * p_diff;
	
		// Specular Light
		vec3 reflectDir = reflect(-lightDir, norm);
	
		float p_spec = pow(max(dot(norm, halfWay), 0.0), matVal);
		vec3 p_specular = p_spec * lights[c].specular * specVal;
	
		float p_r = 1.0 - dot(viewDir, norm);
		vec3 p_rim = p_r * lights[c].diffuse;

		float shadow = ShadowCalc(fragPos, c);
	
		result += p_ambient + shadow * (p_diffuse + p_specular + p_rim) * attenuation;
		//pointColor = vec3(shadow, shadow, shadow);
	}

	outColor = vec4(result, 1.0);
}