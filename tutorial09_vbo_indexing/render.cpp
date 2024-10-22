/*
Author: Da Teng
Class: ECE6122
Last Date Modified: 10/22/2024
Description:
This file contains the render function to render different objects
*/
// Include standard headers
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

float oneGridLength = 275.0f;

void render(int right, int down, glm::mat4 referenceModel, GLuint MatrixID,
            GLuint ModelMatrixID, GLuint ViewMatrixID, GLuint Texture,
            GLuint TextureID, GLuint elementBuffer, GLuint vertexBuffer,
            GLuint uvBuffer, GLuint normalBuffer,
            vector<unsigned short> indices)
{
    /**
     * @brief Renders a 3D object in the scene.
     *
     * This function sets up the transformation matrices for a specified object,
     * applies the appropriate texture, and draws the object using its vertex
     * and index buffers. The object is positioned based on the provided grid
     * coordinates, relative to a reference model matrix.
     *
     * @param right         The horizontal offset from the reference model, in
     *                      grid units.
     * @param down          The vertical offset from the reference model, in
     *                      grid units.
     * @param referenceModel The base model matrix to which the translation is
     *                       applied.
     * @param MatrixID      The uniform location for the Model-View-Projection
     *                      (MVP) matrix.
     * @param ModelMatrixID The uniform location for the model matrix.
     * @param ViewMatrixID  The uniform location for the view matrix.
     * @param Texture       The OpenGL texture identifier for the object.
     * @param TextureID     The uniform location for the texture sampler.
     * @param elementBuffer  The buffer ID for the object's index data.
     * @param vertexBuffer   The buffer ID for the object's vertex data.
     * @param uvBuffer       The buffer ID for the object's UV mapping data.
     * @param normalBuffer   The buffer ID for the object's normal data.
     * @param indices        A vector containing the index data used for drawing
     *                       the object.
     *
     * @return void
     *
     * This function does not return a value. It directly interacts with OpenGL
     * to render the specified object based on the input parameters.
     */

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
