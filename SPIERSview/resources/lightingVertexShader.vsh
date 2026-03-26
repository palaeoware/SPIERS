#version 330 core

// ---------------------------------------------------------------------------
// Lighting vertex shader — with optional shadow map support
//
// When SHADOWS_ENABLED=1, also transforms each vertex into each light's
// clip space and passes it to the fragment shader for shadow map lookup.
// When SHADOWS_ENABLED=0, this extra code is compiled out entirely.
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
    vec4 eyeVertex    = mvMatrix * vertex;
    varyingFragPos    = eyeVertex.xyz / eyeVertex.w;
    varyingNormal     = normalMatrix * normal;
    varyingViewDir    = -varyingFragPos;

#if SHADOWS_ENABLED
    // Transform vertex into each light's clip space for shadow lookup.
    // lightSpaceMatrices[i] = lightOrthoProjection * lightViewMatrix
    // We use the raw vertex here (world space) not eye space.
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (i >= lightCount) break;
        fragPosLightSpace[i] = lightSpaceMatrices[i] * vertex;
    }
#endif

    gl_Position = mvpMatrix * vertex;
}
