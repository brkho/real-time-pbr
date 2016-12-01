#version 330 core

layout (location = 0) in vec3 position;

out vec3 Position;

uniform mat4 view_transform;
uniform mat4 projection_transform;

void main() {
  Position = position;
  gl_Position = projection_transform * view_transform * vec4(position, 1.0);
}
