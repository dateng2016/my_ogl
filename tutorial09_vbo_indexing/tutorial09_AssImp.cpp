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
    // ALFIERE02
    // ALFIERE3
    // Object02
    // Object3
    // PEDONE12
    // PEDONE13
    // RE01
    // RE2
    // REGINA01
    // REGINA2
    // TORRE02
    // TORRE3

    // Load it into a VBO
    // GLuint vertexbuffer2, uvbuffer2, normalbuffer2, elementbuffer2;
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
    for (int i = 0; i < 12; i++)
    {
        glGenBuffers(1, &chessVertexBuffers[i]);
        glBindBuffer(GL_ARRAY_BUFFER, chessVertexBuffers[i]);
        glBufferData(GL_ARRAY_BUFFER,
                     chessIndexedVertices[i].size() * sizeof(glm::vec3),
                     &chessIndexedVertices[i][0], GL_STATIC_DRAW);

        glGenBuffers(1, &chessUvBuffers[i]);
        glBindBuffer(GL_ARRAY_BUFFER, chessUvBuffers[i]);
        glBufferData(GL_ARRAY_BUFFER,
                     chessIndexedUvs[i].size() * sizeof(glm::vec2), // FIXME:
                     &chessIndexedUvs[i][0], GL_STATIC_DRAW);

        glGenBuffers(1, &chessNomralBuffers[i]);
        glBindBuffer(GL_ARRAY_BUFFER, chessNomralBuffers[i]);
        glBufferData(GL_ARRAY_BUFFER,
                     chessIndexedNormals[i].size() *
                         sizeof(glm::vec3), // FIXME:
                     &chessIndexedNormals[i][0], GL_STATIC_DRAW);

        glGenBuffers(1, &chessElementBuffers[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chessElementBuffers[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     chessIndices[i].size() * sizeof(unsigned short), // FIXME:
                     &chessIndices[i][0], GL_STATIC_DRAW);
    }

    // glGenBuffers(1, &vertexbuffer2);
    // glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
    // glBufferData(GL_ARRAY_BUFFER,
    //              chessIndexedVertices[0].size() * sizeof(glm::vec3), //
    //              FIXME: &chessIndexedVertices[0][0], GL_STATIC_DRAW);

    // glGenBuffers(1, &uvbuffer2);
    // glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
    // glBufferData(GL_ARRAY_BUFFER,
    //              chessIndexedUvs[0].size() * sizeof(glm::vec2), // FIXME:
    //              &chessIndexedUvs[0][0], GL_STATIC_DRAW);

    // glGenBuffers(1, &normalbuffer2);
    // glBindBuffer(GL_ARRAY_BUFFER, normalbuffer2);
    // glBufferData(GL_ARRAY_BUFFER,
    //              chessIndexedNormals[0].size() * sizeof(glm::vec3), // FIXME:
    //              &chessIndexedNormals[0][0], GL_STATIC_DRAW);

    // glGenBuffers(1, &elementbuffer2);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer2);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    //              chessIndices[0].size() * sizeof(unsigned short), // FIXME:
    //              &chessIndices[0][0], GL_STATIC_DRAW);

    // GLuint Texture2 = loadDDS("Chess_New/wooddark5.dds");

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
        // * THE SECOND OBJECT

        // Now render the second object
        // Use our shader for the second object (or the same shader)
        // glm::mat4 ModelMatrix2 = glm::translate(
        //     glm::mat4(1.0),
        //     glm::vec3(2.0f, 0.0f, 0.0f)); // Translate to the right
        double scaleFactor2 = 0.001;
        glm::mat4 ModelMatrix2 =
            glm::scale(glm::mat4(1.0),
                       glm::vec3(scaleFactor2, scaleFactor2, scaleFactor2));
        MVP = ProjectionMatrix * ViewMatrix * ModelMatrix2;

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix2[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

        // Bind the texture for the second object
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture2); // Texture for the second object
        glUniform1i(TextureID2, 0); // Set the sampler to use Texture Unit 0

        for (int i = 0; i < 6; i++)
        { // Bind buffers and draw the second object
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chessElementBuffers[i]);
            // Set attribute pointers for the second object
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, chessVertexBuffers[i]);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, chessUvBuffers[i]);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, chessNomralBuffers[i]);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glDrawElements(GL_TRIANGLES, chessIndices[i].size(), // FIXME:
                           GL_UNSIGNED_SHORT, (void*)0);
        }

        // // Bind buffers and draw the second object
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer2);
        // // Set attribute pointers for the second object
        // glEnableVertexAttribArray(0);
        // glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // glEnableVertexAttribArray(1);
        // glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
        // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // glEnableVertexAttribArray(2);
        // glBindBuffer(GL_ARRAY_BUFFER, normalbuffer2);
        // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Draw the second object
        // glDrawElements(GL_TRIANGLES, chessIndices[0].size(), // FIXME:
        //                GL_UNSIGNED_SHORT, (void*)0);

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
