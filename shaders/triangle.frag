#version 330 core

in vec2 text_coord;

out vec4 frag_color;

uniform sampler2D container_texture;
uniform sampler2D face_texture;

void main()
{
   frag_color = mix(texture(container_texture, text_coord), texture(face_texture, text_coord), 0.2f);
} 
