#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include "shader.hpp"
#include "model.hpp" //Klasa za ucitavanje modela
#include "renderable.hpp" //Klasa za bafere
#include <irrKlang.h>
#include "stb_image.h"
#include "texture.hpp"
using namespace irrklang;


int WindowWidth = 1600;
int WindowHeight = 900;
const std::string WindowTitle = "AMOGUS 2.0";
const float TargetFPS = 60.0f;
const float TargetFrameTime = 1.0f / TargetFPS;
bool firstMouse = true;
double lastX;
double lastY;

float ugao = 0.0f;
float direkcija = 1.0f;
float jacina = 1.0f;
bool playedOnce = false;

unsigned CubeDiffuseTexture;
unsigned CubeSpecularTexture;
unsigned ListDif;
unsigned DrvoDifuse;
unsigned voda;
unsigned pesakDifuse;
unsigned vodaDifuse;
unsigned vodaSpecular;
unsigned sunceDifuse;
unsigned oblakDifuse;
unsigned mesecDifuse;

Model* Ivetic;
Model* Firepit;
Model* Lighthouse;
Model* Nebo;
Model* Fox;
Model* Amogus;

Renderable* cube;
Renderable* waterCube;
Renderable* ostrvo;
Renderable* drvo;
Renderable* leaf;

struct Params {
	float dt;
	bool test1 = false;
	bool clouds = true;
	bool cull = true;
	bool dan = true;

	bool isCurosIn;
	double xPosC = 0.0;
	double yPosC = 0.0;

	double frontX;
	double frontY;
	double frontZ;
	glm::vec3 cameraFront = glm::vec3(0.0, -0.5, 0.866025);
	glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

	glm::vec3 position = glm::vec3(0.0, 1.7, -3.0);
	glm::vec3 objPos = glm::vec3(0.0, 1.0, 0.0);

	double camYaw = 90;
	double camPitch = -30;

	float camX;
	float camY;
	float camZ;

	bool wDown;
	bool sDown;
	bool aDown;
	bool dDown;

	bool spaceDown = false;
	bool shiftDown = false;
};

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) //Aktivira se pri mijenjanju velicine prozora
{
	glViewport(0, 0, width, height);
	/*glViewport(X, Y, w, h) mapira prikaz scene na dio prozora. Vrijednosti parametara su pozitivni cijeli brojevi
	* (radi sa rezolucijom ekrana, ne sa NDC sistemom)
	* X, Y = koordinate donjeg lijevog coska (0,0 je donji lijevi cosak prozora)
	* width, height = duzina i visina prikaza
	 */
}

static void window_size_callback(GLFWwindow* window, int width, int height)
{
	WindowWidth = width;
	WindowHeight = height;
}

static void
KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	Params* params = (Params*)glfwGetWindowUserPointer(window);
	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		std::cout << "glm::vec3(" << params->objPos.x << "," << params->objPos.y << "," << params->objPos.z << ")" << std::endl;
	}

	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		params->clouds = !params->clouds;
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		params->dan = !params->dan;
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		params->test1 = !params->test1;
	}

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		params->cull = !params->cull;
	}

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		params->wDown = true;
	}
	if (key == GLFW_KEY_W && action == GLFW_RELEASE)
	{
		params->wDown = false;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		params->sDown = true;
	}
	if (key == GLFW_KEY_S && action == GLFW_RELEASE)
	{
		params->sDown = false;
	}

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		params->aDown = true;
	}
	if (key == GLFW_KEY_A && action == GLFW_RELEASE)
	{
		params->aDown = false;
	}

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		params->dDown = true;
	}
	if (key == GLFW_KEY_D && action == GLFW_RELEASE)
	{
		params->dDown = false;
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		params->spaceDown = true;
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
	{
		params->spaceDown = false;
	}

	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
	{
		params->shiftDown = true;
	}
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
	{
		params->shiftDown = false;
	}



	bool IsDown = action == GLFW_PRESS || action == GLFW_REPEAT;
	switch (key) {
	case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
	}
}

static void CursosPosCallback(GLFWwindow* window, double xPos, double yPos) {
	Params* params = (Params*)glfwGetWindowUserPointer(window);

	if (!params->test1) return;

	if (params->isCurosIn) {
		params->xPosC = xPos;
		params->yPosC = yPos;
	}

	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	double xoffset = xPos - lastX;
	double yoffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;

	float sensitivity = 0.3f;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	params->camYaw += xoffset;
	params->camPitch += yoffset;

	if (params->camPitch > 89.0) {
		params->camPitch = 89.0;
	}
	else if (params->camPitch < -89.0) {
		params->camPitch = -89.0;
	}

	glm::vec3 front;
	front.x = cos(glm::radians(params->camYaw)) * cos(glm::radians(params->camPitch));
	front.y = sin(glm::radians(params->camPitch));
	front.z = sin(glm::radians(params->camYaw)) * cos(glm::radians(params->camPitch));

	params->cameraFront = glm::normalize(front);
}

static void CursorEnterCallback(GLFWwindow* window, int entered) {
	Params* params = (Params*)glfwGetWindowUserPointer(window);

	params->isCurosIn = (bool)entered;
}

static void
ErrorCallback(int error, const char* description) {
	std::cerr << "GLFW Error: " << description << std::endl;
}

static void HandleInput(Params* params) {
	if (params->wDown)
	{
		if (params->test1)
			params->position += 7.2f * params->cameraFront * params->dt;
		else
			params->objPos.z += 0.5f * params->dt;
	}
	if (params->sDown)
	{
		if (params->test1)
			params->position -= 1.2f * params->cameraFront * params->dt;
		else
			params->objPos.z -= 0.5f * params->dt;
	}
	if (params->aDown)
	{

		glm::vec3 strafe = glm::cross(params->cameraFront, params->cameraUp);
		if (params->test1)
			params->position -= 1.2f * strafe * params->dt;
		else
			params->objPos.x += 0.5f * params->dt;
	}
	if (params->dDown)
	{
		glm::vec3 strafe = glm::cross(params->cameraFront, params->cameraUp);
		if (params->test1)
			params->position += 1.2f * strafe * params->dt;
		else
			params->objPos.x -= 0.5f * params->dt;
	}
	if (params->spaceDown)
	{
		if (params->test1)
			params->position.y += 1.2 * params->dt;
		else
			params->objPos.y += 0.5f * params->dt;
	}
	if (params->shiftDown)
	{
		if (params->test1)
			params->position.y -= 1.1 * params->dt;
		else
			params->objPos.y -= 0.5f * params->dt;
	}
}

void RenderScene(Shader &Basic, bool isSpot = false) {
	glm::mat4 m(1.0f);

	//AMOGUS
	m = glm::translate(glm::mat4(1.0f), glm::vec3(-0.659382, 0.520215, 0.192569));
	m = glm::rotate(m, glm::radians(150.0f), glm::vec3(0.0, 1.0, 0.0));
	m = glm::scale(m, glm::vec3(0.05, 0.05, 0.05));
	Basic.SetModel(m);
	Fox->Render();

	//DINP
	m = glm::translate(glm::mat4(1.0f), glm::vec3(-9.5107, 8.26764, 8.97615));
	m = glm::rotate(m, glm::radians(135.0f), glm::vec3(0.0, 1.0, 0.0));
	m = glm::scale(m, glm::vec3(0.12f));
	Basic.SetModel(m);
	Ivetic->Render();

	//Voda
	m = glm::translate(glm::mat4(1.0f), glm::vec3(0, abs(sin(glfwGetTime() / 2)) * 0.08, 0));
	m = glm::rotate(m, (float)glm::radians((sin(glfwGetTime() / 4)) * 0.6f), glm::vec3(1.0, 0.0, 1.0));
	m = glm::scale(m, glm::vec3(300, 0.9, 300));
	m = glm::scale(m, glm::vec3(0.4, 0.4, 0.4));
	Basic.SetModel(m);
	waterCube->Render(vodaDifuse, vodaSpecular);

	//Ostrvo 1
	m = glm::translate(glm::mat4(1.0f), glm::vec3(-0.8, 0.3, 0));
	m = glm::rotate(m, glm::radians(-10.0f), glm::vec3(0.0, 1.0, 0.0));
	m = glm::scale(m, glm::vec3(2.3, 1.7, 2.3));
	Basic.SetModel(m);
	ostrvo->Render(pesakDifuse, NULL);

	//Vatra 1
	m = glm::translate(glm::mat4(1.0f), glm::vec3(-0.8, 0.61, -0.6));
	m = glm::scale(m, glm::vec3(0.2, 0.2, 0.2));
	Basic.SetModel(m);
	Firepit->Render();

	//Ostrvo 2
	m = glm::translate(glm::mat4(1.0f), glm::vec3(4.5, 0.3, -4.5));
	m = glm::rotate(m, glm::radians(-10.0f), glm::vec3(0.0, 1.0, 0.0));
	m = glm::scale(m, glm::vec3(1.1, 3.0, 1.7));
	Basic.SetModel(m);
	ostrvo->Render(pesakDifuse, NULL);

	//Vatra 2
	m = glm::translate(glm::mat4(1.0f), glm::vec3(4.5, 0.751, -4.5));
	m = glm::scale(m, glm::vec3(0.2, 0.2, 0.2));
	Basic.SetModel(m);
	Firepit->Render();

	//Ostrvo 3
	m = glm::translate(glm::mat4(1.0f), glm::vec3(-7.0, 0.3, -5.0));
	m = glm::rotate(m, glm::radians(30.0f), glm::vec3(0.0, 1.0, 0.0));
	m = glm::scale(m, glm::vec3(2.0, 2.0, 1.6));
	Basic.SetModel(m);
	ostrvo->Render(pesakDifuse, NULL);

	//Vatra 3
	m = glm::translate(glm::mat4(1.0f), glm::vec3(-7.0, 0.61, -5.0));
	m = glm::scale(m, glm::vec3(0.2, 0.2, 0.2));
	Basic.SetModel(m);
	Firepit->Render();

	//Drvo
	m = glm::translate(glm::mat4(1.0f), glm::vec3(-0.904153, 0.235815, 0.0949862));
	m = glm::rotate(m, glm::radians(10.0f), glm::vec3(0.0, 0.0, 1.0));
	m = glm::scale(m, glm::vec3(0.6, 1.25, 0.6));
	Basic.SetModel(m);
	drvo->Render(DrvoDifuse, NULL);

	//Zbun na drvo
	m = glm::translate(glm::mat4(1.0f), glm::vec3(-1.17881, 1.82436, 0.093492));
	m = glm::rotate(m, glm::radians(10.0f), glm::vec3(0.0, 0.0, 1.0));
	m = glm::scale(m, glm::vec3(0.65, 0.08, 0.65));
	Basic.SetModel(m);
	drvo->Render(ListDif, NULL);

	//Leaf
	m = glm::translate(glm::mat4(1.0f), glm::vec3(-1.18293, 2.01013, 0.0989037));
	m = glm::rotate(m, glm::radians(-45.0f + (float)sin(glfwGetTime() + 10) * 3.f), glm::vec3(0.0, 1.0, 0.0));
	m = glm::rotate(m, glm::radians(135.0f + (float)sin(glfwGetTime() + 25) * 3.f), glm::vec3(1.0, 0.0, 0.0));
	m = glm::scale(m, glm::vec3(0.2, 0.2, 0.2));
	Basic.SetModel(m);
	leaf->Render(ListDif, NULL);

	//Basic.SetColor(0.25, 0.59, 0.22);
	m = glm::translate(glm::mat4(1.0f), glm::vec3(-1.18293, 2.01013, 0.0989037));
	m = glm::rotate(m, glm::radians(15.0f + (float)sin(glfwGetTime() + 13) * 3.f), glm::vec3(0.0, 1.0, 0.0));
	m = glm::rotate(m, glm::radians(135.0f + (float)sin(glfwGetTime() + 7) * 3.f), glm::vec3(1.0, 0.0, 0.0));
	m = glm::scale(m, glm::vec3(0.2, 0.2, 0.2));
	Basic.SetModel(m);
	leaf->Render(ListDif, NULL);

	//Basic.SetColor(0.3, 0.73, 0.28);
	m = glm::translate(glm::mat4(1.0f), glm::vec3(-1.18293, 2.01013, 0.0989037));
	m = glm::rotate(m, glm::radians(75.0f + (float)sin(glfwGetTime() + 32) * 3.f), glm::vec3(0.0, 1.0, 0.0));
	m = glm::rotate(m, glm::radians(135.0f + (float)sin(glfwGetTime() + 45) * 3.f), glm::vec3(1.0, 0.0, 0.0));
	m = glm::scale(m, glm::vec3(0.2, 0.2, 0.2));
	Basic.SetModel(m);
	leaf->Render(ListDif, NULL);

	//Basic.SetColor(0.19, 0.62, 0.16);
	m = glm::translate(glm::mat4(1.0f), glm::vec3(-1.18293, 2.01013, 0.0989037));
	m = glm::rotate(m, glm::radians(135.0f + (float)sin(glfwGetTime() + 74) * 3.f), glm::vec3(0.0, 1.0, 0.0));
	m = glm::rotate(m, glm::radians(135.0f + (float)sin(glfwGetTime() + 11) * 3.f), glm::vec3(1.0, 0.0, 0.0));
	m = glm::scale(m, glm::vec3(0.2, 0.2, 0.2));
	Basic.SetModel(m);
	leaf->Render(ListDif, NULL);

	//Basic.SetColor(0.15, 0.41, 0.14);
	m = glm::translate(glm::mat4(1.0f), glm::vec3(-1.18293, 2.01013, 0.0989037));
	m = glm::rotate(m, glm::radians(195.0f + (float)sin(glfwGetTime() + 19) * 3.f), glm::vec3(0.0, 1.0, 0.0));
	m = glm::rotate(m, glm::radians(135.0f + (float)sin(glfwGetTime() + 16) * 3.f), glm::vec3(1.0, 0.0, 0.0));
	m = glm::scale(m, glm::vec3(0.2, 0.2, 0.2));
	Basic.SetModel(m);
	leaf->Render(ListDif, NULL);

	//Basic.SetColor(0.23, 0.49, 0.21);
	m = glm::translate(glm::mat4(1.0f), glm::vec3(-1.18293, 2.01013, 0.0989037));
	m = glm::rotate(m, glm::radians(255.0f + (float)sin(glfwGetTime() + 37) * 3.f), glm::vec3(0.0, 1.0, 0.0));
	m = glm::rotate(m, glm::radians(135.0f + (float)sin(glfwGetTime() + 69) * 3.f), glm::vec3(1.0, 0.0, 0.0));
	m = glm::scale(m, glm::vec3(0.2, 0.2, 0.2));
	Basic.SetModel(m);
	leaf->Render(ListDif, NULL);
	//Leaf-----------

	if (!isSpot) {
		//Svetionik
		m = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0, 0.0, 10.0));
		m = glm::scale(m, glm::vec3(0.3, 0.3, 0.3));
		Basic.SetModel(m);
		Lighthouse->Render();
	}
	
}

int main() {
	GLFWwindow* Window = 0;
	if (!glfwInit()) {
		std::cerr << "Failed to init glfw" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwSetErrorCallback(ErrorCallback);


	Window = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle.c_str(), 0, 0);
	if (!Window) {
		std::cerr << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSetWindowPos(Window, 20, 30);

	glfwMakeContextCurrent(Window);
	glfwSetKeyCallback(Window, KeyCallback);
	glfwSetCursorPosCallback(Window, CursosPosCallback);
	glfwSetCursorEnterCallback(Window, CursorEnterCallback);
	glfwSetWindowSizeCallback(Window, window_size_callback);

	glfwSetInputMode(Window, GLFW_STICKY_KEYS, 1);
	glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);

	GLenum GlewError = glewInit();
	if (GlewError != GLEW_OK) {
		std::cerr << "Failed to init glew: " << glewGetErrorString(GlewError) << std::endl;
		glfwTerminate();
		return -1;
	}

	Shader ColorShader("shaders/fbo.vert", "shaders/fbo.frag");
	Shader shadowMapProgram("shaders/shadowMap.vert", "shaders/shadowMap.frag");
	Shader Stari("shaders/basic.vert", "shaders/basic.frag");
	Shader Basic("shaders/basic-phong.vert", "shaders/phong_material_texture.frag");

	glUseProgram(Basic.GetId());
	Basic.SetUniform3f("uDirLight.Direction", glm::vec3(-27.232, -34.2576, 2.09927));
	Basic.SetUniform3f("uDirLight.Ka", glm::vec3(255.0 / 255 / 2, 238.0 / 255 / 2, 204.0 / 255 / 2));
	Basic.SetUniform3f("uDirLight.Kd", glm::vec3(255.0 / 255 / 2, 238.0 / 255 / 2, 204.0 / 255 / 2));
	Basic.SetUniform3f("uDirLight.Ks", glm::vec3(1.0f));

	//Pointlight:
	//Vatre
	glm::vec3 kdVatra = glm::vec3(230.0 / 255 / 0.1, 92.0 / 255 / 0.1, 0.0f);
	glm::vec3 kaVatra = glm::vec3(230.0 / 255 / 50, 92.0 / 255 / 50, 0.0f);
	float klVatra = 1.5f;
	float kcVatra = 1.0f;
	float kqVatra = 0.272f;

	Basic.SetUniform3f("uPointLights[0].Position", glm::vec3(-0.8, 0.61, -0.6));
	Basic.SetUniform3f("uPointLights[0].Ka", kaVatra);
	Basic.SetUniform3f("uPointLights[0].Kd", kdVatra);
	Basic.SetUniform3f("uPointLights[0].Ks", glm::vec3(1.0f));
	Basic.SetUniform1f("uPointLights[0].Kc", kcVatra);
	Basic.SetUniform1f("uPointLights[0].Kl", klVatra);
	Basic.SetUniform1f("uPointLights[0].Kq", kqVatra);

	Basic.SetUniform3f("uPointLights[1].Position", glm::vec3(4.5, 0.751, -4.5));
	Basic.SetUniform3f("uPointLights[1].Ka", kaVatra);
	Basic.SetUniform3f("uPointLights[1].Kd", kdVatra);
	Basic.SetUniform3f("uPointLights[1].Ks", glm::vec3(1.0f));
	Basic.SetUniform1f("uPointLights[1].Kc", kcVatra);
	Basic.SetUniform1f("uPointLights[1].Kl", klVatra);
	Basic.SetUniform1f("uPointLights[1].Kq", kqVatra);

	Basic.SetUniform3f("uPointLights[3].Position", glm::vec3(-7.0, 0.61, -5.0));
	Basic.SetUniform3f("uPointLights[3].Ka", kaVatra);
	Basic.SetUniform3f("uPointLights[3].Kd", kdVatra);
	Basic.SetUniform3f("uPointLights[3].Ks", glm::vec3(1.0f));
	Basic.SetUniform1f("uPointLights[3].Kc", kcVatra);
	Basic.SetUniform1f("uPointLights[3].Kl", klVatra);
	Basic.SetUniform1f("uPointLights[3].Kq", kqVatra);

	//Sunce
	Basic.SetUniform3f("uPointLights[2].Position", glm::vec3(27.232, 34.2576, -2.09927));
	Basic.SetUniform3f("uPointLights[2].Ka", glm::vec3(0.5f, 0.5f, 0.5f));
	Basic.SetUniform3f("uPointLights[2].Kd", glm::vec3(0.1f, 0.1f, 0.1f));
	Basic.SetUniform3f("uPointLights[2].Ks", glm::vec3(1.0f));
	Basic.SetUniform1f("uPointLights[2].Kc", 1.0f);
	Basic.SetUniform1f("uPointLights[2].Kl", 0.092f);
	Basic.SetUniform1f("uPointLights[2].Kq", 0.032f);


	//Sportlight
	Basic.SetUniform3f("uSpotlights[0].Position", glm::vec3(-10.0, 10.0, 10.0));
	Basic.SetUniform3f("uSpotlights[0].Direction", glm::vec3(0.0f, -1.0f, 1.0f));
	Basic.SetUniform3f("uSpotlights[0].Ka", glm::vec3(0.0f, 0.0f, 0.0f));
	Basic.SetUniform3f("uSpotlights[0].Kd", glm::vec3(3.0f, 3.0f, 3.0f));
	Basic.SetUniform3f("uSpotlights[0].Ks", glm::vec3(1.0f));
	Basic.SetUniform1f("uSpotlights[0].Kc", 1.0f);
	Basic.SetUniform1f("uSpotlights[0].Kl", 0.092f);
	Basic.SetUniform1f("uSpotlights[0].Kq", 0.032f);
	Basic.SetUniform1f("uSpotlights[0].InnerCutOff", glm::cos(glm::radians(15.0f)));
	Basic.SetUniform1f("uSpotlights[0].OuterCutOff", glm::cos(glm::radians(20.0f)));

	Basic.SetUniform3f("uSpotlights[1].Position", glm::vec3(-10.0, 10.0, 10.0));
	Basic.SetUniform3f("uSpotlights[1].Direction", glm::vec3(0.0f, -1.0f, -1.0f));
	Basic.SetUniform3f("uSpotlights[1].Ka", glm::vec3(0.0f, 0.0f, 0.0f));
	Basic.SetUniform3f("uSpotlights[1].Kd", glm::vec3(3.0f, 3.0f, 3.0f));
	Basic.SetUniform3f("uSpotlights[1].Ks", glm::vec3(1.0f));
	Basic.SetUniform1f("uSpotlights[1].Kc", 1.0f);
	Basic.SetUniform1f("uSpotlights[1].Kl", 0.092f);
	Basic.SetUniform1f("uSpotlights[1].Kq", 0.032f);
	Basic.SetUniform1f("uSpotlights[1].InnerCutOff", glm::cos(glm::radians(15.0f)));
	Basic.SetUniform1f("uSpotlights[1].OuterCutOff", glm::cos(glm::radians(20.0f)));
	// NOTE(Jovan): Diminishes the light's diffuse component by half, tinting it slightly red
	Basic.SetUniform1i("uMaterial.Kd", 0);
	// NOTE(Jovan): Makes the object really shiny
	Basic.SetUniform1i("uMaterial.Ks", 1);
	Basic.SetUniform1f("uMaterial.Shininess", 32.0f);
	glUseProgram(0);


	//Ucitavanje modela
	Amogus = new Model("bice/uploads_files_2695291_among+us.obj");
	if (!Amogus->Load())
	{
		std::cout << "Failed to load model!\n";
		glfwTerminate();
		return -1;
	}

	Fox = new Model("res/pokemon/Woodpecker.obj");
	if (!Fox->Load()) {
		std::cerr << "Failed to load fox\n";
		glfwTerminate();
		return -1;
	}

	Nebo = new Model("res/nebo/nebo.obj");
	if (!Nebo->Load()) {
		std::cerr << "Failed to load fox\n";
		glfwTerminate();
		return -1;
	}

	Lighthouse = new Model("res/Lighthouse/Lighthouse.obj");
	if (!Lighthouse->Load()) {
		std::cerr << "Failed to load svetionik\n";
		glfwTerminate();
		return -1;
	}

	Firepit = new Model("res/vatra/PUSHILIN_campfire.obj");
	if (!Firepit->Load()) {
		std::cerr << "Failed to load svetionik\n";
		glfwTerminate();
		return -1;
	}

	Ivetic = new Model("bice/ivetic.obj");
	if (!Ivetic->Load()) {
		std::cerr << "Failed to load svetionik\n";
		glfwTerminate();
		return -1;
	}


	float cubeVertices[] = //Bijela kocka. Boju cemo mijenjati sa uCol
	{
		-0.2, -0.2, -0.2,       0.0, 0.0, 0.0,
		+0.2, -0.2, -0.2,       0.0, 0.0, 0.0,
		-0.2, -0.2, +0.2,       0.0, 0.0, 0.0,
		+0.2, -0.2, +0.2,       0.0, 0.0, 0.0,

		-0.2, +0.2, -0.2,       0.0, 0.0, 0.0,
		+0.2, +0.2, -0.2,       0.0, 0.0, 0.0,
		-0.2, +0.2, +0.2,       0.0, 0.0, 0.0,
		+0.2, +0.2, +0.2,       0.0, 0.0, 0.0,
	};

	unsigned int cubeIndices[] = {
		0, 1, 3,
		0, 2, 3,

		4, 6, 7,
		4, 5, 7,

		3, 6, 2,
		3, 6, 7,

		0, 4, 1,
		1, 4, 5,

		0, 6, 2,
		0, 4, 6,

		1, 3, 7,
		1, 7, 5
	}; //Indeksi za formiranje kocke

	float funkyCubeVertices[] = //Sarena kocka
	{
		-0.2, -0.2, -0.2,       0.0, 0.0, 1.0,
		+0.2, -0.2, -0.2,       0.0, 1.0, 0.0,
		-0.2, -0.2, +0.2,       0.0, 1.0, 1.0,
		+0.2, -0.2, +0.2,       1.0, 0.0, 0.0,

		-0.2, +0.2, -0.2,       1.0, 0.0, 1.0,
		+0.2, +0.2, -0.2,       1.0, 1.0, 0.0,
		-0.2, +0.2, +0.2,       1.0, 1.0, 1.0,
		+0.2, +0.2, +0.2,       0.0, 0.0, 0.0,
	};

	float cubeVertices2[] =
	{
		//front
		-0.2, -0.2,  0.2,   0, 0, 0,
		0.2, -0.2,  0.2,    0, 0, 0,
		0.2,  0.2,  0.2,    0, 0, 0,
		-0.2,  0.2,  0.2,   0, 0, 0,
		// back
		-0.2, -0.2, -0.2,   0, 0, 0,
		0.2, -0.2, -0.2,    0, 0, 0,
		0.2,  0.2, -0.2,    0, 0, 0,
		-0.2,  0.2, -0.2,   0, 0, 0
	};

	unsigned int cubeIndices2[] = {
		//front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	float islandVert[] =
	{
		//front
		-0.2, -0.2,  0.2,   0, 0, 0,
		0.2, -0.2,  0.2,    0, 0, 0,
		0.2,  0.2,  0.2,    0, 0, 0,
		-0.2,  0.2,  0.2,   0, 0, 0,
		// back
		-0.1, -0.1, -0.2,   0, 0, 0,
		0.1, -0.1, -0.2,    0, 0, 0,
		0.1,  0.1, -0.2,    0, 0, 0,
		-0.1,  0.1, -0.2,   0, 0, 0
	};

	float leafVert[] =
	{
		//krug
		0, 0, 0,   0, 0, 0,
		0.5,  0,  0.2,    0, 0, 0,
		1.0,  0,  0.0,    0, 0, 0,
		0.5, 0,  -0.2,   0, 0, 0,
		// gore
		0.5, 0.1, 0.0,   0, 0, 0
	};

	unsigned int leafInd[] = {
		0,3,2,
		2,1,0,
		0,1,4,
		4,1,2,
		2,3,4,
		4,3,0
	};


	//Kocka sa teksturom
	std::vector<float> CubeVertices = {
		// X     Y     Z     NX    NY    NZ    U     V    FRONT SIDE
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // L D
		 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // R D
		-0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // L U
		 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // R D
		 0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // R U
		-0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // L U
		// LEFT SIDE
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // L D
		-0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
		-0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
		-0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
		-0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // R U
		-0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
		// RIGHT SIDE
		0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // L D
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
		0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
		0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // R U
		0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
		// BOTTOM SIDE
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // L D
		 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // R D
		-0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // L U
		 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // R D
		 0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // R U
		-0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // L U
		// TOP SIDE
		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // L D
		 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // R D
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // L U
		 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // R D
		 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // R U
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // L U
		// BACK SIDE
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // L D
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // R D
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // L U
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // R D
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // R U
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // L U
	};

	//Kocka za vodu
	std::vector<float> WaterVert = {
		// X     Y     Z     NX    NY    NZ    U     V    FRONT SIDE
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // L D
		 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 100.0f, 0.0f, // R D
		-0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 100.0f, // L U
		 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 100.0f, 0.0f, // R D
		 0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 100.0f, 100.0f, // R U
		-0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 100.0f, // L U
		// LEFT SIDE
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // L D
		-0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 100.0f, 0.0f, // R D
		-0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 100.0f, // L U
		-0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 100.0f, 0.0f, // R D
		-0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 100.0f, 100.0f, // R U
		-0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 100.0f, // L U
		// RIGHT SIDE
		0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // L D
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 100.0f, 0.0f, // R D
		0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 100.0f, // L U
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 100.0f, 0.0f, // R D
		0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 100.0f, 100.0f, // R U
		0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 100.0f, // L U
		// BOTTOM SIDE
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // L D
		 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 100.0f, 0.0f, // R D
		-0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 100.0f, // L U
		 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 100.0f, 0.0f, // R D
		 0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 100.0f, 100.0f, // R U
		-0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 100.0f, // L U
		// TOP SIDE
		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // L D
		 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 100.0f, 0.0f, // R D
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 100.0f, // L U
		 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 100.0f, 0.0f, // R D
		 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 100.0f, 100.0f, // R U
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 100.0f, // L U
		// BACK SIDE
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // L D
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 100.0f, 0.0f, // R D
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 100.0f, // L U
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 100.0f, 0.0f, // R D
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 100.0f, 100.0f, // R U
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 100.0f, // L U
	};

	//Ludi list
	std::vector<float> LudiListVert =
	{
		0.000000, 0.000000, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 0.000000, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		1.000000, 2.493926, 0.000000 ,0.3961, 0.1588, -0.9044, 0.262880, 0.349115,
		0.000000, 2.493926, -0.437991 ,0.3961, 0.1588, -0.9044, 0.411849, 0.338619,
		0.000000, 4.987852, 0.000000 ,0.3961, 0.1588, -0.9044, 0.411849, 0.680616,
		0.000000, 4.987852, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 4.987852, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.3961, -0.1588, -0.9044, 0.513113, 0.341152,
		0.000000, 0.000000, 0.000000 ,-0.3961, -0.1588, -0.9044, 0.537150, 0.682304,
		-1.000000, 2.493926, -0.000000 ,-0.3961, -0.1588, -0.9044, 0.411849, 0.341152,
		0.000000, 2.493926, 0.000000 ,-0.0000, -0.0000, 1.0000, 0.131440, 0.672199,
		0.000000, 4.987852, 0.000000 ,-0.0000, -0.0000, 1.0000, 0.036935, 1.000000,
		-1.000000, 2.493926, -0.000000 ,-0.0000, -0.0000, 1.0000, 0.000000, 0.634305,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 0.000000, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		-1.000000, 2.493926, -0.000000 ,-0.0000, -0.0000, -1.0000, 0.000000, 0.634305,
		0.000000, 0.000000, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 4.987852, 0.000000 ,-0.3961, 0.1588, -0.9044, 0.537150, 0.000000,
		0.000000, 2.493926, -0.437991 ,-0.3961, 0.1588, -0.9044, 0.513113, 0.341152,
		-1.000000, 2.493926, -0.000000 ,-0.3961, 0.1588, -0.9044, 0.411849, 0.341152,
		0.000000, 0.000000, 0.000000 ,0.3961, -0.1588, -0.9044, 0.363893, 0.000000,
		0.000000, 2.493926, -0.437991 ,0.3961, -0.1588, -0.9044, 0.411849, 0.338619,
		1.000000, 2.493926, 0.000000 ,0.3961, -0.1588, -0.9044, 0.262880, 0.349115,
		0.000000, 0.000000, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.131440, 0.672199,
		0.000000, 0.000000, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		1.000000, 2.493926, 0.000000 ,-0.0000, -0.0000, 1.0000, 0.262880, 0.710093,
		0.000000, 4.987852, 0.000000 ,-0.0000, -0.0000, 1.0000, 0.036935, 1.000000,
		0.000000, 2.493926, 0.000000 ,-0.0000, -0.0000, 1.0000, 0.131440, 0.672199,
		0.000000, 4.987852, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 4.987852, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 0.000000, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.262880, 0.000000,
		0.000000, 0.000000, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 0.000000, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		1.000000, 2.493926, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.262880, 0.349115,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.411849, 0.338619,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.411849, 0.338619,
		0.000000, 4.987852, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-1.0000, -0.0000, -0.0000, 0.513113, 0.341152,
		0.000000, 2.493926, -0.437991 ,-1.0000, -0.0000, -0.0000, 0.513113, 0.341152,
		0.000000, 0.000000, 0.000000 ,-1.0000, -0.0000, -0.0000, 0.537150, 0.682304,
		0.000000, 2.493926, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.131440, 0.672199,
		0.000000, 4.987852, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.036935, 1.000000,
		0.000000, 4.987852, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.036935, 1.000000,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 0.000000, 0.000000 ,-0.0000, -0.0000, 1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, 0.000000 ,-0.0000, -0.0000, 1.0000, 0.131440, 0.672199,
		-1.000000, 2.493926, -0.000000 ,-0.0000, -0.0000, 1.0000, 0.000000, 0.634305,
		0.000000, 4.987852, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.537150, 0.000000,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.513113, 0.341152,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.513113, 0.341152,
		0.000000, 0.000000, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.363893, 0.000000,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.411849, 0.338619,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.411849, 0.338619,
		0.000000, 0.000000, 0.000000 ,-0.0000, -0.0000, 1.0000, 0.225944, 0.344398,
		1.000000, 2.493926, 0.000000 ,-0.0000, -0.0000, 1.0000, 0.262880, 0.710093,
		0.000000, 2.493926, 0.000000 ,-0.0000, -0.0000, 1.0000, 0.131440, 0.672199,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		1.000000, 2.493926, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.262880, 0.710093,
		0.000000, 4.987852, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.036935, 1.000000,
		0.000000, 4.987852, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.036935, 1.000000,
		0.000000, 4.987852, 0.000000 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,-0.0000, -0.0000, -1.0000, 0.225944, 0.344398,
		0.000000, 0.000000, 0.000000 ,1.0000, -0.0000, -0.0000, 0.225944, 0.344398,
		0.000000, 2.493926, -0.437991 ,1.0000, -0.0000, -0.0000, 0.262880, 0.000000,
		0.000000, 2.493926, -0.437991 ,1.0000, -0.0000, -0.0000, 0.262880, 0.000000
	};

	std::vector<float> MojModelVert = {
		-1.000000, 1.000000, -1.000000 ,-0.0000, 1.0000, -0.0000, 0.875000, 0.500000,
		1.000000, 1.000000, 1.000000 ,-0.0000, 1.0000, -0.0000, 0.625000, 0.750000,
		1.000000, 1.000000, -1.000000 ,-0.0000, 1.0000, -0.0000, 0.625000, 0.500000,
		1.000000, 1.000000, 1.000000 ,-0.0000, -0.0000, 1.0000, 0.625000, 0.750000,
		-1.000000, -1.000000, 1.000000 ,-0.0000, -0.0000, 1.0000, 0.375000, 1.000000,
		1.000000, -1.000000, 1.000000 ,-0.0000, -0.0000, 1.0000, 0.375000, 0.750000,
		-1.000000, 1.000000, 1.000000 ,-1.0000, -0.0000, -0.0000, 0.625000, 0.000000,
		-1.000000, -1.000000, -1.000000 ,-1.0000, -0.0000, -0.0000, 0.375000, 0.250000,
		-1.000000, -1.000000, 1.000000 ,-1.0000, -0.0000, -0.0000, 0.375000, 0.000000,
		1.000000, -1.000000, -1.000000 ,-0.0000, -1.0000, -0.0000, 0.375000, 0.500000,
		-1.000000, -1.000000, 1.000000 ,-0.0000, -1.0000, -0.0000, 0.125000, 0.750000,
		-1.000000, -1.000000, -1.000000 ,-0.0000, -1.0000, -0.0000, 0.125000, 0.500000,
		1.000000, 1.000000, -1.000000 ,1.0000, -0.0000, -0.0000, 0.625000, 0.500000,
		1.000000, -1.000000, 1.000000 ,1.0000, -0.0000, -0.0000, 0.375000, 0.750000,
		1.000000, -1.000000, -1.000000 ,1.0000, -0.0000, -0.0000, 0.375000, 0.500000,
		-1.000000, 1.000000, -1.000000 ,-0.0000, -0.0000, -1.0000, 0.625000, 0.250000,
		1.000000, -1.000000, -1.000000 ,-0.0000, -0.0000, -1.0000, 0.375000, 0.500000,
		-1.000000, -1.000000, -1.000000 ,-0.0000, -0.0000, -1.0000, 0.375000, 0.250000,
		-1.000000, 1.000000, -1.000000 ,-0.0000, 1.0000, -0.0000, 0.875000, 0.500000,
		-1.000000, 1.000000, 1.000000 ,-0.0000, 1.0000, -0.0000, 0.875000, 0.750000,
		1.000000, 1.000000, 1.000000 ,-0.0000, 1.0000, -0.0000, 0.625000, 0.750000,
		1.000000, 1.000000, 1.000000 ,-0.0000, -0.0000, 1.0000, 0.625000, 0.750000,
		-1.000000, 1.000000, 1.000000 ,-0.0000, -0.0000, 1.0000, 0.625000, 1.000000,
		-1.000000, -1.000000, 1.000000 ,-0.0000, -0.0000, 1.0000, 0.375000, 1.000000,
		-1.000000, 1.000000, 1.000000 ,-1.0000, -0.0000, -0.0000, 0.625000, 0.000000,
		-1.000000, 1.000000, -1.000000 ,-1.0000, -0.0000, -0.0000, 0.625000, 0.250000,
		-1.000000, -1.000000, -1.000000 ,-1.0000, -0.0000, -0.0000, 0.375000, 0.250000,
		1.000000, -1.000000, -1.000000 ,-0.0000, -1.0000, -0.0000, 0.375000, 0.500000,
		1.000000, -1.000000, 1.000000 ,-0.0000, -1.0000, -0.0000, 0.375000, 0.750000,
		-1.000000, -1.000000, 1.000000 ,-0.0000, -1.0000, -0.0000, 0.125000, 0.750000,
		1.000000, 1.000000, -1.000000 ,1.0000, -0.0000, -0.0000, 0.625000, 0.500000,
		1.000000, 1.000000, 1.000000 ,1.0000, -0.0000, -0.0000, 0.625000, 0.750000,
		1.000000, -1.000000, 1.000000 ,1.0000, -0.0000, -0.0000, 0.375000, 0.750000,
		-1.000000, 1.000000, -1.000000 ,-0.0000, -0.0000, -1.0000, 0.625000, 0.250000,
		1.000000, 1.000000, -1.000000 ,-0.0000, -0.0000, -1.0000, 0.625000, 0.500000,
		1.000000, -1.000000, -1.000000 ,-0.0000, -0.0000, -1.0000, 0.375000, 0.500000
	};

	std::vector<float> LudoOstrvoVert =
	{
		-0.848521, 0.125669, -0.487467 ,-0.5267, 0.8143, -0.2440, 0.719524, 0.606671,
		-0.999970, -0.125915, -1.000000 ,-0.5267, 0.8143, -0.2440, 0.408940, 0.679622,
		-1.232760, -0.125915, -0.497572 ,-0.5267, 0.8143, -0.2440, 0.406107, 0.598983,
		0.500030, -0.125915, -1.000000 ,-0.0000, 0.6007, -0.7994, 0.569531, 0.686003,
		0.845211, 0.125669, -0.810945 ,-0.0000, 0.6007, -0.7994, 0.502292, 0.990002,
		1.000030, -0.125915, -1.000000 ,-0.0000, 0.6007, -0.7994, 0.498799, 0.679622,
		1.000030, -0.125915, 0.500000 ,0.6810, 0.7098, -0.1801, 0.187695, 0.731852,
		0.813274, 0.125669, 0.785465 ,0.6810, 0.7098, -0.1801, 0.236136, 0.423062,
		1.190113, -0.125915, 1.218740 ,0.6810, 0.7098, -0.1801, 0.292974, 0.737172,
		-0.591142, 0.125669, 0.852532 ,0.0297, 0.6923, 0.7210, 0.409895, 0.076688,
		-1.100910, -0.125915, 1.115088 ,0.0297, 0.6923, 0.7210, 0.721392, 0.000000,
		-0.478845, -0.125915, 1.089492 ,0.0297, 0.6923, 0.7210, 0.720216, 0.095537,
		1.000030, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.296724, 0.069890,
		0.500030, -0.125915, 1.000000 ,-0.0000, -1.0000, -0.0000, 0.377223, 0.143420,
		0.500030, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.300208, 0.146905,
		-0.499970, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.898394, 0.756628,
		-0.895586, 0.125669, 0.665704 ,-0.0000, 1.0000, -0.0000, 0.921004, 0.694480,
		-0.591142, 0.125669, 0.852532 ,-0.0000, 1.0000, -0.0000, 0.952017, 0.739994,
		0.500030, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.905748, 0.910640,
		0.152163, 0.125669, 0.813325 ,-0.0000, 1.0000, -0.0000, 0.951446, 0.854760,
		0.549283, 0.125669, 0.751806 ,-0.0000, 1.0000, -0.0000, 0.944892, 0.916374,
		0.000030, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.303693, 0.223920,
		-0.478845, -0.125915, 1.089492 ,-0.0000, -1.0000, -0.0000, 0.397829, 0.293573,
		-0.499970, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.307177, 0.300935,
		0.500030, -0.125915, 1.000000 ,0.1310, 0.7012, 0.7009, 0.717287, 0.245442,
		0.152163, 0.125669, 0.813325 ,0.1310, 0.7012, 0.7009, 0.408301, 0.190769,
		0.021155, -0.125915, 1.089492 ,0.1310, 0.7012, 0.7009, 0.719721, 0.172506,
		-0.499970, -0.125915, -1.000000 ,-0.4091, 0.8563, -0.3152, 0.371933, 0.425464,
		-0.061333, 0.125669, -0.885809 ,-0.4091, 0.8563, -0.3152, 0.338203, 0.738635,
		-0.131998, -0.125915, -1.477620 ,-0.4091, 0.8563, -0.3152, 0.292974, 0.420758,
		0.000030, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.149663, 0.230888,
		-0.499970, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.230162, 0.304419,
		-0.499970, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.153147, 0.307903,
		0.000030, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.748059, 0.840989,
		0.500030, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.828742, 0.914317,
		0.500030, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.751736, 0.917995,
		-0.499970, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.744382, 0.763983,
		-0.775212, 0.125669, -0.058707 ,-0.0000, 1.0000, -0.0000, 0.810322, 0.718347,
		-0.499970, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.821388, 0.760305,
		0.500030, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.146178, 0.153873,
		1.000030, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.219709, 0.073374,
		0.500030, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.223193, 0.150389,
		0.878223, 0.125669, -0.314602 ,0.5200, 0.7539, 0.4016, 0.066901, 0.421003,
		1.000030, -0.125915, 0.000000 ,0.5200, 0.7539, 0.4016, 0.111447, 0.730411,
		1.506038, -0.125915, -0.655272 ,0.5200, 0.7539, 0.4016, 0.000000, 0.737172,
		-0.999970, 0.125669, 0.500000 ,-0.9522, -0.0000, 0.3055, 0.720111, 0.452860,
		-1.146257, -0.125915, 0.043998 ,-0.9522, -0.0000, 0.3055, 0.409840, 0.517218,
		-0.999970, -0.125915, 0.500000 ,-0.9522, -0.0000, 0.3055, 0.414461, 0.449493,
		0.500030, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.146178, 0.153873,
		0.000030, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.226678, 0.227404,
		0.000030, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.149663, 0.230888,
		0.878223, 0.125669, -0.314602 ,-0.0000, 1.0000, -0.0000, 0.783072, 0.974877,
		0.500030, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.828742, 0.914317,
		0.927971, 0.125669, 0.011686 ,-0.0000, 1.0000, -0.0000, 0.833690, 0.980139,
		0.813274, 0.125669, 0.785465 ,-0.1945, 0.7653, 0.6135, 0.407036, 0.292297,
		0.500030, -0.125915, 1.000000 ,-0.1945, 0.7653, 0.6135, 0.717287, 0.245442,
		1.190113, -0.125915, 1.218740 ,-0.1945, 0.7653, 0.6135, 0.721392, 0.353584,
		0.500030, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.905748, 0.910640,
		0.813274, 0.125669, 0.785465 ,-0.0000, 1.0000, -0.0000, 0.952017, 0.956784,
		0.757849, 0.125669, 0.314420 ,-0.0000, 1.0000, -0.0000, 0.879063, 0.951712,
		0.000030, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.303693, 0.223920,
		0.500030, -0.125915, 1.000000 ,-0.0000, -1.0000, -0.0000, 0.377223, 0.143420,
		0.021155, -0.125915, 1.089492 ,-0.0000, -1.0000, -0.0000, 0.394345, 0.216558,
		-0.061333, 0.125669, -0.885809 ,0.3047, 0.8629, -0.4032, 0.635120, 1.000000,
		0.500030, -0.125915, -1.000000 ,0.3047, 0.8629, -0.4032, 0.569531, 0.686003,
		-0.131998, -0.125915, -1.477620 ,0.3047, 0.8629, -0.4032, 0.688189, 0.684043,
		-0.499970, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.744382, 0.763983,
		0.000030, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.825065, 0.837311,
		0.000030, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.748059, 0.840989,
		-0.499970, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.153147, 0.307903,
		-1.146257, -0.125915, 0.043998 ,-0.0000, -1.0000, -0.0000, 0.241443, 0.403660,
		-1.232760, -0.125915, -0.497572 ,-0.0000, -1.0000, -0.0000, 0.158628, 0.420758,
		-0.617593, 0.125669, -0.853728 ,-0.0000, 0.5026, -0.8645, 0.731467, 0.010804,
		-0.499970, -0.125915, -1.000000 ,-0.0000, 0.5026, -0.8645, 0.792023, 0.313820,
		-0.999970, -0.125915, -1.000000 ,-0.0000, 0.5026, -0.8645, 0.721392, 0.321230,
		-0.999970, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.310661, 0.377949,
		-0.478845, -0.125915, 1.089492 ,-0.0000, -1.0000, -0.0000, 0.397829, 0.293573,
		-1.100910, -0.125915, 1.115088 ,-0.0000, -1.0000, -0.0000, 0.406107, 0.389211,
		0.000030, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.902071, 0.833634,
		-0.591142, 0.125669, 0.852532 ,-0.0000, 1.0000, -0.0000, 0.952017, 0.739994,
		0.152163, 0.125669, 0.813325 ,-0.0000, 1.0000, -0.0000, 0.951446, 0.854760,
		0.021155, -0.125915, 1.089492 ,-0.0000, 0.6856, 0.7279, 0.719721, 0.172506,
		-0.591142, 0.125669, 0.852532 ,-0.0000, 0.6856, 0.7279, 0.409895, 0.076688,
		-0.478845, -0.125915, 1.089492 ,-0.0000, 0.6856, 0.7279, 0.720216, 0.095537,
		-0.999970, -0.125915, -1.000000 ,-0.0000, -1.0000, -0.0000, 0.079616, 0.388402,
		-0.499970, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.153147, 0.307903,
		-1.232760, -0.125915, -0.497572 ,-0.0000, -1.0000, -0.0000, 0.158628, 0.420758,
		-0.255395, 0.125669, -0.686101 ,-0.0000, 1.0000, -0.0000, 0.717519, 0.803019,
		0.000030, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.748059, 0.840989,
		-0.061333, 0.125669, -0.885809 ,-0.0000, 1.0000, -0.0000, 0.688189, 0.834376,
		0.845211, 0.125669, -0.810945 ,-0.0000, 1.0000, -0.0000, 0.706386, 0.973444,
		0.500030, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.751736, 0.917995,
		0.878223, 0.125669, -0.314602 ,-0.0000, 1.0000, -0.0000, 0.783072, 0.974877,
		0.500030, -0.125915, -1.000000 ,-0.0000, -1.0000, -0.0000, 0.069163, 0.157358,
		0.000030, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.149663, 0.230888,
		-0.131998, -0.125915, -1.477620 ,-0.0000, -1.0000, -0.0000, 0.000000, 0.258037,
		0.845211, 0.125669, -0.810945 ,0.4046, 0.6953, -0.5940, 0.502292, 0.990002,
		1.506038, -0.125915, -0.655272 ,0.4046, 0.6953, -0.5940, 0.406107, 0.680401,
		1.000030, -0.125915, -1.000000 ,0.4046, 0.6953, -0.5940, 0.498799, 0.679622,
		1.000030, -0.125915, -1.000000 ,-0.0000, -1.0000, -0.0000, 0.065679, 0.080343,
		0.500030, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.146178, 0.153873,
		0.500030, -0.125915, -1.000000 ,-0.0000, -1.0000, -0.0000, 0.069163, 0.157358,
		-0.617593, 0.125669, -0.853728 ,-0.0000, 1.0000, -0.0000, 0.689038, 0.748469,
		-0.499970, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.744382, 0.763983,
		-0.255395, 0.125669, -0.686101 ,-0.0000, 1.0000, -0.0000, 0.717519, 0.803019,
		0.403579, 0.125669, -0.795677 ,-0.0000, 1.0000, -0.0000, 0.705489, 0.905315,
		0.000030, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.748059, 0.840989,
		0.500030, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.751736, 0.917995,
		-0.499970, -0.125915, -1.000000 ,-0.0000, -1.0000, -0.0000, 0.076132, 0.311388,
		0.000030, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.149663, 0.230888,
		-0.499970, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.153147, 0.307903,
		-1.146257, -0.125915, 0.043998 ,-0.5473, 0.8324, 0.0874, 0.409840, 0.517218,
		-0.848521, 0.125669, -0.487467 ,-0.5473, 0.8324, 0.0874, 0.719524, 0.606671,
		-1.232760, -0.125915, -0.497572 ,-0.5473, 0.8324, 0.0874, 0.406107, 0.598983,
		-0.499970, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.821388, 0.760305,
		0.000030, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.902071, 0.833634,
		0.000030, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.825065, 0.837311,
		-0.499970, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.230162, 0.304419,
		-0.999970, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.310661, 0.377949,
		-1.146257, -0.125915, 0.043998 ,-0.0000, -1.0000, -0.0000, 0.241443, 0.403660,
		0.500030, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.223193, 0.150389,
		0.000030, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.303693, 0.223920,
		0.000030, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.226678, 0.227404,
		0.500030, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.828742, 0.914317,
		0.757849, 0.125669, 0.314420 ,-0.0000, 1.0000, -0.0000, 0.879063, 0.951712,
		0.927971, 0.125669, 0.011686 ,-0.0000, 1.0000, -0.0000, 0.833690, 0.980139,
		-0.895586, 0.125669, 0.665704 ,-0.8769, 0.4586, -0.1439, 0.722821, 0.429033,
		-0.999970, -0.125915, 0.500000 ,-0.8769, 0.4586, -0.1439, 0.414461, 0.449493,
		-1.100910, -0.125915, 1.115088 ,-0.8769, 0.4586, -0.1439, 0.414694, 0.353584,
		0.000030, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.226678, 0.227404,
		-0.499970, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.307177, 0.300935,
		-0.499970, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.230162, 0.304419,
		0.000030, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.825065, 0.837311,
		0.500030, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.905748, 0.910640,
		0.500030, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.828742, 0.914317,
		-0.775212, 0.125669, -0.058707 ,-0.0000, 1.0000, -0.0000, 0.810322, 0.718347,
		-0.499970, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.898394, 0.756628,
		-0.499970, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.821388, 0.760305,
		1.000030, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.219709, 0.073374,
		0.500030, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.300208, 0.146905,
		0.500030, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.223193, 0.150389,
		1.000030, -0.125915, 0.000000 ,0.7204, 0.6935, -0.0000, 0.111447, 0.730411,
		0.757849, 0.125669, 0.314420 ,0.7204, 0.6935, -0.0000, 0.165565, 0.420758,
		1.000030, -0.125915, 0.500000 ,0.7204, 0.6935, -0.0000, 0.187695, 0.731852,
		-0.848521, 0.125669, -0.487467 ,-0.4492, 0.8474, -0.2832, 0.719524, 0.606671,
		-0.617593, 0.125669, -0.853728 ,-0.4492, 0.8474, -0.2832, 0.722821, 0.666390,
		-0.999970, -0.125915, -1.000000 ,-0.4492, 0.8474, -0.2832, 0.408940, 0.679622,
		0.500030, -0.125915, -1.000000 ,-0.0270, 0.6239, -0.7810, 0.569531, 0.686003,
		0.403579, 0.125669, -0.795677 ,-0.0270, 0.6239, -0.7810, 0.563832, 0.995959,
		0.845211, 0.125669, -0.810945 ,-0.0270, 0.6239, -0.7810, 0.502292, 0.990002,
		1.000030, -0.125915, 0.500000 ,0.7493, 0.6563, -0.0882, 0.187695, 0.731852,
		0.757849, 0.125669, 0.314420 ,0.7493, 0.6563, -0.0882, 0.165565, 0.420758,
		0.813274, 0.125669, 0.785465 ,0.7493, 0.6563, -0.0882, 0.236136, 0.423062,
		-0.591142, 0.125669, 0.852532 ,-0.2387, 0.8897, 0.3890, 0.409895, 0.076688,
		-0.895586, 0.125669, 0.665704 ,-0.2387, 0.8897, 0.3890, 0.406107, 0.028189,
		-1.100910, -0.125915, 1.115088 ,-0.2387, 0.8897, 0.3890, 0.721392, 0.000000,
		1.000030, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.296724, 0.069890,
		1.190113, -0.125915, 1.218740 ,-0.0000, -1.0000, -0.0000, 0.406107, 0.035602,
		0.500030, -0.125915, 1.000000 ,-0.0000, -1.0000, -0.0000, 0.377223, 0.143420,
		-0.499970, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.898394, 0.756628,
		-0.999970, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.894716, 0.679622,
		-0.895586, 0.125669, 0.665704 ,-0.0000, 1.0000, -0.0000, 0.921004, 0.694480,
		0.500030, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.905748, 0.910640,
		0.000030, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.902071, 0.833634,
		0.152163, 0.125669, 0.813325 ,-0.0000, 1.0000, -0.0000, 0.951446, 0.854760,
		0.000030, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.303693, 0.223920,
		0.021155, -0.125915, 1.089492 ,-0.0000, -1.0000, -0.0000, 0.394345, 0.216558,
		-0.478845, -0.125915, 1.089492 ,-0.0000, -1.0000, -0.0000, 0.397829, 0.293573,
		0.500030, -0.125915, 1.000000 ,0.1113, 0.6868, 0.7183, 0.717287, 0.245442,
		0.549283, 0.125669, 0.751806 ,0.1113, 0.6868, 0.7183, 0.406560, 0.251364,
		0.152163, 0.125669, 0.813325 ,0.1113, 0.6868, 0.7183, 0.408301, 0.190769,
		-0.499970, -0.125915, -1.000000 ,-0.3859, 0.8429, -0.3749, 0.371933, 0.425464,
		-0.255395, 0.125669, -0.686101 ,-0.3859, 0.8429, -0.3749, 0.371933, 0.739882,
		-0.061333, 0.125669, -0.885809 ,-0.3859, 0.8429, -0.3749, 0.338203, 0.738635,
		0.000030, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.149663, 0.230888,
		0.000030, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.226678, 0.227404,
		-0.499970, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.230162, 0.304419,
		0.000030, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.748059, 0.840989,
		0.000030, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.825065, 0.837311,
		0.500030, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.828742, 0.914317,
		-0.499970, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.744382, 0.763983,
		-0.848521, 0.125669, -0.487467 ,-0.0000, 1.0000, -0.0000, 0.743748, 0.710210,
		-0.775212, 0.125669, -0.058707 ,-0.0000, 1.0000, -0.0000, 0.810322, 0.718347,
		0.500030, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.146178, 0.153873,
		1.506038, -0.125915, -0.655272 ,-0.0000, -1.0000, -0.0000, 0.115251, 0.000000,
		1.000030, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.219709, 0.073374,
		0.878223, 0.125669, -0.314602 ,0.9494, 0.2787, -0.1448, 0.066901, 0.421003,
		0.927971, 0.125669, 0.011686 ,0.9494, 0.2787, -0.1448, 0.115526, 0.422793,
		1.000030, -0.125915, 0.000000 ,0.9494, 0.2787, -0.1448, 0.111447, 0.730411,
		-0.999970, 0.125669, 0.500000 ,-0.5893, 0.7723, -0.2371, 0.720111, 0.452860,
		-0.775212, 0.125669, -0.058707 ,-0.5893, 0.7723, -0.2371, 0.722576, 0.542011,
		-1.146257, -0.125915, 0.043998 ,-0.5893, 0.7723, -0.2371, 0.409840, 0.517218,
		0.500030, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.146178, 0.153873,
		0.500030, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.223193, 0.150389,
		0.000030, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.226678, 0.227404,
		0.878223, 0.125669, -0.314602 ,-0.0000, 1.0000, -0.0000, 0.783072, 0.974877,
		0.500030, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.751736, 0.917995,
		0.500030, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.828742, 0.914317,
		0.813274, 0.125669, 0.785465 ,-0.0893, 0.7083, 0.7003, 0.407036, 0.292297,
		0.549283, 0.125669, 0.751806 ,-0.0893, 0.7083, 0.7003, 0.406560, 0.251364,
		0.500030, -0.125915, 1.000000 ,-0.0893, 0.7083, 0.7003, 0.717287, 0.245442,
		0.500030, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.905748, 0.910640,
		0.549283, 0.125669, 0.751806 ,-0.0000, 1.0000, -0.0000, 0.944892, 0.916374,
		0.813274, 0.125669, 0.785465 ,-0.0000, 1.0000, -0.0000, 0.952017, 0.956784,
		0.000030, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.303693, 0.223920,
		0.500030, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.300208, 0.146905,
		0.500030, -0.125915, 1.000000 ,-0.0000, -1.0000, -0.0000, 0.377223, 0.143420,
		-0.061333, 0.125669, -0.885809 ,0.1436, 0.6565, -0.7406, 0.635120, 1.000000,
		0.403579, 0.125669, -0.795677 ,0.1436, 0.6565, -0.7406, 0.563832, 0.995959,
		0.500030, -0.125915, -1.000000 ,0.1436, 0.6565, -0.7406, 0.569531, 0.686003,
		-0.499970, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.744382, 0.763983,
		-0.499970, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.821388, 0.760305,
		0.000030, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.825065, 0.837311,
		-0.499970, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.153147, 0.307903,
		-0.499970, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.230162, 0.304419,
		-1.146257, -0.125915, 0.043998 ,-0.0000, -1.0000, -0.0000, 0.241443, 0.403660,
		-0.617593, 0.125669, -0.853728 ,0.3402, 0.5864, -0.7351, 0.731467, 0.010804,
		-0.255395, 0.125669, -0.686101 ,0.3402, 0.5864, -0.7351, 0.792023, 0.000000,
		-0.499970, -0.125915, -1.000000 ,0.3402, 0.5864, -0.7351, 0.792023, 0.313820,
		-0.999970, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.310661, 0.377949,
		-0.499970, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.307177, 0.300935,
		-0.478845, -0.125915, 1.089492 ,-0.0000, -1.0000, -0.0000, 0.397829, 0.293573,
		0.000030, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.902071, 0.833634,
		-0.499970, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.898394, 0.756628,
		-0.591142, 0.125669, 0.852532 ,-0.0000, 1.0000, -0.0000, 0.952017, 0.739994,
		0.021155, -0.125915, 1.089492 ,0.0360, 0.7302, 0.6823, 0.719721, 0.172506,
		0.152163, 0.125669, 0.813325 ,0.0360, 0.7302, 0.6823, 0.408301, 0.190769,
		-0.591142, 0.125669, 0.852532 ,0.0360, 0.7302, 0.6823, 0.409895, 0.076688,
		-0.999970, -0.125915, -1.000000 ,-0.0000, -1.0000, -0.0000, 0.079616, 0.388402,
		-0.499970, -0.125915, -1.000000 ,-0.0000, -1.0000, -0.0000, 0.076132, 0.311388,
		-0.499970, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.153147, 0.307903,
		-0.255395, 0.125669, -0.686101 ,-0.0000, 1.0000, -0.0000, 0.717519, 0.803019,
		-0.499970, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.744382, 0.763983,
		0.000030, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.748059, 0.840989,
		0.845211, 0.125669, -0.810945 ,-0.0000, 1.0000, -0.0000, 0.706386, 0.973444,
		0.403579, 0.125669, -0.795677 ,-0.0000, 1.0000, -0.0000, 0.705489, 0.905315,
		0.500030, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.751736, 0.917995,
		0.500030, -0.125915, -1.000000 ,-0.0000, -1.0000, -0.0000, 0.069163, 0.157358,
		0.500030, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.146178, 0.153873,
		0.000030, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.149663, 0.230888,
		0.845211, 0.125669, -0.810945 ,0.3606, 0.9324, -0.0240, 0.502292, 0.990002,
		0.878223, 0.125669, -0.314602 ,0.3606, 0.9324, -0.0240, 0.467227, 1.000000,
		1.506038, -0.125915, -0.655272 ,0.3606, 0.9324, -0.0240, 0.406107, 0.680401,
		1.000030, -0.125915, -1.000000 ,-0.0000, -1.0000, -0.0000, 0.065679, 0.080343,
		1.506038, -0.125915, -0.655272 ,-0.0000, -1.0000, -0.0000, 0.115251, 0.000000,
		0.500030, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.146178, 0.153873,
		-0.617593, 0.125669, -0.853728 ,-0.0000, 1.0000, -0.0000, 0.689038, 0.748469,
		-0.848521, 0.125669, -0.487467 ,-0.0000, 1.0000, -0.0000, 0.743748, 0.710210,
		-0.499970, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.744382, 0.763983,
		0.403579, 0.125669, -0.795677 ,-0.0000, 1.0000, -0.0000, 0.705489, 0.905315,
		-0.061333, 0.125669, -0.885809 ,-0.0000, 1.0000, -0.0000, 0.688189, 0.834376,
		0.000030, 0.125669, -0.500000 ,-0.0000, 1.0000, -0.0000, 0.748059, 0.840989,
		-0.499970, -0.125915, -1.000000 ,-0.0000, -1.0000, -0.0000, 0.076132, 0.311388,
		-0.131998, -0.125915, -1.477620 ,-0.0000, -1.0000, -0.0000, 0.000000, 0.258037,
		0.000030, -0.125915, -0.500000 ,-0.0000, -1.0000, -0.0000, 0.149663, 0.230888,
		-1.146257, -0.125915, 0.043998 ,-0.5411, 0.8358, 0.0925, 0.409840, 0.517218,
		-0.775212, 0.125669, -0.058707 ,-0.5411, 0.8358, 0.0925, 0.722576, 0.542011,
		-0.848521, 0.125669, -0.487467 ,-0.5411, 0.8358, 0.0925, 0.719524, 0.606671,
		-0.499970, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.821388, 0.760305,
		-0.499970, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.898394, 0.756628,
		0.000030, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.902071, 0.833634,
		-0.499970, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.230162, 0.304419,
		-0.499970, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.307177, 0.300935,
		-0.999970, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.310661, 0.377949,
		0.500030, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.223193, 0.150389,
		0.500030, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.300208, 0.146905,
		0.000030, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.303693, 0.223920,
		0.500030, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.828742, 0.914317,
		0.500030, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.905748, 0.910640,
		0.757849, 0.125669, 0.314420 ,-0.0000, 1.0000, -0.0000, 0.879063, 0.951712,
		-0.895586, 0.125669, 0.665704 ,-0.8461, -0.0000, 0.5330, 0.722821, 0.429033,
		-0.999970, 0.125669, 0.500000 ,-0.8461, -0.0000, 0.5330, 0.720111, 0.452860,
		-0.999970, -0.125915, 0.500000 ,-0.8461, -0.0000, 0.5330, 0.414461, 0.449493,
		0.000030, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.226678, 0.227404,
		0.000030, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.303693, 0.223920,
		-0.499970, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.307177, 0.300935,
		0.000030, 0.125669, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.825065, 0.837311,
		0.000030, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.902071, 0.833634,
		0.500030, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.905748, 0.910640,
		-0.775212, 0.125669, -0.058707 ,-0.0000, 1.0000, -0.0000, 0.810322, 0.718347,
		-0.999970, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.894716, 0.679622,
		-0.499970, 0.125669, 0.500000 ,-0.0000, 1.0000, -0.0000, 0.898394, 0.756628,
		1.000030, -0.125915, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.219709, 0.073374,
		1.000030, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.296724, 0.069890,
		0.500030, -0.125915, 0.500000 ,-0.0000, -1.0000, -0.0000, 0.300208, 0.146905,
		1.000030, -0.125915, 0.000000 ,0.8502, 0.2213, 0.4778, 0.111447, 0.730411,
		0.927971, 0.125669, 0.011686 ,0.8502, 0.2213, 0.4778, 0.115526, 0.422793,
		0.757849, 0.125669, 0.314420 ,0.8502, 0.2213, 0.4778, 0.165565, 0.420758
	};

	std::vector<float> LudoDrvoVert =
	{
		0.000030, 1.354215, -0.152404 ,0.0980, -0.0000, -0.9952, 1.000000, 1.000000,
		0.029762, -0.015773, -0.149476 ,0.0980, -0.0000, -0.9952, 0.968750, 0.500000,
		0.000030, -0.015773, -0.152404 ,0.0980, -0.0000, -0.9952, 1.000000, 0.500000,
		0.029762, 1.354215, -0.149476 ,0.2903, -0.0000, -0.9569, 0.968750, 1.000000,
		0.058352, -0.015773, -0.140803 ,0.2903, -0.0000, -0.9569, 0.937500, 0.500000,
		0.029762, -0.015773, -0.149476 ,0.2903, -0.0000, -0.9569, 0.968750, 0.500000,
		0.058352, 1.354215, -0.140803 ,0.4714, -0.0000, -0.8819, 0.937500, 1.000000,
		0.084701, -0.015773, -0.126720 ,0.4714, -0.0000, -0.8819, 0.906250, 0.500000,
		0.058352, -0.015773, -0.140803 ,0.4714, -0.0000, -0.8819, 0.937500, 0.500000,
		0.084701, 1.354215, -0.126720 ,0.6344, -0.0000, -0.7730, 0.906250, 1.000000,
		0.107796, -0.015773, -0.107766 ,0.6344, -0.0000, -0.7730, 0.875000, 0.500000,
		0.084701, -0.015773, -0.126720 ,0.6344, -0.0000, -0.7730, 0.906250, 0.500000,
		0.107796, 1.354215, -0.107766 ,0.7730, -0.0000, -0.6344, 0.875000, 1.000000,
		0.126749, -0.015773, -0.084671 ,0.7730, -0.0000, -0.6344, 0.843750, 0.500000,
		0.107796, -0.015773, -0.107766 ,0.7730, -0.0000, -0.6344, 0.875000, 0.500000,
		0.126749, 1.354215, -0.084671 ,0.8819, -0.0000, -0.4714, 0.843750, 1.000000,
		0.140833, -0.015773, -0.058323 ,0.8819, -0.0000, -0.4714, 0.812500, 0.500000,
		0.126749, -0.015773, -0.084671 ,0.8819, -0.0000, -0.4714, 0.843750, 0.500000,
		0.140833, 1.354215, -0.058323 ,0.9569, -0.0000, -0.2903, 0.812500, 1.000000,
		0.149506, -0.015773, -0.029733 ,0.9569, -0.0000, -0.2903, 0.781250, 0.500000,
		0.140833, -0.015773, -0.058323 ,0.9569, -0.0000, -0.2903, 0.812500, 0.500000,
		0.149506, 1.354215, -0.029733 ,0.9952, -0.0000, -0.0980, 0.781250, 1.000000,
		0.152434, -0.015773, 0.000000 ,0.9952, -0.0000, -0.0980, 0.750000, 0.500000,
		0.149506, -0.015773, -0.029733 ,0.9952, -0.0000, -0.0980, 0.781250, 0.500000,
		0.152434, 1.354215, 0.000000 ,0.9952, -0.0000, 0.0980, 0.750000, 1.000000,
		0.149506, -0.015773, 0.029733 ,0.9952, -0.0000, 0.0980, 0.718750, 0.500000,
		0.152434, -0.015773, 0.000000 ,0.9952, -0.0000, 0.0980, 0.750000, 0.500000,
		0.149506, 1.354215, 0.029733 ,0.9569, -0.0000, 0.2903, 0.718750, 1.000000,
		0.140833, -0.015773, 0.058323 ,0.9569, -0.0000, 0.2903, 0.687500, 0.500000,
		0.149506, -0.015773, 0.029733 ,0.9569, -0.0000, 0.2903, 0.718750, 0.500000,
		0.140833, 1.354215, 0.058323 ,0.8819, -0.0000, 0.4714, 0.687500, 1.000000,
		0.126749, -0.015773, 0.084671 ,0.8819, -0.0000, 0.4714, 0.656250, 0.500000,
		0.140833, -0.015773, 0.058323 ,0.8819, -0.0000, 0.4714, 0.687500, 0.500000,
		0.126749, 1.354215, 0.084671 ,0.7730, -0.0000, 0.6344, 0.656250, 1.000000,
		0.107796, -0.015773, 0.107766 ,0.7730, -0.0000, 0.6344, 0.625000, 0.500000,
		0.126749, -0.015773, 0.084671 ,0.7730, -0.0000, 0.6344, 0.656250, 0.500000,
		0.107796, 1.354215, 0.107766 ,0.6344, -0.0000, 0.7730, 0.625000, 1.000000,
		0.084701, -0.015773, 0.126720 ,0.6344, -0.0000, 0.7730, 0.593750, 0.500000,
		0.107796, -0.015773, 0.107766 ,0.6344, -0.0000, 0.7730, 0.625000, 0.500000,
		0.084701, 1.354215, 0.126720 ,0.4714, -0.0000, 0.8819, 0.593750, 1.000000,
		0.058352, -0.015773, 0.140803 ,0.4714, -0.0000, 0.8819, 0.562500, 0.500000,
		0.084701, -0.015773, 0.126720 ,0.4714, -0.0000, 0.8819, 0.593750, 0.500000,
		0.058352, 1.354215, 0.140803 ,0.2903, -0.0000, 0.9569, 0.562500, 1.000000,
		0.029762, -0.015773, 0.149476 ,0.2903, -0.0000, 0.9569, 0.531250, 0.500000,
		0.058352, -0.015773, 0.140803 ,0.2903, -0.0000, 0.9569, 0.562500, 0.500000,
		0.029762, 1.354215, 0.149476 ,0.0980, -0.0000, 0.9952, 0.531250, 1.000000,
		0.000030, -0.015773, 0.152404 ,0.0980, -0.0000, 0.9952, 0.500000, 0.500000,
		0.029762, -0.015773, 0.149476 ,0.0980, -0.0000, 0.9952, 0.531250, 0.500000,
		0.000030, 1.354215, 0.152404 ,-0.0980, -0.0000, 0.9952, 0.500000, 1.000000,
		-0.029703, -0.015773, 0.149476 ,-0.0980, -0.0000, 0.9952, 0.468750, 0.500000,
		0.000030, -0.015773, 0.152404 ,-0.0980, -0.0000, 0.9952, 0.500000, 0.500000,
		-0.029703, 1.354215, 0.149476 ,-0.2903, -0.0000, 0.9569, 0.468750, 1.000000,
		-0.058293, -0.015773, 0.140803 ,-0.2903, -0.0000, 0.9569, 0.437500, 0.500000,
		-0.029703, -0.015773, 0.149476 ,-0.2903, -0.0000, 0.9569, 0.468750, 0.500000,
		-0.058293, 1.354215, 0.140803 ,-0.4714, -0.0000, 0.8819, 0.437500, 1.000000,
		-0.084642, -0.015773, 0.126720 ,-0.4714, -0.0000, 0.8819, 0.406250, 0.500000,
		-0.058293, -0.015773, 0.140803 ,-0.4714, -0.0000, 0.8819, 0.437500, 0.500000,
		-0.084642, 1.354215, 0.126720 ,-0.6344, -0.0000, 0.7730, 0.406250, 1.000000,
		-0.107737, -0.015773, 0.107766 ,-0.6344, -0.0000, 0.7730, 0.375000, 0.500000,
		-0.084642, -0.015773, 0.126720 ,-0.6344, -0.0000, 0.7730, 0.406250, 0.500000,
		-0.107737, 1.354215, 0.107766 ,-0.7730, -0.0000, 0.6344, 0.375000, 1.000000,
		-0.126690, -0.015773, 0.084671 ,-0.7730, -0.0000, 0.6344, 0.343750, 0.500000,
		-0.107737, -0.015773, 0.107766 ,-0.7730, -0.0000, 0.6344, 0.375000, 0.500000,
		-0.126690, 1.354215, 0.084671 ,-0.8819, -0.0000, 0.4714, 0.343750, 1.000000,
		-0.140774, -0.015773, 0.058323 ,-0.8819, -0.0000, 0.4714, 0.312500, 0.500000,
		-0.126690, -0.015773, 0.084671 ,-0.8819, -0.0000, 0.4714, 0.343750, 0.500000,
		-0.140774, 1.354215, 0.058323 ,-0.9569, -0.0000, 0.2903, 0.312500, 1.000000,
		-0.149446, -0.015773, 0.029733 ,-0.9569, -0.0000, 0.2903, 0.281250, 0.500000,
		-0.140774, -0.015773, 0.058323 ,-0.9569, -0.0000, 0.2903, 0.312500, 0.500000,
		-0.149446, 1.354215, 0.029733 ,-0.9952, -0.0000, 0.0980, 0.281250, 1.000000,
		-0.152375, -0.015773, 0.000000 ,-0.9952, -0.0000, 0.0980, 0.250000, 0.500000,
		-0.149446, -0.015773, 0.029733 ,-0.9952, -0.0000, 0.0980, 0.281250, 0.500000,
		-0.152375, 1.354215, 0.000000 ,-0.9952, -0.0000, -0.0980, 0.250000, 1.000000,
		-0.149446, -0.015773, -0.029733 ,-0.9952, -0.0000, -0.0980, 0.218750, 0.500000,
		-0.152375, -0.015773, 0.000000 ,-0.9952, -0.0000, -0.0980, 0.250000, 0.500000,
		-0.149446, 1.354215, -0.029733 ,-0.9569, -0.0000, -0.2903, 0.218750, 1.000000,
		-0.140774, -0.015773, -0.058323 ,-0.9569, -0.0000, -0.2903, 0.187500, 0.500000,
		-0.149446, -0.015773, -0.029733 ,-0.9569, -0.0000, -0.2903, 0.218750, 0.500000,
		-0.140774, 1.354215, -0.058323 ,-0.8819, -0.0000, -0.4714, 0.187500, 1.000000,
		-0.126690, -0.015773, -0.084671 ,-0.8819, -0.0000, -0.4714, 0.156250, 0.500000,
		-0.140774, -0.015773, -0.058323 ,-0.8819, -0.0000, -0.4714, 0.187500, 0.500000,
		-0.126690, 1.354215, -0.084671 ,-0.7730, -0.0000, -0.6344, 0.156250, 1.000000,
		-0.107737, -0.015773, -0.107766 ,-0.7730, -0.0000, -0.6344, 0.125000, 0.500000,
		-0.126690, -0.015773, -0.084671 ,-0.7730, -0.0000, -0.6344, 0.156250, 0.500000,
		-0.107737, 1.354215, -0.107766 ,-0.6344, -0.0000, -0.7730, 0.125000, 1.000000,
		-0.084642, -0.015773, -0.126720 ,-0.6344, -0.0000, -0.7730, 0.093750, 0.500000,
		-0.107737, -0.015773, -0.107766 ,-0.6344, -0.0000, -0.7730, 0.125000, 0.500000,
		-0.084642, 1.354215, -0.126720 ,-0.4714, -0.0000, -0.8819, 0.093750, 1.000000,
		-0.058293, -0.015773, -0.140803 ,-0.4714, -0.0000, -0.8819, 0.062500, 0.500000,
		-0.084642, -0.015773, -0.126720 ,-0.4714, -0.0000, -0.8819, 0.093750, 0.500000,
		-0.058293, 1.354215, 0.140803 ,-0.0000, 1.0000, -0.0000, 0.158156, 0.028269,
		0.140833, 1.354215, 0.058323 ,-0.0000, 1.0000, -0.0000, 0.471731, 0.158156,
		0.058352, 1.354215, -0.140803 ,-0.0000, 1.0000, -0.0000, 0.341844, 0.471731,
		-0.058293, 1.354215, -0.140803 ,-0.2903, -0.0000, -0.9569, 0.062500, 1.000000,
		-0.029703, -0.015773, -0.149476 ,-0.2903, -0.0000, -0.9569, 0.031250, 0.500000,
		-0.058293, -0.015773, -0.140803 ,-0.2903, -0.0000, -0.9569, 0.062500, 0.500000,
		-0.029703, 1.354215, -0.149476 ,-0.0980, -0.0000, -0.9952, 0.031250, 1.000000,
		0.000030, -0.015773, -0.152404 ,-0.0980, -0.0000, -0.9952, 0.000000, 0.500000,
		-0.029703, -0.015773, -0.149476 ,-0.0980, -0.0000, -0.9952, 0.031250, 0.500000,
		0.029762, -0.015773, 0.149476 ,-0.0000, -1.0000, -0.0000, 0.796822, 0.014612,
		-0.149446, -0.015773, 0.029733 ,-0.0000, -1.0000, -0.0000, 0.514612, 0.203178,
		-0.029703, -0.015773, -0.149476 ,-0.0000, -1.0000, -0.0000, 0.703178, 0.485388,
		0.000030, 1.354215, -0.152404 ,0.0980, -0.0000, -0.9952, 1.000000, 1.000000,
		0.029762, 1.354215, -0.149476 ,0.0980, -0.0000, -0.9952, 0.968750, 1.000000,
		0.029762, -0.015773, -0.149476 ,0.0980, -0.0000, -0.9952, 0.968750, 0.500000,
		0.029762, 1.354215, -0.149476 ,0.2903, -0.0000, -0.9569, 0.968750, 1.000000,
		0.058352, 1.354215, -0.140803 ,0.2903, -0.0000, -0.9569, 0.937500, 1.000000,
		0.058352, -0.015773, -0.140803 ,0.2903, -0.0000, -0.9569, 0.937500, 0.500000,
		0.058352, 1.354215, -0.140803 ,0.4714, -0.0000, -0.8819, 0.937500, 1.000000,
		0.084701, 1.354215, -0.126720 ,0.4714, -0.0000, -0.8819, 0.906250, 1.000000,
		0.084701, -0.015773, -0.126720 ,0.4714, -0.0000, -0.8819, 0.906250, 0.500000,
		0.084701, 1.354215, -0.126720 ,0.6344, -0.0000, -0.7730, 0.906250, 1.000000,
		0.107796, 1.354215, -0.107766 ,0.6344, -0.0000, -0.7730, 0.875000, 1.000000,
		0.107796, -0.015773, -0.107766 ,0.6344, -0.0000, -0.7730, 0.875000, 0.500000,
		0.107796, 1.354215, -0.107766 ,0.7730, -0.0000, -0.6344, 0.875000, 1.000000,
		0.126749, 1.354215, -0.084671 ,0.7730, -0.0000, -0.6344, 0.843750, 1.000000,
		0.126749, -0.015773, -0.084671 ,0.7730, -0.0000, -0.6344, 0.843750, 0.500000,
		0.126749, 1.354215, -0.084671 ,0.8819, -0.0000, -0.4714, 0.843750, 1.000000,
		0.140833, 1.354215, -0.058323 ,0.8819, -0.0000, -0.4714, 0.812500, 1.000000,
		0.140833, -0.015773, -0.058323 ,0.8819, -0.0000, -0.4714, 0.812500, 0.500000,
		0.140833, 1.354215, -0.058323 ,0.9569, -0.0000, -0.2903, 0.812500, 1.000000,
		0.149506, 1.354215, -0.029733 ,0.9569, -0.0000, -0.2903, 0.781250, 1.000000,
		0.149506, -0.015773, -0.029733 ,0.9569, -0.0000, -0.2903, 0.781250, 0.500000,
		0.149506, 1.354215, -0.029733 ,0.9952, -0.0000, -0.0980, 0.781250, 1.000000,
		0.152434, 1.354215, 0.000000 ,0.9952, -0.0000, -0.0980, 0.750000, 1.000000,
		0.152434, -0.015773, 0.000000 ,0.9952, -0.0000, -0.0980, 0.750000, 0.500000,
		0.152434, 1.354215, 0.000000 ,0.9952, -0.0000, 0.0980, 0.750000, 1.000000,
		0.149506, 1.354215, 0.029733 ,0.9952, -0.0000, 0.0980, 0.718750, 1.000000,
		0.149506, -0.015773, 0.029733 ,0.9952, -0.0000, 0.0980, 0.718750, 0.500000,
		0.149506, 1.354215, 0.029733 ,0.9569, -0.0000, 0.2903, 0.718750, 1.000000,
		0.140833, 1.354215, 0.058323 ,0.9569, -0.0000, 0.2903, 0.687500, 1.000000,
		0.140833, -0.015773, 0.058323 ,0.9569, -0.0000, 0.2903, 0.687500, 0.500000,
		0.140833, 1.354215, 0.058323 ,0.8819, -0.0000, 0.4714, 0.687500, 1.000000,
		0.126749, 1.354215, 0.084671 ,0.8819, -0.0000, 0.4714, 0.656250, 1.000000,
		0.126749, -0.015773, 0.084671 ,0.8819, -0.0000, 0.4714, 0.656250, 0.500000,
		0.126749, 1.354215, 0.084671 ,0.7730, -0.0000, 0.6344, 0.656250, 1.000000,
		0.107796, 1.354215, 0.107766 ,0.7730, -0.0000, 0.6344, 0.625000, 1.000000,
		0.107796, -0.015773, 0.107766 ,0.7730, -0.0000, 0.6344, 0.625000, 0.500000,
		0.107796, 1.354215, 0.107766 ,0.6344, -0.0000, 0.7730, 0.625000, 1.000000,
		0.084701, 1.354215, 0.126720 ,0.6344, -0.0000, 0.7730, 0.593750, 1.000000,
		0.084701, -0.015773, 0.126720 ,0.6344, -0.0000, 0.7730, 0.593750, 0.500000,
		0.084701, 1.354215, 0.126720 ,0.4714, -0.0000, 0.8819, 0.593750, 1.000000,
		0.058352, 1.354215, 0.140803 ,0.4714, -0.0000, 0.8819, 0.562500, 1.000000,
		0.058352, -0.015773, 0.140803 ,0.4714, -0.0000, 0.8819, 0.562500, 0.500000,
		0.058352, 1.354215, 0.140803 ,0.2903, -0.0000, 0.9569, 0.562500, 1.000000,
		0.029762, 1.354215, 0.149476 ,0.2903, -0.0000, 0.9569, 0.531250, 1.000000,
		0.029762, -0.015773, 0.149476 ,0.2903, -0.0000, 0.9569, 0.531250, 0.500000,
		0.029762, 1.354215, 0.149476 ,0.0980, -0.0000, 0.9952, 0.531250, 1.000000,
		0.000030, 1.354215, 0.152404 ,0.0980, -0.0000, 0.9952, 0.500000, 1.000000,
		0.000030, -0.015773, 0.152404 ,0.0980, -0.0000, 0.9952, 0.500000, 0.500000,
		0.000030, 1.354215, 0.152404 ,-0.0980, -0.0000, 0.9952, 0.500000, 1.000000,
		-0.029703, 1.354215, 0.149476 ,-0.0980, -0.0000, 0.9952, 0.468750, 1.000000,
		-0.029703, -0.015773, 0.149476 ,-0.0980, -0.0000, 0.9952, 0.468750, 0.500000,
		-0.029703, 1.354215, 0.149476 ,-0.2903, -0.0000, 0.9569, 0.468750, 1.000000,
		-0.058293, 1.354215, 0.140803 ,-0.2903, -0.0000, 0.9569, 0.437500, 1.000000,
		-0.058293, -0.015773, 0.140803 ,-0.2903, -0.0000, 0.9569, 0.437500, 0.500000,
		-0.058293, 1.354215, 0.140803 ,-0.4714, -0.0000, 0.8819, 0.437500, 1.000000,
		-0.084642, 1.354215, 0.126720 ,-0.4714, -0.0000, 0.8819, 0.406250, 1.000000,
		-0.084642, -0.015773, 0.126720 ,-0.4714, -0.0000, 0.8819, 0.406250, 0.500000,
		-0.084642, 1.354215, 0.126720 ,-0.6344, -0.0000, 0.7730, 0.406250, 1.000000,
		-0.107737, 1.354215, 0.107766 ,-0.6344, -0.0000, 0.7730, 0.375000, 1.000000,
		-0.107737, -0.015773, 0.107766 ,-0.6344, -0.0000, 0.7730, 0.375000, 0.500000,
		-0.107737, 1.354215, 0.107766 ,-0.7730, -0.0000, 0.6344, 0.375000, 1.000000,
		-0.126690, 1.354215, 0.084671 ,-0.7730, -0.0000, 0.6344, 0.343750, 1.000000,
		-0.126690, -0.015773, 0.084671 ,-0.7730, -0.0000, 0.6344, 0.343750, 0.500000,
		-0.126690, 1.354215, 0.084671 ,-0.8819, -0.0000, 0.4714, 0.343750, 1.000000,
		-0.140774, 1.354215, 0.058323 ,-0.8819, -0.0000, 0.4714, 0.312500, 1.000000,
		-0.140774, -0.015773, 0.058323 ,-0.8819, -0.0000, 0.4714, 0.312500, 0.500000,
		-0.140774, 1.354215, 0.058323 ,-0.9569, -0.0000, 0.2903, 0.312500, 1.000000,
		-0.149446, 1.354215, 0.029733 ,-0.9569, -0.0000, 0.2903, 0.281250, 1.000000,
		-0.149446, -0.015773, 0.029733 ,-0.9569, -0.0000, 0.2903, 0.281250, 0.500000,
		-0.149446, 1.354215, 0.029733 ,-0.9952, -0.0000, 0.0980, 0.281250, 1.000000,
		-0.152375, 1.354215, 0.000000 ,-0.9952, -0.0000, 0.0980, 0.250000, 1.000000,
		-0.152375, -0.015773, 0.000000 ,-0.9952, -0.0000, 0.0980, 0.250000, 0.500000,
		-0.152375, 1.354215, 0.000000 ,-0.9952, -0.0000, -0.0980, 0.250000, 1.000000,
		-0.149446, 1.354215, -0.029733 ,-0.9952, -0.0000, -0.0980, 0.218750, 1.000000,
		-0.149446, -0.015773, -0.029733 ,-0.9952, -0.0000, -0.0980, 0.218750, 0.500000,
		-0.149446, 1.354215, -0.029733 ,-0.9569, -0.0000, -0.2903, 0.218750, 1.000000,
		-0.140774, 1.354215, -0.058323 ,-0.9569, -0.0000, -0.2903, 0.187500, 1.000000,
		-0.140774, -0.015773, -0.058323 ,-0.9569, -0.0000, -0.2903, 0.187500, 0.500000,
		-0.140774, 1.354215, -0.058323 ,-0.8819, -0.0000, -0.4714, 0.187500, 1.000000,
		-0.126690, 1.354215, -0.084671 ,-0.8819, -0.0000, -0.4714, 0.156250, 1.000000,
		-0.126690, -0.015773, -0.084671 ,-0.8819, -0.0000, -0.4714, 0.156250, 0.500000,
		-0.126690, 1.354215, -0.084671 ,-0.7730, -0.0000, -0.6344, 0.156250, 1.000000,
		-0.107737, 1.354215, -0.107766 ,-0.7730, -0.0000, -0.6344, 0.125000, 1.000000,
		-0.107737, -0.015773, -0.107766 ,-0.7730, -0.0000, -0.6344, 0.125000, 0.500000,
		-0.107737, 1.354215, -0.107766 ,-0.6344, -0.0000, -0.7730, 0.125000, 1.000000,
		-0.084642, 1.354215, -0.126720 ,-0.6344, -0.0000, -0.7730, 0.093750, 1.000000,
		-0.084642, -0.015773, -0.126720 ,-0.6344, -0.0000, -0.7730, 0.093750, 0.500000,
		-0.084642, 1.354215, -0.126720 ,-0.4714, -0.0000, -0.8819, 0.093750, 1.000000,
		-0.058293, 1.354215, -0.140803 ,-0.4714, -0.0000, -0.8819, 0.062500, 1.000000,
		-0.058293, -0.015773, -0.140803 ,-0.4714, -0.0000, -0.8819, 0.062500, 0.500000,
		0.058352, 1.354215, -0.140803 ,-0.0000, 1.0000, -0.0000, 0.341844, 0.471731,
		0.029762, 1.354215, -0.149476 ,-0.0000, 1.0000, -0.0000, 0.296822, 0.485388,
		0.000030, 1.354215, -0.152404 ,-0.0000, 1.0000, -0.0000, 0.250000, 0.490000,
		0.000030, 1.354215, -0.152404 ,-0.0000, 1.0000, -0.0000, 0.250000, 0.490000,
		-0.029703, 1.354215, -0.149476 ,-0.0000, 1.0000, -0.0000, 0.203178, 0.485388,
		-0.058293, 1.354215, -0.140803 ,-0.0000, 1.0000, -0.0000, 0.158156, 0.471731,
		-0.058293, 1.354215, -0.140803 ,-0.0000, 1.0000, -0.0000, 0.158156, 0.471731,
		-0.084642, 1.354215, -0.126720 ,-0.0000, 1.0000, -0.0000, 0.116663, 0.449553,
		-0.107737, 1.354215, -0.107766 ,-0.0000, 1.0000, -0.0000, 0.080294, 0.419706,
		-0.107737, 1.354215, -0.107766 ,-0.0000, 1.0000, -0.0000, 0.080294, 0.419706,
		-0.126690, 1.354215, -0.084671 ,-0.0000, 1.0000, -0.0000, 0.050447, 0.383337,
		-0.140774, 1.354215, -0.058323 ,-0.0000, 1.0000, -0.0000, 0.028269, 0.341844,
		-0.140774, 1.354215, -0.058323 ,-0.0000, 1.0000, -0.0000, 0.028269, 0.341844,
		-0.149446, 1.354215, -0.029733 ,-0.0000, 1.0000, -0.0000, 0.014612, 0.296822,
		-0.152375, 1.354215, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.010000, 0.250000,
		-0.152375, 1.354215, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.010000, 0.250000,
		-0.149446, 1.354215, 0.029733 ,-0.0000, 1.0000, -0.0000, 0.014612, 0.203178,
		-0.140774, 1.354215, 0.058323 ,-0.0000, 1.0000, -0.0000, 0.028269, 0.158156,
		-0.140774, 1.354215, 0.058323 ,-0.0000, 1.0000, -0.0000, 0.028269, 0.158156,
		-0.126690, 1.354215, 0.084671 ,-0.0000, 1.0000, -0.0000, 0.050447, 0.116663,
		-0.107737, 1.354215, 0.107766 ,-0.0000, 1.0000, -0.0000, 0.080294, 0.080294,
		-0.107737, 1.354215, 0.107766 ,-0.0000, 1.0000, -0.0000, 0.080294, 0.080294,
		-0.084642, 1.354215, 0.126720 ,-0.0000, 1.0000, -0.0000, 0.116663, 0.050447,
		-0.058293, 1.354215, 0.140803 ,-0.0000, 1.0000, -0.0000, 0.158156, 0.028269,
		-0.058293, 1.354215, 0.140803 ,-0.0000, 1.0000, -0.0000, 0.158156, 0.028269,
		-0.029703, 1.354215, 0.149476 ,-0.0000, 1.0000, -0.0000, 0.203178, 0.014612,
		0.000030, 1.354215, 0.152404 ,-0.0000, 1.0000, -0.0000, 0.250000, 0.010000,
		0.000030, 1.354215, 0.152404 ,-0.0000, 1.0000, -0.0000, 0.250000, 0.010000,
		0.029762, 1.354215, 0.149476 ,-0.0000, 1.0000, -0.0000, 0.296822, 0.014612,
		0.058352, 1.354215, 0.140803 ,-0.0000, 1.0000, -0.0000, 0.341844, 0.028269,
		0.058352, 1.354215, 0.140803 ,-0.0000, 1.0000, -0.0000, 0.341844, 0.028269,
		0.084701, 1.354215, 0.126720 ,-0.0000, 1.0000, -0.0000, 0.383337, 0.050447,
		0.107796, 1.354215, 0.107766 ,-0.0000, 1.0000, -0.0000, 0.419706, 0.080294,
		0.107796, 1.354215, 0.107766 ,-0.0000, 1.0000, -0.0000, 0.419706, 0.080294,
		0.126749, 1.354215, 0.084671 ,-0.0000, 1.0000, -0.0000, 0.449553, 0.116663,
		0.140833, 1.354215, 0.058323 ,-0.0000, 1.0000, -0.0000, 0.471731, 0.158156,
		0.140833, 1.354215, 0.058323 ,-0.0000, 1.0000, -0.0000, 0.471731, 0.158156,
		0.149506, 1.354215, 0.029733 ,-0.0000, 1.0000, -0.0000, 0.485388, 0.203178,
		0.152434, 1.354215, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.490000, 0.250000,
		0.152434, 1.354215, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.490000, 0.250000,
		0.149506, 1.354215, -0.029733 ,-0.0000, 1.0000, -0.0000, 0.485388, 0.296822,
		0.140833, 1.354215, -0.058323 ,-0.0000, 1.0000, -0.0000, 0.471731, 0.341844,
		0.140833, 1.354215, -0.058323 ,-0.0000, 1.0000, -0.0000, 0.471731, 0.341844,
		0.126749, 1.354215, -0.084671 ,-0.0000, 1.0000, -0.0000, 0.449553, 0.383337,
		0.107796, 1.354215, -0.107766 ,-0.0000, 1.0000, -0.0000, 0.419706, 0.419706,
		0.107796, 1.354215, -0.107766 ,-0.0000, 1.0000, -0.0000, 0.419706, 0.419706,
		0.084701, 1.354215, -0.126720 ,-0.0000, 1.0000, -0.0000, 0.383337, 0.449553,
		0.058352, 1.354215, -0.140803 ,-0.0000, 1.0000, -0.0000, 0.341844, 0.471731,
		0.058352, 1.354215, -0.140803 ,-0.0000, 1.0000, -0.0000, 0.341844, 0.471731,
		0.000030, 1.354215, -0.152404 ,-0.0000, 1.0000, -0.0000, 0.250000, 0.490000,
		-0.058293, 1.354215, -0.140803 ,-0.0000, 1.0000, -0.0000, 0.158156, 0.471731,
		-0.058293, 1.354215, -0.140803 ,-0.0000, 1.0000, -0.0000, 0.158156, 0.471731,
		-0.107737, 1.354215, -0.107766 ,-0.0000, 1.0000, -0.0000, 0.080294, 0.419706,
		-0.140774, 1.354215, -0.058323 ,-0.0000, 1.0000, -0.0000, 0.028269, 0.341844,
		-0.140774, 1.354215, -0.058323 ,-0.0000, 1.0000, -0.0000, 0.028269, 0.341844,
		-0.152375, 1.354215, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.010000, 0.250000,
		-0.140774, 1.354215, 0.058323 ,-0.0000, 1.0000, -0.0000, 0.028269, 0.158156,
		-0.140774, 1.354215, 0.058323 ,-0.0000, 1.0000, -0.0000, 0.028269, 0.158156,
		-0.107737, 1.354215, 0.107766 ,-0.0000, 1.0000, -0.0000, 0.080294, 0.080294,
		-0.058293, 1.354215, 0.140803 ,-0.0000, 1.0000, -0.0000, 0.158156, 0.028269,
		-0.058293, 1.354215, 0.140803 ,-0.0000, 1.0000, -0.0000, 0.158156, 0.028269,
		0.000030, 1.354215, 0.152404 ,-0.0000, 1.0000, -0.0000, 0.250000, 0.010000,
		0.058352, 1.354215, 0.140803 ,-0.0000, 1.0000, -0.0000, 0.341844, 0.028269,
		0.058352, 1.354215, 0.140803 ,-0.0000, 1.0000, -0.0000, 0.341844, 0.028269,
		0.107796, 1.354215, 0.107766 ,-0.0000, 1.0000, -0.0000, 0.419706, 0.080294,
		0.140833, 1.354215, 0.058323 ,-0.0000, 1.0000, -0.0000, 0.471731, 0.158156,
		0.140833, 1.354215, 0.058323 ,-0.0000, 1.0000, -0.0000, 0.471731, 0.158156,
		0.152434, 1.354215, 0.000000 ,-0.0000, 1.0000, -0.0000, 0.490000, 0.250000,
		0.140833, 1.354215, -0.058323 ,-0.0000, 1.0000, -0.0000, 0.471731, 0.341844,
		0.140833, 1.354215, -0.058323 ,-0.0000, 1.0000, -0.0000, 0.471731, 0.341844,
		0.107796, 1.354215, -0.107766 ,-0.0000, 1.0000, -0.0000, 0.419706, 0.419706,
		0.058352, 1.354215, -0.140803 ,-0.0000, 1.0000, -0.0000, 0.341844, 0.471731,
		0.058352, 1.354215, -0.140803 ,-0.0000, 1.0000, -0.0000, 0.341844, 0.471731,
		-0.058293, 1.354215, -0.140803 ,-0.0000, 1.0000, -0.0000, 0.158156, 0.471731,
		-0.140774, 1.354215, -0.058323 ,-0.0000, 1.0000, -0.0000, 0.028269, 0.341844,
		-0.140774, 1.354215, -0.058323 ,-0.0000, 1.0000, -0.0000, 0.028269, 0.341844,
		-0.140774, 1.354215, 0.058323 ,-0.0000, 1.0000, -0.0000, 0.028269, 0.158156,
		-0.058293, 1.354215, 0.140803 ,-0.0000, 1.0000, -0.0000, 0.158156, 0.028269,
		-0.058293, 1.354215, 0.140803 ,-0.0000, 1.0000, -0.0000, 0.158156, 0.028269,
		0.058352, 1.354215, 0.140803 ,-0.0000, 1.0000, -0.0000, 0.341844, 0.028269,
		0.140833, 1.354215, 0.058323 ,-0.0000, 1.0000, -0.0000, 0.471731, 0.158156,
		0.140833, 1.354215, 0.058323 ,-0.0000, 1.0000, -0.0000, 0.471731, 0.158156,
		0.140833, 1.354215, -0.058323 ,-0.0000, 1.0000, -0.0000, 0.471731, 0.341844,
		0.058352, 1.354215, -0.140803 ,-0.0000, 1.0000, -0.0000, 0.341844, 0.471731,
		0.058352, 1.354215, -0.140803 ,-0.0000, 1.0000, -0.0000, 0.341844, 0.471731,
		-0.140774, 1.354215, -0.058323 ,-0.0000, 1.0000, -0.0000, 0.028269, 0.341844,
		-0.058293, 1.354215, 0.140803 ,-0.0000, 1.0000, -0.0000, 0.158156, 0.028269,
		-0.058293, 1.354215, -0.140803 ,-0.2903, -0.0000, -0.9569, 0.062500, 1.000000,
		-0.029703, 1.354215, -0.149476 ,-0.2903, -0.0000, -0.9569, 0.031250, 1.000000,
		-0.029703, -0.015773, -0.149476 ,-0.2903, -0.0000, -0.9569, 0.031250, 0.500000,
		-0.029703, 1.354215, -0.149476 ,-0.0980, -0.0000, -0.9952, 0.031250, 1.000000,
		0.000030, 1.354215, -0.152404 ,-0.0980, -0.0000, -0.9952, 0.000000, 1.000000,
		0.000030, -0.015773, -0.152404 ,-0.0980, -0.0000, -0.9952, 0.000000, 0.500000,
		-0.029703, -0.015773, -0.149476 ,-0.0000, -1.0000, -0.0000, 0.703178, 0.485388,
		0.000030, -0.015773, -0.152404 ,-0.0000, -1.0000, -0.0000, 0.750000, 0.490000,
		0.029762, -0.015773, -0.149476 ,-0.0000, -1.0000, -0.0000, 0.796822, 0.485388,
		0.029762, -0.015773, -0.149476 ,-0.0000, -1.0000, -0.0000, 0.796822, 0.485388,
		0.058352, -0.015773, -0.140803 ,-0.0000, -1.0000, -0.0000, 0.841844, 0.471731,
		0.084701, -0.015773, -0.126720 ,-0.0000, -1.0000, -0.0000, 0.883337, 0.449553,
		0.084701, -0.015773, -0.126720 ,-0.0000, -1.0000, -0.0000, 0.883337, 0.449553,
		0.107796, -0.015773, -0.107766 ,-0.0000, -1.0000, -0.0000, 0.919706, 0.419706,
		0.126749, -0.015773, -0.084671 ,-0.0000, -1.0000, -0.0000, 0.949553, 0.383337,
		0.126749, -0.015773, -0.084671 ,-0.0000, -1.0000, -0.0000, 0.949553, 0.383337,
		0.140833, -0.015773, -0.058323 ,-0.0000, -1.0000, -0.0000, 0.971731, 0.341844,
		0.149506, -0.015773, -0.029733 ,-0.0000, -1.0000, -0.0000, 0.985388, 0.296822,
		0.149506, -0.015773, -0.029733 ,-0.0000, -1.0000, -0.0000, 0.985388, 0.296822,
		0.152434, -0.015773, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.990000, 0.250000,
		0.149506, -0.015773, 0.029733 ,-0.0000, -1.0000, -0.0000, 0.985388, 0.203178,
		0.149506, -0.015773, 0.029733 ,-0.0000, -1.0000, -0.0000, 0.985388, 0.203178,
		0.140833, -0.015773, 0.058323 ,-0.0000, -1.0000, -0.0000, 0.971731, 0.158156,
		0.126749, -0.015773, 0.084671 ,-0.0000, -1.0000, -0.0000, 0.949553, 0.116663,
		0.126749, -0.015773, 0.084671 ,-0.0000, -1.0000, -0.0000, 0.949553, 0.116663,
		0.107796, -0.015773, 0.107766 ,-0.0000, -1.0000, -0.0000, 0.919706, 0.080294,
		0.084701, -0.015773, 0.126720 ,-0.0000, -1.0000, -0.0000, 0.883337, 0.050447,
		0.084701, -0.015773, 0.126720 ,-0.0000, -1.0000, -0.0000, 0.883337, 0.050447,
		0.058352, -0.015773, 0.140803 ,-0.0000, -1.0000, -0.0000, 0.841844, 0.028269,
		0.029762, -0.015773, 0.149476 ,-0.0000, -1.0000, -0.0000, 0.796822, 0.014612,
		0.029762, -0.015773, 0.149476 ,-0.0000, -1.0000, -0.0000, 0.796822, 0.014612,
		0.000030, -0.015773, 0.152404 ,-0.0000, -1.0000, -0.0000, 0.750000, 0.010000,
		-0.029703, -0.015773, 0.149476 ,-0.0000, -1.0000, -0.0000, 0.703178, 0.014612,
		-0.029703, -0.015773, 0.149476 ,-0.0000, -1.0000, -0.0000, 0.703178, 0.014612,
		-0.058293, -0.015773, 0.140803 ,-0.0000, -1.0000, -0.0000, 0.658156, 0.028269,
		-0.084642, -0.015773, 0.126720 ,-0.0000, -1.0000, -0.0000, 0.616663, 0.050447,
		-0.084642, -0.015773, 0.126720 ,-0.0000, -1.0000, -0.0000, 0.616663, 0.050447,
		-0.107737, -0.015773, 0.107766 ,-0.0000, -1.0000, -0.0000, 0.580294, 0.080294,
		-0.126690, -0.015773, 0.084671 ,-0.0000, -1.0000, -0.0000, 0.550447, 0.116663,
		-0.126690, -0.015773, 0.084671 ,-0.0000, -1.0000, -0.0000, 0.550447, 0.116663,
		-0.140774, -0.015773, 0.058323 ,-0.0000, -1.0000, -0.0000, 0.528269, 0.158156,
		-0.149446, -0.015773, 0.029733 ,-0.0000, -1.0000, -0.0000, 0.514612, 0.203178,
		-0.149446, -0.015773, 0.029733 ,-0.0000, -1.0000, -0.0000, 0.514612, 0.203178,
		-0.152375, -0.015773, 0.000000 ,-0.0000, -1.0000, -0.0000, 0.510000, 0.250000,
		-0.149446, -0.015773, -0.029733 ,-0.0000, -1.0000, -0.0000, 0.514612, 0.296822,
		-0.149446, -0.015773, -0.029733 ,-0.0000, -1.0000, -0.0000, 0.514612, 0.296822,
		-0.140774, -0.015773, -0.058323 ,-0.0000, -1.0000, -0.0000, 0.528269, 0.341844,
		-0.126690, -0.015773, -0.084671 ,-0.0000, -1.0000, -0.0000, 0.550447, 0.383337,
		-0.126690, -0.015773, -0.084671 ,-0.0000, -1.0000, -0.0000, 0.550447, 0.383337,
		-0.107737, -0.015773, -0.107766 ,-0.0000, -1.0000, -0.0000, 0.580294, 0.419706,
		-0.084642, -0.015773, -0.126720 ,-0.0000, -1.0000, -0.0000, 0.616663, 0.449553,
		-0.084642, -0.015773, -0.126720 ,-0.0000, -1.0000, -0.0000, 0.616663, 0.449553,
		-0.058293, -0.015773, -0.140803 ,-0.0000, -1.0000, -0.0000, 0.658156, 0.471731,
		-0.029703, -0.015773, -0.149476 ,-0.0000, -1.0000, -0.0000, 0.703178, 0.485388,
		-0.029703, -0.015773, -0.149476 ,-0.0000, -1.0000, -0.0000, 0.703178, 0.485388,
		0.029762, -0.015773, -0.149476 ,-0.0000, -1.0000, -0.0000, 0.796822, 0.485388,
		0.084701, -0.015773, -0.126720 ,-0.0000, -1.0000, -0.0000, 0.883337, 0.449553,
		0.084701, -0.015773, -0.126720 ,-0.0000, -1.0000, -0.0000, 0.883337, 0.449553,
		0.126749, -0.015773, -0.084671 ,-0.0000, -1.0000, -0.0000, 0.949553, 0.383337,
		0.149506, -0.015773, -0.029733 ,-0.0000, -1.0000, -0.0000, 0.985388, 0.296822,
		0.149506, -0.015773, -0.029733 ,-0.0000, -1.0000, -0.0000, 0.985388, 0.296822,
		0.149506, -0.015773, 0.029733 ,-0.0000, -1.0000, -0.0000, 0.985388, 0.203178,
		0.126749, -0.015773, 0.084671 ,-0.0000, -1.0000, -0.0000, 0.949553, 0.116663,
		0.126749, -0.015773, 0.084671 ,-0.0000, -1.0000, -0.0000, 0.949553, 0.116663,
		0.084701, -0.015773, 0.126720 ,-0.0000, -1.0000, -0.0000, 0.883337, 0.050447,
		0.029762, -0.015773, 0.149476 ,-0.0000, -1.0000, -0.0000, 0.796822, 0.014612,
		0.029762, -0.015773, 0.149476 ,-0.0000, -1.0000, -0.0000, 0.796822, 0.014612,
		-0.029703, -0.015773, 0.149476 ,-0.0000, -1.0000, -0.0000, 0.703178, 0.014612,
		-0.084642, -0.015773, 0.126720 ,-0.0000, -1.0000, -0.0000, 0.616663, 0.050447,
		-0.084642, -0.015773, 0.126720 ,-0.0000, -1.0000, -0.0000, 0.616663, 0.050447,
		-0.126690, -0.015773, 0.084671 ,-0.0000, -1.0000, -0.0000, 0.550447, 0.116663,
		-0.149446, -0.015773, 0.029733 ,-0.0000, -1.0000, -0.0000, 0.514612, 0.203178,
		-0.149446, -0.015773, 0.029733 ,-0.0000, -1.0000, -0.0000, 0.514612, 0.203178,
		-0.149446, -0.015773, -0.029733 ,-0.0000, -1.0000, -0.0000, 0.514612, 0.296822,
		-0.126690, -0.015773, -0.084671 ,-0.0000, -1.0000, -0.0000, 0.550447, 0.383337,
		-0.126690, -0.015773, -0.084671 ,-0.0000, -1.0000, -0.0000, 0.550447, 0.383337,
		-0.084642, -0.015773, -0.126720 ,-0.0000, -1.0000, -0.0000, 0.616663, 0.449553,
		-0.029703, -0.015773, -0.149476 ,-0.0000, -1.0000, -0.0000, 0.703178, 0.485388,
		-0.029703, -0.015773, -0.149476 ,-0.0000, -1.0000, -0.0000, 0.703178, 0.485388,
		0.084701, -0.015773, -0.126720 ,-0.0000, -1.0000, -0.0000, 0.883337, 0.449553,
		0.149506, -0.015773, -0.029733 ,-0.0000, -1.0000, -0.0000, 0.985388, 0.296822,
		0.149506, -0.015773, -0.029733 ,-0.0000, -1.0000, -0.0000, 0.985388, 0.296822,
		0.126749, -0.015773, 0.084671 ,-0.0000, -1.0000, -0.0000, 0.949553, 0.116663,
		0.029762, -0.015773, 0.149476 ,-0.0000, -1.0000, -0.0000, 0.796822, 0.014612,
		0.029762, -0.015773, 0.149476 ,-0.0000, -1.0000, -0.0000, 0.796822, 0.014612,
		-0.084642, -0.015773, 0.126720 ,-0.0000, -1.0000, -0.0000, 0.616663, 0.050447,
		-0.149446, -0.015773, 0.029733 ,-0.0000, -1.0000, -0.0000, 0.514612, 0.203178,
		-0.149446, -0.015773, 0.029733 ,-0.0000, -1.0000, -0.0000, 0.514612, 0.203178,
		-0.126690, -0.015773, -0.084671 ,-0.0000, -1.0000, -0.0000, 0.550447, 0.383337,
		-0.029703, -0.015773, -0.149476 ,-0.0000, -1.0000, -0.0000, 0.703178, 0.485388,
		-0.029703, -0.015773, -0.149476 ,-0.0000, -1.0000, -0.0000, 0.703178, 0.485388,
		0.149506, -0.015773, -0.029733 ,-0.0000, -1.0000, -0.0000, 0.985388, 0.296822,
		0.029762, -0.015773, 0.149476 ,-0.0000, -1.0000, -0.0000, 0.796822, 0.014612
	};


	//Pravljenje Renderable objekta (Generise VAO i VBO pri konstrukciji)
	/*Renderable cube(cubeVertices, sizeof(cubeVertices), cubeIndices, sizeof(cubeIndices));
	Renderable funkyCube(funkyCubeVertices, sizeof(funkyCubeVertices), cubeIndices, sizeof(cubeIndices));
	Renderable cube2(cubeVertices2, sizeof(cubeVertices2), cubeIndices2, sizeof(cubeIndices2));
	Renderable island(islandVert, sizeof(islandVert), cubeIndices2, sizeof(cubeIndices2));
	Renderable leaf(leafVert, sizeof(leafVert), leafInd, sizeof(leafInd));*/

	leaf = new Renderable(LudiListVert);
	cube = new Renderable(CubeVertices);
	waterCube = new Renderable(WaterVert);
	ostrvo = new Renderable(LudoOstrvoVert);
	drvo = new Renderable(LudoDrvoVert);

	glm::mat4 m(1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.0, 0.5, 0.0, 1.0);

	//my vars
	Params params;
	params.camX = 0.0f;
	params.camY = 1.0f;
	params.camZ = -2.0f;
	params.wDown = false;
	params.sDown = false;
	params.aDown = false;
	params.dDown = false;

	glfwSetWindowUserPointer(Window, &params);


	float FrameStartTime = glfwGetTime();
	float FrameEndTime = glfwGetTime();
	float dt = FrameEndTime - FrameStartTime;
	glm::vec3 camPos = params.position;

	ISoundEngine* SoundEngine = createIrrKlangDevice();
	ISoundEngine* SoundEngine2 = createIrrKlangDevice();
	SoundEngine2->play2D("audio/ocean.mp3", true);

	SoundEngine->play2D("audio/amougs.ogg", true);
	SoundEngine->setAllSoundsPaused();


	//Textures
	CubeDiffuseTexture = Texture::LoadImageToTexture("res/container_diffuse.png");
	CubeSpecularTexture = Texture::LoadImageToTexture("res/container_specular.png");
	ListDif = Texture::LoadImageToTexture("res/lisce.png");
	DrvoDifuse = Texture::LoadImageToTexture("res/drvoDiff.png");
	voda = Texture::LoadImageToTexture("res/voda.png");
	pesakDifuse = Texture::LoadImageToTexture("res/ostrvo.png");
	vodaDifuse = Texture::LoadImageToTexture("res/vodaDiff.png");
	vodaSpecular = Texture::LoadImageToTexture("res/vodaSpec.png");
	sunceDifuse = Texture::LoadImageToTexture("res/sunce.png");
	oblakDifuse = Texture::LoadImageToTexture("res/oblak.png");
	mesecDifuse = Texture::LoadImageToTexture("res/mesec.png");

	// Framebuffer for Shadow Map
	unsigned int shadowMapFBO;
	glGenFramebuffers(1, &shadowMapFBO);

	// Texture for Shadow Map FBO
	unsigned int shadowMapWidth = 2046*6, shadowMapHeight = 2046*6;
	unsigned int shadowMap;
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// Prevents darkness outside the frustrum
	float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	// Needed since we don't touch the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Matrices needed for the light's perspective
	glm::mat4 orthgonalProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
	glm::mat4 lightView = glm::lookAt(1.0f * glm::vec3(27.232, 34.2576, -2.09927), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection = orthgonalProjection * lightView;

	// Framebuffer for Shadow Map
	unsigned int shadowMapFBO2;
	glGenFramebuffers(1, &shadowMapFBO2);

	// Texture for Shadow Map FBO
	unsigned int shadowMapWidth2 = 2046 * 6, shadowMapHeight2 = 2046 * 6;
	unsigned int shadowMap2;
	glGenTextures(1, &shadowMap2);
	glBindTexture(GL_TEXTURE_2D, shadowMap2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth2, shadowMapHeight2, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// Prevents darkness outside the frustrum
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap2, 0);
	// Needed since we don't touch the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Framebuffer for Shadow Map
	unsigned int shadowMapFBO3;
	glGenFramebuffers(1, &shadowMapFBO3);

	// Texture for Shadow Map FBO
	unsigned int shadowMapWidth3 = 2046 * 6, shadowMapHeight3 = 2046 * 6;
	unsigned int shadowMap3;
	glGenTextures(1, &shadowMap3);
	glBindTexture(GL_TEXTURE_2D, shadowMap3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth3, shadowMapHeight3, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// Prevents darkness outside the frustrum
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO3);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap3, 0);
	// Needed since we don't touch the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Matrices needed for the light's perspective
	glm::mat4 perspectiveProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
	glm::mat4 lightView2 = glm::lookAt(glm::vec3(-10.0, 11.0, 10.0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection2 = perspectiveProjection * lightView2;

	glm::mat4 lightView3 = glm::lookAt(glm::vec3(-10.0, 11.0, 10.0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection3 = perspectiveProjection * lightView3;

	glUseProgram(shadowMapProgram.GetId());
	shadowMapProgram.SetUniform4m("lightProjection", lightProjection);

	glEnable(GL_BLEND);
	params.dt = dt;
	while (!glfwWindowShouldClose(Window)) {

		//glUseProgram(shadowMapProgram.GetId());
		// Depth testing needed for Shadow Map
		glEnable(GL_DEPTH_TEST);

		//------------------------------------------------------------------------------------
		// Preparations for the Shadow Map
		glUseProgram(shadowMapProgram.GetId());
		shadowMapProgram.SetUniform4m("lightProjection", lightProjection);

		glViewport(0, 0, shadowMapWidth, shadowMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Draw scene for shadow map
		glUseProgram(shadowMapProgram.GetId());
		RenderScene(shadowMapProgram);

		//------------------------------------------------------------------------------------
		// Preparations for the Shadow Map2
		glUseProgram(shadowMapProgram.GetId());
		shadowMapProgram.SetUniform4m("lightProjection", lightProjection2);

		glViewport(0, 0, shadowMapWidth2, shadowMapHeight2);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO2);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Draw scene for shadow map
		glUseProgram(shadowMapProgram.GetId());
		RenderScene(shadowMapProgram, true);
		//------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------
		// Preparations for the Shadow Map2
		glUseProgram(shadowMapProgram.GetId());
		shadowMapProgram.SetUniform4m("lightProjection", lightProjection3);

		glViewport(0, 0, shadowMapWidth3, shadowMapHeight3);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO3);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Draw scene for shadow map
		glUseProgram(shadowMapProgram.GetId());
		RenderScene(shadowMapProgram, true);
		//------------------------------------------------------------------------------------

		// Switch back to the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, WindowWidth, WindowHeight);


		glUseProgram(Basic.GetId());
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		FrameStartTime = glfwGetTime();

		//glfwSetCursorPos(Window, WindowWidth / 2, WindowHeight / 2);
		HandleInput(&params);

		if (params.cull) {
			glEnable(GL_CULL_FACE);
			//glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_CULL_FACE);
			//glDisable(GL_DEPTH_TEST);
		}

		//Matrica projekcije
		glm::mat4 p = glm::perspective(glm::radians(90.0f), (float)WindowWidth / WindowHeight, 0.1f, 100.0f);
		//perspective(FOV ugao, aspect ratio prozora, prednja odsjecna ravan i zadnja odsjecna ravan)
		Basic.SetProjection(p);
		//Matrica pogleda (kamere)
		glm::mat4 v = glm::lookAt(params.position, params.position + params.cameraFront, params.cameraUp);
		//lookAt(pozicija kamere, tacka u koju kamera gleda, kako je kamera rotirana po osi definisanoj sa prethodne dvije tacke)
		Basic.SetView(v);

		Basic.SetUniform3f("uViewPos", params.position);

		//Skybox
		glClearColor(135.0 / 255, 206.0 / 255, 235.0 / 255, 1.0);
		//-------------------------------Skybox
		if (params.dan) {
			Basic.SetUniform3f("uDirLight.Ka", glm::vec3(255.0 / 255 / 2, 238.0 / 255 / 2, 204.0 / 255 / 2));
			Basic.SetUniform3f("uDirLight.Kd", glm::vec3(255.0 / 255 / 2, 238.0 / 255 / 2, 204.0 / 255 / 2));
		}
		else
		{
			Basic.SetUniform3f("uDirLight.Ka", glm::vec3(255.0 / 255 / 10, 238.0 / 255 / 10, 204.0 / 255 / 10));
			Basic.SetUniform3f("uDirLight.Kd", glm::vec3(255.0 / 255 / 10, 238.0 / 255 / 10, 204.0 / 255 / 10));
		}


		// Send the light matrix to the shader
		Basic.SetUniform4m("lightProjection", lightProjection);

		// Bind the Shadow Map
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		Basic.SetUniform1i("shadowMap", 2);

		// Send the light matrix to the shader
		Basic.SetUniform4m("lightProjection2", lightProjection2);

		// Bind the Shadow Map
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, shadowMap2);
		Basic.SetUniform1i("shadowMap2", 3);

		// Send the light matrix to the shader
		Basic.SetUniform4m("lightProjection3", lightProjection3);

		// Bind the Shadow Map
		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_2D, shadowMap3);
		Basic.SetUniform1i("shadowMap3", 4);

		//Zika
		auto distance = glm::length(params.position - glm::vec3(-9.5107, 8.26764, 8.97615));

		if (distance <= 3.0f) {
			SoundEngine->setAllSoundsPaused(false);
		}
		else {
			SoundEngine->setAllSoundsPaused(true);
		}

		jacina = 0.7f + abs(sin(glfwGetTime()));
		kdVatra = glm::vec3(230.0 / 255 / 0.2 * jacina, 92.0 / 255 / 0.2 * jacina, 0.0f);
		Basic.SetUniform3f("uPointLights[0].Kd", kdVatra);

		jacina = 0.7f + abs(sin(glfwGetTime() + 10.0f));
		kdVatra = glm::vec3(230.0 / 255 / 0.2 * jacina, 92.0 / 255 / 0.2 * jacina, 0.0f);
		Basic.SetUniform3f("uPointLights[1].Kd", kdVatra);

		jacina = 0.7f + abs(sin(glfwGetTime() + 30.0f));
		kdVatra = glm::vec3(230.0 / 255 / 0.2 * jacina, 92.0 / 255 / 0.2 * jacina, 0.0f);
		Basic.SetUniform3f("uPointLights[3].Kd", kdVatra);

		/*//Cube
		m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 1.0, -3.0));
		Basic.SetModel(m);
		cube->Render(shadowMap2, NULL);

		//Cube
		m = glm::translate(glm::mat4(1.0f), glm::vec3(2.0, 1.0, -3.0));
		Basic.SetModel(m);
		cube->Render(shadowMap, NULL);*/

		//Nebo
		m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -1.0, 0.0));
		m = glm::rotate(m, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
		m = glm::scale(m, glm::vec3(50.0, 50.0, 50.0));
		Basic.SetModel(m);
		Nebo->Render();

		RenderScene(Basic);

		if (params.clouds) {
			Basic.SetUniform3f("uSpotlights[0].Kd", glm::vec3(0.0f, 0.0f, 0.0f));
			Basic.SetUniform3f("uSpotlights[1].Kd", glm::vec3(0.0f, 0.0f, 0.0f));
			//Oblak
			m = glm::translate(glm::mat4(1.0f), glm::vec3(4.89246, 16.30573, 10.690904));
			m = glm::rotate(m, glm::radians(10.0f), glm::vec3(0.0, 0.0, 1.0));
			m = glm::scale(m, glm::vec3(2, 0.5, 0.6));
			Basic.SetModel(m);
			cube->Render(oblakDifuse, NULL);

			//Basic.SetColor(0.83, 0.91, 0.92);
			m = glm::translate(glm::mat4(1.0f), glm::vec3(3.20269, 16.74744, 2.33708));
			m = glm::rotate(m, glm::radians(-30.0f), glm::vec3(1.0, 0.0, 1.0));
			m = glm::scale(m, glm::vec3(3, 0.7, 0.6));
			Basic.SetModel(m);
			cube->Render(oblakDifuse, NULL);

			//Basic.SetColor(0.83, 0.91, 0.92);
			m = glm::translate(glm::mat4(1.0f), glm::vec3(-6.82594, 16.72362, -10.07543));
			m = glm::rotate(m, glm::radians(15.0f), glm::vec3(1.0, 0.0, 1.0));
			m = glm::scale(m, glm::vec3(5, 1.2, 1.6));
			Basic.SetModel(m);
			cube->Render(oblakDifuse, NULL);

			//Basic.SetColor(0.83, 0.91, 0.92);
			m = glm::translate(glm::mat4(1.0f), glm::vec3(-3.84682, 16.62594, 8.28329));
			m = glm::rotate(m, glm::radians(-8.0f), glm::vec3(1.0, 0.0, 1.0));
			m = glm::scale(m, glm::vec3(6, 0.5, 1.6));
			Basic.SetModel(m);
			cube->Render(oblakDifuse, NULL);
		}
		else {
			Basic.SetUniform3f("uSpotlights[0].Kd", glm::vec3(3.0f, 3.0f, 3.0f));
			Basic.SetUniform3f("uSpotlights[1].Kd", glm::vec3(3.0f, 3.0f, 3.0f));
		}

		//Sunce
		m = glm::translate(glm::mat4(1.0f), glm::vec3(27.232, 34.2576, -2.09927));
		m = glm::rotate(m, glm::radians(30.0f), glm::vec3(0.0, 0.0, 1.0));
		m = glm::rotate(m, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
		m = glm::scale(m, glm::vec3(10.0, 10.0, 10.0));
		Basic.SetModel(m);
		if (params.dan) {
			cube->Render(sunceDifuse, NULL);
		}
		else {
			cube->Render(mesecDifuse, NULL);
		}

		//Okretanje svetala na svetioniku
		glm::vec3 front;
		ugao += 40.0f * params.dt;
		front.x = cos(glm::radians(ugao)) * cos(glm::radians(-25.0f));
		front.y = sin(glm::radians(-25.0f));
		front.z = sin(glm::radians(ugao)) * cos(glm::radians(-25.0f));
		Basic.SetUniform3f("uSpotlights[0].Direction", glm::normalize(front));

		glm::mat4 lightView2 = glm::lookAt(1.0f * glm::vec3(-10.0f, 11.0f, 10.0f), glm::normalize(front), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightProjection2 = perspectiveProjection * lightView2;

		float ugao2 = ugao + 180.0f;
		front.x = cos(glm::radians(ugao2)) * cos(glm::radians(-25.0f));
		front.y = sin(glm::radians(-25.0f));
		front.z = sin(glm::radians(ugao2)) * cos(glm::radians(-25.0f));
		Basic.SetUniform3f("uSpotlights[1].Direction", glm::normalize(front));

		glm::mat4 lightView3 = glm::lookAt(1.0f * glm::vec3(-10.0f, 11.0f, 10.0f), glm::normalize(front), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightProjection3 = perspectiveProjection * lightView3;


		glUseProgram(0);
		glfwSwapBuffers(Window);

		FrameEndTime = glfwGetTime();
		/*dt = FrameEndTime - FrameStartTime;
		if (dt < TargetFPS) {
			int DeltaMS = (int)((TargetFrameTime - dt) * 1e3f);
			std::this_thread::sleep_for(std::chrono::milliseconds(DeltaMS/4));
			FrameEndTime = glfwGetTime();
		}*/
		dt = FrameEndTime - FrameStartTime;
		params.dt = dt;
	}

	SoundEngine->drop();
	SoundEngine2->drop();
	glfwTerminate();
	return 0;
}



