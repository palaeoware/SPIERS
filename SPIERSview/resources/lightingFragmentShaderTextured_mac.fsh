#version 150

uniform float ambientReflection;
uniform float diffuseReflection;
uniform float specularReflection;
uniform float shininess;
uniform float alpha;

in vec3 varyingNormal;
in vec3 varyingLightDirection;
in vec3 varyingViewerDirection;
in vec4 varyingColour;

out vec4 fragColor;

void main(void)
{
    vec4 specularColor=varyingColour;
    vec4 diffuseColor=varyingColour/1.5;
    vec4 ambientColor=varyingColour/4.0;

    vec3 normal = normalize(varyingNormal);
    if (!gl_FrontFacing) normal=-normal;
    vec3 lightDirection = normalize(varyingLightDirection);
    vec3 viewerDirection = normalize(varyingViewerDirection);
    vec4 ambientIllumination = ambientReflection * ambientColor;
    vec4 diffuseIllumination = diffuseReflection * max(0.0, dot(lightDirection, normal)) * diffuseColor;
    vec4 specularIllumination = specularReflection * pow(max(0.0, dot(-reflect(lightDirection, normal), viewerDirection)), shininess) * specularColor;
    fragColor=ambientIllumination + diffuseIllumination + specularIllumination;
    fragColor[3]=alpha;
}
