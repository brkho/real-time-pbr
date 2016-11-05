#version 330 core

layout (location = 0) in vec3 position;

out vec2 UV;

void main() {
  UV = (vec2(position.x, position.y) + 1.0) / 2.0;
  gl_Position = vec4(position, 1.0);
}
