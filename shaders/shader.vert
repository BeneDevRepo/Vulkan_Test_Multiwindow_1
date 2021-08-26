#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

// push constants block
layout(push_constant) uniform constants {
	vec2 scale;
	vec2 offset;
} pushConstants;

// vec2 positions[6] = vec2[](
// 	vec2(-0.5, -0.5),
// 	vec2(0.5,  -0.5),
// 	vec2(0.5, 0.5),

// 	vec2(-0.5, -0.5),
// 	vec2(0.5,  0.5),
// 	vec2(-0.5, 0.5)
// );

// vec3 colors[6] = vec3[](
// 	vec3(1.0, 0.0, 0.0),
// 	vec3(0.0, 1.0, 0.0),
// 	vec3(0.0, 0.0, 1.0),

// 	vec3(1.0, 0.0, 0.0),
// 	vec3(0.0, 0.0, 1.0),
// 	vec3(1.0, 1.0, 1.0)
// );

// vec2 positions[3] = vec2[](
// 	vec2(0.0, -0.5),
// 	vec2(0.5,  0.5),
// 	vec2(-0.5, 0.5)
// );

// vec3 colors[3] = vec3[](
// 	vec3(1.0, 0.0, 0.0),
// 	vec3(0.0, 1.0, 0.0),
// 	vec3(0.0, 0.0, 1.0)
// );

void main() {
	gl_Position = vec4(pushConstants.offset + inPosition, 0.0, 1.0);
    fragColor = inColor;
    // gl_Position = vec4(pushConstants.offset + positions[gl_VertexIndex], 0., 1.0);
    // fragColor = colors[gl_VertexIndex];
    // fragColor = vec3(1., 0., 0.);
	gl_Position.xy *= pushConstants.scale;
}