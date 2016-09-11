#version 330 core

uniform float ambient_coefficient;
uniform vec4 base_color;

out vec4 out_color;

void main() {
  vec4 total_color = vec4(ambient_coefficient * base_color);
  out_color = total_color;
}
