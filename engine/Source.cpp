
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION
#include "Mesh.h"
#define CERR_OUTPUT
#include "TextureManager.h"
#include "core.h"
#define SCR_WIDTH 1920
#define SCR_HEIGHT 1080
#define SCR_RES SCR_WIDTH, SCR_HEIGHT
#include "Actor.h"
#include "Player.h"
std::shared_ptr<Mesh> getSphereMesh(TextureManager *const manager, unsigned int sectorCount = 128, unsigned int stackCount = 128, float radius = 1.0f) {
	std::vector<Mesh::Vertex> vertices; std::vector<unsigned int> indices;
	std::vector<glm::vec3> normals;
	Mesh::Vertex vertex;
	float xy;                              
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2 * 3.1415927f / sectorCount;
	float stackStep = 3.1415927f / stackCount;
	float sectorAngle, stackAngle;
	float tminx = 1.0f, tminy = 1.0f;
	unsigned char* data = new unsigned char[3 * (sectorCount+1) * (stackCount+1)];

	int k1, k2;

	for (int i = 0; i <= stackCount; ++i) {
		stackAngle = 3.1415927f / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = 1.0f * cosf(stackAngle);             // r * cos(u)
		vertex.position.z = 1.0f * sinf(stackAngle);              // r * sin(u)
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectorCount; ++j, ++k1, ++k2) {
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			vertex.position.x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			vertex.position.y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

			// normalized vertex normal (nx, ny, nz)
			data[i * sectorCount * 3 + j * 3 + 0] = 128 + 255 * vertex.position.x * lengthInv;
			data[i * sectorCount * 3 + j * 3 + 1] = 128 + 255 * vertex.position.y * lengthInv;
			data[i * sectorCount * 3 + j * 3 + 2] = 128 + 255 * vertex.position.z * lengthInv;

			vertex.texCoords.x = (float)j / sectorCount;
			vertex.texCoords.y = (float)i / stackCount;
			if (vertex.texCoords.x < tminx) {
				tminx = vertex.texCoords.x;
			}
			if (vertex.texCoords.y < tminx) { 
				tminx = vertex.texCoords.y;
			}
			vertices.push_back(vertex);
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stackCount - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	std::vector<std::shared_ptr<Texture>> textures;
	typedef Texture::glTexParameterValues tParam;
	std::vector<tParam> v;
	v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
	v.push_back(tParam(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	textures.push_back(manager->Load(data, sectorCount, stackCount, 3, v, Texture::Type::NORMALS));
	delete[] data;
	return std::shared_ptr<Mesh>(new Mesh(vertices, indices, textures));
}
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
	Player player(&input, glm::vec3(0,1,0));
	TextureManager* manager = new TextureManager();
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::shared_ptr<Mesh> sphereMesh = getSphereMesh(manager);
	meshes.push_back(sphereMesh);
	std::vector<Mesh::Vertex> ver; std::vector<unsigned int> indices; std::vector<std::shared_ptr<Texture>> t;
	Mesh::Vertex vertex;
	vertex.position = glm::vec3(0, 0, 0); vertex.texCoords = glm::vec2(0, 0);
	ver.push_back(vertex);
	vertex.position = glm::vec3(1, 0, 0); vertex.texCoords = glm::vec2(1, 0);
	ver.push_back(vertex);
	vertex.position = glm::vec3(0, 1, 0); vertex.texCoords = glm::vec2(0, 1);
	ver.push_back(vertex);
	vertex.position = glm::vec3(1, 1, 0); vertex.texCoords = glm::vec2(1, 1);
	ver.push_back(vertex);
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);
	Actor<1> sphere(meshes, manager);
	meshes.clear();
	std::shared_ptr<Mesh> m(new Mesh(ver, indices, t));
	m->textures.push_back(sphereMesh->textures[0]);
	meshes.push_back(m);
	Actor<1> square(meshes, manager);
	Actor<1> actor("Models\\WineBarrel\\objBarrel.obj", manager);
	sphere.move(1, 1, 1);
	square.move(-1, 1, -1);
	float deltaTime = 0;
	float lastFrame = 0;
	unsigned int frameTick = 0;
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
        input.Update(frameTick, deltaTime);
        player.Update(frameTick, deltaTime);
        actor.Update(frameTick, deltaTime);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
		shader->use();
		shader->setMat4("view", player.camera->GetViewMatrix());
		glm::mat4 projection = glm::perspective(
			glm::radians(player.camera->getFOV()),
			(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		shader->setMat4("projection", projection);
		actor.Draw(shader);
		sphere.Draw(shader);
		square.Draw(shader);
		frameTick++;
        glfwSwapBuffers(window);
        glfwPollEvents();
	}
}