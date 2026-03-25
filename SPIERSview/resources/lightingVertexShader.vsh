#version 330 core

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;

in vec4 vertex;
in vec3 normal;

out vec3 varyingNormal;
out vec3 varyingViewDir;
out vec3 varyingFragPos;

void main(void)
{
    vec4 eyeVertex = mvMatrix * vertex;
    varyingFragPos = eyeVertex.xyz / eyeVertex.w;
    varyingNormal = normalMatrix * normal;
    varyingViewDir = -varyingFragPos;
    gl_Position = mvpMatrix * vertex;
}
