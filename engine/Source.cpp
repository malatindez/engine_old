
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION
#include "defines.h"
#include "Mesh.h"
#include "TextureManager.h"
#include "core.h"
#define SCR_WIDTH 1920
#define SCR_HEIGHT 1080
#define SCR_RES SCR_WIDTH, SCR_HEIGHT
#include "Actor.h"
#include "Player.h"
#include "Mesh.h"
#include "Planet.h"
std::pair<
	std::pair<
	std::shared_ptr<std::vector<Mesh::Vertex>>,
	std::shared_ptr<std::vector<unsigned int>>>, std::shared_ptr<Texture>> 
	getTriangleSphereMesh(TextureManager* const manager, unsigned int sectorCount = 256, unsigned int stackCount = 256, float radius = 1.0f) {
	std::shared_ptr<std::vector<Mesh::Vertex>> vertices(new std::vector<Mesh::Vertex>((stackCount + 1) * (sectorCount + 1)));
	std::shared_ptr<std::vector<unsigned int>> indices(new std::vector<unsigned int>);
	std::vector<glm::vec3> normals;
	float xy;
	float lengthInv = 1.0f / radius;    // vertex normal

	float sectorStep = 2 * 3.1415927f / sectorCount;
	float stackStep = 3.1415927f / stackCount;
	float sectorAngle, stackAngle;
	unsigned char* data = new unsigned char[3 * (sectorCount + 1) * (stackCount + 1)];

	int k1, k2;

	for (unsigned int i = 0, m = 0; i <= stackCount; ++i) {
		stackAngle = 3.1415927f / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = 1.0f * cosf(stackAngle);             // r * cos(u)
		float z = 1.0f * sinf(stackAngle);              // r * sin(u)
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (unsigned int j = 0; j <= sectorCount; ++j, ++k1, ++k2, ++m) {
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			(*vertices)[m].position.x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			(*vertices)[m].position.y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			(*vertices)[m].position.z = z;
			// normalized vertex normal (nx, ny, nz)
			data[i * sectorCount * 3 + j * 3 + 0] = (unsigned char)(128 + 255 * (float)(*vertices)[m].position.x * lengthInv);
			data[i * sectorCount * 3 + j * 3 + 1] = (unsigned char)(128 + 255 * (float)(*vertices)[m].position.y * lengthInv);
			data[i * sectorCount * 3 + j * 3 + 2] = (unsigned char)(128 + 255 * (float)(*vertices)[m].position.z * lengthInv);

			(*vertices)[m].texCoords.x = (float)j / sectorCount;
			(*vertices)[m].texCoords.y = (float)i / stackCount;
			//vertices[m] = vertex;
			if (i != 0) {
				indices->push_back(k1);
				indices->push_back(k2);
				indices->push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stackCount - 1)) {
				indices->push_back(k1 + 1);
				indices->push_back(k2);
				indices->push_back(k2 + 1);
			}
		}
	}
	typedef Texture::glTexParameterValues tParam;
	std::vector<tParam> v;
	v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
	v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

	std::shared_ptr<Texture> texture(manager->Load(data, sectorCount, stackCount, 3, v, Texture::Type::NORMALS));
	delete[] data;
	return std::pair<
		std::pair<
		std::shared_ptr<std::vector<Mesh::Vertex>>,
		std::shared_ptr<std::vector<unsigned int>>>, std::shared_ptr<Texture>> 
		(std::pair<std::shared_ptr<std::vector<Mesh::Vertex>>,
			std::shared_ptr<std::vector<unsigned int>>>(vertices,indices), texture);
}
#include "ShaderWorker.h"



int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_RES, "engine", NULL, NULL);
		
	glfwMakeContextCurrent(window);
	
	if (window == NULL) {
#ifdef CERR_OUTPUT
		std::cerr << "Failed to create GLFW window" << std::endl;
#endif
		glfwTerminate();
		return -1;
	}
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
#ifdef CERR_OUTPUT
		std::cerr << "Failed to initialize GLAD" << std::endl;
#endif
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	ShaderWorker s(ShaderWorker::LoadSourceCode("triangle.vert"), ShaderWorker::LoadSourceCode("triangle.frag"));
	std::shared_ptr<ShaderProgram> shader(s.assemble());
	Input input(window);
	Player player(&input, glm::vec3(0,1,0),glm::vec3(0,1,0));
	TextureManager* manager = new TextureManager();

	typedef Texture::glTexParameterValues tParam;
	std::vector<tParam> v;
	v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
	v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	
	std::vector<std::shared_ptr<Mesh>> meshes;
	//auto data = getTriangleSphereMesh(manager);

	//std::shared_ptr<std::vector<Mesh::Vertex>> sphereVertices = data.first.first;
	//std::shared_ptr<std::vector<unsigned int>> sphereIndices = data.first.second;
	//std::shared_ptr<Texture> sphereNormals = data.second;
	//std::vector<std::shared_ptr<Texture>> sphereTextures;
	//sphereTextures.push_back(sphereNormals);
//	std::shared_ptr<Mesh> sphereMesh(
//		new Mesh(sphereVertices, 
//			sphereIndices, 
//			sphereTextures));
//	meshes.push_back(sphereMesh);
	
	Actor actor(60,"Models\\cube.obj", manager,glm::vec3(0),glm::vec3(0),glm::vec3(1.0f));
	float deltaTime = 0;
	float lastFrame = 0;
	unsigned int frameTick = 0;
	actor.move(0, -1.0, 0);
	player.camera->setFOV(60);

	//Planet<1> mercury(meshes, manager, "a");

//	std::shared_ptr<Actor> earth = Earth::getInstance(sphereVertices, sphereIndices, sphereTextures, manager, glm::vec3(1,1,1),glm::vec3(-3.1415927f / 2,0,0), glm::vec3(10, 9.8, 10));
	while (!glfwWindowShouldClose(window)) {
		std::cout << player.getPosition().y << std::endl;
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
        input.Update(frameTick, deltaTime);
        player.Update(frameTick, deltaTime);
        actor.Update(frameTick, deltaTime);
		//earth->Update(frameTick, deltaTime);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
		shader->use();
		glm::mat4 matrix = glm::perspective(
			glm::radians(player.camera->getFOV()),
			(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.0000001f, 100.0f) * 
			player.camera->GetViewMatrix(); // projection matrix multiplied by view matrix
		shader->setMat4("fullMatrix", matrix);
		actor.Draw(shader);
		//earth->Draw(shader, matrix);
		frameTick++;
        glfwSwapBuffers(window);
        glfwPollEvents();
	}
}