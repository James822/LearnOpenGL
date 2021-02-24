#version 330 core

layout (location = 0) in vec3 va_pos;
layout (location = 1) in vec2 va_text_coord;

out vec2 text_coord;

uniform mat4 MVP;

void main()
{
   gl_Position = MVP * vec4(va_pos, 1.0);
   text_coord = va_text_coord;
}
