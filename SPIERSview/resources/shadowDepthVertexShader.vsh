#version 330 core

// ---------------------------------------------------------------------------
// Shadow depth pass vertex shader.
// lightSpaceMatrix already incorporates the per-object model transform —
// it is set per draw call as (lightOrtho * lightView * modelMatrix).
// ---------------------------------------------------------------------------

uniform mat4 lightSpaceMatrix;

in vec4 vertex;

void main(void)
{
    gl_Position = lightSpaceMatrix * vertex;
}
