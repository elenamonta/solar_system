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
#include "mesh.h"


class Model {
public:
	vector<Mesh> Model3D;
	vec3 positionVec, scaleVec, rotationVec; 
	vec4 min_BB_obj, max_BB_obj, min_BB, max_BB; 
	float angle; 

	Model(const char* path, vec3 pos, vec3 scale, vec3 rotation, float angleDegree)
	{
		positionVec = pos; 
		scaleVec = scale; 
		rotationVec = rotation; 
		angle = angleDegree;
		bool flag = loadAssImp(path);
		normalizeModel();
		for (int i = 0; i < Model3D.size(); i++) {
			Model3D[i].INIT_VAO();
			Model3D[i].findBB();
		}
		updateOverallBB();
		creaBB(); 
	}

	// rendering function
	void draw(Shader& shader, shaderOpt shaderType, Shader& shBB)
	{
		for (int i = 0; i < Model3D.size(); i++) {
			Model3D[i].setShader(shaderType); 
			Model3D[i].updateBB(); 
			//all obj meshes have mixFactor = 0.0f, because the final color is determinate on lighting calculations 
			Model3D[i].draw(shader, 0.0f);
		}
		updateOverallBB();
		drawBB(shBB); 
	}

	void clear_objModel() {
		int i;
		for (i = 0; i < Model3D.size(); i++)
		{
			Model3D.at(i).vertices.clear();
			Model3D.at(i).colors.clear();
			Model3D.at(i).normals.clear();
			Model3D.at(i).indices.clear();
			Model3D.at(i).texCoords.clear();
		}
	}

private: 
	float minX, maxX, minY, maxY, minZ, maxZ;
	vector<vec3> vertices;
	vector<vec4> colors;
	vector<GLuint> indices;
	GLuint VAO, VBO_vertices, VBO_colors, EBO_indices;

	// load texture with assimp
	bool loadAssImp(const char* path)
	{

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!scene) {
			fprintf(stderr, importer.GetErrorString());
			getchar();
			return false;
		}
		const aiMesh* mesh;

		// Fill vertices positions
		int num_meshes = scene->mNumMeshes;  //Numero di oggetti che compongono il modello
		
		for (int i = 0; i < num_meshes; ++i) {
			Mesh tmp(obj, "obj", positionVec, scaleVec, rotationVec, angle);
			Model3D.push_back(tmp);
		}

		for (unsigned int nm = 0; nm < num_meshes; nm++)
		{
			//Per ogni mesh dell'oggetto 
			mesh = scene->mMeshes[nm];
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			aiColor3D color;
			float value;
			// Read mtl file vertex data

			if (aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, color))
			{
				Model3D[nm].material.ambient = glm::vec3(color.r, color.g, color.b);
			}
			else
			{
				printf("Errore in ambientale \n");
			}

			if (aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
			{

				Model3D[nm].material.diffuse = glm::vec3(color.r, color.g, color.b);
				//cout << Model3D[nm].material.diffuse.x << " ," << Model3D[nm].material.diffuse.y << " ," << Model3D[nm].material.diffuse.z << endl;
			}
			else
			{
				Model3D[nm].material.diffuse = glm::vec3(1.0, 0.2, 0.1);
			}

			if (aiReturn_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, color))
			{
				Model3D[nm].material.specular = glm::vec3(color.r, color.g, color.b);
			}

			else
			{
				printf("Errore in specular \n");
				Model3D[nm].material.specular = glm::vec3(0.5, 0.5, 0.5);
			}
			if (aiReturn_SUCCESS == material->Get(AI_MATKEY_SHININESS_STRENGTH, value))
			{
				Model3D[nm].material.shininess = value;
			}
			else
			{
				Model3D[nm].material.shininess = 50.0f;

			}

			for (unsigned int i = 0; i < mesh->mNumVertices; i++) {

				aiVector3D pos = mesh->mVertices[i];
				Model3D[nm].vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));

			}

			// Fill vertices texture coordinates

			for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
				Model3D[nm].texCoords.push_back(glm::vec2(0.0, 0.0));
				Model3D[nm].colors.push_back(vec4(1.0, 0.0, 1.0, 1.0));  //Colore di default assegnato ad ogni vertice


			}

			// Fill vertices normals

			for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
				aiVector3D n = mesh->mNormals[i];
				Model3D[nm].normals.push_back(glm::vec3(n.x, n.y, n.z));
			}


			// Fill face indices

			for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
				// Assume the model has only triangles.
				Model3D[nm].indices.push_back(mesh->mFaces[i].mIndices[0]);
				Model3D[nm].indices.push_back(mesh->mFaces[i].mIndices[1]);
				Model3D[nm].indices.push_back(mesh->mFaces[i].mIndices[2]);
			}

		}
		return true;
	}

	// normalize the 3D model represented by a set of meshes
	void normalizeModel() {
		int i, k;
		float minx, miny, minz, maxx, maxy, maxz;
		vec3 centroid = { 0.0f, 0.0f, 0.0f };

		//Calcolo il centroide della mesh (facendo la media dei suoi vertici)
		int numVertices = 0;
		for (i = 0; i < Model3D.size(); i++)

			for (k = 0; k < Model3D[i].vertices.size(); k++)
			{
				centroid += Model3D[i].vertices[k];
				numVertices++;
			}


		centroid /= numVertices;


		for (i = 0; i < Model3D.size(); i++)
			for (k = 0; k < Model3D[i].vertices.size(); k++)
				Model3D[i].vertices[k] -= centroid;


		// Troviamo i valori minimi e massimi per tutte le coordinate del modello
		minX = minY = minZ = std::numeric_limits<float>::max();
		maxX = maxY = maxZ = std::numeric_limits<float>::lowest();

		// Iteriamo su tutte le mesh e troviamo i minimi e massimi globali

		for (i = 0; i < Model3D.size(); i++)
		{
			for (k = 0; k < Model3D[i].vertices.size(); k++)
			{
				minX = std::min(minX, Model3D[i].vertices[k].x);
				maxX = std::max(maxX, Model3D[i].vertices[k].x);
				minY = std::min(minY, Model3D[i].vertices[k].y);
				maxY = std::max(maxY, Model3D[i].vertices[k].y);
				minZ = std::min(minZ, Model3D[i].vertices[k].z);
				maxZ = std::max(maxZ, Model3D[i].vertices[k].z);
				
			}
		}

		
		// Calcoliamo if fattore di scala per ogni dimensione (per mantenere le proporzioni
		float rangeX = maxX - minX;
		float rangeY = maxY - minY;
		float rangeZ = maxZ - minZ;


		float maxRange = std::max({ rangeX, rangeY, rangeZ });
		for (i = 0; i < Model3D.size(); i++)
			for (k = 0; k < Model3D[i].vertices.size(); k++)
			{
				Model3D[i].vertices[k] /= maxRange; 
			}

		this->min_BB_obj = vec4(minX / maxRange, minY / maxRange, minZ / maxRange, 1.0f);
		this->max_BB_obj = vec4(maxX / maxRange, maxY / maxRange, maxZ / maxRange, 1.0f);

	}

	// update a global Bounding Box that contains all meshes BB
	void updateOverallBB() {
		for (int i = 0; i < Model3D.size(); i++) {
			this->min_BB = this->min_BB_obj;
			this->max_BB = this->max_BB_obj;
			this->min_BB = Model3D[i].Model * this->min_BB;
			this->max_BB = Model3D[i].Model * this->max_BB;
		}
	}

	void creaBB() {
		vertices = {
			{min_BB.x, min_BB.y, min_BB.z}, // V0
			{max_BB.x, min_BB.y, min_BB.z}, // V1
			{max_BB.x, max_BB.y, min_BB.z}, // V2
			{min_BB.x, max_BB.y, min_BB.z}, // V3
			{min_BB.x, min_BB.y, max_BB.z}, // V4
			{max_BB.x, min_BB.y, max_BB.z}, // V5
			{max_BB.x, max_BB.y, max_BB.z}, // V6
			{min_BB.x, max_BB.y, max_BB.z}  // V7
		};

		colors = {
			{1.0f, 0.0f, 0.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f},
		};


		indices = {
			0, 1, 1, 2, 2, 3, 3, 0, // Lato inferiore
			4, 5, 5, 6, 6, 7, 7, 4, // Lato superiore
			0, 4, 1, 5, 2, 6, 3, 7  // Connettori tra superiore e inferiore
		};

		int nv = vertices.size();
		indices.push_back(nv - 1);


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

		glGenBuffers(1, &EBO_indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	}

	void drawBB(Shader& shader) {
		updateOverallBB(); 
		
		for (int i = 0; i < Model3D.size(); i++) {
			shader.use();
			shader.setMat4("model", Model3D[i].Model);
		}
		glBindVertexArray(VAO);
		glDrawElements(GL_LINE_LOOP, (indices.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}

};