#version 330 core

#define PI 3.1415926535897932384626433832795

struct DirectionalLight {
  bool enabled;
  vec3 direction;
  vec3 irradiance;
};

uniform DirectionalLight directional_light;
uniform float ambient_coefficient;
uniform vec4 base_color;
uniform vec3 camera_position;

in vec3 Normal;
in vec4 WorldPosition;

out vec4 out_color;

void main() {
  vec4 total_color = vec4(ambient_coefficient * vec3(base_color), 1.0);
  
  if (directional_light.enabled) {
    vec3 adjusted_diffuse_rgb = vec3(base_color) / PI;
    vec3 normalized_direction = -normalize(directional_light.direction);
    vec3 diffuse = adjusted_diffuse_rgb *
        (directional_light.irradiance * max(dot(Normal, normalized_direction), 0.0));
    total_color += vec4(diffuse, base_color.w);
  }
  out_color = clamp(total_color, 0.0, 1.0);
}
