#version 330 core

// A fairly granular value for Pi.
#define PI 3.1415926535897932384626433832795

in vec3 Position;

out vec4 out_color;

uniform float skybox_blur;
uniform sampler2D environment_map;

void main() {
  vec3 direction = normalize(Position);
  vec2 uv = vec2((1.0 + atan(direction.x, direction.z) / PI) / 2.0, acos(direction.y) / PI);
  vec3 environment_color = vec3(textureLod(environment_map, uv, skybox_blur));
  out_color = vec4(environment_color, 1.0);
}