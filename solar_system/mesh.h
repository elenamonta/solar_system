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
#include <algorithm>
#include "material.h"


using namespace glm; 
using namespace std; 

enum meshType {
	cubo, 
	sfera,
	obj
};

enum shaderOpt {
	Phong,
	BlinnPhong
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
	vec4 ancora_obj, ancora_world, min_BB_obj, max_BB_obj, min_BB, max_BB;
	GLuint textureID;
	vec3 positions, scaleFactor;
	int sceltaShader; 
	float angle; 

	Mesh(meshType type, string meshName, vec3 positionVec, vec3 scaleVec, vec3 rotationVec, float angleDegree)
	{
		if (type == meshType::cubo)
			crea_cubo();
		if (type == meshType::sfera)
			crea_sfera();

		name = meshName;
		material = Material();
		textureID = 0;
		ancora_obj = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		INIT_VAO();

		Model = mat4(1.0f);

		positions = positionVec;
		Model = translate(Model, positionVec);

		scaleFactor = scaleVec; 
		Model = scale(Model, scaleVec);

		angle = angleDegree; 
		Model = rotate(Model, radians(angle), rotationVec);

		// default shader
		sceltaShader = shaderOpt::BlinnPhong;
	}

	void setMaterial(Material newMaterial) {
		material = newMaterial;
	}

	void setTexture(GLuint id) {
		textureID = id;
	}

	void setShader(shaderOpt shader){
		sceltaShader = shader;
	}


	// rendering function
	// all meshes have mixFactor = 1.0, because the final color is determinate only on a texture  
	void draw(Shader& shader, float mixFactor) {
		shader.use();
		shader.setMat4("model", Model); 		
		shader.setVec3("material.ambient", material.ambient);
		shader.setVec3("material.diffuse", material.diffuse);
		shader.setVec3("material.specular", material.specular);
		shader.setFloat("material.shininess", material.shininess);
		shader.setFloat("mixFactor", mixFactor);
		shader.setInt("texture_diffuse", 0);
		shader.setInt("sceltaShader", sceltaShader);

		updateBB();

		ancora_world = ancora_obj; 
		ancora_world = Model * ancora_obj;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, (indices.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

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

	void DESTROY_VAO() {
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);

		glDeleteBuffers(1, &VBO_vertices);
		glDeleteBuffers(1, &VBO_colors);
		glDeleteBuffers(1, &VBO_normals);
		glDeleteBuffers(1, &VBO_texCoords);
		glDeleteBuffers(1, &EBO_indices);

		glDeleteVertexArrays(1, &VAO);
	}
	
	// create meshes Axis-Aligned Bounding Box
	void findBB() {
		minx = maxx = this->vertices[0].x;
		miny = maxy = this->vertices[0].y;
		minz = maxz = this->vertices[0].z;

		for (int i = 1; i < this->vertices.size(); i++) {
			minx = std::min(minx, this->vertices[i].x);
			miny = std::min(miny, this->vertices[i].y);
			minz = std::min(minz, this->vertices[i].z);

			maxx = std::max(maxx, this->vertices[i].x);
			maxy = std::max(maxy, this->vertices[i].y);
			maxz = std::max(maxz, this->vertices[i].z);
		}

		this->min_BB_obj = vec4(minx, miny, minz, 1.0f);
		this->max_BB_obj = vec4(maxx, maxy, maxz, 1.0f);
	}

	// update meshes BB
	void updateBB() {
		this->min_BB = this->min_BB_obj;
		this->max_BB = this->max_BB_obj;
		this->min_BB = this->Model * this->min_BB;
		this->max_BB = this->Model * this->max_BB;
	}
private:

	float minx, maxx, miny, maxy, minz, maxz;

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

		findBB();
	}

		
};