#version 330 core

#define GAMMA 2.2

in vec2 UV;

out vec4 out_color;

uniform usampler2D bayer_matrix;
uniform sampler2D hdrBuffer;
uniform uvec2 dimensions;

void main() {
  // Gamma correction and tone mapping.
  vec3 hdr_color = vec3(texture(hdrBuffer, UV));
  vec3 reinhard_mapped = hdr_color / (hdr_color + vec3(1.0));
  vec3 gamma_corrected = pow(reinhard_mapped, vec3(1.0 / GAMMA));
  out_color = vec4(gamma_corrected, 1.0);

  // Ordered dithering.
  vec2 texel_dimensions = UV * vec2(dimensions);
  ivec2 texel_coords = ivec2(int(texel_dimensions.x) % 8, int(texel_dimensions.y) % 8);
  float bayer_value = float(texelFetch(bayer_matrix, texel_coords, 0).r);
  out_color = out_color + (bayer_value / 32.0 / 255.0) + (1.0 / 255.0);
}