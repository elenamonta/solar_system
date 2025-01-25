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
        
        return distance < 1.0f;
    }

    bool rayBoxIntersection(glm::vec4 min_BB, glm::vec4 max_BB, glm::vec3 rayStart, glm::vec3 rayDirection) {
        // Usa solo x, y, z delle bounding box (componente w non viene usata)
        glm::vec3 minBox(min_BB.x, min_BB.y, min_BB.z);
        glm::vec3 maxBox(max_BB.x, max_BB.y, max_BB.z);

        // Calcola i bounds relativi al raggio
        glm::vec3 boundsFirst = minBox - rayStart;
        glm::vec3 boundsSecond = maxBox - rayStart;

        // Calcola i tMin e tMax per ciascuna direzione
        float txMin = boundsFirst.x / rayDirection.x;
        float txMax = boundsSecond.x / rayDirection.x;
        float tyMin = boundsFirst.y / rayDirection.y;
        float tyMax = boundsSecond.y / rayDirection.y;
        float tzMin = boundsFirst.z / rayDirection.z;
        float tzMax = boundsSecond.z / rayDirection.z;

        // Calcola i tMin e tMax finali, tenendo conto delle intersezioni lungo tutte le dimensioni
        float tMin = std::max(std::min(txMin, txMax), std::min(tyMin, tyMax));
        tMin = std::max(tMin, std::min(tzMin, tzMax));

        float tMax = std::min(std::max(txMin, txMax), std::max(tyMin, tyMax));
        tMax = std::min(tMax, std::max(tzMin, tzMax));

        // Se tMax è maggiore di tMin, significa che c'è un'intersezione
        return tMax > tMin;
    }


private:
    float x = 0.0f, y = 0.f, z = 0.0f;

};