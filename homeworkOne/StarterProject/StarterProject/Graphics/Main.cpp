#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Utils.h"
using namespace std;

#define numVAOs 1
#define numVBOs 2

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
float pyrLocX, pyrLocY, pyrLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// ADDED, THESE ARE FOR CHANGING CAMERA ORIENTATION
glm::vec4 cameraVX, cameraVY, cameraVZ;

// variable allocation for display
GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;

void setupVertices(void) {
	float vertexPositions[108] =
	{ -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
	};
	float pyramidPositions[54] =
	{ -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,    //front
		1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,    //right
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  //back
		-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  //left
		-1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, //LF
		1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f  //RR
	};
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);
}

void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	// ADDED, THESE SET UP THE CAMERA ORIENTATION THAT DIFFER WHEN THE CAMERA MOVES??? IDK
	cameraVX = glm::vec4(1.0, 0.0, 0.0, 0.0);
	cameraVY = glm::vec4(0.0, 1.0, 0.0, 0.0);
	cameraVZ = glm::vec4(0.0, 0.0, 1.0, 0.0);

	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 8.0f;
	cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f;
	pyrLocX = 2.0f; pyrLocY = 2.0f; pyrLocZ = 0.0f;
	setupVertices();
}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	// CHANGED
	// vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	// why do we do the inverse again for the view matrix? OH YEAH BECAUSE THE CAMERA ORIENTATION CHANGES
	vMat = glm::inverse(glm::mat4(cameraVX, cameraVY, cameraVZ, glm::vec4(cameraX, cameraY, cameraZ, 1.0)));

	// draw the cube using buffer #0
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubeLocX, cubeLocY, cubeLocZ));
	mvMat = vMat * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	// draw the pyramid using buffer #1
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(pyrLocX, pyrLocY, pyrLocZ));
	mvMat = vMat * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, 18);
}

// created a function for key_callback with the following structure:
// checks each key
// called a print statement so we can be sure we made it there
// call rotation
// 
// goal: update view matrix with each key press and reset the mv matrix
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == 65) {
		// move left along x axis by -0.02
		cout << "made it to key_callback for A\n";
		cameraX -= .02;
		vMat = glm::inverse(glm::mat4(cameraVX, cameraVY, cameraVZ, glm::vec4(cameraX, cameraY, cameraZ, 1.0)));
		mvMat = vMat * mMat;
	} else if (key == 68){
		// move right along x axis by +0.02
		cout << "made it to key_callback for D\n";
		cameraX += .02;
		vMat = glm::inverse(glm::mat4(cameraVX, cameraVY, cameraVZ, glm::vec4(cameraX, cameraY, cameraZ, 1.0)));
		mvMat = vMat * mMat;
	} else if (key == 87) {
		// move forward along z axis by 0.02
		cout << "made it to key_callback for W\n";
		cameraVZ = cameraVZ + (glm::vec4(0.0, 0.0, 0.02, 0.0));
		vMat = glm::inverse(glm::mat4(cameraVX, cameraVY, cameraVZ, glm::vec4(cameraX, cameraY, cameraZ, 1.0)));
		mvMat = vMat * mMat;
	}
	else if (key == 83) {
		// move backward along z axis by -0.02
		cout << "made it to key_callback for S\n";
		cameraVZ = cameraVZ + (glm::vec4(0.0, 0.0, -0.02, 0.0));
		vMat = glm::inverse(glm::mat4(cameraVX, cameraVY, cameraVZ, glm::vec4(cameraX, cameraY, cameraZ, 1.0)));
		mvMat = vMat * mMat;
	}
	else if (key == 265) {
		cout << "made it to key_callback for UP\n";
		// rotation matrices up on x axis by 10 degrees
		glm::mat4 rmat = glm::rotate(glm::mat4(1.0f), (float)(atan(1) / 18), glm::vec3(cameraVX)); // atan(1) * 4 = pi
		cameraVX = rmat * cameraVX;
		cameraVY = rmat * cameraVY;
		cameraVZ = rmat * cameraVZ;
	}
	else if (key == 264) {
		cout << "made it to key_callback for DOWN\n";
		// rotation matrices down on x axis by 10 degrees
		glm::mat4 smat = glm::rotate(glm::mat4(1.0f), (float) (-atan(1) / 18), glm::vec3(cameraVX)); // atan(1) * 4 = pi
		cameraVX = smat * cameraVX;
		cameraVY = smat * cameraVY;
		cameraVZ = smat * cameraVZ;
	}
	else if (key == 263) {
		cout << "made it to key_callback for LEFT\n";
		// rotation matrices up on y axis by 10 degrees
		glm::mat4 tmat = glm::rotate(glm::mat4(1.0f), (float)(atan(1) / 18), glm::vec3(cameraVY)); // atan(1) * 4 = pi
		cameraVX = tmat * cameraVX;
		cameraVY = tmat * cameraVY;
		cameraVZ = tmat * cameraVZ;

	} else if (key == 262) {
		cout << "made it to key_callback for RIGHT\n";
		// rotation matrices down on y axis by 10 degrees
		glm::mat4 tmat = glm::rotate(glm::mat4(1.0f), (float)(-atan(1) / 18), glm::vec3(cameraVY)); // atan(1) * 4 = pi
		cameraVX = tmat * cameraVX;
		cameraVY = tmat * cameraVY;
		cameraVZ = tmat * cameraVZ;

		vMat = glm::inverse(glm::mat4(cameraVX, cameraVY, cameraVZ, glm::vec4(cameraX, cameraY, cameraZ, 1.0)));
		mvMat = vMat * mMat;
	} else {
		cout << "an invalid character was pressed";
	}
}
void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter 4 - program 3", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init(window);

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
		glfwSetKeyCallback(window, key_callback); // get notified when a key is pressed

		// when a key is pressed, run this
		 if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			cout << "w was pressed\n";
			const int scancode = glfwGetKeyScancode(GLFW_KEY_W);
			int key = 87;
			key_callback(window, key, scancode, GLFW_PRESS, 1);
			glfwSetKeyCallback(window, key_callback);
		 }
		 if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			 cout << "a was pressed\n";
			 const int scancode = glfwGetKeyScancode(GLFW_KEY_A);
			 int key = 65;
			 key_callback(window, key, scancode, GLFW_PRESS, 1);
			 glfwSetKeyCallback(window, key_callback);
		 }
		 if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			 cout << "s was pressed\n";
			 const int scancode = glfwGetKeyScancode(GLFW_KEY_S);
			 int key = 83;
			 key_callback(window, key, scancode, GLFW_PRESS, 1);
			 glfwSetKeyCallback(window, key_callback);
		 }
		 if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			 cout << "d was pressed\n";
			 const int scancode = glfwGetKeyScancode(GLFW_KEY_D);
			 int key = 68;
			 key_callback(window, key, scancode, GLFW_PRESS, 1);
			 glfwSetKeyCallback(window, key_callback);
		 }
		 if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			 cout << "UP was pressed\n";
			 const int scancode = glfwGetKeyScancode(GLFW_KEY_UP);
			 int key = 265;
			 key_callback(window, key, scancode, GLFW_PRESS, 1);
			 glfwSetKeyCallback(window, key_callback);
		 }
		 if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			 cout << "DOWN was pressed\n";
			 const int scancode = glfwGetKeyScancode(GLFW_KEY_DOWN);
			 int key = 264;
			 key_callback(window, key, scancode, GLFW_PRESS, 1);
			 glfwSetKeyCallback(window, key_callback);
		 }
		 if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			 cout << "LEFT was pressed\n";
			 const int scancode = glfwGetKeyScancode(GLFW_KEY_LEFT);
			 int key = 263;
			 key_callback(window, key, scancode, GLFW_PRESS, 1);
			 glfwSetKeyCallback(window, key_callback);
		 }
		 if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			 cout << "RIGHT was pressed\n";
			 const int scancode = glfwGetKeyScancode(GLFW_KEY_RIGHT);
			 int key = 262;
			 key_callback(window, key, scancode, GLFW_PRESS, 1);
			 glfwSetKeyCallback(window, key_callback);
		 }
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}