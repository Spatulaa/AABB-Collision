#version 330 core

layout (location = 0) in vec3 v_pos;

uniform mat4 u_mvp;

void main(void)
{
    gl_Position = u_mvp * vec4(v_pos, 1.0);
}