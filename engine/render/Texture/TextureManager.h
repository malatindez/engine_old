#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H
#include "Texture.h"
#include "hash/xxh3.h"
#include "FrameTicker.h"
#include <memory>
#include <unordered_map>
class TextureManager : public FrameTicker {
	std::unordered_map<XXH32_hash_t, std::shared_ptr<Texture>> textures;
public:
	TextureManager() : FrameTicker(4096) { }
	virtual void Update(const unsigned int tick, const float deltaTime) noexcept {
		if (tick % getTickRate() == 0) {
			std::vector<XXH32_hash_t> valuesToErase;
			for (auto kv : textures) {
				if (kv.second.unique()) {
					valuesToErase.push_back(kv.first);
				}
			}
			for (auto itr = valuesToErase.begin(); itr != valuesToErase.end(); itr++) {
				textures.erase((*itr));
			}
		}
	}
	std::shared_ptr<Texture> Load(std::string path, std::vector<Texture::glTexParameterValues> values, Texture::Type type) {
		int width, height, nrChannels;
		//stbi_set_flip_vertically_on_load(true);
		
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		if (data == nullptr) {
			throw EXCEPTIONS::FILE::PATH_NOT_FOUND;
		}
		try {
			return Load(data, width, height, nrChannels, values, type, path);
		}
		catch (EXCEPTIONS::HASH e) {
			stbi_image_free(data);
			throw e;
		}
		stbi_image_free(data);

	}
private:
	static XXH32_hash_t CalculateTextureHash(unsigned char* data, int width, int height, int channels, std::vector<Texture::glTexParameterValues> values) {
		XXH32_state_t* const state = XXH32_createState();
		if (state == nullptr) {
			throw EXCEPTIONS::HASH::FAILED_CREATING_STATE;
		}

		XXH64_hash_t const seed = 0;   /* or any other value */
		if (XXH32_reset(state, seed) == XXH_ERROR) {
			throw EXCEPTIONS::HASH::FAILED_INITIALIZING_STATE;
		}
		if (XXH32_update(state, data, width*height*channels) == XXH_ERROR) {
			throw EXCEPTIONS::HASH::FAILED_UPDATING_HASH;
		}
		for (auto itr = values.begin(); itr != values.end(); itr++) {
			if (XXH32_update(state, &(*itr).target, sizeof(GLenum)) == XXH_ERROR) {
				throw EXCEPTIONS::HASH::FAILED_UPDATING_HASH;
			}
			if (XXH32_update(state, &(*itr).pname, sizeof(GLenum)) == XXH_ERROR) {
				throw EXCEPTIONS::HASH::FAILED_UPDATING_HASH;
			}
			if (XXH32_update(state, &(*itr).type, sizeof(Texture::glTexParameterType)) == XXH_ERROR) {
				throw EXCEPTIONS::HASH::FAILED_UPDATING_HASH;
			}
			int size = 0;
			switch ((*itr).type) {
			case Texture::glTexParameterType::FLOAT:
				size = sizeof(GLfloat);
				break;
			case Texture::glTexParameterType::INTEGER:
				size = sizeof(GLint);
				break;
			case Texture::glTexParameterType::FLOATVEC:
				size = sizeof(glm::vec3);
				break;
			case Texture::glTexParameterType::IINTVEC:
			case Texture::glTexParameterType::INTVEC:
				size = sizeof(glm::ivec3);
				break;
			case Texture::glTexParameterType::IUINTVEC:
				size = sizeof(glm::uvec3);
				break;
			}
			if (XXH32_update(state, (*itr).ptr, size) == XXH_ERROR) {
				throw EXCEPTIONS::HASH::FAILED_UPDATING_HASH;
			}
		}

		XXH32_hash_t const hash = XXH32_digest(state);
		XXH32_freeState(state);
		return hash;
	}
public:
	std::shared_ptr<Texture> Load(unsigned char* data, int width, int height, int channels, std::vector<Texture::glTexParameterValues> values, Texture::Type type, std::string path = "") {
		XXH32_hash_t hash = CalculateTextureHash(data, width, height, channels, values);
		if (textures.find(hash) != textures.end()) {
			return textures.at(hash);
		}
		std::shared_ptr<Texture> returnValue(new Texture(data, width, height, channels, values, type, path));
		textures[hash] = returnValue;
		return returnValue;
	}
};

#endif
