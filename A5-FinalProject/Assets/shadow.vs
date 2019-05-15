#version 330

in vec3 position;
in vec2 textureCoord;
in vec3 normal;

uniform mat4 ModelView;

out vec2 outTextureCoord;

void main()
{
	gl_Position = ModelView * vec4(position, 1.0);
	outTextureCoord = textureCoord;
}
