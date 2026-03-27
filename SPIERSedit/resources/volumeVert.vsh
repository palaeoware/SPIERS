#version 330 core

in vec3 position;

out vec3 vPos;

uniform mat4 mvp;

void main()
{
    // position is in [-0.5, 0.5]; shift to [0, 1] for texture space
    vPos = position + vec3(0.5);
    gl_Position = mvp * vec4(position, 1.0);
}
