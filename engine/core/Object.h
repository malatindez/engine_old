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
		glm::vec3 coords = glm::vec3(0.0f), glm::vec3 angle = glm::vec3(2*3.1415927f),
		glm::vec3 scale = glm::vec3(1.0f)
	) : Model(path, mngr) {
		this->move(coords);
		this->rotate(angle);
		this->scale(scale);
	}
	// angle should be defined in radians
	Object(std::vector<std::shared_ptr<Mesh>> meshes, TextureManager* const mngr,
		glm::vec3 coords = glm::vec3(0.0f), glm::vec3 angle = glm::vec3(0.0f),
		glm::vec3 scale = glm::vec3(1.0f)) : Model(meshes, mngr) {
		this->move(coords);
		this->rotate(angle);
		this->scale(scale);
	}
	// This function returns rotation & coordinate matrix, which we can process & use in shader
	glm::mat4 getObjectMatrix() noexcept;

	//move object by this coords(object.x += coords.x, object.y += coords.y etc.)
	void move(const glm::vec3 coords) noexcept;
	//move object by this coords(object.x += coords.x, object.y += coords.y etc.)
	void move(const float x, const float y, const float z) noexcept;
	// set translation matrix
	void setTranslationMatrix(const glm::mat4 mat) noexcept;
	// set current position
	void setPosition(const glm::vec3 pos) noexcept;

	// angle should be defined in radians
	// rotate object by given angle
	void rotate(const float anglex, const float angley, const float anglez) noexcept;
	// angle should be defined in radians
	// rotate object by given angle
	void rotate(const glm::vec3 angle) noexcept;
	// angle should be defined in radians
	// rotate object by given angle
	void rotateX(const float angle) noexcept;
	// angle should be defined in radians
	// rotate object by given angle
	void rotateY(const float angle) noexcept;
	// angle should be defined in radians
	// rotate object by given angle
	void rotateZ(const float angle) noexcept;
	// set rotation matrix
	void setRotationMatrix(const glm::mat4 mat) noexcept;
	// set rotation angles
	void setRotation(const glm::vec3 angle) noexcept;
	// set rotation angles
	void setRotation(const float anglex, const float angley, const float anglez) noexcept;

	// scale object by value (transforms current scale)
	void scale(const glm::vec3 scale) noexcept;
	// scale object by value (transforms current scale)
	void scale(const int x, const int y, const int z) noexcept;
	// scale object by value (transforms current scale)
	void scaleX(const int scale) noexcept;
	// scale object by value (transforms current scale)
	void scaleY(const int scale) noexcept;
	// scale object by value (transforms current scale)
	void scaleZ(const int scale) noexcept;
	// set scale matrix
	void setScaleMatrix(const glm::mat4 mat) noexcept;
	// set current scale
	void setScale(const glm::vec3 scale) noexcept;

	virtual void Draw(std::shared_ptr<ShaderProgram> program, glm::mat4 matrix=glm::mat4(1.0f)) noexcept {
		program->setMat4("model", getObjectMatrix());
		Model::Draw(program);
	}
};

#endif