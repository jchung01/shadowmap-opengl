/**************************************************
Light is a class for a camera object.
*****************************************************/
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#ifndef __LIGHT_H__
#define __LIGHT_H__

struct Light {
    glm::vec4 position = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    glm::mat4 view = glm::mat4(1.0f);   // view matrix
    glm::mat4 proj = glm::mat4(1.0f);   // projection matrix
    Camera* camera;

    GLuint depthMap;
    GLuint depthMapFBO;
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    // call this when you know the light has been set up.
    void computeMatrices() {
        // light's view matrix computed using lookat()
        view = glm::lookAt(glm::vec3(position), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // light's projection matrix using ortho()
        // this value is scene-dependent, currently tuned to custom scene.
        const float ortho = 3.0f;
        const float near = 0.01f;
        const float far = 8.0f;
        proj = glm::ortho(-1.0f * ortho, ortho, -1.0f * ortho, ortho, near, far);
    }

    Light() {
        glGenFramebuffers(1, &depthMapFBO);

        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
            SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT,
            GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE); // Omitting color data
        glReadBuffer(GL_NONE); // Omitting color data
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

#endif 
