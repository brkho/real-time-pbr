#version 330 core

#define PI 3.1415926535897932384626433832795

struct DirectionalLight {
  bool enabled;
  vec3 direction;
  vec3 irradiance;
};

uniform DirectionalLight directional_light;
uniform float ambient_coefficient;
uniform float shininess;
uniform vec4 base_color;
uniform vec3 camera_position;
uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;

in vec3 Normal;
in vec2 UV;
in vec3 WorldPosition;

out vec4 out_color;

float clamped_cosine(vec3 a, vec3 b) {
  return max(dot(a, b), 0.0);
}

void main() {
  vec4 diffuse_color = texture(diffuse_texture, UV) * base_color;
  vec4 total_color = ambient_coefficient * diffuse_color;
  vec3 view_vector = normalize(camera_position - WorldPosition);

  if (directional_light.enabled) {
    vec3 adjusted_diffuse_rgb = vec3(diffuse_color) / PI;
    vec3 normalized_direction = -normalize(directional_light.direction);
    vec3 incoming_irradiance = directional_light.irradiance *
        clamped_cosine(Normal, normalized_direction);
    vec3 diffuse = adjusted_diffuse_rgb * incoming_irradiance;
    total_color += vec4(diffuse, diffuse_color.w);

    float adjusted_shininess = (shininess + 8.0) / (8.0 * PI);
    vec3 half_angle_vector = normalize(normalized_direction + view_vector);
    vec3 specular_color = vec3(texture(specular_texture, UV));
    vec3 specular = (adjusted_shininess *
        pow(clamped_cosine(Normal, half_angle_vector), shininess) * specular_color) *
        incoming_irradiance;
    total_color += vec4(specular, base_color.w);
  }
  // out_color = vec4(UV, 1.0, 1.0);
  // out_color = texture(diffuse_texture, UV);
  out_color = clamp(total_color, 0.0, 1.0);
}
