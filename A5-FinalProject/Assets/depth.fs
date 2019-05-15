#version 330

//layout (location = 0) out float fragmentDepth;
out vec4 fragmentDepth;

void main() {
	fragmentDepth = vec4(gl_FragCoord.z);

}
