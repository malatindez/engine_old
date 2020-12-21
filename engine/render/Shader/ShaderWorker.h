#ifndef SHADER_WORKER_H
#define SHADER_WORKER_H

#ifdef CERR_OUTPUT
#include <iostream>
#endif
#include <fstream>
#include <sstream>
#include <vector>

#include "ShaderProgram.h"
// Class for modyfing and compiling an existing shader
class ShaderWorker {
 public:
  class SourceCode {
   public:
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
    SourceCode(std::string source) {
      this->source_ = source;
      std::string current_line = "";
      for (size_t i = 0; i < source.size(); i++) {
        if (source[i] == '\r' or source[i] == '\n') {  // nextline
          if (current_line.size() > 8 and
              !memcmp(current_line.c_str(), "#version", 8)) {
            version_ = current_line;
          }
          if (current_line.size() > 7 and
              !memcmp(current_line.c_str(), "#define", 7)) {
            defines_.push_back(Define(current_line));
          }
          current_line = "";
        } else {
          current_line += source[i];
        }
      }
      if (current_line.size() > 8 and
          !memcmp(current_line.c_str(), "#version", 8)) {
        version_ = current_line;
      }
      if (current_line.size() > 7 and
          !memcmp(current_line.c_str(), "#define", 7)) {
        defines_.push_back(Define(current_line));
      }
    }
    ~SourceCode() {}

    std::vector<Define> getVariables() { return defines_; }

    void changeVersion(std::string new_version) {
      auto itr = this->source_.find(this->version_);
      this->source_.replace(itr, this->version_.size(), new_version);
    }
    bool changeVariable(std::string name, std::string new_value) {
      for (auto itr = defines_.begin(); itr != defines_.end(); itr++) {
        if ((*itr).name == name) {
          std::string previous = (*itr).text;
          std::string newtxt = (*itr).text;
          auto nitr = newtxt.find((*itr).value);
          (*itr).value = new_value;
          (*itr).text = newtxt;
          newtxt.replace(nitr, (*itr).value.size(), new_value);
          auto nnitr = this->source_.find(previous);
          this->source_.replace(nnitr, previous.size(), newtxt);
          return true;
        }
      }
      return false;
    }
    bool addDefinition(std::string define, std::string value = "") {
      for (auto itr = defines_.begin(); itr != defines_.end(); itr++) {
        if ((*itr).name == define) {
          return false;
        }
      }
      auto itr = this->source_.find("\n");
      defines_.push_back(Define("#define " + define + " " + value));
      this->source_.replace(itr, 0, "\n#define " + define + " " + value);
      return true;
    }
    bool removeDefinition(std::string define) {
      for (auto itr = defines_.begin(); itr != defines_.end(); itr++) {
        if ((*itr).name == define) {
          auto itr2 = this->source_.find((*itr).text);
          this->source_.replace(itr2, (*itr).text.size(), "");
          return true;
        }
      }
      return false;
    }

    std::vector<Define> defines_;
    std::string source_;
    std::string version_;  // #version xxx core
  };
  enum class Type { kVertex, kFragment, kGeometry };

  static std::string LoadSourceCode(std::string path) {
    std::ifstream shader_file;
    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    std::string return_value = "";
    try {
      shader_file.open(path);
      std::stringstream shader_stream;
      shader_stream << shader_file.rdbuf();
      shader_file.close();
      return_value = shader_stream.str();
    } catch (std::ifstream::failure&) {
#ifdef CERR_OUTPUT
      std::cerr << "ERROR. Error occured while reading a shader file"
                << std::endl;
#endif
      throw "file not found";
    }
    return return_value;
  }

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
  ShaderWorker(std::string vertex, std::string fragment,
               std::string geometry = "\0")
      : vertex_(vertex), fragment_(fragment), geometry_(geometry) {}
  ShaderProgram* assemble() const {
    ShaderProgram* returnValue = new ShaderProgram(
        vertex_.source_, fragment_.source_, geometry_.source_);
    return returnValue;
  }

 private:
  SourceCode& switchByType(Type type) {
    if (type == Type::kVertex) {
      return vertex_;
    }
    if (type == Type::kFragment) {
      return fragment_;
    }
    return geometry_;
  }

  SourceCode vertex_, fragment_, geometry_;
  std::string version_;
};

#endif