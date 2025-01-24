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
	}

	// rendering function
	void draw(Shader& shader, shaderOpt shaderType)
	{
		for (int i = 0; i < Model3D.size(); i++) {
			Model3D[i].setShader(shaderType); 
			//all obj meshes have mixFactor = 0.0f, because the final color is determinate on lighting calculations 
			Model3D[i].draw(shader, 0.0f);
		}
		updateOverallBB(); 
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

		//Calcolo il centroide della mesh (facendo la medua dei suoi vertici)
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
		minX = minX = minZ = std::numeric_limits<float>::max();
		maxX = maxY = maxZ = -std::numeric_limits<float>::max();

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

		this->min_BB_obj = vec4(minX, minY, minZ, 1.0f);
		this->max_BB_obj = vec4(maxX, maxY, maxZ, 1.0f);

		// Calcoliamo if fattore di scala per ogni dimensione (per mantenere le proporzioni
		float rangeX = maxX - minX;
		float rangeY = maxY - minY;
		float rangeZ = maxZ - minZ;


		float maxRange = std::max({ rangeX, rangeY, rangeZ });
		for (i = 0; i < Model3D.size(); i++)
			for (k = 0; k < Model3D[i].vertices.size(); k++)
			{
				Model3D[i].vertices[k].x = 2.0f * (Model3D[i].vertices[k].x - minX) / maxRange - 1.0f;
				Model3D[i].vertices[k].y = 2.0f * (Model3D[i].vertices[k].y - minY) / maxRange - 1.0f;
				Model3D[i].vertices[k].z = 2.0f * (Model3D[i].vertices[k].z - minZ) / maxRange - 1.0f;
				
			}
	}

	// update a global Bounding Box that contains all meshes BB
	void updateOverallBB() {
		this->min_BB = this->min_BB_obj;
		this->max_BB = this->max_BB_obj;

		for (int i = 0; i < Model3D.size(); i++) {
			min_BB.x = std::min(min_BB.x, Model3D[i].min_BB.x);
			max_BB.x = std::max(max_BB.x, Model3D[i].max_BB.x);

			min_BB.y = std::min(min_BB.y, Model3D[i].min_BB.y);
			max_BB.y = std::max(max_BB.y, Model3D[i].max_BB.y);

			min_BB.z = std::min(min_BB.z, Model3D[i].min_BB.z);
			max_BB.z = std::max(max_BB.z, Model3D[i].max_BB.z);
		}
	}
};