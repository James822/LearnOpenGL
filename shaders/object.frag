#version 330 core

in vec2 text_coord;
in vec3 light_color;

out vec4 frag_color;

uniform sampler2D container_texture;
uniform sampler2D face_texture;

void main()
{
   frag_color = vec4(light_color, 1.0f) * 
      mix(texture(container_texture, text_coord), texture(face_texture, text_coord), 0.2f);
}
