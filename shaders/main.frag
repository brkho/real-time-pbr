#version 330 core

#define PI 3.1415926535897932384626433832795
// Make sure this matches the MAX_POINT_LIGHTS in gfx/constants.h!
#define MAX_POINT_LIGHTS 3

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
uniform int shader_type;
uniform bool albedo_enabled;
uniform bool specular_enabled;
uniform bool gloss_enabled;
uniform bool ior_enabled;
uniform bool normal_enabled;
uniform bool ao_enabled;
uniform float ambient_coefficient;
uniform float shininess;
uniform vec4 base_color;
uniform vec3 camera_position;
uniform sampler2D albedo_map;
uniform sampler2D specular_map;
uniform sampler2D gloss_map;
uniform sampler2D ior_map;
uniform sampler2D normal_map;
uniform sampler2D ao_map;

in vec2 UV;
in vec3 WorldPosition;
in mat3 TBN;

out vec4 out_color;

float clamped_cosine(vec3 a, vec3 b) {
  return max(dot(a, b), 0.0);
}

vec4 get_light_contribution_helper(vec4 albedo_color, vec3 specular_color, vec3 normal,
    vec3 incoming_irradiance, vec3 reversed_direction) {
  vec3 view_vector = normalize(camera_position - WorldPosition);
  vec3 albedo = vec3(albedo_color) * incoming_irradiance;

  vec3 specular = vec3(0.0, 0.0, 0.0);
  if (specular_enabled) {
    vec3 halfway = normalize(reversed_direction + view_vector);
    float cos_nha = pow(clamped_cosine(normal, halfway), 50.0);
    specular = vec3(cos_nha * specular_color * incoming_irradiance);
  }
  return vec4(albedo, albedo_color.w) + vec4(specular, 1.0);
}

vec4 get_directional_light_contribution(vec4 albedo_color, vec3 specular_color, vec3 normal) {
  vec3 reversed_direction = -directional_light.direction;
  vec3 incoming_irradiance = directional_light.irradiance *
      clamped_cosine(normal, reversed_direction);
  return get_light_contribution_helper(albedo_color, specular_color, normal, incoming_irradiance,
      reversed_direction);
}

vec4 get_point_light_contribution(int light_index, vec4 albedo_color, vec3 specular_color,
    vec3 normal) {
  PointLight light = point_lights[light_index];
  vec3 reversed_direction = normalize(light.position - WorldPosition);
  float dist = distance(WorldPosition, light.position);
  vec3 incoming_irradiance = (light.irradiance / (light.const_atten + light.linear_atten * dist +
      light.quad_atten * (dist * dist))) * clamped_cosine(normal, reversed_direction);
  return get_light_contribution_helper(albedo_color, specular_color, normal, incoming_irradiance,
      reversed_direction);
}

void main() {
  vec4 albedo_color = albedo_enabled ? texture(albedo_map, UV) : vec4(1.0, 1.0, 1.0, 1.0);
  vec3 specular_color = specular_enabled ? vec3(texture(specular_map, UV)) :
      vec3(1.0, 1.0, 1.0);
  vec3 ao_value = ao_enabled ? vec3(texture(ao_map, UV)) : vec3(1.0, 1.0, 1.0);
  vec3 tangent_space_normal = normal_enabled ? vec3(texture(normal_map, UV)) : vec3(0.5, 0.5, 1.0);
  tangent_space_normal = normalize((tangent_space_normal * 2.0) - 1.0) * vec3(1.0, -1.0, 1.0);
  tangent_space_normal = vec3(tangent_space_normal.x, tangent_space_normal.y, tangent_space_normal.z);
  vec3 normal = normalize(TBN * tangent_space_normal);
  vec4 total_color = vec4((ambient_coefficient * ao_value), 1.0) * albedo_color;

  if (directional_light.enabled) {
    total_color += get_directional_light_contribution(albedo_color, specular_color, normal);
  }
  if (point_lights[0].enabled) {
    total_color += get_point_light_contribution(0, albedo_color, specular_color, normal);
  }
  if (point_lights[1].enabled) {
    total_color += get_point_light_contribution(1, albedo_color, specular_color, normal);
  }
  if (point_lights[2].enabled) {
    total_color += get_point_light_contribution(2, albedo_color, specular_color, normal);
  }

  out_color = total_color;
}
