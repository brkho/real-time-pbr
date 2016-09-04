#version 330 core

in vec3 ourColor;

out vec4 color;

uniform float alpha;

void main() {
  color = vec4(ourColor, alpha);
} 