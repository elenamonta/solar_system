#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <iostream>
#include <vector>

#include "mesh.h"
#include "Model.h"

class Utils {
public: 
    // verify the collision with a mesh and the camera. Camera is consider as a sphere with center = camera.Position and radius = 1.0f
    bool isColliding(vec3 cameraPosition, Mesh mesh) {
        x = std::max(mesh.min_BB.x, std::min(cameraPosition.x, mesh.max_BB.x));
        y = std::max(mesh.min_BB.y, std::min(cameraPosition.y, mesh.max_BB.y));
        z = std::max(mesh.min_BB.z, std::min(cameraPosition.z, mesh.max_BB.z));

        float distance = glm::distance(cameraPosition, vec3(x, y, z));

        return distance < 1.0f;
    }

    bool isCollidingObj(vec3 cameraPosition, Model model) {
        x = std::max(model.min_BB.x, std::min(cameraPosition.x, model.max_BB.x));
        y = std::max(model.min_BB.y, std::min(cameraPosition.y, model.max_BB.y));
        z = std::max(model.min_BB.z, std::min(cameraPosition.z, model.max_BB.z));

        float distance = glm::distance(cameraPosition, vec3(x, y, z));

        return distance < 0.5f;
    }


private:
    float x = 0.0f, y = 0.f, z = 0.0f;

};