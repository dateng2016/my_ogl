#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;

#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/vboindexer.hpp>
#pragma once
void render(int right, int down, glm::mat4 referenceModel, GLuint MatrixID,
            GLuint ModelMatrixID, GLuint ViewMatrixID, GLuint Texture,
            GLuint TextureID, GLuint elementBuffer, GLuint vertexBuffer,
            GLuint uvBuffer, GLuint normalBuffer,
            vector<unsigned short> indices);