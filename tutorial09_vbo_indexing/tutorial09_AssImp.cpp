/*
Author: Da Teng
Class: ECE6122
Last Date Modified: 10/22/2024
Description:
This is the main file for the assignment that renders the objects
*/

// Include standard headers
#include "render.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

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

int main(void)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                   GL_TRUE); // To make macOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(1024, 768, "Tutorial 09 - Loading with AssImp",
                              NULL, NULL);
    if (window == NULL)
    {
        fprintf(
            stderr,
            "Failed to open GLFW window. If you have an Intel GPU, they are "
            "not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it is closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("StandardShading.vertexshader",
                                   "StandardShading.fragmentshader");

    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

    GLuint Texture =
        loadDDS("Stone_Chess_Board/12951_Stone_Chess_Board_diff.dds");
    // Get a handle for our "myTextureSampler" uniform
    GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

    // Read our .obj file
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;

    bool res =
        loadAssImp("Stone_Chess_Board/12951_Stone_Chess_Board_v1_L3.obj",
                   indices, indexed_vertices, indexed_uvs, indexed_normals);

    // Load it into a VBO

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3),
                 &indexed_vertices[0], GL_STATIC_DRAW);

    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2),
                 &indexed_uvs[0], GL_STATIC_DRAW);

    GLuint normalbuffer;
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3),
                 &indexed_normals[0], GL_STATIC_DRAW);

    // Generate a buffer for the indices as well
    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned short), &indices[0],
                 GL_STATIC_DRAW);

    //  **********************************************************

    //    There are 12 meshes in the .obj file. We need vectors of size 12 of
    //    vectors to contain all the informations
    // * NOTE that each of the vectors below contains 12 vectors
    vector<vector<unsigned short>> chessIndices;
    vector<vector<glm::vec3>> chessIndexedVertices;
    vector<vector<glm::vec2>> chessIndexedUvs;
    vector<vector<glm::vec3>> chessIndexedNormals;

    bool res2 = loadAssImpMultiple("Chess_New/chess.obj", chessIndices,
                                   chessIndexedVertices, chessIndexedUvs,
                                   chessIndexedNormals);

    // *Here are the names of the 12 objects
    // ALFIERE02        0 -> Bishop
    // ALFIERE3
    // Object02         2 -> Knight
    // Object3
    // PEDONE12         4 -> Pawn (The smallest one)
    // PEDONE13
    // RE01             6 -> King
    // RE2
    // REGINA01         8 -> Queen
    // REGINA2
    // TORRE02          10 -> Rook
    // TORRE3

    // Assigning accordingly
    // 0 -> Bishop
    // 2 -> Knight
    // 4 -> Pawn (The smallest one)
    // 6 -> King
    // 8 -> Queen
    // 10 -> Rook
    GLuint bishopVertexBuffer, knightVertexBuffer, pawnVertexBuffer,
        kingVertexBuffer, queenVertexBuffer, rookVertexBuffer, bishopUvBuffer,
        knightUvBuffer, pawnUvBuffer, kingUvBuffer, queenUvBuffer, rookUvBuffer,
        bishopNormalBuffer, knightNormalBuffer, pawnNormalBuffer,
        kingNormalBuffer, queenNormalBuffer, rookNormalBuffer,
        bishopElementBuffer, knightElementBuffer, pawnElementBuffer,
        kingElementBuffer, queenElementBuffer, rookElementBuffer;
    vector<unsigned short> bishopIndices, knightIndices, pawnIndices,
        kingIndices, queenIndices, rookIndices;
    for (int i = 0; i < 12; i += 2)
    {
        // Here we assign buffer according to their indcies i
        if (i == 0)
        {
            bishopIndices = chessIndices[i];
            // This is Bishop
            glGenBuffers(1, &bishopVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, bishopVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedVertices[i].size() * sizeof(glm::vec3),
                         &chessIndexedVertices[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &bishopUvBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, bishopUvBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedUvs[i].size() * sizeof(glm::vec2),
                         &chessIndexedUvs[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &bishopNormalBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, bishopNormalBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedNormals[i].size() * sizeof(glm::vec3),
                         &chessIndexedNormals[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &bishopElementBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bishopElementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         chessIndices[i].size() * sizeof(unsigned short),
                         &chessIndices[i][0], GL_STATIC_DRAW);
        }
        if (i == 2)
        {
            knightIndices = chessIndices[i];
            // This is knight
            glGenBuffers(1, &knightVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, knightVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedVertices[i].size() * sizeof(glm::vec3),
                         &chessIndexedVertices[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &knightUvBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, knightUvBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedUvs[i].size() * sizeof(glm::vec2),
                         &chessIndexedUvs[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &knightNormalBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, knightNormalBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedNormals[i].size() * sizeof(glm::vec3),
                         &chessIndexedNormals[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &knightElementBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, knightElementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         chessIndices[i].size() * sizeof(unsigned short),
                         &chessIndices[i][0], GL_STATIC_DRAW);
        }
        if (i == 4)
        {
            pawnIndices = chessIndices[i];
            // This is pawn
            glGenBuffers(1, &pawnVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, pawnVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedVertices[i].size() * sizeof(glm::vec3),
                         &chessIndexedVertices[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &pawnUvBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, pawnUvBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedUvs[i].size() * sizeof(glm::vec2),
                         &chessIndexedUvs[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &pawnNormalBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, pawnNormalBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedNormals[i].size() * sizeof(glm::vec3),
                         &chessIndexedNormals[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &pawnElementBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pawnElementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         chessIndices[i].size() * sizeof(unsigned short),
                         &chessIndices[i][0], GL_STATIC_DRAW);
        }
        if (i == 6)
        {
            kingIndices = chessIndices[i];
            // This is king
            glGenBuffers(1, &kingVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, kingVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedVertices[i].size() * sizeof(glm::vec3),
                         &chessIndexedVertices[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &kingUvBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, kingUvBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedUvs[i].size() * sizeof(glm::vec2),
                         &chessIndexedUvs[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &kingNormalBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, kingNormalBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedNormals[i].size() * sizeof(glm::vec3),
                         &chessIndexedNormals[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &kingElementBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kingElementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         chessIndices[i].size() * sizeof(unsigned short),
                         &chessIndices[i][0], GL_STATIC_DRAW);
        }
        if (i == 8)
        {
            queenIndices = chessIndices[i];
            // This is queen
            glGenBuffers(1, &queenVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, queenVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedVertices[i].size() * sizeof(glm::vec3),
                         &chessIndexedVertices[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &queenUvBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, queenUvBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedUvs[i].size() * sizeof(glm::vec2),
                         &chessIndexedUvs[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &queenNormalBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, queenNormalBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedNormals[i].size() * sizeof(glm::vec3),
                         &chessIndexedNormals[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &queenElementBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, queenElementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         chessIndices[i].size() * sizeof(unsigned short),
                         &chessIndices[i][0], GL_STATIC_DRAW);
        }
        if (i == 10)
        {
            rookIndices = chessIndices[i];
            // This is rook
            glGenBuffers(1, &rookVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, rookVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedVertices[i].size() * sizeof(glm::vec3),
                         &chessIndexedVertices[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &rookUvBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, rookUvBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedUvs[i].size() * sizeof(glm::vec2),
                         &chessIndexedUvs[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &rookNormalBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, rookNormalBuffer);
            glBufferData(GL_ARRAY_BUFFER,
                         chessIndexedNormals[i].size() * sizeof(glm::vec3),
                         &chessIndexedNormals[i][0], GL_STATIC_DRAW);

            glGenBuffers(1, &rookElementBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rookElementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         chessIndices[i].size() * sizeof(unsigned short),
                         &chessIndices[i][0], GL_STATIC_DRAW);
        }
    }

    GLuint Texture2 = loadBMP_custom("Chess_New/wooddar3.bmp");
    // Get a handle for our "myTextureSampler" uniform
    GLuint TextureID2 = glGetUniformLocation(programID, "myTextureSampler");

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID =
        glGetUniformLocation(programID, "LightPosition_worldspace");

    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    do
    {
        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0)
        { // If last prinf() was more than 1sec ago
            // printf and reset
            printf("%f ms/frame\n", 1000.0 / double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        // glm::mat4 ModelMatrix = glm::mat4(1.0);
        float scaleFactor = 0.1f; // Scale factor for all axes
        glm::mat4 ModelMatrix = glm::scale(
            glm::mat4(1.0), glm::vec3(scaleFactor, scaleFactor, scaleFactor));

        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

        glm::vec3 lightPos = glm::vec3(0, 0, 6);
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // Set our "myTextureSampler" sampler to use Texture Unit 0
        glUniform1i(TextureID, 0);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0,        // attribute
                              3,        // size
                              GL_FLOAT, // type
                              GL_FALSE, // normalized?
                              0,        // stride
                              (void*)0  // array buffer offset
        );

        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(1,        // attribute
                              2,        // size
                              GL_FLOAT, // type
                              GL_FALSE, // normalized?
                              0,        // stride
                              (void*)0  // array buffer offset
        );

        // 3rd attribute buffer : normals
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(2,        // attribute
                              3,        // size
                              GL_FLOAT, // type
                              GL_FALSE, // normalized?
                              0,        // stride
                              (void*)0  // array buffer offset
        );

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

        // Draw the triangles !
        glDrawElements(GL_TRIANGLES,      // mode
                       indices.size(),    // count
                       GL_UNSIGNED_SHORT, // type
                       (void*)0           // element array buffer offset
        );

        // *********************************************************************************
        // * THE CHESS MESHES

        double scaleFactor2 = 0.002;
        glm::mat4 chessModelMatrix =
            glm::scale(glm::mat4(1.0),
                       glm::vec3(scaleFactor2, scaleFactor2, scaleFactor2));
        // * First We need to translate and rotate to put the chess pieces ON
        // * the board
        chessModelMatrix =
            glm::translate(chessModelMatrix, glm::vec3(0.0f, -100.0f, -100.0f));

        chessModelMatrix = glm::rotate(chessModelMatrix, glm::radians(90.0f),
                                       glm::vec3(1.0f, 0.0f, 0.0f));

        // * Render KING
        render(-2, 2, chessModelMatrix, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, kingElementBuffer, kingVertexBuffer,
               kingUvBuffer, kingNormalBuffer, kingIndices);
        render(-2, -5, chessModelMatrix, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, kingElementBuffer, kingVertexBuffer,
               kingUvBuffer, kingNormalBuffer, kingIndices);
        // * Render QUEEN
        render(0, 2, chessModelMatrix, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, queenElementBuffer, queenVertexBuffer,
               queenUvBuffer, queenNormalBuffer, queenIndices);
        render(0, -5, chessModelMatrix, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, queenElementBuffer, queenVertexBuffer,
               queenUvBuffer, queenNormalBuffer, queenIndices);
        // * Render BISHOP
        render(-1, 2, chessModelMatrix, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, bishopElementBuffer, bishopVertexBuffer,
               bishopUvBuffer, bishopNormalBuffer, bishopIndices);
        render(-1, -5, chessModelMatrix, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, bishopElementBuffer, bishopVertexBuffer,
               bishopUvBuffer, bishopNormalBuffer, bishopIndices);
        render(2, 2, chessModelMatrix, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, bishopElementBuffer, bishopVertexBuffer,
               bishopUvBuffer, bishopNormalBuffer, bishopIndices);
        render(2, -5, chessModelMatrix, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, bishopElementBuffer, bishopVertexBuffer,
               bishopUvBuffer, bishopNormalBuffer, bishopIndices);

        // * Render KNIGHT
        render(-1, 2, chessModelMatrix, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, knightElementBuffer, knightVertexBuffer,
               knightUvBuffer, knightNormalBuffer, knightIndices);
        render(-1, -5, chessModelMatrix, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, knightElementBuffer, knightVertexBuffer,
               knightUvBuffer, knightNormalBuffer, knightIndices);
        render(4, 2, chessModelMatrix, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, knightElementBuffer, knightVertexBuffer,
               knightUvBuffer, knightNormalBuffer, knightIndices);
        render(4, -5, chessModelMatrix, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, knightElementBuffer, knightVertexBuffer,
               knightUvBuffer, knightNormalBuffer, knightIndices);
        // * Render ROOK
        render(-1, 2, chessModelMatrix, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, rookElementBuffer, rookVertexBuffer,
               rookUvBuffer, rookNormalBuffer, rookIndices);
        render(-1, -5, chessModelMatrix, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, rookElementBuffer, rookVertexBuffer,
               rookUvBuffer, rookNormalBuffer, rookIndices);
        render(6, 2, chessModelMatrix, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, rookElementBuffer, rookVertexBuffer,
               rookUvBuffer, rookNormalBuffer, rookIndices);
        render(6, -5, chessModelMatrix, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, rookElementBuffer, rookVertexBuffer,
               rookUvBuffer, rookNormalBuffer, rookIndices);
        // * Render PAWN
        for (int i = -6; i < 2; i++)
        {
            render(i, 1, chessModelMatrix, MatrixID, ModelMatrixID,
                   ViewMatrixID, Texture2, TextureID2, pawnElementBuffer,
                   pawnVertexBuffer, pawnUvBuffer, pawnNormalBuffer,
                   pawnIndices);
            render(i, -4, chessModelMatrix, MatrixID, ModelMatrixID,
                   ViewMatrixID, Texture2, TextureID2, pawnElementBuffer,
                   pawnVertexBuffer, pawnUvBuffer, pawnNormalBuffer,
                   pawnIndices);
        }

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteProgram(programID);
    // glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
