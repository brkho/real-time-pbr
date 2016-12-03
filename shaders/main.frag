#version 330 core

// A fairly granular value for Pi.
#define PI 3.1415926535897932384626433832795
// Make sure this matches the MAX_POINT_LIGHTS in gfx/constants.h!
#define MAX_POINT_LIGHTS 3
// The maximum gloss to apply as a power.
#define MAX_GLOSS 64.0
// The gamma for converting between linear and sRGB.
#define GAMMA 2.2
// The number of samples to take for IBL. This must match the value defined in gfx/constants.h.
#define NUM_IBL_SAMPLES 32

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

struct MapInfo {
  bool enabled;
  vec3 default_value;
  sampler2D map;
};

uniform DirectionalLight directional_light;
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform MapInfo albedo_map;
uniform MapInfo metallic_map;
uniform MapInfo roughness_map;
uniform MapInfo normal_map;
uniform MapInfo ao_map;
uniform MapInfo environment_map;
uniform vec2 hammersley_points[NUM_IBL_SAMPLES];
uniform int shader_type;
uniform float ambient_coefficient;
uniform vec4 base_color;
uniform vec3 camera_position;

in vec2 UV;
in vec3 WorldPosition;
in mat3 TBN;

out vec4 out_color;

float clamped_cosine(vec3 a, vec3 b) {
  return min(max(dot(a, b), 0.0), 1.0);
}

float normal_distribution_function(vec3 normal, vec3 halfway, float roughness) {
  float alpha_2 = pow(roughness, 4);
  return alpha_2 / (PI * pow((pow(dot(normal, halfway), 2) * (alpha_2 - 1) + 1), 2));
}

float get_geometric_attenuation(vec3 direction, vec3 view, vec3 normal, vec3 halfway,
    float roughness) {
  float k = pow(roughness + 1, 2) / 8;
  float g1l = dot(normal, direction) / (dot(normal, direction) * (1 - k) + k);
  float g1v = dot(normal, view) / (dot(normal, view) * (1 - k) + k);
  return g1l * g1v;
}

vec3 get_fresnel(vec3 view, vec3 halfway, vec3 f0) {
  float dot_vh = dot(view, halfway);
  return f0 + (1 - f0) * pow(2.0, (-5.55473 * dot_vh - 6.98316) * dot_vh);
}

vec3 get_light_contribution_helper(vec3 albedo, vec3 f0, float roughness, vec3 normal,
    vec3 incoming_irradiance, vec3 reversed_direction) {
  vec3 view = normalize(camera_position - WorldPosition);
  vec3 halfway = normalize(reversed_direction + view);

  vec3 albedo_contribution = albedo / PI;
  float d = normal_distribution_function(normal, halfway, roughness);
  vec3 f = get_fresnel(view, halfway, f0);
  float g = get_geometric_attenuation(reversed_direction, view, normal, halfway, roughness);
  vec3 specular_contribution = (d * f * g) / (dot(normal, reversed_direction) * dot(normal, view));

  return (albedo_contribution + specular_contribution) * incoming_irradiance;
}

vec3 get_light_contribution(vec3 albedo, float metallic, float roughness, vec3 normal,
    vec3 incoming_irradiance, vec3 reversed_direction) {
  vec3 dielectric_contribution = get_light_contribution_helper(albedo, vec3(0.04, 0.04, 0.04),
      roughness, normal, incoming_irradiance, reversed_direction);
  vec3 metallic_contribution = get_light_contribution_helper(vec3(0.0, 0.0, 0.0), albedo,
      roughness, normal, incoming_irradiance, reversed_direction);
  return mix(dielectric_contribution, metallic_contribution, metallic);
}

vec3 get_directional_light_contribution(vec3 albedo, float metallic, float roughness,
    vec3 normal) {
  vec3 reversed_direction = -directional_light.direction;
  vec3 incoming_irradiance = directional_light.irradiance *
      clamped_cosine(normal, reversed_direction);
  return get_light_contribution(albedo, metallic, roughness, normal, incoming_irradiance,
      reversed_direction);
}

vec3 get_point_light_contribution(int light_index, vec3 albedo, float metallic, float roughness,
    vec3 normal) {
  PointLight light = point_lights[light_index];
  vec3 reversed_direction = normalize(light.position - WorldPosition);
  float dist = distance(WorldPosition, light.position);
  float falloff = 1.0 / (dist * dist + 1);
  float light_radius = 100.0;
  falloff = pow(clamp(1 - pow(dist / light_radius, 4), 0.0, 1.0), 2) / (pow(dist, 2) + 1);

  vec3 incoming_irradiance = (light.irradiance * falloff) * clamped_cosine(normal,
      reversed_direction);
  return get_light_contribution(albedo, metallic, roughness, normal, incoming_irradiance,
      reversed_direction);
}

vec3 get_ibl_sample_contribution(vec2 hammersley, float roughness, vec3 normal, vec3 albedo,
    float metallic) {
  // Get the sample direction from the Hammersley point. See GGX paper for derivation of closed
  // solutions for theta and phi.
  float alpha = pow(roughness, 2.0);
  float theta = atan((alpha * sqrt(hammersley.x)) / sqrt(1.0 - hammersley.x));
  float phi = 2.0 * PI * hammersley.y;
  vec3 h_tangent = vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));

  // This direction is in tangent space, so we need to transform it to world space.
  vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
  vec3 tangent_x = normalize(cross(up, normal));
  vec3 tangent_y = cross(normal, tangent_x);
  vec3 h = normalize(tangent_x * h_tangent.x + tangent_y * h_tangent.y + normal * h_tangent.z);

  vec3 v = normalize(camera_position - WorldPosition);
  vec3 l = normalize(2 * abs(dot(h, v)) * h - v);
  vec3 n = normal;

  // Calculate the light contribution from the sample and the environment map.
  if (clamped_cosine(n, l) > 0) {
    vec2 uv1 = vec2((1.0 + atan(l.x, l.z) / PI) / 2.0, acos(l.y) / PI);

    // Hacky, but fast computation of LOD. Perhaps I should opt for the more traditional and
    // analytic solid angle approach here...
    float lod = mix(6.0, 0.0, metallic);
    vec3 sample_color = vec3(textureLod(environment_map.map, uv1, lod));

    vec3 f0 = mix(vec3(0.04, 0.04, 0.04), albedo, metallic);
    vec3 f = get_fresnel(v, h, f0);
    float g = get_geometric_attenuation(l, v, n, h, roughness);
    return (f * g * sample_color * clamped_cosine(v, h)) /
        (clamped_cosine(n, h) * clamped_cosine(n, v));
  }
  return vec3(0.0, 0.0, 0.0);
}

void main() {
  // TODO(brkho): Have a separate shader compilation step to avoid this branching.
  vec3 albedo = albedo_map.enabled ? vec3(texture(albedo_map.map, UV)) :
      albedo_map.default_value;
  float metallic = metallic_map.enabled ? texture(metallic_map.map, UV).x :
      metallic_map.default_value.x;
  float roughness = roughness_map.enabled ? texture(roughness_map.map, UV).x :
      roughness_map.default_value.x;
  vec3 tangent_space_normal = normal_map.enabled ? vec3(texture(normal_map.map, UV)) :
      normal_map.default_value;
  vec3 ao = ao_map.enabled ? vec3(texture(ao_map.map, UV)) : ao_map.default_value;

  tangent_space_normal = normalize((tangent_space_normal * 2.0) - 1.0) * vec3(1.0, -1.0, 1.0);
  tangent_space_normal = vec3(tangent_space_normal.x, tangent_space_normal.y,
      tangent_space_normal.z);
  vec3 normal = normalize(TBN * tangent_space_normal);

  vec3 total_color = vec3(0.0, 0.0, 0.0);
  if (environment_map.enabled) {
    // TODO(brkho): Investigate unrolling this loop for better performance.
    for (int i = 0; i < NUM_IBL_SAMPLES; i++) {
      total_color += get_ibl_sample_contribution(hammersley_points[i], roughness, normal, albedo,
          metallic);
    }
    total_color /= float(NUM_IBL_SAMPLES);
  } else {
    total_color = mix(albedo * 0.05, vec3(0.0), metallic);
  }

  if (directional_light.enabled) {
    total_color += get_directional_light_contribution(albedo, metallic, roughness, normal);
  }
  if (point_lights[0].enabled) {
    total_color += get_point_light_contribution(0, albedo, metallic, roughness, normal);
  }
  if (point_lights[1].enabled) {
    total_color += get_point_light_contribution(1, albedo, metallic, roughness, normal);
  }
  if (point_lights[2].enabled) {
    total_color += get_point_light_contribution(2, albedo, metallic, roughness, normal);
  }

  // Bias AO because it will eventually be gamma corrected.
  total_color = total_color * pow(ao, vec3(GAMMA));
  out_color = vec4(total_color, 1.0);
}
