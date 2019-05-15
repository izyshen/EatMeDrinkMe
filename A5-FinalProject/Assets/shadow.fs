#version 330

in vec2 textureCoord;
uniform sampler2D shadowMap;

out vec4 fragColour;

void main() 
{
	float depth = texture (shadowMap, textureCoord).x;
	depth = 1.0 - (1.0 - depth) * 25.0;
	fragColour = vec4(depth);
}
