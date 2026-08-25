#version 330 core

// ---------------------------------------------------------------------------
// OIT accumulation pass vertex shader
// Identical to the lighting vertex shader — reuses the same varyings.
// Shadow support via preprocessor defines, same as lighting shader.
// ---------------------------------------------------------------------------

#define MAX_LIGHTS 3

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;

#if SHADOWS_ENABLED
uniform mat4 lightSpaceMatrices[MAX_LIGHTS];
uniform int  lightCount;
#endif

in vec4 vertex;
in vec3 normal;

out vec3 varyingNormal;
out vec3 varyingViewDir;
out vec3 varyingFragPos;

#if SHADOWS_ENABLED
out vec4 fragPosLightSpace[MAX_LIGHTS];
#endif

void main(void)
{
    vec4 eyeVertex = mvMatrix * vertex;
    varyingFragPos = eyeVertex.xyz / eyeVertex.w;
    varyingNormal  = normalMatrix * normal;
    varyingViewDir = -varyingFragPos;

#if SHADOWS_ENABLED
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (i >= lightCount) break;
        fragPosLightSpace[i] = lightSpaceMatrices[i] * vertex;
    }
#endif

    gl_Position = mvpMatrix * vertex;
}
