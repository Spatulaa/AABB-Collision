#version 330 core

layout (location = 0) out vec4 colour;

uniform int u_colliding;

void main(void)
{
    if (u_colliding == 0)
    {
        colour = vec4(0.0, 1.0, 0.0, 1.0);
    }
    else if (u_colliding == 1)
    {
        colour = vec4(1.0, 0.0, 0.0, 1.0);
    }
}