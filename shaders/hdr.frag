#version 330 core

#define GAMMA 2.2

in vec2 UV;

out vec4 out_color;

uniform sampler2D hdrBuffer;

void main() {
  vec3 hdr_color = vec3(texture(hdrBuffer, UV));
  vec3 reinhard_mapped = hdr_color / (hdr_color + vec3(1.0));
  vec3 gamma_corrected = pow(reinhard_mapped, vec3(1.0 / GAMMA));
  out_color = vec4(gamma_corrected, 1.0);
}