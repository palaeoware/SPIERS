#version 330 core

uniform vec4 pickColour;
out vec4 fragColor;

void main(void)
{
    fragColor = pickColour;
}
