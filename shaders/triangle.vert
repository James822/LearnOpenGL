#version 330 core

layout (location = 0) in vec3 va_pos;
layout (location = 1) in vec3 va_vert_color;
layout (location = 2) in vec2 va_text_coord;

out vec3 vert_color;
out vec2 text_coord;

uniform mat4 transform;

void main()
{
   gl_Position = transform * vec4(va_pos, 1.0);
   vert_color = va_vert_color;
   text_coord = va_text_coord;
}
