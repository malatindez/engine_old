#ifndef PLANET_H
#define PLANET_H
#include "Actor.h"
#include "ShaderWorker.h"
template<int tickrate>
class Planet : public Actor {
public:
	Planet(std::vector<std::shared_ptr<Mesh>> meshes, 
		TextureManager* const mngr,
		std::string path,
		glm::vec3 coords = glm::vec3(0.0f),
		glm::vec3 angle = glm::vec3(3.1415927f),
		glm::vec3 scale = glm::vec3(1.0f)) : Actor(1, meshes, mngr, coords, angle, scale) {
		typedef Texture::glTexParameterValues tParam;
		std::vector<tParam> v;
		v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
		v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

		meshes[0]->textures.push_back(mngr->Load(path, v, Texture::Type::DIFFUSE));

	}
};
class Earth : public Actor {
protected:
	Earth(
		std::vector<std::shared_ptr<Mesh>> meshes,
		std::shared_ptr<std::vector<Mesh::Vertex>> vertices,
		std::shared_ptr<std::vector<unsigned int>> indices,
		std::vector<std::shared_ptr<Texture>> sphereTextures,
		TextureManager* const mngr,
		glm::vec3 coords,
		glm::vec3 angle,
		glm::vec3 scale
	) : Actor(1, meshes, mngr, coords, angle, scale) {
		cloudShader = std::shared_ptr<ShaderProgram>(
			new ShaderProgram(
				ShaderWorker::LoadSourceCode("triangle.vert"),
				ShaderWorker::LoadSourceCode("clouds.frag")));

		typedef Texture::glTexParameterValues tParam;
		std::vector<tParam> v;
		v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
		v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		std::vector<tParam> v2;
		v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		std::vector<std::shared_ptr<Mesh>> cloudMeshes;
		sphereTextures.clear();
		sphereTextures.push_back(mngr->Load("textures\\8k_earth_clouds.jpg", v2, Texture::Type::DIFFUSE));

		cloudMeshes.push_back(std::shared_ptr<Mesh>(new Mesh(vertices, indices, sphereTextures)));

		clouds = std::shared_ptr<Object>(new Object(cloudMeshes, mngr, coords, angle, scale * 1.01f));
		meshes[0]->textures.erase(meshes[0]->textures.begin(), meshes[0]->textures.end());

		meshes[0]->textures.push_back(mngr->Load("textures\\8k_earth_nightmap.jpg", v, Texture::Type::DIFFUSE));
		meshes[0]->textures.push_back(mngr->Load("textures\\8k_earth_normal_map.png", v, Texture::Type::NORMALS));
		meshes[0]->textures.push_back(mngr->Load("textures\\8k_earth_specular_map.png", v, Texture::Type::SPECULAR));
	}
public:
	std::shared_ptr<Object> clouds;
	std::shared_ptr<ShaderProgram> cloudShader;
	float stepf;
	virtual void Update(const unsigned int ticks, const float delta) noexcept {
		// 5 degrees per second
		this->rotateZ(3.1415927f / 36 * delta);
		stepf = ticks;
		this->clouds->setTranslationMatrix(this->translationMatrix);
		this->clouds->setRotationMatrix(this->rotationMatrix);
		this->clouds->setScaleMatrix(glm::scale(this->scaleMatrix,glm::vec3(1.01f)));
	}
	static std::shared_ptr<Actor> getInstance(
		std::shared_ptr<std::vector<Mesh::Vertex>> vertices,
		std::shared_ptr<std::vector<unsigned int>> indices,
		std::vector<std::shared_ptr<Texture>> sphereTextures,
		TextureManager* const mngr,
		glm::vec3 coords = glm::vec3(0.0f),
		glm::vec3 angle = glm::vec3(0.0f),
		glm::vec3 scale = glm::vec3(1.0f)) {

		std::vector<std::shared_ptr<Mesh>> meshes;
		meshes.push_back(std::shared_ptr<Mesh>(new Mesh(vertices, indices, sphereTextures)));
		return std::shared_ptr<Actor>(new Earth(meshes, vertices, indices, sphereTextures, mngr, coords, angle, scale));
	}
	virtual void Draw(std::shared_ptr<ShaderProgram> program, glm::mat4 matrix) noexcept {
		Object::Draw(program, matrix);
		cloudShader->use();
		cloudShader->setFloat("stepF", stepf);
		cloudShader->setMat4("fullMatrix", matrix);
		this->clouds->Draw(cloudShader);
	}
};
#endif