#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h>
#include "material.h"


using namespace glm; 
using namespace std; 

enum meshType {
	cubo, 
	sfera
};

class Mesh {
public: 
	vector<vec3> vertices;
	vector<vec4> colors; 
	vector<GLuint> indices;
	vector<vec3> normals; 
	vector<vec2> texCoords;
	mat4 Model; 
	GLuint VAO, VBO_vertices, VBO_colors, VBO_normals, EBO_indices, VBO_texCoords;
	string name; 
	Material material;
	vec4 ancora_obj;
	vec4 ancora_world;
	GLuint textureID;
	vec3 positions;

	Mesh(meshType type, string meshName) {
		switch (type) {
		case cubo:
			crea_cubo(); 
			break;
		case sfera:
			crea_sfera(); 
			break;
		default:
			break;
		}
		name = meshName;

		INIT_VAO();
	}

	void setMaterial(Material newMaterial) {
		material = newMaterial;
	}

	void setTexture(GLuint id) {
		textureID = id;
	}

	void draw(Shader& shader) {
		shader.use();
		shader.setMat4("model", Model);
		shader.setVec3("material.ambient", this->material.ambient);
		shader.setVec3("material.diffuse", this->material.diffuse);
		shader.setVec3("material.specular", this->material.specular);
		shader.setFloat("material.shininess", this->material.shininess);

		ancora_world = ancora_obj; 
		ancora_world = Model * ancora_obj;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, (indices.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

private:
	void INIT_VAO() {
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &VBO_colors);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_colors);
		glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec4), colors.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &VBO_normals);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), normals.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(2);

		glGenBuffers(1, &VBO_texCoords);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_texCoords);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(vec2), texCoords.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(3);

		glGenBuffers(1, &EBO_indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	}

	void crea_cubo() {
		texCoords = {
			{0.0f, 0.0f}, // Bottom-left
			{1.0f, 0.0f}, // Bottom-right
			{1.0f, 1.0f}, // Top-right
			{0.0f, 1.0f}  // Top-left
		};

		vertices = {
			{-1.0f, -1.0f, 1.0f},
			{1.0f, -1.0f, 1.0f},
			{1.0f, 1.0f, 1.0f},
			{-1.0f, 1.0f, 1.0f},
			{-1.0f, -1.0f, -1.0f},
			{1.0f, -1.0f, -1.0f},
			{1.0f, 1.0f, -1.0f},
			{-1.0f, 1.0f, -1.0f},
			{0.0f, 0.0f, 0.0f}
		};

		colors = {
			{1.0f, 0.0f, 0.0f, 0.5f},
			{0.0f, 1.0f, 0.0f, 0.5f},
			{0.0f, 0.0f, 1.0f, 0.5f},
			{1.0f, 0.0f, 1.0f, 0.5f},
			{1.0f, 1.0f, 1.0f, 0.5f},
			{1.0f, 1.0f, 1.0f, 0.5f},
			{1.0f, 1.0f, 1.0f, 0.5f},
			{1.0f, 1.0f, 1.0f, 0.5f},
			{0.0f, 1.0f, 0.0f, 1.0f},
		};

		ancora_obj = (vec4(0.0, 0.0, 0.0, 1.0));

		indices = {
			0,1,2,
			2,3,0,

			1,5,6,
			6,2,1,

			7,6,5,
			5,4,7,

			4,0,3,
			3,7,4,

			4,5,1,
			1,0,4,

			3,2,6,
			6,7,3
		};

		normals = {
			{0,0,1},
			{0,0,1},
			{0,0,1},
			{0,0,1},
			{0,0,-1},
			{0,0,-1},
			{0,0,-1},
			{0,0,-1},
		};

		int nv = vertices.size();
		indices.push_back(nv - 1);

		for (int i = 0; i < 8; i++) {
			texCoords.push_back(texCoords[i % texCoords.size()]);
		}

		for (int i = 0; i < 8; i++) {
			normals.push_back(normalize(normals[i]));
		}
	}

	void crea_sfera() {
		float s, t;
		vec3 centro = vec3(0.0, 0.0, 0.0);
		vec3 raggio = vec3(1.0, 1.0, 1.0);

		int Stacks = 30;
		int Slices = 30;

		for (int i = 0; i <= Stacks; ++i) {

			float V = i / (float)Stacks;
			float phi = V * pi <float>();

			for (int j = 0; j <= Slices; ++j) {

				float U = j / (float)Slices;
				float theta = U * (pi <float>() * 2);

				float x = centro.x + raggio.x * (cosf(theta) * sinf(phi));
				float y = centro.y + raggio.y * cosf(phi);
				float z = centro.z + raggio.z * sinf(theta) * sinf(phi);

				vertices.push_back(vec3(x, y, z)),
				normals.push_back(vec3(x, y, z));
				s = U;
				t = V;
				texCoords.push_back(vec2(s, t));

			}
		}

		for (int i = 0; i < Slices * Stacks + Slices; ++i) {
			indices.push_back(i);
			indices.push_back(i + Slices + 1);
			indices.push_back(i + Slices);
			indices.push_back(i + Slices + 1);
			indices.push_back(i);
			indices.push_back(i + 1);
		}

		vertices.push_back(vec3(0.0, 0.0, 0.0));
		colors.push_back(vec4(0.0, 1.0, 0.0, 1.0));
		ancora_obj = (vec4(0.0, 0.0, 0.0, 1.0));

		int nv = vertices.size();
		indices.push_back(nv - 1);
	}
};