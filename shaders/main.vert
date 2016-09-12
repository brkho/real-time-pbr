#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model_transform;
uniform mat4 normal_transform;
uniform mat4 view_transform;
uniform mat4 projection_transform;

out vec3 Normal;
out vec4 WorldPosition;

void main() {
  gl_Position = projection_transform * view_transform * model_transform * vec4(position, 1.0);
  WorldPosition = model_transform * vec4(position, 1.0);
  Normal = normalize(mat3(normal_transform) * normal);
}
