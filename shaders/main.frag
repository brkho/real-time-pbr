#version 330 core

#define PI 3.1415926535897932384626433832795
// Make sure this matches the MAX_POINT_LIGHTS in gfx/constants.h!
#define MAX_POINT_LIGHTS 3
#define GAMMA 2.2

struct DirectionalLight {
  bool enabled;
  vec3 direction;
  vec3 irradiance;
};

struct PointLight {
    bool enabled;
    vec3 position;
    vec3 irradiance;
    float const_atten;
    float linear_atten;
    float quad_atten;
};

uniform DirectionalLight directional_light;
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform bool diffuse_enabled;
uniform bool specular_enabled;
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

vec4 get_light_contribution_helper(vec4 diffuse_color, vec3 specular_color,
    vec3 incoming_irradiance, vec3 reversed_direction) {
  vec3 view_vector = normalize(camera_position - WorldPosition);
  // vec3 adjusted_diffuse_rgb = vec3(diffuse_color);
  // vec3 diffuse = adjusted_diffuse_rgb * incoming_irradiance;
  vec3 diffuse = vec3(diffuse_color) * incoming_irradiance;

  // float adjusted_shininess = (shininess + 8.0) / (8.0 * PI);
  // vec3 half_angle_vector = normalize(reversed_direction + view_vector);
  // // what, why not use adjusted_shininess here
  // vec3 specular = (100.0 *
  //     pow(clamped_cosine(Normal, half_angle_vector), 100.0) * specular_color) *
  //     incoming_irradiance;

  vec3 halfway = normalize(reversed_direction + view_vector);
  float cos_nha = pow(clamped_cosine(Normal, halfway), 50.0);
  vec3 specular = vec3(cos_nha * specular_color * incoming_irradiance);
  return vec4(diffuse, diffuse_color.w) + vec4(specular, 1.0);
}

vec4 get_directional_light_contribution(vec4 diffuse_color, vec3 specular_color) {
  vec3 reversed_direction = -directional_light.direction;
  vec3 incoming_irradiance = directional_light.irradiance *
      clamped_cosine(Normal, reversed_direction);
  return get_light_contribution_helper(diffuse_color, specular_color, incoming_irradiance,
      reversed_direction);
}

vec4 get_point_light_contribution(int light_index, vec4 diffuse_color, vec3 specular_color) {
  PointLight light = point_lights[light_index];
  vec3 reversed_direction = normalize(light.position - WorldPosition);
  float dist = distance(WorldPosition, light.position);
  vec3 incoming_irradiance = (light.irradiance / (light.const_atten + light.linear_atten * dist +
      light.quad_atten * (dist * dist))) * clamped_cosine(Normal, reversed_direction);
  // vec3 i2 = (vec3(1.0, 1.0, 1.0) / (light.const_atten + light.linear_atten * dist +
      // light.quad_atten * (dist * dist))) * clamped_cosine(Normal, reversed_direction);
  return get_light_contribution_helper(diffuse_color, specular_color, incoming_irradiance,
      reversed_direction);
}

void main() {
  vec4 diffuse_color = diffuse_enabled ? texture(diffuse_texture, UV) : vec4(0.0, 0.0, 0.0, 1.0);
  vec3 specular_color = specular_enabled ? vec3(texture(specular_texture, UV)) :
      vec3(1.0, 1.0, 1.0);

  vec4 total_color = ambient_coefficient * diffuse_color;

  if (directional_light.enabled) {
    total_color += get_directional_light_contribution(diffuse_color, specular_color);
  }

  if (point_lights[0].enabled) {
    total_color += get_point_light_contribution(0, diffuse_color, specular_color);
  }

  if (point_lights[1].enabled) {
    total_color += get_point_light_contribution(1, diffuse_color, specular_color);
  }

  if (point_lights[2].enabled) {
    total_color += get_point_light_contribution(2, diffuse_color, specular_color);
  }

  total_color = pow(total_color, vec4(1.0 / GAMMA));
  out_color = clamp(total_color, 0.0, 1.0);
}
