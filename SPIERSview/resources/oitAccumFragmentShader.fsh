#version 330 core

// ---------------------------------------------------------------------------
// OIT weighted blended accumulation fragment shader
// (McGuire & Bavoil 2013)
//
// Instead of blending transparent fragments into the framebuffer directly
// (which requires back-to-front sorting), this shader outputs two values:
//
//   accumTarget (RGBA16F) — weighted sum of (colour * alpha * weight)
//   revealTarget (R16F)   — product of (1 - alpha * weight) across fragments
//
// The weight function w(z, alpha) makes nearer, more opaque fragments
// contribute more strongly. The composite shader later combines these
// with the opaque scene to produce the final image.
//
// Blend equations required (set in C++ before this pass):
//   accumTarget:  GL_ONE, GL_ONE           (additive)
//   revealTarget: GL_ZERO, GL_ONE_MINUS_SRC_COLOR (multiplicative)
//
// Compiled in three variants via preprocessor defines:
//   SHADOWS_ENABLED 0/1, PCF_ENABLED 0/1
// ---------------------------------------------------------------------------

#define MAX_LIGHTS 3

// Material — identical uniforms to lighting shader
uniform vec3  albedo;
uniform float roughness;
uniform float metallic;
uniform float alpha;
uniform float ambientStrength;

// Lighting — identical to lighting shader
uniform vec3  lightDirections[MAX_LIGHTS];
uniform vec3  lightColors[MAX_LIGHTS];
uniform int   lightCount;

#if SHADOWS_ENABLED
uniform sampler2DShadow shadowMaps[MAX_LIGHTS];
uniform int   lightCastsShadow[MAX_LIGHTS];
uniform float shadowBias;
#if PCF_ENABLED
uniform float shadowMapTexelSize;
#endif
#endif

in vec3 varyingNormal;
in vec3 varyingViewDir;
in vec3 varyingFragPos;

#if SHADOWS_ENABLED
in vec4 fragPosLightSpace[MAX_LIGHTS];
#endif

// Two output targets — bound to the OIT FBO colour attachments
layout(location = 0) out vec4 accumTarget;   // GL_COLOR_ATTACHMENT0
layout(location = 1) out vec4 revealTarget;  // GL_COLOR_ATTACHMENT1

// ---------------------------------------------------------------------------
// BRDF functions — identical to lighting shader
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

float geometrySchlickGGX(float NdotV, float rough)
{
    float r = rough + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float geometrySmith(float NdotV, float NdotL, float rough)
{
    return geometrySchlickGGX(NdotV, rough) * geometrySchlickGGX(NdotL, rough);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

#if SHADOWS_ENABLED
float sampleShadow(int lightIndex, vec4 lightSpacePos, float NdotL)
{
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0) return 1.0;
    float bias = max(shadowBias * (1.0 - NdotL), shadowBias * 0.1);
    float compareDepth = projCoords.z - bias;
#if PCF_ENABLED
    float shadow = 0.0;
    for (int x = -2; x <= 2; x++)
    {
        for (int y = -2; y <= 2; y++)
        {
            vec2 offset = vec2(x, y) * shadowMapTexelSize;
            vec3 uvz = vec3(projCoords.xy + offset, compareDepth);
            if      (lightIndex == 0) shadow += texture(shadowMaps[0], uvz);
            else if (lightIndex == 1) shadow += texture(shadowMaps[1], uvz);
            else                      shadow += texture(shadowMaps[2], uvz);
        }
    }
    return shadow / 25.0;
#else
    vec3 uvz = vec3(projCoords.xy, compareDepth);
    if      (lightIndex == 0) return texture(shadowMaps[0], uvz);
    else if (lightIndex == 1) return texture(shadowMaps[1], uvz);
    else                      return texture(shadowMaps[2], uvz);
#endif
}
#endif

/*
// ---------------------------------------------------------------------------
// OIT weight function (McGuire 2013 recommended form)
// Balances near fragments against far fragments.
// Uses eye-space depth (varyingFragPos.z is negative in OpenGL eye space).
// ---------------------------------------------------------------------------
float oitWeight(float a, float z)
{
    // Clamp z to avoid numerical issues at extreme depths
    float depth = clamp(z, 0.1, 500.0);
    return a * max(1e-2, min(3e3, 10.0 / (1e-5 + pow(depth / 5.0, 2.0) + pow(depth / 200.0, 6.0))));
}
*/

//Simpler version - maybe better?
float oitWeight(float a, float z)
{
	return a * clamp(0.03 / (1e-5 + pow(z / 200.0, 4.0)), 1e-2, 3e3);
}

void main(void)
{
    vec3 N = normalize(varyingNormal);
    if (!gl_FrontFacing) N = -N;

    vec3  V     = normalize(varyingViewDir);
    float NdotV = max(dot(N, V), 0.0);
    vec3  F0    = mix(vec3(0.04), albedo, metallic);

    // Compute lit colour using same BRDF as opaque pass
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (i >= lightCount) break;

        vec3  L     = normalize(lightDirections[i]);
        vec3  H     = normalize(V + L);
        float NdotL = max(dot(N, L), 0.0);

        float NDF      = distributionGGX(N, H, roughness);
        float G        = geometrySmith(NdotV, NdotL, roughness);
        vec3  F        = fresnelSchlick(max(dot(H, V), 0.0), F0);
        vec3  specular = (NDF * G * F) / (4.0 * NdotV * NdotL + 0.0001);
        vec3  kD       = (vec3(1.0) - F) * (1.0 - metallic);
        vec3  diffuse  = kD * albedo / 3.14159265;

        float shadowFactor = 1.0;
#if SHADOWS_ENABLED
        if (lightCastsShadow[i] == 1)
            shadowFactor = sampleShadow(i, fragPosLightSpace[i], NdotL);
#endif
        Lo += (diffuse + specular) * lightColors[i] * NdotL * shadowFactor;
    }

    vec3 ambient = ambientStrength * albedo;
    vec3 color   = ambient + Lo;

    // Tone mapping and gamma — match opaque pass
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    // OIT weight — use abs(eye-space z) as depth estimate
    float depth  = abs(varyingFragPos.z);
    float weight = oitWeight(alpha, depth);

    // Output weighted colour to accum target
    // Alpha channel stores weighted alpha for the composite pass
    accumTarget = vec4(color * alpha * weight, alpha * weight);

    // Output reveal factor — multiplicative blend reduces this per fragment
    // Stored as single float in R channel; layout expects vec4 so pad with 1s
    revealTarget = vec4(alpha);
}
