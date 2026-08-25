#version 330 core

// ---------------------------------------------------------------------------
// Shadow depth pass fragment shader
// Depth is written automatically to the depth attachment of the shadow FBO.
// No colour output needed.
// ---------------------------------------------------------------------------

void main(void)
{
    // gl_FragDepth is written automatically — nothing to do here.
    // Explicit write would be: gl_FragDepth = gl_FragCoord.z;
}
