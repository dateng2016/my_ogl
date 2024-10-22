// Include standard headers
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
using namespace std;

float oneGridLength = 275.0f;

void render(int right, int down, glm::mat4 referenceModel, GLuint MatrixID,
            GLuint ModelMatrixID, GLuint ViewMatrixID, GLuint Texture,
            GLuint TextureID, GLuint elementBuffer, GLuint vertexBuffer,
            GLuint uvBuffer, GLuint normalBuffer,
            vector<unsigned short> indices)
{
    glm::mat4 modelMatrix =
        glm::translate(referenceModel, glm::vec3(right * oneGridLength, 0.0f,
                                                 down * oneGridLength));
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * modelMatrix;

    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

    // Bind the texture for the second object
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,
                  Texture);    // Texture for the second object
    glUniform1i(TextureID, 0); // Set the sampler to use Texture Unit 0

    // Bind buffers and draw the second object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    // Set attribute pointers for the second object
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
}

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

    // std::vector<unsigned short> indices2;
    // std::vector<glm::vec3> indexed_vertices2;
    // std::vector<glm::vec2> indexed_uvs2;
    // std::vector<glm::vec3> indexed_normals2;

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

    // ***********************
    // TODO: Assign each pieces accordingly

    // Load it into a VBO
    // ! To DELETE
    vector<GLuint> chessVertexBuffers, chessUvBuffers, chessNomralBuffers,
        chessElementBuffers;
    for (int i = 0; i < 12; i++)
    {
        GLuint chessVertexBuffer, chessUvBuffer, chessNomralBuffer,
            chessElementBuffer;
        chessVertexBuffers.push_back(chessVertexBuffer);
        chessUvBuffers.push_back(chessUvBuffer);
        chessNomralBuffers.push_back(chessUvBuffer);
        chessElementBuffers.push_back(chessElementBuffer);
    }

    // TODO: Assigning accordingly
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
        // ! TO DELETE
        // glGenBuffers(1, &chessVertexBuffers[i]);
        // glBindBuffer(GL_ARRAY_BUFFER, chessVertexBuffers[i]);
        // glBufferData(GL_ARRAY_BUFFER,
        //              chessIndexedVertices[i].size() * sizeof(glm::vec3),
        //              &chessIndexedVertices[i][0], GL_STATIC_DRAW);

        // glGenBuffers(1, &chessUvBuffers[i]);
        // glBindBuffer(GL_ARRAY_BUFFER, chessUvBuffers[i]);
        // glBufferData(GL_ARRAY_BUFFER,
        //              chessIndexedUvs[i].size() * sizeof(glm::vec2),
        //              &chessIndexedUvs[i][0], GL_STATIC_DRAW);

        // glGenBuffers(1, &chessNomralBuffers[i]);
        // glBindBuffer(GL_ARRAY_BUFFER, chessNomralBuffers[i]);
        // glBufferData(GL_ARRAY_BUFFER,
        //              chessIndexedNormals[i].size() * sizeof(glm::vec3),
        //              &chessIndexedNormals[i][0], GL_STATIC_DRAW);

        // glGenBuffers(1, &chessElementBuffers[i]);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chessElementBuffers[i]);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        //              chessIndices[i].size() * sizeof(unsigned short),
        //              &chessIndices[i][0], GL_STATIC_DRAW);
    }

    GLuint Texture2 = loadBMP_custom("Chess_New/wooddar3.bmp");
    // Get a handle for our "myTextureSampler" uniform
    GLuint TextureID2 = glGetUniformLocation(programID, "myTextureSampler");

    //  **********************************************************

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
        glm::mat4 ModelMatrix2 =
            glm::scale(glm::mat4(1.0),
                       glm::vec3(scaleFactor2, scaleFactor2, scaleFactor2));
        // * First We need to translate towards -y and -z to put it ON board
        ModelMatrix2 =
            glm::translate(ModelMatrix2, glm::vec3(0.0f, -100.0f, -100.0f));

        // * Then we do the rotation Now queen is located at the middle to
        // * the left
        ModelMatrix2 = glm::rotate(ModelMatrix2, glm::radians(90.0f),
                                   glm::vec3(1.0f, 0.0f, 0.0f));

        // TODO: +z towards bottom +x towards right ONE grid is about 275
        // float oneGridLength = 275.0f;
        // * Render KING

        render(-2, 2, ModelMatrix2, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, kingElementBuffer, kingVertexBuffer,
               kingUvBuffer, kingNormalBuffer, kingIndices);
        render(-2, -5, ModelMatrix2, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, kingElementBuffer, kingVertexBuffer,
               kingUvBuffer, kingNormalBuffer, kingIndices);
        // * Render QUEEN
        render(0, 2, ModelMatrix2, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, queenElementBuffer, queenVertexBuffer,
               queenUvBuffer, queenNormalBuffer, queenIndices);
        render(0, -5, ModelMatrix2, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, queenElementBuffer, queenVertexBuffer,
               queenUvBuffer, queenNormalBuffer, queenIndices);
        // * Render BISHOP
        render(-1, 2, ModelMatrix2, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, bishopElementBuffer, bishopVertexBuffer,
               bishopUvBuffer, bishopNormalBuffer, bishopIndices);
        render(-1, -5, ModelMatrix2, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, bishopElementBuffer, bishopVertexBuffer,
               bishopUvBuffer, bishopNormalBuffer, bishopIndices);
        render(2, 2, ModelMatrix2, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, bishopElementBuffer, bishopVertexBuffer,
               bishopUvBuffer, bishopNormalBuffer, bishopIndices);
        render(2, -5, ModelMatrix2, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, bishopElementBuffer, bishopVertexBuffer,
               bishopUvBuffer, bishopNormalBuffer, bishopIndices);

        // * Render KNIGHT

        // This one needs rotationx
        render(-1, 2,
               glm::rotate(ModelMatrix2, glm::radians(180.0f),
                           glm::vec3(0.0f, 1.0f, 0.0f)),
               MatrixID, ModelMatrixID, ViewMatrixID, Texture2, TextureID2,
               knightElementBuffer, knightVertexBuffer, knightUvBuffer,
               knightNormalBuffer, knightIndices);
        render(-1, -5, ModelMatrix2, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, knightElementBuffer, knightVertexBuffer,
               knightUvBuffer, knightNormalBuffer, knightIndices);
        // This one needs rotation
        render(4, 2,
               glm::rotate(ModelMatrix2, glm::radians(180.0f),
                           glm::vec3(0.0f, 1.0f, 0.0f)),
               MatrixID, ModelMatrixID, ViewMatrixID, Texture2, TextureID2,
               knightElementBuffer, knightVertexBuffer, knightUvBuffer,
               knightNormalBuffer, knightIndices);
        render(4, -5, ModelMatrix2, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, knightElementBuffer, knightVertexBuffer,
               knightUvBuffer, knightNormalBuffer, knightIndices);
        // * Render ROOK
        render(-1, 2, ModelMatrix2, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, rookElementBuffer, rookVertexBuffer,
               rookUvBuffer, rookNormalBuffer, rookIndices);
        render(-1, -5, ModelMatrix2, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, rookElementBuffer, rookVertexBuffer,
               rookUvBuffer, rookNormalBuffer, rookIndices);
        render(6, 2, ModelMatrix2, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, rookElementBuffer, rookVertexBuffer,
               rookUvBuffer, rookNormalBuffer, rookIndices);
        render(6, -5, ModelMatrix2, MatrixID, ModelMatrixID, ViewMatrixID,
               Texture2, TextureID2, rookElementBuffer, rookVertexBuffer,
               rookUvBuffer, rookNormalBuffer, rookIndices);

        // ! TO DELETE
        // glm::mat4 kingModelMatrix1 =
        //     glm::translate(ModelMatrix2, glm::vec3(-2 * oneGridLength,
        //     0.0f,
        //                                            2 * oneGridLength));

        // MVP = ProjectionMatrix * ViewMatrix * kingModelMatrix1;

        // glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        // glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE,
        // &kingModelMatrix1[0][0]); glUniformMatrix4fv(ViewMatrixID, 1,
        // GL_FALSE, &ViewMatrix[0][0]);

        // // Bind the texture for the second object
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D,
        //               Texture2);    // Texture for the second object
        // glUniform1i(TextureID2, 0); // Set the sampler to use Texture
        // Unit 0

        // // Bind buffers and draw the second object
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kingElementBuffer);
        // // Set attribute pointers for the second object
        // glEnableVertexAttribArray(0);
        // glBindBuffer(GL_ARRAY_BUFFER, kingVertexBuffer);
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // glEnableVertexAttribArray(1);
        // glBindBuffer(GL_ARRAY_BUFFER, kingUvBuffer);
        // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // glEnableVertexAttribArray(2);
        // glBindBuffer(GL_ARRAY_BUFFER, kingNormalBuffer);
        // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        // glDrawElements(GL_TRIANGLES, kingIndices.size(),
        // GL_UNSIGNED_SHORT,
        //                (void*)0);

        // ! TODELETE
        // for (int i = 0; i < 0; i += 2)
        // {

        //     ModelMatrix2 =
        //         glm::translate(ModelMatrix2, glm::vec3(100.0f, 0.0f, 0.0f));

        //     MVP = ProjectionMatrix * ViewMatrix * ModelMatrix2;

        //     glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        //     glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE,
        //     &ModelMatrix2[0][0]); glUniformMatrix4fv(ViewMatrixID, 1,
        //     GL_FALSE, &ViewMatrix[0][0]);

        //     // Bind the texture for the second object
        //     glActiveTexture(GL_TEXTURE0);
        //     glBindTexture(GL_TEXTURE_2D,
        //                   Texture2);    // Texture for the second object
        //     glUniform1i(TextureID2, 0); // Set the sampler to use Texture
        //     Unit 0

        //     // Bind buffers and draw the second object
        //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kingElementBuffer);
        //     // Set attribute pointers for the second object
        //     glEnableVertexAttribArray(0);
        //     glBindBuffer(GL_ARRAY_BUFFER, kingVertexBuffer);
        //     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        //     glEnableVertexAttribArray(1);
        //     glBindBuffer(GL_ARRAY_BUFFER, kingUvBuffer);
        //     glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        //     glEnableVertexAttribArray(2);
        //     glBindBuffer(GL_ARRAY_BUFFER, kingNormalBuffer);
        //     glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        //     glDrawElements(GL_TRIANGLES, kingIndices.size(),
        //     GL_UNSIGNED_SHORT,
        //                    (void*)0);
        // }

        // *********************************************************************************

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
