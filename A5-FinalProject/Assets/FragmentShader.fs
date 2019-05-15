#version 330

out vec4 fragColour;

uniform bool picking;
uniform bool isTextureSet;
uniform bool shadowMap;
uniform bool textureMap;
uniform bool allowTransparency;

struct LightSource {
    vec3 position;
    vec3 rgbIntensity;
};

in VsOutFsIn {
    vec3 position_ES; // Eye-space position
    vec3 normal_ES;   // Eye-space normal
    LightSource light;
    vec2 uv;
    vec4 shadowCoord;
} fs_in;

struct Material {
    vec3 kd;
    vec3 ks;
    float shininess;
};
uniform Material material;

uniform sampler2D textureSampler;
uniform sampler2D shadowMapSampler;
uniform float transparency;

// Ambient light intensity for each RGB component.
uniform vec3 ambientIntensity;

float calculateShadow(vec4 shadowCoordinate) {
    
    vec3 normShadowCoordinate = shadowCoordinate.xyz / shadowCoordinate.w;    // normalize

    normShadowCoordinate = normShadowCoordinate * 0.5 + 0.5;        // transform to [0, 1]

    float closestDepth  = texture( shadowMapSampler, normShadowCoordinate.xy).r;
    float currDepth = normShadowCoordinate.z;
    
    float mbias = 0.001;                           // adjust for shadow acne
    float shadow = (currDepth - mbias) > closestDepth  ? 0.7 : 0.0;  

    return shadow;
}

vec3 phongModel(vec3 fragPosition, vec3 fragNormal) {

	LightSource light = fs_in.light;

    // Direction from fragment to light source.
    vec3 l = normalize(light.position - fragPosition);

    // Direction from fragment to viewer (origin - fragPosition).
    vec3 v = normalize(-fragPosition.xyz);

    float n_dot_l = max(dot(fragNormal, l), 0.0);

	vec3 diffuse;
	diffuse = material.kd * n_dot_l;

    vec3 specular = vec3(0.0);

    if (n_dot_l > 0.0) {
		// Halfway vector.
		vec3 h = normalize(v + l);
        float n_dot_h = max(dot(fragNormal, h), 0.0);

        specular = material.ks * pow(n_dot_h, material.shininess);
    }


    return (ambientIntensity + 
        light.rgbIntensity * (diffuse + specular));
}


void main() {

    float shadow;
    if (shadowMap) {
        shadow = calculateShadow(fs_in.shadowCoord);
    } else {
        shadow = 0.0;
    }

    if (!isTextureSet) {                                     // normal colour
        fragColour = vec4(phongModel(fs_in.position_ES, fs_in.normal_ES), 1.0);
    } else if (length(texture(textureSampler, fs_in.uv).rgb -       // invisible colour
        vec3(200/255.0, 191/255.0, 231/255.0)) < 0.1) {
        fragColour = vec4(phongModel(fs_in.position_ES, fs_in.normal_ES), 1.0);
    } else {                                                        // apply texture
        if (textureMap) {
            fragColour = vec4(texture(textureSampler, fs_in.uv).rgb, 1.0);
        } else {
            fragColour = vec4(phongModel(fs_in.position_ES, fs_in.normal_ES), 1.0);
        }
    }

    fragColour = fragColour * (1.0 - shadow);

    if (allowTransparency) {
        fragColour.a = transparency;
    } else {
        fragColour.a = 1.0;
    }

    if (picking) {
        fragColour =  vec4(material.kd, 1.0);
    }

}
