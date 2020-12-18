#ifndef SHADER_WORKER_H
#define SHADER_WORKER_H

#ifdef CERR_OUTPUT
#include <iostream>
#endif
#include "ShaderProgram.h"
#include <vector>
#include <fstream>
#include <sstream>
// Class for modyfing and compiling an existing shader
class ShaderWorker {
public:
	struct SourceCode {
		std::string source;
		std::string version; // #version xxx core
		// list of defines
		struct Define {
			std::string name, value, text;
			Define(std::string txt) {
				text = txt;
				txt.erase(txt.begin(), txt.begin() + 8);
				size_t i = 0;
				for (; txt[i] != ' ' and i < txt.size(); i++) {
					name += txt[i];
				}
				if (++i > txt.size()) {
					i = txt.size();
				}
				txt.erase(txt.begin(), txt.begin() + i);
				value = txt;
			}
		};
		std::vector<Define> defines;
		SourceCode(std::string source) {
			this->source = source;
			std::string current_line = "";
			for (size_t i = 0; i < source.size(); i++) {
				if (source[i] == '\r' or source[i] == '\n') { // nextline
					if (current_line.size() > 8 and !memcmp(current_line.c_str(), "#version", 8)) {
						version = current_line;
					}
					if (current_line.size() > 7 and !memcmp(current_line.c_str(), "#define", 7)) {
						defines.push_back(Define(current_line));
					}
					current_line = "";
				}
				else {
					current_line += source[i];
				}
			}
			if (current_line.size() > 8 and !memcmp(current_line.c_str(), "#version", 8)) {
				version = current_line;
			}
			if (current_line.size() > 7 and !memcmp(current_line.c_str(), "#define", 7)) {
				defines.push_back(Define(current_line));
			}
		}
		void changeVersion(std::string new_version) {
			auto itr = this->source.find(this->version);
			this->source.replace(itr, this->version.size(), new_version);
		}
		std::vector<Define> getVariables() {
			return defines;
		}
		bool changeVariable(std::string name, std::string new_value) {
			for (auto itr = defines.begin(); itr != defines.end(); itr++) {
				if ((*itr).name == name) {
					std::string previous = (*itr).text;
					std::string newtxt = (*itr).text;
					auto nitr = newtxt.find((*itr).value);
					(*itr).value = new_value;
					(*itr).text = newtxt;
					newtxt.replace(nitr, (*itr).value.size(), new_value);
					auto nnitr = this->source.find(previous);
					this->source.replace(nnitr, previous.size(), newtxt);
					return true;
				}
			}
			return false;
		}
		bool addDefinition(std::string define, std::string value = "") {
			for (auto itr = defines.begin(); itr != defines.end(); itr++) {
				if ((*itr).name == define) {
					return false;
				}
			}
			auto itr = this->source.find("\n");
			defines.push_back(Define("#define " + define + " " + value));
			this->source.replace(itr, 0, "\n#define " + define + " " + value);
			return true;
		} 
		bool removeDefinition(std::string define) {
			for (auto itr = defines.begin(); itr != defines.end(); itr++) {
				if ((*itr).name == define) {
					auto itr2 = this->source.find((*itr).text);
					this->source.replace(itr2, (*itr).text.size(), "");
					return true;
				}
			}
			return false;
		}
		~SourceCode() {

		}
	};
private:
	SourceCode vertex, fragment, geometry;
	std::string version;
public:
	enum class Type {VERTEX, FRAGMENT, GEOMETRY};
	enum class exceptions {FILE_NOT_FOUND};
	static std::string LoadSourceCode(std::string path) {
		std::ifstream shaderFile;
		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		std::string returnValue = "";
		try {
			shaderFile.open(path);
			std::stringstream shaderStream;
			shaderStream << shaderFile.rdbuf();
			shaderFile.close();
			returnValue = shaderStream.str();
		}
		catch (std::ifstream::failure&){
#ifdef CERR_OUTPUT
			std::cerr << "ERROR. Error occured while reading a shader file" << std::endl;
#endif
			throw exceptions::FILE_NOT_FOUND;
		}
		return returnValue;
	}
private:
	SourceCode& switchByType(Type type) {
		if (type == Type::VERTEX) {
			return vertex;
		}
		if (type == Type::FRAGMENT) {
			return fragment;
		}
		return geometry;
	}
public:
	void changeVersion(Type type, std::string new_version) {
		switchByType(type).changeVersion(new_version);
	}
	std::vector<SourceCode::Define> getVariables(Type type) {
		return switchByType(type).getVariables();
	}
	bool changeVariable(Type type, std::string name, std::string new_value) {
		return switchByType(type).changeVariable(name, new_value);
	}
	bool addDefinition(Type type, std::string define, std::string value = "") {
		return switchByType(type).addDefinition(define, value);
	}
	bool removeDefinition(Type type, std::string define) {
		return switchByType(type).removeDefinition(define);
	}
	ShaderWorker(std::string vertex, std::string fragment, std::string geometry = "\0") : vertex(vertex), fragment(fragment), geometry(geometry) {

	}
	ShaderProgram* assemble() const {
		ShaderProgram *returnValue = new ShaderProgram(vertex.source, fragment.source, geometry.source);
		return returnValue;
	}

};


#endif