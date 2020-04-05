#version 330

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform vec3 lightPosition;

in vec4 vertex;
in vec3 normal;
in vec4 colour;

out vec3 varyingNormal;
out vec3 varyingLightDirection;
out vec3 varyingViewerDirection;
out vec4 varyingColour;

void main(void)
{
    varyingColour=colour;
    vec4 eyeVertex = mvMatrix * vertex;
    eyeVertex /= eyeVertex.w;
    varyingNormal = normalMatrix * normal;
    varyingLightDirection = lightPosition - eyeVertex.xyz;
    varyingViewerDirection = -eyeVertex.xyz;
    gl_Position = mvpMatrix * vertex;
}
