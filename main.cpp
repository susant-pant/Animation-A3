// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <vector>
#include <cstdlib>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// specify that we want the OpenGL core profile before including GLFW headers
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "camera.h"
#include "spring.h"

#define PI 3.14159265359

using namespace std;
using namespace glm;

//Forward definitions
bool CheckGLErrors(string location);
void QueryGLVersion();
string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);

GLenum drawMode;

vec2 mousePos;
bool leftmousePressed = false;
bool rightmousePressed = false;
int scene = 0;

Camera* activeCamera;

GLFWwindow* window = 0;

mat4 winRatio = mat4(1.f);

// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
	cout << "GLFW ERROR " << error << ":" << endl;
	cout << description << endl;
}

// handles keyboard input events
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	else if (key == GLFW_KEY_W && action == GLFW_PRESS){
		if (scene < 3)	scene++;
	}

	else if (key == GLFW_KEY_S && action == GLFW_PRESS){
		if (scene > 0)	scene--;
	}
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if( (action == GLFW_PRESS) || (action == GLFW_RELEASE) ){
		if(button == GLFW_MOUSE_BUTTON_LEFT)
			leftmousePressed = !leftmousePressed;
		else if(button == GLFW_MOUSE_BUTTON_RIGHT)
			rightmousePressed = !rightmousePressed;
	}
}

void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	vec2 newPos = vec2(xpos/(double)vp[2], -ypos/(double)vp[3])*2.f - vec2(1.f);

	vec2 diff = newPos - mousePos;
	if(leftmousePressed){
		activeCamera->trackballRight(-diff.x);
		activeCamera->trackballUp(-diff.y);
	}
	else if(rightmousePressed){
		float zoomBase = (diff.y > 0) ? 1.f/2.f : 2.f;

		activeCamera->zoom(pow(zoomBase, abs(diff.y)));
	}

	mousePos = newPos;
}

void resizeCallback(GLFWwindow* window, int width, int height)
{
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	glViewport(0, 0, width, height);

	float minDim = float(std::min(width, height));

	winRatio[0][0] = minDim/float(width);
	winRatio[1][1] = minDim/float(height);
}

//==========================================================================
// TUTORIAL STUFF


//vec2 and vec3 are part of the glm math library. 
//Include in your own project by putting the glm directory in your project, 
//and including glm/glm.hpp as I have at the top of the file.
//"using namespace glm;" will allow you to avoid writing everyting as glm::vec2

struct VertexBuffers{
	enum{ VERTICES=0, COUNT};

	GLuint id[COUNT];
};

//Describe the setup of the Vertex Array Object
bool initVAO(GLuint vao, const VertexBuffers& vbo)
{
	glBindVertexArray(vao);		//Set the active Vertex Array

	glEnableVertexAttribArray(0);		//Tell opengl you're using layout attribute 0 (For shader input)
	glBindBuffer( GL_ARRAY_BUFFER, vbo.id[VertexBuffers::VERTICES] );		//Set the active Vertex Buffer
	glVertexAttribPointer(
		0,				//Attribute
		3,				//Size # Components
		GL_FLOAT,	//Type
		GL_TRUE, 	//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
		);

	return !CheckGLErrors("initVAO");		//Check for errors in initialize
}


//Loads buffers with data
bool loadBuffer(const VertexBuffers& vbo, const vector<vec3>& points)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo.id[VertexBuffers::VERTICES]);
	glBufferData(
		GL_ARRAY_BUFFER,				//Which buffer you're loading too
		sizeof(vec3)*points.size(),		//Size of data in array (in bytes)
		&points[0],						//Start of array (&points[0] will give you pointer to start of vector)
		GL_STATIC_DRAW					//GL_DYNAMIC_DRAW if you're changing the data often
										//GL_STATIC_DRAW if you're changing seldomly
		);

	return !CheckGLErrors("loadBuffer");	
}

//Compile and link shaders, storing the program ID in shader array
GLuint initShader(string vertexName, string fragmentName)
{	
	string vertexSource = LoadSource(vertexName);		//Put vertex file text into string
	string fragmentSource = LoadSource(fragmentName);		//Put fragment file text into string

	GLuint vertexID = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragmentID = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	
	return LinkProgram(vertexID, fragmentID);	//Link and store program ID in shader array
}

//Initialization
void initGL()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(0.f, 0.f, 0.f, 0.f);		//Color to clear the screen with (R, G, B, Alpha)
}

bool loadUniforms(GLuint program, mat4 perspective, mat4 modelview)
{
	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "modelviewMatrix"),
						1,
						false,
						&modelview[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(program, "perspectiveMatrix"),
						1,
						false,
						&perspective[0][0]);

	return !CheckGLErrors("loadUniforms");
}

//Draws buffers to screen
void render(GLuint vao, int startElement, int numElements)
{
	glBindVertexArray(vao);		//Use the LINES vertex array
	glDrawArrays(
			drawMode,		//What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
			startElement,
			numElements		//How many indices
			);

	CheckGLErrors("render");
}

void drawConnection(vector<vec3>* vertices, vector<Spring>& springs) {
	vertices->clear();
	for(uint i = 0; i < springs.size(); i++){
		vertices->push_back(springs[i].mass1->pos);
		vertices->push_back(springs[i].mass2->pos);
	}
}

void drawWeights(vector<vec3>* vertices, vector<Mass>& masses) {
	vertices->clear();
	for(uint i =0; i < masses.size(); i++){
		//first triangle
		vertices->push_back(vec3(masses[i].pos) + vec3(0.05f, 0.05f, 0.f));
		vertices->push_back(vec3(masses[i].pos) + vec3(0.05f, -0.05f, 0.f));
		vertices->push_back(vec3(masses[i].pos) + vec3(-0.05f, 0.05f, 0.f));
		//second triangle
		vertices->push_back(vec3(masses[i].pos) + vec3(0.05f, -0.05f, 0.f));
		vertices->push_back(vec3(masses[i].pos) + vec3(-0.05f, -0.05f, 0.f));
		vertices->push_back(vec3(masses[i].pos) + vec3(-0.05f, 0.05f, 0.f));
	}
}

GLFWwindow* createGLFWWindow()
{
	// initialize the GLFW windowing system
	if (!glfwInit()) {
		cout << "ERROR: GLFW failed to initialize, TERMINATING" << endl;
		return NULL;
	}
	glfwSetErrorCallback(ErrorCallback);

	// attempt to create a window with an OpenGL 4.1 core profile context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(1024, 1024, "Animation Assignment 3", 0, 0);
	if (!window) {
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return NULL;
	}

	// set keyboard callback function and make our context current (active)
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetWindowSizeCallback(window, resizeCallback);
	glfwMakeContextCurrent(window);

	return window;
}

void initMassSpring(int currScene, vector<Mass>& masses, vector<Spring>& springs){
	masses.clear();
	springs.clear();

	if (currScene == 0) {
		masses.push_back(Mass(1.f, vec3(0.f, 3.f, -5.f), true));
		masses.push_back(Mass(1.f, vec3(0.f, -0.5f, -5.f), false));

		springs.push_back(Spring(&masses.at(0), &masses.at(1), 11.f, 0.5f));
	}

	if (currScene == 1) {
		masses.push_back(Mass(1.f, vec3(0.f, 2.85f, -5.f), true));
		masses.push_back(Mass(0.1f, vec3(-0.25f, 1.75f, -5.f), false));
		masses.push_back(Mass(0.5f, vec3(0.25f, 1.f, -5.f), false));
		masses.push_back(Mass(0.8f, vec3(1.75f, 0.49f, -5.f), false));

		springs.push_back(Spring(&masses.at(0), &masses.at(1), 20.f, 0.3f));
		springs.push_back(Spring(&masses.at(1), &masses.at(2), 8.f, 0.5f));
		springs.push_back(Spring(&masses.at(2), &masses.at(3), 10.f, 0.2f));
	}

	if (currScene == 2) {
		int cubeDimension = 5;
		float unitLength = 1.f;

		for (int i = 0; i < cubeDimension; i++) {
			for (int j = 0; j < cubeDimension; j++) {
				for (int k = 0; k < cubeDimension; k++) {
					float xPos = float(i - (0.5 * (cubeDimension - 1))) * unitLength;
					float yPos = float(j - (0.5 * (cubeDimension - 1))) * unitLength - 0.8f;
					float zPos =(float(k - (0.5 * (cubeDimension - 1))) * unitLength) - 8.f;
					masses.push_back(Mass(1.f, vec3(xPos, yPos, zPos), false));
		}	}	}

		for (uint i = 0; i < masses.size(); i++) {
			for (uint j = i + 1; j < masses.size(); j++) {
				float dist = masses.at(i).findDistTo(masses.at(j));
				if (dist <= sqrt(3.f * (unitLength * unitLength))) {
					for (int k = 0; k <= cubeDimension; k++) {
						springs.push_back(Spring(&masses.at(i), &masses.at(j), float(cubeDimension) * 8, (4.f*unitLength)/3.f));
					}
				}
		}	}
	}

	if (currScene == 3) {
		int xDimension = 13;
		int yDimension = 6;
		float unitLength = 0.8f;

		for (int x = 0; x < xDimension; x++) {
			for (int y = 0; y < yDimension; y++) {
				float xPos = float(x - (0.5 * (xDimension - 1))) * unitLength;
				float yPos = float(y - (0.5 * (yDimension - 1))) * unitLength + 1.5f;
				if (y == yDimension - 1) {
					masses.push_back(Mass(1.f, vec3(xPos, yPos, -5.f), true, false));
				} else {
					masses.push_back(Mass(0.05f, vec3(xPos, yPos, -5.f), false, false));
				}
			}
		}

		for (int x = 0; x < xDimension; x++) {
			for (int y = 0; y < yDimension - 1; y++) {
				int massIndex = x*yDimension + y;
				springs.push_back(Spring(&masses.at(massIndex), &masses.at(massIndex + 1), 5.f, (10.f*unitLength)/6.f));
			}
		}

		for (int i = 0; i < yDimension; i++) {
			for (int j = 0; j < xDimension - 1; j++) {
				springs.push_back(Spring(&masses.at(i + (j*yDimension)), &masses.at(i + ((j + 1) * yDimension)), 0.05f, unitLength));
			}
		}
	}
}

// ==========================================================================
// PROGRAM ENTRY POINT

int main(int argc, char *argv[]) {
	window = createGLFWWindow();
	if(window == NULL)
		return -1;

	//Initialize glad
	if (!gladLoadGL())
	{
		cout << "GLAD init failed" << endl;
		return -1;
	}

	// query and print out information about our OpenGL environment
	QueryGLVersion();

	initGL();

	//Initialize shader
	GLuint program = initShader("vertex.glsl", "fragment.glsl");

	GLuint vaoSprings;
	VertexBuffers vboSprings;
	//Generate object ids
	glGenVertexArrays(1, &vaoSprings);
	glGenBuffers(VertexBuffers::COUNT, vboSprings.id);
	initVAO(vaoSprings, vboSprings);

	GLuint vaoMasses;
	VertexBuffers vboMasses;
	glGenVertexArrays(1, &vaoMasses);
	glGenBuffers(VertexBuffers::COUNT, vboMasses.id);
	initVAO(vaoMasses, vboMasses);

	//Geometry information
	vector<vec3> points;
	vector<vec3> trings;

	vector<Mass> masses;
	vector<Spring> springs;

	int currScene = scene;
	cout << "Now on Scene: " << scene + 1 << " of 4" << endl;
	initMassSpring(currScene, masses, springs);

	Camera cam = Camera(vec3(0, 0, -1), vec3(0, 0, 1));
	activeCamera = &cam;
	//float fovy, float aspect, float zNear, float zFar
	mat4 perspectiveMatrix = perspective(radians(80.f), 1.f, 1.f, 20.f);

	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//Clear color and depth buffers (Haven't covered yet)
		glUseProgram(program);

		if (currScene != scene) {
			currScene = scene;
			initMassSpring(currScene, masses, springs);
		}

		drawConnection(&points, springs);
		loadBuffer(vboSprings, points);
		drawWeights(&trings, masses);
		loadBuffer(vboMasses, trings);

		loadUniforms(program, winRatio*perspectiveMatrix*cam.getMatrix(), mat4(1.f));

		// call function to draw our scene
		drawMode = GL_LINES;
		render(vaoSprings, 0, points.size());
		drawMode = GL_TRIANGLES;
		render(vaoMasses, 0, trings.size());

		for(uint i = 0; i < masses.size(); i++){
			masses[i].Fspring = vec3(0.f);
		}
		for(uint i = 0; i < springs.size(); i++){
			vec3 springForce = springs[i].findSpringForce();
			if (!springs[i].mass1->fixed)	springs[i].mass1->Fspring += -1.f * springForce;
			if (!springs[i].mass2->fixed)	springs[i].mass2->Fspring += springForce;
		}
		for(uint i = 0; i < masses.size(); i++){
			if (!masses[i].fixed)	masses[i].updatePos();
		}

		// scene is rendered to the back buffer, so swap to front for display
		glfwSwapBuffers(window);
		// sleep until next event before drawing again
		glfwPollEvents();
	}

	// clean up allocated resources before exit
	glDeleteVertexArrays(1, &vaoSprings);
	glDeleteBuffers(VertexBuffers::COUNT, vboSprings.id);
	glDeleteVertexArrays(1, &vaoMasses);
	glDeleteBuffers(VertexBuffers::COUNT, vboMasses.id);
	glDeleteProgram(program);

	glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
	// query opengl version and renderer information
	string version  = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	string glslver  = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

	cout << "OpenGL [ " << version << " ] "
		 << "with GLSL [ " << glslver << " ] "
		 << "on renderer [ " << renderer << " ]" << endl;
}

bool CheckGLErrors(string location)
{
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		cout << "OpenGL ERROR:  ";
		switch (flag) {
		case GL_INVALID_ENUM:
			cout << location << ": " << "GL_INVALID_ENUM" << endl; break;
		case GL_INVALID_VALUE:
			cout << location << ": " << "GL_INVALID_VALUE" << endl; break;
		case GL_INVALID_OPERATION:
			cout << location << ": " << "GL_INVALID_OPERATION" << endl; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << location << ": " << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
		case GL_OUT_OF_MEMORY:
			cout << location << ": " << "GL_OUT_OF_MEMORY" << endl; break;
		default:
			cout << "[unknown error code]" << endl;
		}
		error = true;
	}
	return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
	string source;

	ifstream input(filename.c_str());
	if (input) {
		copy(istreambuf_iterator<char>(input),
			 istreambuf_iterator<char>(),
			 back_inserter(source));
		input.close();
	}
	else {
		cout << "ERROR: Could not load shader source from file " << filename << endl;
	}

	return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
	// allocate shader object name
	GLuint shaderObject = glCreateShader(shaderType);

	// try compiling the source as a shader of the given type
	const GLchar *source_ptr = source.c_str();
	glShaderSource(shaderObject, 1, &source_ptr, 0);
	glCompileShader(shaderObject);

	// retrieve compile status
	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
		cout << "ERROR compiling shader:" << endl;
		cout << source << endl;
		cout << info << endl;
	}

	return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)   glAttachShader(programObject, vertexShader);
	if (fragmentShader) glAttachShader(programObject, fragmentShader);

	// try linking the program with given attachments
	glLinkProgram(programObject);

	// retrieve link status
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
	}

	return programObject;
}


// ==========================================================================
