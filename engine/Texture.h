#ifndef TEXTURE_H
#define TEXTURE_H
#include "stb_image.h"
#include "exceptions.h"
#ifdef CERR_OUTPUT
#include <iostream>
#endif
#include <vector>
#include <stdint.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// after creating this object you should bind it(glBindTexture(GL_TEXTURE_2D, obj.getID());
// and set parameters
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// set texture filtering parameters
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 
;
class Texture {
    uint32_t id = 0;
    std::string path;

    // copy constructor
    Texture(const Texture& other) noexcept : type(Type::DIFFUSE) { }

    // move constructor
    Texture(Texture&& other) noexcept : type(Type::DIFFUSE)  {  }

    // copy assignment
    Texture& operator=(const Texture& other) noexcept { return *this; }

    // move assignment
    Texture& operator=(Texture&& other) noexcept { return *this;  }


    const std::string types[17] {
        "diffuse", "specular", "ambient", "emissive", 
        "height", "normals", "shininess", "opacity",
        "displacement", "lightmap", "reflection",
        "base_color", "normal_camera", "emission_color",
        "metalness", "diffuse_roughness", "ambient_occlusion"
    };
public:
    enum class Type { 
    /** The texture is combined with the result of the diffuse
    *  lighting equation.
    */        
        DIFFUSE,
    /** The texture is combined with the result of the specular
    *  lighting equation.
    */
        SPECULAR,
    /** The texture is combined with the result of the ambient
    *  lighting equation.
    */
        AMBIENT, 
    /** The texture is added to the result of the lighting
    *  calculation. It isn't influenced by incoming light.
    */
        EMISSIVE,
    /** The texture is a height map.
     *
     *  By convention, higher gray-scale values stand for
     *  higher elevations from the base height.
     */
        HEIGHT, 
    /** The texture is a (tangent space) normal-map.
     *
     *  Again, there are several conventions for tangent-space
     *  normal maps. Assimp does (intentionally) not
     *  distinguish here.
     */
        NORMALS, 
    /** The texture defines the glossiness of the material.
     *
     *  The glossiness is in fact the exponent of the specular
     *  (phong) lighting equation. Usually there is a conversion
     *  function defined to map the linear color values in the
     *  texture to a suitable exponent. Have fun.
    */
        SHININESS, 
     /** The texture defines per-pixel opacity.
     *
     *  Usually 'white' means opaque and 'black' means
     *  'transparency'. Or quite the opposite. Have fun.
    */
        OPACITY,
    /** Displacement texture
     *
     *  The exact purpose and format is application-dependent.
     *  Higher color values stand for higher vertex displacements.
    */
        DISPLACEMENT, 
     /** Lightmap texture (aka Ambient Occlusion)
     *
     *  Both 'Lightmaps' and dedicated 'ambient occlusion maps' are
     *  covered by this material property. The texture contains a
     *  scaling value for the final color value of a pixel. Its
     *  intensity is not affected by incoming light.
    */
        LIGHTMAP, 
    /** Reflection texture
     *
     * Contains the color of a perfect mirror reflection.
     * Rarely used, almost never for real-time applications.
    */
        REFLECTION, 
    /** PBR Materials
     * PBR definitions from maya and other modelling packages now use this standard.
     * This was originally introduced around 2012.
     * Support for this is in game engines like Godot, Unreal or Unity3D.
     * Modelling packages which use this are very common now.
     */
        BASE_COLOR, NORMAL_CAMERA, EMISSION_COLOR, 
        METALNESS, DIFFUSE_ROUGHNESS, AMBIENT_OCCLUSION    
    } type;
    enum class glTexParameterType { FLOAT, INTEGER, FLOATVEC, IINTVEC, INTVEC, IUINTVEC };
    struct glTexParameterValues {
        // copy constructor
        glTexParameterValues(const glTexParameterValues& other) {
            switch (other.type) {
            case glTexParameterType::FLOAT:
                ptr = new GLfloat((*(GLfloat*)other.ptr));
                break;
            case glTexParameterType::INTEGER:
                ptr = new GLint((*(GLint*)other.ptr));
                break;
            case glTexParameterType::FLOATVEC:
                ptr = new glm::vec3((*(glm::vec3*)other.ptr));
                break;
            case glTexParameterType::IINTVEC:
            case glTexParameterType::INTVEC:
                ptr = new glm::ivec3((*(glm::ivec3*)other.ptr));
                break;
            case glTexParameterType::IUINTVEC:
                ptr = new glm::uvec3((*(glm::uvec3*)other.ptr));
                break;
            }
            type = other.type;
            target = other.target;
            pname = other.pname;
        }

        // move constructor
        glTexParameterValues(glTexParameterValues&& other) noexcept {
            type = other.type;
            target = other.target;
            pname = other.pname;
            ptr = other.ptr; other.ptr = nullptr;
        }

        // copy assignment
        glTexParameterValues& operator=(const glTexParameterValues& other) {
            switch (other.type) {
            case glTexParameterType::FLOAT:
                ptr = new GLfloat((*(GLfloat*)other.ptr));
                break;
            case glTexParameterType::INTEGER:
                ptr = new GLint((*(GLint*)other.ptr));
                break;
            case glTexParameterType::FLOATVEC:
                ptr = new glm::vec3((*(glm::vec3*)other.ptr));
                break;
            case glTexParameterType::IINTVEC:
            case glTexParameterType::INTVEC:
                ptr = new glm::ivec3((*(glm::ivec3*)other.ptr));
                break;
            case glTexParameterType::IUINTVEC:
                ptr = new glm::uvec3((*(glm::uvec3*)other.ptr));
                break;
            }
            type = other.type;
            target = other.target;
            pname = other.pname;
            return *this;
        }

        // move assignment
        glTexParameterValues& operator=(glTexParameterValues&& other) noexcept {
            type = other.type;
            target = other.target;
            pname = other.pname;
            ptr = other.ptr; other.ptr = nullptr;
            return *this;
        }
        ~glTexParameterValues() {
            if (ptr != nullptr) {
                switch (type) {
                case glTexParameterType::FLOAT:
                    delete (GLfloat*)(ptr);
                    break;
                case glTexParameterType::INTEGER:
                    delete (GLint*)(ptr);
                    break;
                case glTexParameterType::FLOATVEC:
                    delete (glm::vec3*)(ptr);
                    break;
                case glTexParameterType::IINTVEC:
                case glTexParameterType::INTVEC:
                    delete (glm::ivec3*)(ptr);
                    break;
                case glTexParameterType::IUINTVEC:
                    delete (glm::uvec3*)(ptr);
                    break;
                }
            }
        }

        glTexParameterType type;
        GLenum target, pname;
        void* ptr; // pointer to memory where is our *type* value stored

        glTexParameterValues(GLenum target, GLenum pname, GLfloat param) noexcept : target(target), pname(pname) {
            this->ptr = new GLfloat(param);
            type = glTexParameterType::FLOAT;
        }
        glTexParameterValues(GLenum target, GLenum pname, GLint param) noexcept : target(target), pname(pname) {
            this->ptr = new GLint(param);
            type = glTexParameterType::INTEGER;
        }
        glTexParameterValues(GLenum target, GLenum pname, glm::vec3 param) noexcept : target(target), pname(pname) {
            this->ptr = new glm::vec3(param);
            type = glTexParameterType::FLOATVEC;
        }
        // modify = false -> glTexParameterIiv
        // modify = true -> glTexParameteriv
        glTexParameterValues(GLenum target, GLenum pname, glm::ivec3 param, bool modify = true) noexcept : target(target), pname(pname) {
            this->ptr = new glm::ivec3(param);
            if (not modify) {
                type = glTexParameterType::IINTVEC;
            }
            else {
                type = glTexParameterType::INTVEC;
            }
        }
        glTexParameterValues(GLenum target, GLenum pname, glm::uvec3 param) noexcept : target(target), pname(pname) {
            this->ptr = new glm::uvec3(param);
            type = glTexParameterType::IUINTVEC;
        }
        void execute() noexcept {
            if (ptr != nullptr) {
                switch (type) {
                case glTexParameterType::FLOAT:
                    glTexParameterf(target, pname, (*((GLfloat*)ptr)));
                    break;
                case glTexParameterType::INTEGER:
                    glTexParameteri(target, pname, (*((GLint*)ptr)));
                    break;
                case glTexParameterType::FLOATVEC:
                    glTexParameterfv(target, pname, ((GLfloat*)ptr));
                    break;
                case glTexParameterType::IINTVEC:
                    glTexParameterIiv(target, pname, ((GLint*)ptr));
                    break;
                case glTexParameterType::INTVEC:
                    glTexParameteriv(target, pname, ((GLint*)ptr));
                    break;
                case glTexParameterType::IUINTVEC:
                    glTexParameterIuiv(target, pname, ((GLuint*)ptr));
                    break;
                }
            }
        }
    };
private:
    const std::vector<glTexParameterValues> values;
public:

	Texture(unsigned char* data, int width, int height, int channels, std::vector<glTexParameterValues> values, Type type) : values(values), type(type) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        if (data) {
            GLenum format = GL_RED;
            if (channels == 1)
                format = GL_RED;
            else if (channels == 3)
                format = GL_RGB;
            else if (channels == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            for (auto itr = values.begin(); itr != values.end(); itr++) {
                itr->execute();
            }

        } else {
#ifdef CERR_OUTPUT
            std::cerr << "Failed to load texture" << std::endl;
#endif
            throw EXCEPTIONS::FILE::PATH_NOT_FOUND;
        }
	}

    ~Texture() {
        glDeleteTextures(1, &id);
    }

    uint32_t getID() const noexcept {
        return id;
    }
    std::string getPath() const noexcept {
        return path;
    }

};

#endif