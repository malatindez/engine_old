#ifndef OBJECT_H
#define OBJECT_H

#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure

#include <assimp/Importer.hpp>  // C++ importer interface

#include "../render/Model/Model.h"
class Object : public Model {
 public:
  // angle should be defined in radians
  Object(std::string path, TextureManager* const mngr,
         glm::vec3 coords = glm::vec3(0.0f),
         glm::vec3 angle = glm::vec3(2 * 3.1415927f),
         glm::vec3 scale = glm::vec3(1.0f))
      : Model(path, mngr) {
    this->Move(coords);
    this->Rotate(angle);
    this->Scale(scale);
  }

  // angle should be defined in radians
  Object(std::vector<std::shared_ptr<Mesh>> meshes, TextureManager* const mngr,
         glm::vec3 coords = glm::vec3(0.0f), glm::vec3 angle = glm::vec3(0.0f),
         glm::vec3 scale = glm::vec3(1.0f))
      : Model(meshes, mngr) {
    this->Move(coords);
    this->Rotate(angle);
    this->Scale(scale);
  }
  // This function returns rotation & coordinate matrix, which we can process &
  // use in shader

  glm::mat4 GetObjectMatrix() noexcept;

  // move object by this coords(object.x += coords.x, object.y += coords.y etc.)
  void Move(const glm::vec3 coords) noexcept;
  // move object by this coords(object.x += coords.x, object.y += coords.y etc.)
  void Move(const float x, const float y, const float z) noexcept;
  // set translation matrix
  void SetTranslationMatrix(const glm::mat4 mat) noexcept;
  // set current position
  void SetPosition(const glm::vec3 pos) noexcept;

  // angle should be defined in radians
  // rotate object by given angle
  void Rotate(const float anglex, const float angley,
              const float anglez) noexcept;
  // angle should be defined in radians
  // rotate object by given angle
  void Rotate(const glm::vec3 angle) noexcept;
  // angle should be defined in radians
  // rotate object by given angle
  void RotateX(const float angle) noexcept;
  // angle should be defined in radians
  // rotate object by given angle
  void RotateY(const float angle) noexcept;
  // angle should be defined in radians
  // rotate object by given angle
  void RotateZ(const float angle) noexcept;
  // set rotation matrix
  void SetRotationMatrix(const glm::mat4 mat) noexcept;
  // set rotation angles
  void SetRotation(const glm::vec3 angle) noexcept;
  // set rotation angles
  void SetRotation(const float anglex, const float angley,
                   const float anglez) noexcept;

  // scale object by value (transforms current scale)
  void Scale(const glm::vec3 scale) noexcept;
  // scale object by value (transforms current scale)
  void Scale(const int x, const int y, const int z) noexcept;
  // scale object by value (transforms current scale)
  void ScaleX(const int scale) noexcept;
  // scale object by value (transforms current scale)
  void ScaleY(const int scale) noexcept;
  // scale object by value (transforms current scale)
  void ScaleZ(const int scale) noexcept;
  // set scale matrix
  void SetScaleMatrix(const glm::mat4 mat) noexcept;
  // set current scale
  void SetScale(const glm::vec3 scale) noexcept;

#pragma warning(disable : 4100)
  virtual void Draw(std::shared_ptr<ShaderProgram> program,
                    glm::mat4 matrix = glm::mat4(1.0f)) noexcept {
    program->SetMat4("model", GetObjectMatrix());
    Model::Draw(program);
  }
#pragma warning(default : 4100)

 protected:
  void UpdateFullMatrix() noexcept;

  glm::mat4 translation_matrix_ = glm::mat4(1.0f);
  glm::mat4 rotation_matrix = glm::mat4(1.0f);
  glm::mat4 scale_matrix_ = glm::mat4(1.0f);
  glm::mat4 full_matrix_ = glm::mat4(1.0f);

  glm::mat4 buf_translation_matrix_ = glm::mat4(1.0f);
  glm::mat4 buf_rotation_matrix_ = glm::mat4(1.0f);
  glm::mat4 buf_scale_matrix_ = glm::mat4(1.0f);
};

#endif