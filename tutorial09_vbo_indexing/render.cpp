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
