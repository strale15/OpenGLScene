#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

out vec2 UV;
out vec3 vWorldSpaceFragment;
out vec3 vWorldSpaceNormal;

out vec4 fragPosLight;
out vec4 fragPosLight2;
out vec4 fragPosLight3;

uniform mat4 lightProjection;
uniform mat4 lightProjection2;
uniform mat4 lightProjection3;

void main() {
	vWorldSpaceFragment = vec3(uModel * vec4(aPos, 1.0f));
	vWorldSpaceNormal = normalize(mat3(transpose(inverse(uModel))) * aNormal);

	fragPosLight = lightProjection * vec4(vWorldSpaceFragment, 1.0f);
	fragPosLight2 = lightProjection2 * vec4(vWorldSpaceFragment, 1.0f);
	fragPosLight3 = lightProjection3 * vec4(vWorldSpaceFragment, 1.0f);

	UV = aUV;
	gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);
}