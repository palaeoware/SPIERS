#version 330 core

// ---------------------------------------------------------------------------
// Cook-Torrance BRDF fragment shader — multi-light + optional shadow mapping
//
// Compiled in three variants via preprocessor defines prepended at runtime:
//   #define SHADOWS_ENABLED 0  — no shadow code, zero overhead
//   #define SHADOWS_ENABLED 1, PCF_ENABLED 0  — hard shadows
//   #define SHADOWS_ENABLED 1, PCF_ENABLED 1  — soft shadows (PCF)
//
// Shadow notes:
//   Each shadow-casting light has its own depth texture (sampler2DShadow)
//   and light-space matrix. sampler2DShadow performs hardware depth
//   comparison, returning 1.0 (lit) or 0.0 (shadowed).
//   PCF samples a 3x3 neighbourhood and averages results for soft edges.
// ---------------------------------------------------------------------------

#define MAX_LIGHTS 3

// Material
uniform vec3  albedo;
uniform float roughness;
uniform float metallic;
uniform float alpha;
uniform float ambientStrength;

// Lighting
uniform vec3  lightDirections[MAX_LIGHTS];
uniform vec3  lightColors[MAX_LIGHTS];
uniform int   lightCount;

#if SHADOWS_ENABLED
// One shadow map per light slot — bound/unbound based on which lights
// have shadows enabled. Lights without shadows use a dummy 1x1 white texture.
uniform sampler2DShadow shadowMaps[MAX_LIGHTS];
uniform int   lightCastsShadow[MAX_LIGHTS]; // 1 if this light casts shadows
uniform float shadowBias;                   // typically 0.005
#if PCF_ENABLED
uniform float shadowMapTexelSize;           // 1.0 / SHADOW_MAP_SIZE (e.g. 1/2048)
#endif
#endif

in vec3 varyingNormal;
in vec3 varyingViewDir;
in vec3 varyingFragPos;

#if SHADOWS_ENABLED
in vec4 fragPosLightSpace[MAX_LIGHTS];
#endif

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
// Schlick-GGX Geometry Function
// ---------------------------------------------------------------------------
float geometrySchlickGGX(float NdotV, float rough)
{
    float r = rough + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float geometrySmith(float NdotV, float NdotL, float rough)
{
    return geometrySchlickGGX(NdotV, rough)
         * geometrySchlickGGX(NdotL, rough);
}

// ---------------------------------------------------------------------------
// Fresnel-Schlick
// ---------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// ---------------------------------------------------------------------------
// Shadow sampling
// ---------------------------------------------------------------------------
#if SHADOWS_ENABLED
float sampleShadow(int lightIndex, vec4 lightSpacePos, float NdotL)
{
    // Perspective divide to NDC, then remap [-1,1] -> [0,1] for texture lookup
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Fragment outside the light frustum — treat as fully lit
    if (projCoords.z > 1.0) return 1.0;

    // Slope-scaled bias reduces shadow acne on surfaces angled away from light
    float bias = max(shadowBias * (1.0 - NdotL), shadowBias * 0.1);
    float compareDepth = projCoords.z - bias;

#if PCF_ENABLED
    // 5x5 PCF kernel — average 25 shadow tests for soft edges
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
    // Hard shadow — single sample
    vec3 uvz = vec3(projCoords.xy, compareDepth);
    if      (lightIndex == 0) return texture(shadowMaps[0], uvz);
    else if (lightIndex == 1) return texture(shadowMaps[1], uvz);
    else                      return texture(shadowMaps[2], uvz);
#endif
}
#endif

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------
void main(void)
{
    vec3 N = normalize(varyingNormal);
    if (!gl_FrontFacing) N = -N;

    vec3  V     = normalize(varyingViewDir);
    float NdotV = max(dot(N, V), 0.0);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (i >= lightCount) break;

        vec3  L     = normalize(lightDirections[i]);
        vec3  H     = normalize(V + L);
        float NdotL = max(dot(N, L), 0.0);

        // Cook-Torrance BRDF
        float NDF      = distributionGGX(N, H, roughness);
        float G        = geometrySmith(NdotV, NdotL, roughness);
        vec3  F        = fresnelSchlick(max(dot(H, V), 0.0), F0);
        vec3  specular = (NDF * G * F) / (4.0 * NdotV * NdotL + 0.0001);

        vec3 kD      = (vec3(1.0) - F) * (1.0 - metallic);
        vec3 diffuse = kD * albedo / 3.14159265;

        // Shadow factor — 1.0 fully lit, 0.0 fully shadowed
        float shadowFactor = 1.0;
#if SHADOWS_ENABLED
        if (lightCastsShadow[i] == 1)
            shadowFactor = sampleShadow(i, fragPosLightSpace[i], NdotL);
#endif

        Lo += (diffuse + specular) * lightColors[i] * NdotL * shadowFactor;
    }

    vec3 ambient = ambientStrength * albedo;
    vec3 color   = ambient + Lo;

    // Reinhard tone mapping
    color = color / (color + vec3(1.0));

    // Gamma correction
    color = pow(color, vec3(1.0 / 2.2));

    fragColor = vec4(color, alpha);
}
