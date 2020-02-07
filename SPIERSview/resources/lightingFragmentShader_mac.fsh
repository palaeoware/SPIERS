#version 330

uniform vec4 ambientColor;
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform float ambientReflection;
uniform float diffuseReflection;
uniform float specularReflection;
uniform float shininess;
uniform float alpha;

in vec3 varyingNormal;
in vec3 varyingLightDirection;
in vec3 varyingViewerDirection;

out vec4 fragColor;

void main(void)
{

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
