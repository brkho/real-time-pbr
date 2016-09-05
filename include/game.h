// Preprocessor Directives
#ifndef GAME_H
#define GAME_H
#pragma once

// System Headers
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <stb_image.h>

const int kWindowWidth = 1280;
const int kWindowHeight = 800;
const double kRotateSensitivity = 0.005;
const double kPanSensitivity = 0.005;
const double kZoomSensitivity = 0.01;

#endif /* GAME_H */
