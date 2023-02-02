#include "Shader.h"
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#ifndef __DEPTHSHADER_H__
#define __DEPTHSHADER_H__

struct DepthShader : Shader {
    // view and projection, thse should be essentially view_light, model_light.
    glm::mat4 view = glm::mat4(1.0f); GLuint view_loc;
    // model instead of modelview
    glm::mat4 model = glm::mat4(1.0f); GLuint model_loc;
    // glm::mat4 modelview = glm::mat4(1.0f); GLuint modelview_loc;
    glm::mat4 projection = glm::mat4(1.0f); GLuint projection_loc;

    // uniform to allow lighting toggle
    GLboolean enablelighting = GL_FALSE; // are we lighting at all (global).
    GLuint enablelighting_loc;

    // uniform to pass in near/far of camera (using default vals)
    GLfloat near = 1.0f; GLuint near_loc;
    GLfloat far = 8.0f; GLuint far_loc;

    void initUniforms() {
        view_loc = glGetUniformLocation(program, "view");
        model_loc = glGetUniformLocation(program, "model");
        //modelview_loc  = glGetUniformLocation( program, "modelview" );
        projection_loc = glGetUniformLocation(program, "projection");

        enablelighting_loc = glGetUniformLocation(program, "enablelighting");
        near_loc = glGetUniformLocation(program, "near");
        far_loc = glGetUniformLocation(program, "far");
    }

    void setUniforms() {
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model[0][0]);
        //glUniformMatrix4fv(modelview_loc, 1, GL_FALSE, &modelview[0][0]);
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &projection[0][0]);

        glUniform1i(enablelighting_loc, enablelighting);
        glUniform1f(near_loc, near);
        glUniform1f(far_loc, far);
    }
};

#endif 