#version 330 core

#define GAMMA 2.2

in vec2 UV;

out vec4 out_color;

uniform usampler2D bayer_matrix;
uniform sampler2DMS hdrBuffer;
uniform uvec2 dimensions;

vec3 reinhard_map(vec3 hdr_color) {
  vec3 mapped = hdr_color / (hdr_color + vec3(1.0));
  return mapped;
}

void main() {
  // We do a custom MSAA resolve in order to do tone mapping before the resolve.
  ivec2 coords = ivec2(int(UV.s * dimensions.x), int(UV.t * dimensions.y));
  vec3 hdr_color = vec3(0.0);
  hdr_color += reinhard_map(vec3(texelFetch(hdrBuffer, coords, 0)));
  hdr_color += reinhard_map(vec3(texelFetch(hdrBuffer, coords, 1)));
  hdr_color += reinhard_map(vec3(texelFetch(hdrBuffer, coords, 2)));
  hdr_color += reinhard_map(vec3(texelFetch(hdrBuffer, coords, 3)));
  hdr_color /= 4.0;

  // Gamma correction.
  out_color = vec4(pow(hdr_color, vec3(1.0 / GAMMA)), 1.0);

  // Ordered dithering.
  vec2 texel_dimensions = UV * vec2(dimensions);
  ivec2 texel_coords = ivec2(int(texel_dimensions.x) % 8, int(texel_dimensions.y) % 8);
  float bayer_value = float(texelFetch(bayer_matrix, texel_coords, 0).r);
  out_color = out_color + (bayer_value / 32.0 / 255.0) + (1.0 / 255.0);
}