#version 330 core

// ---------------------------------------------------------------------------
// OIT composite pass vertex shader
// Draws a fullscreen quad — no transformation needed.
// Positions are in NDC directly.
// ---------------------------------------------------------------------------

in vec2 vertex;
out vec2 texCoord;

void main(void)
{
    texCoord    = vertex * 0.5 + 0.5;  // remap [-1,1] -> [0,1] for texture lookup
    gl_Position = vec4(vertex, 0.0, 1.0);
}
