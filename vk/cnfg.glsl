#version 460

#ifdef VERT
#define PASS out
#endif
#ifdef BATCH
#define PASS in
#endif
#ifdef IMG
#define PASS in
#endif

layout (location = 0) PASS vec4 f_color;
#ifdef VERT
// Vertex Shader

layout (set = 0, binding = 0) readonly buffer vert {
	vec4 x;
	float v[];
};

layout (set = 0, binding = 1) readonly buffer color {
	uint c[];
};

vec4 colorHexToVec4(uint hex) {
	vec4 colVec;
	colVec.r = ((hex >> 24) & 0xFF)/255.0f;
	colVec.g = ((hex >> 16) & 0xFF)/255.0f;
	colVec.b = ((hex >> 8) & 0xFF)/255.0f;
	colVec.a = ((hex >> 0) & 0xFF)/255.0f;
	return colVec;
}

void main() {
	vec3 pos = vec3(v[gl_VertexIndex*3], v[gl_VertexIndex*3+1], v[gl_VertexIndex*3+2]);
	uint col = c[gl_VertexIndex];
	f_color = colorHexToVec4(col);
	gl_Position = vec4(pos.xy * x.xy + x.zw, 0, 1);
}

#endif
#ifdef BATCH
// Fragment Shader (Batch)

layout (location = 0) out vec4 i_color;

void main() {
	i_color = f_color;
}

#endif
#ifdef IMG

layout (location = 0) out vec4 i_color;
layout (set = 0, binding = 2) uniform sampler2D f_tex;

void main() {
	i_color = texture(f_tex, f_color.rg).abgr;
}

#endif