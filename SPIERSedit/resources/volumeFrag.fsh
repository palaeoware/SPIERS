#version 330 core

in vec3 vPos;

out vec4 fragColor;

uniform sampler3D volumeTex;
uniform vec3 cameraPosObj;  // camera position in object space [0,1]^3
uniform float stepSize;     // ray march step size (fraction of volume)
uniform vec3 volScale;      // aspect ratio correction (w/d, h/d, 1.0)

void main()
{
    vec3 rayOrigin = vPos;
    vec3 rayDir = normalize(vPos - cameraPosObj);

    // Intersect ray with the unit cube [0,1]^3
    vec3 invDir = 1.0 / rayDir;
    vec3 t0 = (vec3(0.0) - rayOrigin) * invDir;
    vec3 t1 = (vec3(1.0) - rayOrigin) * invDir;
    vec3 tMin = min(t0, t1);
    vec3 tMax = max(t0, t1);
    float tNear = max(max(tMin.x, tMin.y), tMin.z);
    float tFar  = min(min(tMax.x, tMax.y), tMax.z);

    if (tNear >= tFar) discard;
    tNear = max(tNear, 0.0);

    vec4 acc = vec4(0.0);
    float t = tNear + stepSize * 0.5;

    for (int i = 0; i < 1024 && t < tFar; i++, t += stepSize)
    {
        vec3 samplePos = rayOrigin + t * rayDir;
        vec4 s = texture(volumeTex, samplePos);

        if (s.a > 0.01)
        {
            // Front-to-back compositing
            float alpha = s.a * (1.0 - acc.a);
            acc.rgb += alpha * s.rgb;
            acc.a   += alpha;

            if (acc.a > 0.99) break;
        }
    }

    if (acc.a < 0.01) discard;
    acc.rgb /= acc.a;  // un-premultiply for display
    fragColor = acc;
}
