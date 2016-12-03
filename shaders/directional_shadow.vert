#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 model_transform;
uniform mat4 light_space_transform;

void main() {
  gl_Position = light_space_transform * model_transform * vec4(position, 1.0f);
}
