#version 330 core

uniform vec3 light_color;
uniform float ambient_light_strength;

in vec2 text_coord;

out vec4 frag_color;

uniform sampler2D container_texture;
uniform sampler2D face_texture;

void main()
{
   vec3 phong_ambient = ambient_light_strength * light_color;
   frag_color = vec4(phong_ambient, 1.0f) * 
      mix(texture(container_texture, text_coord), texture(face_texture, text_coord), 0.2f);
}
