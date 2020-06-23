#ifndef OBJECT_H
#define OBJECT_H

#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include "Model.h"
class Object : public Model {
protected:
	glm::mat4 translationMatrix = glm::mat4(1.0f);
	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	glm::mat4 scaleMatrix = glm::mat4(1.0f);
	glm::mat4 fullMatrix = glm::mat4(1.0f);

	glm::mat4 bufTranslationMatrix = glm::mat4(1.0f);
	glm::mat4 bufRotationMatrix = glm::mat4(1.0f); 
	glm::mat4 bufScaleMatrix = glm::mat4(1.0f);


	void UpdateFullMatrix() noexcept;
public:
	// angle should be defined in radians
	Object(std::string path, TextureManager* const mngr, 
		glm::vec3 coords = glm::vec3(0.0f), glm::vec3 angle = glm::vec3(3.1415927f),
		glm::vec3 scale = glm::vec3(1.0f)
	) : Model(path, mngr) {
		this->move(coords);
		this->rotate(angle);
		this->scale(scale);
	}
	Object(std::vector<std::shared_ptr<Mesh>> meshes, TextureManager* const mngr,
		glm::vec3 coords = glm::vec3(0.0f), glm::vec3 angle = glm::vec3(0.0f),
		glm::vec3 scale = glm::vec3(1.0f)) : Model(meshes, mngr) {
		this->move(coords);
		this->rotate(angle);
		this->scale(scale);
	}
	// This function returns rotation & coordinate matrix, which we can process & use in shader
	glm::mat4 getObjectMatrix() noexcept;

	void move(const glm::vec3 coords) noexcept;
	void move(const float x, const float y, const float z) noexcept;


	// angle should be defined in radians
	void rotate(const float anglex, const float angley, const float anglez) noexcept;
	void rotate(const glm::vec3 angle) noexcept;
	void rotateX(const float angle) noexcept;
	void rotateY(const float angle) noexcept;
	void rotateZ(const float angle) noexcept;

	void scale(const glm::vec3 scale) noexcept;
	void scale(const int x, const int y, const int z) noexcept;
	void scaleX(const int scale) noexcept;
	void scaleY(const int scale) noexcept;
	void scaleZ(const int scale) noexcept;

	virtual void Draw(std::shared_ptr<ShaderProgram> program) noexcept {
		program->setMat4("model", getObjectMatrix());
		Model::Draw(program);
	}
};

#endif