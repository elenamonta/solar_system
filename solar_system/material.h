#pragma once
#include "glm/vec3.hpp"
#include "shader_maker.h"
#include <iostream>
#include <string>

enum MaterialType {
    RedPlastic,
    Brass,
    Emerald,
    SnowWhite,
    Yellow,
    Pink,
    Brown
};


class Material {
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess; 
    std::string name; 

    Material()
        : ambient(0.0f, 0.0f, 0.0f),
        diffuse(0.0f, 0.0f, 0.0f),
        specular(0.0f, 0.0f, 0.0f),
        shininess(0.0f),
        name("") {
    }

    Material(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shin, std::string materialName)
        : ambient(amb), diffuse(diff), specular(spec), shininess(shin), name(materialName){
    }


    static Material getMaterial(MaterialType type) {
        switch (type) {
        case MaterialType::RedPlastic:
            return Material({ 0.1, 0.0, 0.0 }, { 0.6, 0.1, 0.1 }, { 0.7, 0.6, 0.6 }, 150.0f, "RedPlastic");
        case MaterialType::Brass:
            return Material({ 0.5, 0.06, 0.015 }, { 0.78, 0.57, 0.11 }, { 0.99, 0.91, 0.81 }, 27.8f, "Brass");
        case MaterialType::Emerald:
            return Material({ 0.0215, 0.04745, 0.0215 }, { 0.07568, 0.61424, 0.07568 }, { 0.633, 0.727811, 0.633 }, 78.8f, "Emerald");
        case MaterialType::SnowWhite:
            return Material({ 0.2, 0.2, 0.2 }, { 0.95, 0.96, 0.98 }, { 0.8, 0.8, 0.8 }, 1.78125f, "SnowWhite");
        case MaterialType::Yellow:
            return Material({ 0.8, 0.8, 0.0 }, { 1.0, 1.0, 0.6 }, { 0.9, 0.9, 0.04 }, 1.78125f, "Yellow");
        case MaterialType::Pink:
            return Material({ 0.05f, 0.0f, 0.0f }, { 0.5f, 0.4f, 0.4f }, { 0.7f, 0.04f, 0.04f }, 1.78125f, "Pink");
        case MaterialType::Brown:
            return Material({ 0.19125f, 0.0735f, 0.0225f }, { 0.7038f, 0.27048f, 0.0828f }, { 0.256777f, 0.137622f, 0.086014f }, 12.8f, "Brown");
        }
        return Material({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, 0.0f, "");
    }


};