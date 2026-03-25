#version 330 core

// ---------------------------------------------------------------------------
// Cook-Torrance BRDF fragment shader â€” multi-light version
//
// Supports up to MAX_LIGHTS directional (distant) lights.
// Active light count is controlled by the lightCount uniform â€” unused
// slots in the arrays are never evaluated.
//
// The loop is bounded by the compile-time constant MAX_LIGHTS (3), so the
// driver will unroll it entirely. Performance with lightCount=1 is
// identical to a single-light shader.
//
// BRDF components:
//   NDF : GGX/Trowbridge-Reitz normal distribution
//   G   : Smith/Schlick-GGX geometry (masking + shadowing)
//   F   : Fresnel-Schlick approximation
//
// Material parameters:
//   albedo    - object colour, 0-1 per channel
//   roughness - 0.0 (mirror) to 1.0 (fully matte)
//   metallic  - always 0.0 for fossil/biological surfaces
//   alpha     - transparency
//
// Lighting parameters:
//   lightDirections[i] - normalised direction TO light, in eye space
//   lightColors[i]     - RGB intensity (values > 1.0 allowed, tone-mapped)
//   lightCount         - number of active lights (1-3)
//   ambientStrength    - global ambient scale, 0.0-1.0
// ---------------------------------------------------------------------------

#define MAX_LIGHTS 3

uniform vec3  albedo;
uniform float roughness;
uniform float metallic;
uniform float alpha;
uniform float ambientStrength;

uniform vec3  lightDirections[MAX_LIGHTS];
uniform vec3  lightColors[MAX_LIGHTS];
uniform int   lightCount;

in vec3 varyingNormal;
in vec3 varyingViewDir;
in vec3 varyingFragPos;

out vec4 fragColor;

// ---------------------------------------------------------------------------
// GGX Normal Distribution Function
// ---------------------------------------------------------------------------
float distributionGGX(vec3 N, vec3 H, float rough)
{
    float a      = rough * rough;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (3.14159265 * denom * denom);
}

// ---------------------------------------------------------------------------
// Schlick-GGX Geometry Function (single term)
// ---------------------------------------------------------------------------
float geometrySchlickGGX(float NdotV, float rough)
{
    float r = rough + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

// Smith method â€” view and light terms combined
float geometrySmith(float NdotV, float NdotL, float rough)
{
    return geometrySchlickGGX(NdotV, rough)
         * geometrySchlickGGX(NdotL, rough);
}

// ---------------------------------------------------------------------------
// Fresnel-Schlick Approximation
// F0 = 0.04 for all non-metals (correct for bone, shell, rock, etc.)
// ---------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main(void)
{
    vec3 N = normalize(varyingNormal);
    if (!gl_FrontFacing) N = -N;

    vec3 V    = normalize(varyingViewDir);
    float NdotV = max(dot(N, V), 0.0);

    // Base reflectance at normal incidence â€” 0.04 for all non-metals
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    // Accumulate contribution from each active light
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (i >= lightCount) break;

        vec3  L     = normalize(lightDirections[i]);
        vec3  H     = normalize(V + L);
        float NdotL = max(dot(N, L), 0.0);

        // Cook-Torrance specular BRDF
        float NDF     = distributionGGX(N, H, roughness);
        float G       = geometrySmith(NdotV, NdotL, roughness);
        vec3  F       = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3  numerator   = NDF * G * F;
        float denominator = 4.0 * NdotV * NdotL + 0.0001;
        vec3  specular    = numerator / denominator;

        // Energy-conserving diffuse: kD + kS <= 1
        // F represents the specular fraction (kS)
        // Metals have no diffuse component
        vec3 kD      = (vec3(1.0) - F) * (1.0 - metallic);
        vec3 diffuse = kD * albedo / 3.14159265;

        Lo += (diffuse + specular) * lightColors[i] * NdotL;
    }

    // Ambient â€” single global term, not per-light
    vec3 ambient = ambientStrength * albedo;

    vec3 color = ambient + Lo;

    // Reinhard tone mapping â€” prevents highlight blowout without HDR framebuffer
    color = color / (color + vec3(1.0));

    // Gamma correction (approximate sRGB)
    color = pow(color, vec3(1.0 / 2.2));

    fragColor = vec4(color, alpha);
}
