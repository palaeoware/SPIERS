#version 330 core

// ---------------------------------------------------------------------------
// OIT composite fragment shader
//
// Combines the weighted accumulation buffers from the OIT accum pass
// with the opaque scene already in the default framebuffer.
//
// Formula (McGuire & Bavoil 2013):
//   averageColor = accumRGB / max(accumA, 1e-4)
//   transmittance = revealR   (product of (1-alpha) across fragments)
//   finalColor = mix(averageColor, opaqueColor, transmittance)
//
// This is blended over the opaque scene using:
//   glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA)
// so the fragment shader outputs (averageColor, 1-transmittance)
// ---------------------------------------------------------------------------

uniform sampler2D accumTexture;   // RGBA16F — weighted colour accumulation
uniform sampler2D revealTexture;  // R16F    — weighted reveal factor

in vec2 texCoord;
out vec4 fragColor;

void main(void)
{
    vec4  accum      = texture(accumTexture,  texCoord);
    float reveal     = texture(revealTexture, texCoord).r;

	// Discard pixels with no transparent fragments � leave opaque scene untouched
	if (reveal >= 0.9999) discard;

    // Avoid division by zero for pixels with no transparent fragments
    // (accum.a will be ~0 there)
    vec3 averageColor = accum.rgb / max(accum.a, 1e-4);

    // reveal is the product of (1 - alpha) — i.e. transmittance
    // (1 - reveal) is the total opacity of the transparent layers
    // Output: colour = averageColor, alpha = (1 - reveal)
    // C++ blend: GL_ONE_MINUS_SRC_ALPHA (dest) + GL_SRC_ALPHA (src)
    // => result = averageColor * (1-reveal) + opaqueColor * reveal
    fragColor = vec4(averageColor, 1.0 - reveal);
}
