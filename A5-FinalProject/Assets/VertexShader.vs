#version 330

// Model-Space coordinates
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 inTextureCoord;

struct LightSource {
    vec3 position;
    vec3 rgbIntensity;
};
uniform LightSource light;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Perspective;
uniform mat4 depthView;
uniform mat4 bias;

// Remember, this is transpose(inverse(ModelView)).  Normals should be
// transformed using this matrix instead of the ModelView matrix.
mat3 NormalMatrix;

out VsOutFsIn {
	vec3 position_ES; // Eye-space position
	vec3 normal_ES;   // Eye-space normal
	LightSource light;
	vec2 uv;
	vec4 shadowCoord;
} vs_out;


void main() {
	vec4 pos4 = vec4(position, 1.0);

	NormalMatrix = transpose(inverse(mat3( View * Model)));

	//-- Convert position and normal to Eye-Space:
	vs_out.position_ES = ((View * Model) * pos4).xyz;
	vs_out.normal_ES = normalize(NormalMatrix * normal);

	vs_out.light = light;

	gl_Position = Perspective * View * Model * vec4(position, 1.0);

	vs_out.uv = inTextureCoord;

	vs_out.shadowCoord = (Perspective * depthView)* vec4((Model*pos4).xyz, 1.0);
}
