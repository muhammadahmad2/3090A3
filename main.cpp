// Muhammad Ahmad 100518917
// Harshan Mohanaraj 100520376
// Rohil Arya 100585195
// Yash Pandya

#define GLM_ENABLE_EXPERIMENTAL

#include <GLM/gtc/matrix_access.hpp>
#include <GLM/gtx/rotate_vector.hpp>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/constants.hpp>
#include <SOIL.h>
//#include <GL/glut.h>

// GUI Library
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

// Standard Libraries
#include <iostream> // Used for std::cout
#include <vector>   // Used for std::vector<vec3>
#include <map>      // Used for std::map

// Custom headers
#include "shaders.h"
#include "mesh.h"

using namespace glm;

/*---------------------------- Variables ----------------------------*/
// GLFW window
GLFWwindow* window;
int width = 1280, height = 720;

// Shader programs
GLuint phongProgram, skyboxProgram, emissiveProgram;

// Variables for uniforms
mat4 projectionMatrix, viewMatrix, modelMatrix[11];
vec3 cameraPosition, cameraTarget, lightPosition;

// Solar system variables
float earthDays = 17.62f;
float moonRotation = 0.0f;
float simulationSpeed = 0.01f;
int viewMode = 3;

// Textures
GLuint skyboxTexture;
GLuint diffuseTexture, specularTexture;
GLuint moonTexture, sunTexture;
GLuint mercuryTexture, venusTexture, marsTexture, jupiterTexture, saturnTexture, uranusTexture, neptuneTexture;

bool ast = false;
float astx;
float astz;
int multx;
int multz;
float valx;
float valz;

enum
{
	EARTH = 0,
	SUN = 1,
	MOON = 2,
	MERCURY = 3,
	VENUS = 4,
	MARS = 5,
	JUPITER = 6,
	SATURN = 7,
	URANUS = 8,
	NEPTUNE = 9,
	AST = 10
};


bool astDestroyed = false;
bool moonDestroyed = false;
bool mercuryDestroyed = false;
bool venusDestroyed = false;
bool earthDestroyed = false;
bool marsDestroyed = false;
bool jupiterDestroyed = false;
bool saturnDestroyed = false;
bool uranusDestroyed = false;
bool neptuneDestroyed = false;

void Initialize()
{
	// Make a simple shader for the sphere we're drawing
	{
		GLuint vs = buildShader(GL_VERTEX_SHADER, ASSETS"simpleLights.vert");
		GLuint fs = buildShader(GL_FRAGMENT_SHADER, ASSETS"simpleLights.frag");
		phongProgram = buildProgram(vs, fs, 0);
		phongProgram = linkProgram(phongProgram);
		dumpProgram(phongProgram, "Simple program for phong lighting");
	}

	// Make a simple shader for the skybox
	{
		GLuint vs = buildShader(GL_VERTEX_SHADER, ASSETS"skybox.vert");
		GLuint fs = buildShader(GL_FRAGMENT_SHADER, ASSETS"skybox.frag");
		skyboxProgram = buildProgram(vs, fs, 0);
		skyboxProgram = linkProgram(skyboxProgram);
		dumpProgram(skyboxProgram, "Simple program for the skybox");
	}

	// Make a simple shader for the sun
	{
		GLuint vs = buildShader(GL_VERTEX_SHADER, ASSETS"emissive.vert");
		GLuint fs = buildShader(GL_FRAGMENT_SHADER, ASSETS"emissive.frag");
		emissiveProgram = buildProgram(vs, fs, 0);
		emissiveProgram = linkProgram(emissiveProgram);
		dumpProgram(emissiveProgram, "Simple program for the sun");
	}

	// Load in all 6 faces of the skybox cube
	skyboxTexture = SOIL_load_OGL_cubemap
	(
		ASSETS"textures/star_sky/stars.png", // posx
		ASSETS"textures/star_sky/stars.png", // negx
		ASSETS"textures/star_sky/stars.png", // posy
		ASSETS"textures/star_sky/stars.png", // negy
		ASSETS"textures/star_sky/stars.png", // posz
		ASSETS"textures/star_sky/stars.png", // negz
		SOIL_LOAD_RGB,      // This means we're expecting it to have RGB channels
		SOIL_CREATE_NEW_ID, // This means we want to create a new texture instead of overwriting one 
		SOIL_FLAG_MIPMAPS   // This means we want it to generate mip-maps.
	);

	diffuseTexture = SOIL_load_OGL_texture
	(
		ASSETS"textures/earthDiffuse.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	specularTexture = SOIL_load_OGL_texture
	(
		ASSETS"textures/earthSpecular.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	moonTexture = SOIL_load_OGL_texture
	(
		ASSETS"textures/moonTexture.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	mercuryTexture = SOIL_load_OGL_texture
	(
		ASSETS"textures/mercurymap.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
	venusTexture = SOIL_load_OGL_texture
	(
		ASSETS"textures/venusTexture.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
	marsTexture = SOIL_load_OGL_texture
	(
		ASSETS"textures/marsTexture.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
	jupiterTexture = SOIL_load_OGL_texture
	(
		ASSETS"textures/jupiterTexture.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
	saturnTexture = SOIL_load_OGL_texture
	(
		ASSETS"textures/saturnTexture.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
	uranusTexture = SOIL_load_OGL_texture
	(
		ASSETS"textures/uranusTexture.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
	neptuneTexture = SOIL_load_OGL_texture
	(
		ASSETS"textures/neptuneTexture.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	sunTexture = SOIL_load_OGL_texture
	(
		ASSETS"textures/sunTexture.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	cameraPosition = vec3(0, 0, -5);
	cameraTarget = vec3(0, 0, 0);
}


mat4 updateCam(float deltaTime)
{
	//using namespace glm;

	auto left = vec3(column(viewMatrix, 0));
	auto up = vec3(column(viewMatrix, 1));
	auto forward = vec3(column(viewMatrix, 2));
	cameraPosition = vec3(column(viewMatrix, 3));

	if (glfwGetKey(window, GLFW_KEY_W)) // Move forward
		cameraPosition -= forward * deltaTime * 4.0f;
	if (glfwGetKey(window, GLFW_KEY_S)) // Move backward
		cameraPosition += forward * deltaTime * 4.0f;
	if (glfwGetKey(window, GLFW_KEY_A)) // Move Left
		cameraPosition -= left * deltaTime * 4.0f;
	if (glfwGetKey(window, GLFW_KEY_D)) // Move right
		cameraPosition += left * deltaTime * 4.0f;
	POINT p;
	if (GetCursorPos(&p))
	{
		//cursor position now in p.x and p.y
	}

	float ox = p.x;
	float oy = p.y;

	
	if (ox<((width / 2)-100))
	{
		forward = rotate(forward, deltaTime, vec3(0.0f, 1.0f, 0.0f));
		left = rotate(left, deltaTime, vec3(0.0f, 1.0f, 0.0f));
		up = rotate(up, deltaTime, vec3(0.0f, 1.0f, 0.0f));
	}
	if (ox>((width / 2) + 100))
	{
		forward = rotate(forward, -deltaTime, vec3(0.0f, 1.0f, 0.0f));
		left = rotate(left, -deltaTime, vec3(0.0f, 1.0f, 0.0f));
		up = rotate(up, -deltaTime, vec3(0.0f, 1.0f, 0.0f));
	}
	if (oy<((height / 2) - 100))
	{
		forward = rotate(forward, deltaTime, left);
		up = rotate(up, deltaTime, left);
	}
	if (oy>((height / 2) + 100))
	{
		forward = rotate(forward, -deltaTime, left);
		up = rotate(up, -deltaTime, left);
	}

	viewMatrix = mat4(
		left.x, left.y, left.z, 0.0f,  // Col 1
		up.x, up.y, up.z, 0.0f,  // Col 2
		forward.x, forward.y, forward.z, 0.0f,  // Col 3
		cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f); // Col 4

	/**vNoPos = mat4(
	left.x, left.y, left.z, 0.0f,  // Col 1
	up.x, up.y, up.z, 0.0f,  // Col 2
	forward.x, forward.y, forward.z, 0.0f,  // Col 3
	0.0f, 0.0f, 0.0f, 1.0f); // Col 4
	**/

	return viewMatrix;
}



void Update(float deltaTime)
{
	mat4 identity, translation, scaling, rotation;
	identity = mat4(1.0f);
	const float pi2 = 2.0f * pi<float>();

	// Add to the rotation, in days.
	earthDays += deltaTime * simulationSpeed;

	float planetRotations = earthDays;



	// Compute orbits and rotations
	float earthOrbit = (planetRotations / 365.0f) * pi2;
	float earthRotate = fract(planetRotations) * pi2;
	float moonOrbit = (planetRotations / 27.322f) * pi2;
	float moonRotate = -fract(planetRotations / 27.0f) * pi2;

	float mercuryOrbit = (planetRotations / 87.97f) * pi2;
	float mercuryRotate = fract(planetRotations / 58.6f) * pi2;

	float venusOrbit = (planetRotations / 224.7f) * pi2;
	float venusRotate = fract(planetRotations / 243.0f) * pi2;



	float marsOrbit = (planetRotations / 686.2f) * pi2;
	float marsRotate = fract(planetRotations / 1.03f) * pi2;

	float jupiterOrbit = (planetRotations / 4328.9f) * pi2;
	float jupiterRotate = fract(planetRotations / 0.41f) * pi2;

	float saturnOrbit = (planetRotations / 10752.9f) * pi2;
	float saturnRotate = fract(planetRotations / 0.45f) * pi2;

	float uranusOrbit = (planetRotations / 30663.65f) * pi2;
	float uranusRotate = fract(planetRotations / 0.72f) * pi2;

	float neptuneOrbit = (planetRotations / 60148.35f) * pi2;
	float neptuneRotate = fract(planetRotations / 0.67f) * pi2;

	vec3 earthPosition = vec3(cos(earthOrbit), 0, sin(earthOrbit)) * 30.0f;
	vec3 moonPosition = earthPosition + vec3(cos(moonOrbit), 0, sin(moonOrbit)) * 4.0f;

	vec3 mercuryPosition = vec3(cos(mercuryOrbit), 0, sin(mercuryOrbit)) * 10.0f;
	vec3 venusPosition = vec3(cos(venusOrbit), 0, sin(venusOrbit)) * 20.0f;
	vec3 marsPosition = vec3(cos(marsOrbit), 0, sin(marsOrbit)) * 40.0f;
	vec3 jupiterPosition = vec3(cos(jupiterOrbit), 0, sin(jupiterOrbit)) * 50.0f;
	vec3 saturnPosition = vec3(cos(saturnOrbit), 0, sin(saturnOrbit)) * 60.0f;
	vec3 uranusPosition = vec3(cos(uranusOrbit), 0, sin(uranusOrbit)) * 70.0f;
	vec3 neptunePosition = vec3(cos(neptuneOrbit), 0, sin(neptuneOrbit)) * 80.0f;

	if (!earthDestroyed) {
		translation = translate(identity, earthPosition);
		scaling = scale(identity, vec3(1)); // The earth stays the same size
		rotation = rotate(identity, earthRotate, vec3(0, 1, 0));
		modelMatrix[EARTH] = translation * rotation * scaling;
	}
	
	if (!moonDestroyed) {
		translation = translate(identity, moonPosition);
		scaling = scale(identity, vec3(0.27)); // The moon is 27% the size of earth
		rotation = rotate(identity, moonRotate, vec3(0, 1, 0));
		modelMatrix[MOON] = translation * rotation * scaling;
	}

	scaling = scale(identity, vec3(3)); // Make the sun 10 times larger
	modelMatrix[SUN] = scaling * identity;

	if (!mercuryDestroyed) {
		translation = translate(identity, mercuryPosition);
		scaling = scale(identity, vec3(0.3)); // The earth stays the same size
		rotation = rotate(identity, mercuryRotate, vec3(0, 1, 0));
		modelMatrix[MERCURY] = translation * rotation * scaling;
	}
	if (!venusDestroyed) {
		translation = translate(identity, venusPosition);
		scaling = scale(identity, vec3(1)); // The earth stays the same size
		rotation = rotate(identity, venusRotate, vec3(0, 1, 0));
		modelMatrix[VENUS] = translation * rotation * scaling;
	}

	if (!marsDestroyed) {
		translation = translate(identity, marsPosition);
		scaling = scale(identity, vec3(0.7)); // The earth stays the same size
		rotation = rotate(identity, marsRotate, vec3(0, 1, 0));
		modelMatrix[MARS] = translation * rotation * scaling;
	}

	if (!jupiterDestroyed) {
		translation = translate(identity, jupiterPosition);
		scaling = scale(identity, vec3(5)); // The earth stays the same size
		rotation = rotate(identity, jupiterRotate, vec3(0, 1, 0));
		modelMatrix[JUPITER] = translation * rotation * scaling;
	}

	if (!saturnDestroyed) {
		translation = translate(identity, saturnPosition);
		scaling = scale(identity, vec3(4)); // The earth stays the same size
		rotation = rotate(identity, saturnRotate, vec3(0, 1, 0));
		modelMatrix[SATURN] = translation * rotation * scaling;
	}

	if (!uranusDestroyed) {
		translation = translate(identity, uranusPosition);
		scaling = scale(identity, vec3(2)); // The earth stays the same size
		rotation = rotate(identity, uranusRotate, vec3(0, 1, 0));
		modelMatrix[URANUS] = translation * rotation * scaling;
	}

	if (!neptuneDestroyed) {
		translation = translate(identity, neptunePosition);
		scaling = scale(identity, vec3(3)); // The earth stays the same size
		rotation = rotate(identity, neptuneRotate, vec3(0, 1, 0));
		modelMatrix[NEPTUNE] = translation * rotation * scaling;
	}

	if (glfwGetKey(window, GLFW_KEY_P)) {
		ast = true;
		astx = rand() % 50 + 1;
		astz = rand() % 50 + 1;
		multx = rand() % 2 ? 1 : -1;
		multz = rand() % 2 ? 1 : -1;
		valx = (rand() % 20) * 0.001;
		valz = (rand() % 20) * 0.001;
		astDestroyed = false;

	}
	if (ast&!astDestroyed) {
		astx += valx*multx;
		astz += valz*multz;
		translation = translate(identity, vec3(astx,0.0f,astz));
		scaling = scale(identity, vec3(0.27));
		rotation = rotate(identity, moonRotate, vec3(0, 1, 0));
		modelMatrix[AST] = translation * rotation * scaling;
		

		/*************/
		if (!earthDestroyed) {
			if (((earthPosition[0] - astx) < 1 + 0.5 & (earthPosition[0] - astx) > 0)&((earthPosition[2] - astz) < 1 + 0.5 & (earthPosition[0] - astz) > 0)) {
				astx -= 0.02;
				astz -= 0.02;
			}
			else if (((earthPosition[0] - astx) > -1 - 0.5 & (earthPosition[0] - astx) < 0)&((earthPosition[2] - astz) > -1 - 0.5 & (earthPosition[0] - astz) < 0)) {
				astx += 0.02;
				astz += 0.02;
			}

			if ((abs(earthPosition[0] - astx) < 1)&(abs(earthPosition[2] - astz) < 1)) {
				earthDestroyed = true;
				astDestroyed = true;
				modelMatrix[EARTH] = (mat4)0;
				modelMatrix[AST] = (mat4)0;
				earthPosition = (vec3)0;
			}
		}
		/*************/
		if (!venusDestroyed) {
			if (((venusPosition[0] - astx) < 1 + 0.5 & (venusPosition[0] - astx) > 0)&((venusPosition[2] - astz) < 1 + 0.5&(venusPosition[0] - astz) > 0)) {
				astx -= 0.02;
				astz -= 0.02;
			}
			else if (((venusPosition[0] - astx) > -1 - 0.5 &(venusPosition[0] - astx) < 0)&((venusPosition[2] - astz) > -1 - 0.5&(venusPosition[0] - astz) < 0)) {
				astx += 0.02;
				astz += 0.02;
			}

			if ((abs(venusPosition[0] - astx) < 1)&(abs(venusPosition[2] - astz) < 1)) {
				venusDestroyed = true;
				astDestroyed = true;
				modelMatrix[VENUS] = (mat4)0;
				modelMatrix[AST] = (mat4)0;
				venusPosition = (vec3)0;
			}
		}
		/*************/
		if (!mercuryDestroyed) {
			if (((mercuryPosition[0] - astx) < 0.3 + 0.5 & (mercuryPosition[0] - astx) > 0)&((mercuryPosition[2] - astz) < 0.3 + 0.5&(mercuryPosition[0] - astz) > 0)) {
				astx -= 0.02*0.3;
				astz -= 0.02*0.3;
			}
			else if (((mercuryPosition[0] - astx) > -0.3 - 0.5 &(mercuryPosition[0] - astx) < 0)&((mercuryPosition[2] - astz) > -0.3 - 0.5&(mercuryPosition[0] - astz) < 0)) {
				astx += 0.02*0.3;
				astz += 0.02*0.3;
			}

			if ((abs(mercuryPosition[0] - astx) < 0.3)&(abs(mercuryPosition[2] - astz) < 0.3)) {
				mercuryDestroyed = true;
				astDestroyed = true;
				modelMatrix[MERCURY] = (mat4)0;
				modelMatrix[AST] = (mat4)0;
				mercuryPosition = (vec3)0;
			}
		}
		/*************/
		if (!moonDestroyed) {
			if (((moonPosition[0] - astx) < 0.27 + 0.5 &(moonPosition[0] - astx) > 0)&((moonPosition[2] - astz) < 0.27 + 0.5&(moonPosition[0] - astz) > 0)) {
				astx -= 0.02*0.27;
				astz -= 0.02*0.27;
			}
			else if (((moonPosition[0] - astx) > -0.27 - 0.5&(moonPosition[0] - astx) < 0)&((moonPosition[2] - astz) > -0.27 - 0.5&(moonPosition[0] - astz) < 0)) {
				astx += 0.02*0.27;
				astz += 0.02*0.27;
			}

			if ((abs(moonPosition[0] - astx) < 0.27)&(abs(moonPosition[2] - astz) < 0.27)) {
				moonDestroyed = true;
				astDestroyed = true;
				modelMatrix[MOON] = (mat4)0;
				modelMatrix[AST] = (mat4)0;
				moonPosition = (vec3)0;
			}

		}
		/*************/
		if (!marsDestroyed) {
			if (((marsPosition[0] - astx) < 0.7 + 0.5&(marsPosition[0] - astx) > 0)&((marsPosition[2] - astz) < 0.7 + 0.5&(marsPosition[0] - astz) > 0)) {
				astx -= 0.02*0.7;
				astz -= 0.02*0.7;
			}
			else if (((marsPosition[0] - astx) > -0.7 - 0.5&(marsPosition[0] - astx) < 0)&((marsPosition[2] - astz) > -0.7 - 0.5&(marsPosition[0] - astz) < 0)) {
				astx += 0.02*0.7;
				astz += 0.02*0.7;
			}

			if ((abs(marsPosition[0] - astx) < 0.7)&(abs(marsPosition[2] - astz) < 0.7)) {
				marsDestroyed = true;
				astDestroyed = true;
				modelMatrix[MARS] = (mat4)0;
				modelMatrix[AST] = (mat4)0;
				marsPosition = (vec3)0;
			}

		}
		/*************/
		if (!jupiterDestroyed) {
			if (((jupiterPosition[0] - astx) < 5 + 0.5&(jupiterPosition[0] - astx) > 0)&((jupiterPosition[2] - astz) < 5 + 0.5&(jupiterPosition[0] - astz) > 0)) {
				astx -= 0.02 * 5;
				astz -= 0.02 * 5;
			}
			else if (((jupiterPosition[0] - astx) > -5 - 0.5&(jupiterPosition[0] - astx) < 0)&((jupiterPosition[2] - astz) > -5 - 0.5&(jupiterPosition[0] - astz) < 0)) {
				astx += 0.02 * 5;
				astz += 0.02 * 5;
			}

			if ((abs(jupiterPosition[0] - astx) < 5)&(abs(jupiterPosition[2] - astz) < 5)) {
				jupiterDestroyed = true;
				astDestroyed = true;
				modelMatrix[JUPITER] = (mat4)0;
				modelMatrix[AST] = (mat4)0;
				jupiterPosition = (vec3)0;
			}
		}

		/*************/
		if (!saturnDestroyed) {
			if (((saturnPosition[0] - astx) < 4 + 0.5&(saturnPosition[0] - astx) > 0)&((saturnPosition[2] - astz) < 4 + 0.5 && (saturnPosition[0] - astz) > 0)) {
				astx -= 0.02 * 4;
				astz -= 0.02 * 4;
			}
			else if (((saturnPosition[0] - astx) > -4 - 0.5&(saturnPosition[0] - astx) < 0)&((saturnPosition[2] - astz) > -4 - 0.5&(saturnPosition[0] - astz) < 0)) {
				astx += 0.02 * 4;
				astz += 0.02 * 4;
			}

			if ((abs(saturnPosition[0] - astx) < 4)&(abs(saturnPosition[2] - astz) < 4)) {
				saturnDestroyed = true;
				astDestroyed = true;
				modelMatrix[SATURN] = (mat4)0;
				modelMatrix[AST] = (mat4)0;
				saturnPosition = (vec3)0;
			}

		}
		/*************/
		if (!uranusDestroyed) {
			if (((uranusPosition[0] - astx) < 2 + 0.5&(uranusPosition[0] - astx) > 0)&((uranusPosition[2] - astz) < 2 + 0.5&(uranusPosition[0] - astz) > 0)) {
				astx -= 0.02 * 2;
				astz -= 0.02 * 2;
			}
			else if (((uranusPosition[0] - astx) > -2 - 0.5&(uranusPosition[0] - astx) < 0)&((uranusPosition[2] - astz) > -2 - 0.5&(uranusPosition[0] - astz) < 0)) {
				astx += 0.02 * 2;
				astz += 0.02 * 2;
			}

			if ((abs(uranusPosition[0] - astx) < 2)&(abs(uranusPosition[2] - astz) < 2)) {
				uranusDestroyed = true;
				astDestroyed = true;
				modelMatrix[URANUS] = (mat4)0;
				modelMatrix[AST] = (mat4)0;
				uranusPosition = (vec3)0;
			}

		}
		/*************/
		if (!neptuneDestroyed) {
			if (((neptunePosition[0] - astx) < 3 + 0.5&(neptunePosition[0] - astx) > 0)&((neptunePosition[2] - astz) < 3 + 0.5&(neptunePosition[0] - astz) > 0)) {
				astx -= 0.02 * 3;
				astz -= 0.02 * 3;
			}
			else if (((neptunePosition[0] - astx) > -3 - 0.5&(neptunePosition[0] - astx) < 0)&((neptunePosition[2] - astz) > -3 - 0.5&(neptunePosition[0] - astz) < 0)) {
				astx += 0.02 * 3;
				astz += 0.02 * 3;
			}

			if ((abs(neptunePosition[0] - astx) < 3)&(abs(neptunePosition[2] - astz) < 3)) {
				neptuneDestroyed = true;
				astDestroyed = true;
				modelMatrix[NEPTUNE] = (mat4)0;
				modelMatrix[AST] = (mat4)0;
				neptunePosition = (vec3)0;
			}
		}


	}

	//FreeCam(deltaTime);

	
	if (viewMode == 0)
	{   // Look from in front of the earth, at the earth
		viewMatrix = inverse(lookAt(earthPosition + normalize(vec3(-3.0f, 1.0f, 3.0f)) * 5.0f, earthPosition, vec3(0, 1, 0)));
	}
	else if (viewMode == 1)
	{   // Look from behind the earth, at the earth and moon
		viewMatrix = inverse(lookAt(earthPosition + normalize(vec3(-3.0f, -2.0f, 3.0f)) * -5.0f, (earthPosition + moonPosition) * 0.5f, vec3(0, 1, 0)));
	}
	else if (viewMode == 2)
	{   // Look from the sun, at the earth and moon
		viewMatrix = inverse(lookAt(vec3(0.0f), (earthPosition + moonPosition) * 0.5f, vec3(0, 1, 0)));
	}
	else if (viewMode == 3)
	{   // Look from the moon, at the earth
		viewMatrix = inverse(lookAt(vec3(0.0f), (mercuryPosition + neptunePosition) * 0.5f, vec3(0, 1, 0)));
	}
	else if (viewMode == 4)
	{   // Static view
		viewMatrix = updateCam(deltaTime);//inverse(lookAt(moonPosition, (earthPosition), vec3(0, 1, 0)));
	}

	//std::cout << planetRotations << std::endl;
}

void Render()
{
	//------------------------------------------------------------------------------------------------ Draw Skybox

	{
		// Use the special skybox program
		glUseProgram(skyboxProgram);                                    // <- Use the skybox shader program. This has the vertex and fragment  shader for the skybox

																		// Getting uniform locations  
		GLuint sLoc = glGetUniformLocation(skyboxProgram, "skybox");    // <- Get the uniform location for the skybox
		GLuint vLoc = glGetUniformLocation(skyboxProgram, "view");      // <- Get the uniform location for the view matrix
		GLuint pLoc = glGetUniformLocation(skyboxProgram, "proj");      // <- Get the uniform location for the projection matrix

																		// Binding skybox texture
		glUniform1i(sLoc, 0);                                           // <- 1) Get the uniform location for the cubemap sampler, and set it to index zero                       
		glActiveTexture(GL_TEXTURE0);                                   // <- 2) Set the active texture to also be index zero, matching above                             
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);              // <- 3) Bind the skybox texture. This texture is bound to zero, so it will be sampled              

																		// Passing up view-projection matrix
		glUniformMatrix4fv(vLoc, 1, GL_FALSE,                           // <- Pass through a special version of the view matrix. This has no position information, as
			&inverse(mat4(mat3(viewMatrix)))[0][0]);                    //    the position was removed by downcasting to mat3, then back up to mat4. It's inverted as well
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, &projectionMatrix[0][0]); // <- Pass through the projection matrix here to the vertex shader

																		// Drawing the skybox
		Primitive::DrawSkybox();                                        // <- Draw the skybox here. It's an inverted cube around the camera                                     

																		// Unbinding the texture and program
		glBindTexture(GL_TEXTURE_CUBE_MAP, GL_NONE);                    // <- Unbind the texture after we've drawn the skybox here                                  
		glUseProgram(GL_NONE);                                          // <- Unbind the shader program after we've used it here                                    
	}

	//------------------------------------------------------------------------------------------------ Draw Models

	{   //----------------------------------------------------------- EARTH ----------------------------------------------------------------------
		// Use the phong program
		glUseProgram(phongProgram);                                         // <- Use the phong lighting shader program

																			// Getting uniform locations
		GLuint dtLoc = glGetUniformLocation(phongProgram, "diffuseTex");    // <- Get the uniform location for the diffuse texture
		GLuint stLoc = glGetUniformLocation(phongProgram, "specularTex");   // <- Get the uniform location for the diffuse texture
		GLuint cLoc = glGetUniformLocation(phongProgram, "cameraPos");      // <- Get the uniform location for the projection matrix
		GLuint mLoc = glGetUniformLocation(phongProgram, "model");          // <- Get the uniform location for the model matrix
		GLuint vLoc = glGetUniformLocation(phongProgram, "view");           // <- Get the uniform location for the view matrix
		GLuint pLoc = glGetUniformLocation(phongProgram, "proj");           // <- Get the uniform location for the projection matrix
		GLuint nLoc = glGetUniformLocation(phongProgram, "norm");           // <- Get the uniform location for the normal matrix

																			// Binding diffuse texture
		glUniform1i(dtLoc, 0);                                              // <- 1) Get the uniform location for the 2D sampler, and set it to index zero                       
		glActiveTexture(GL_TEXTURE0);                                       // <- 2) Set the active texture to also be index zero, matching above                             
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);                       // <- 3) Bind the diffuse texture (bound to index 0)

																			// Binding specular texture
		glUniform1i(stLoc, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularTexture);

		// Passing MVP matrix
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, &modelMatrix[EARTH][0][0]);   // <- Pass through the model matrix here to the vertex shader
		glUniformMatrix4fv(vLoc, 1, GL_FALSE, &inverse(viewMatrix)[0][0]);  // <- Pass through the inverse of the view matrix here to the vertex shader
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, &projectionMatrix[0][0]);     // <- Pass through the projection matrix here to the vertex shader
		glUniformMatrix4fv(nLoc, 1, GL_FALSE,                               // <- Pass through the transpose of the inverse of the model matrix
			&transpose(inverse(modelMatrix[EARTH]))[0][0]);                 //    so that we correctly transform the normals into world space as well

																			// Passing up additional information
		glUniform3fv(cLoc, 1, &cameraPosition[0]);                          // <- Pass through the camera location to the shader

		Primitive::DrawSphere();    // Earth

									// Unbinding textures
		glActiveTexture(GL_TEXTURE1);                               // <- Set Texture 1 to be active, and then                
		glBindTexture(GL_TEXTURE_2D, GL_NONE);                      // <- unbind it here
		glActiveTexture(GL_TEXTURE0);                               // <- Set Texture 0 to be active, and then         
		glBindTexture(GL_TEXTURE_2D, GL_NONE);                      // <- unbind it here            

																	//----------------------------------------------------------- THE MOON (see above for comments) --------------------------------------------------

																	// Binding diffuse texture
		glUniform1i(dtLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, moonTexture);

		// Binding specular texture
		glUniform1i(stLoc, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, moonTexture);

		// Passing MVP matrix
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, &modelMatrix[MOON][0][0]);
		glUniformMatrix4fv(vLoc, 1, GL_FALSE, &inverse(viewMatrix)[0][0]);
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
		glUniformMatrix4fv(nLoc, 1, GL_FALSE,
			&transpose(inverse(modelMatrix[MOON]))[0][0]);

		// Passing up additional information
		glUniform3fv(cLoc, 1, &cameraPosition[0]);

		Primitive::DrawSphere();    // Moon

									// Unbinding textures
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);



		//-----------------------------------------------------------------------------//
		// Binding diffuse texture
		glUniform1i(dtLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mercuryTexture);

		// Binding specular texture
		glUniform1i(stLoc, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mercuryTexture);

		// Passing MVP matrix
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, &modelMatrix[MERCURY][0][0]);
		glUniformMatrix4fv(vLoc, 1, GL_FALSE, &inverse(viewMatrix)[0][0]);
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
		glUniformMatrix4fv(nLoc, 1, GL_FALSE,
			&transpose(inverse(modelMatrix[MERCURY]))[0][0]);

		// Passing up additional information
		glUniform3fv(cLoc, 1, &cameraPosition[0]);

		Primitive::DrawSphere();    // Mercury

									// Unbinding textures
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);





		//-----------------------------------------------------------------------------//
		// Binding diffuse texture
		glUniform1i(dtLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, venusTexture);

		// Binding specular texture
		glUniform1i(stLoc, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, venusTexture);

		// Passing MVP matrix
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, &modelMatrix[VENUS][0][0]);
		glUniformMatrix4fv(vLoc, 1, GL_FALSE, &inverse(viewMatrix)[0][0]);
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
		glUniformMatrix4fv(nLoc, 1, GL_FALSE,
			&transpose(inverse(modelMatrix[VENUS]))[0][0]);

		// Passing up additional information
		glUniform3fv(cLoc, 1, &cameraPosition[0]);

		Primitive::DrawSphere();    // Moon

									// Unbinding textures
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);



		//-----------------------------------------------------------------------------//
		// Binding diffuse texture
		glUniform1i(dtLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, marsTexture);

		// Binding specular texture
		glUniform1i(stLoc, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, marsTexture);

		// Passing MVP matrix
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, &modelMatrix[MARS][0][0]);
		glUniformMatrix4fv(vLoc, 1, GL_FALSE, &inverse(viewMatrix)[0][0]);
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
		glUniformMatrix4fv(nLoc, 1, GL_FALSE,
			&transpose(inverse(modelMatrix[MARS]))[0][0]);

		// Passing up additional information
		glUniform3fv(cLoc, 1, &cameraPosition[0]);

		Primitive::DrawSphere();    // Moon

									// Unbinding textures
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);




		//-----------------------------------------------------------------------------//
		// Binding diffuse texture
		glUniform1i(dtLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, jupiterTexture);

		// Binding specular texture
		glUniform1i(stLoc, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, jupiterTexture);

		// Passing MVP matrix
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, &modelMatrix[JUPITER][0][0]);
		glUniformMatrix4fv(vLoc, 1, GL_FALSE, &inverse(viewMatrix)[0][0]);
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
		glUniformMatrix4fv(nLoc, 1, GL_FALSE,
			&transpose(inverse(modelMatrix[JUPITER]))[0][0]);

		// Passing up additional information
		glUniform3fv(cLoc, 1, &cameraPosition[0]);

		Primitive::DrawSphere();    // Moon

									// Unbinding textures
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);



		//-----------------------------------------------------------------------------//
		// Binding diffuse texture
		glUniform1i(dtLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, saturnTexture);

		// Binding specular texture
		glUniform1i(stLoc, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, saturnTexture);

		// Passing MVP matrix
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, &modelMatrix[SATURN][0][0]);
		glUniformMatrix4fv(vLoc, 1, GL_FALSE, &inverse(viewMatrix)[0][0]);
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
		glUniformMatrix4fv(nLoc, 1, GL_FALSE,
			&transpose(inverse(modelMatrix[SATURN]))[0][0]);

		// Passing up additional information
		glUniform3fv(cLoc, 1, &cameraPosition[0]);

		Primitive::DrawSphere();    // Moon

									// Unbinding textures
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);


		//-----------------------------------------------------------------------------//
		// Binding diffuse texture
		glUniform1i(dtLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, uranusTexture);

		// Binding specular texture
		glUniform1i(stLoc, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, uranusTexture);

		// Passing MVP matrix
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, &modelMatrix[URANUS][0][0]);
		glUniformMatrix4fv(vLoc, 1, GL_FALSE, &inverse(viewMatrix)[0][0]);
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
		glUniformMatrix4fv(nLoc, 1, GL_FALSE,
			&transpose(inverse(modelMatrix[URANUS]))[0][0]);

		// Passing up additional information
		glUniform3fv(cLoc, 1, &cameraPosition[0]);

		Primitive::DrawSphere();    // Moon

									// Unbinding textures
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);



		//-----------------------------------------------------------------------------//
		// Binding diffuse texture
		glUniform1i(dtLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, neptuneTexture);

		// Binding specular texture
		glUniform1i(stLoc, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, neptuneTexture);

		// Passing MVP matrix
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, &modelMatrix[NEPTUNE][0][0]);
		glUniformMatrix4fv(vLoc, 1, GL_FALSE, &inverse(viewMatrix)[0][0]);
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
		glUniformMatrix4fv(nLoc, 1, GL_FALSE,
			&transpose(inverse(modelMatrix[NEPTUNE]))[0][0]);

		// Passing up additional information
		glUniform3fv(cLoc, 1, &cameraPosition[0]);

		Primitive::DrawSphere();    // Moon

									// Unbinding textures
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		if (ast) {
			//-----------------------------------------------------------------------------//
			// Binding diffuse texture
			glUniform1i(dtLoc, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, moonTexture);

			// Binding specular texture
			glUniform1i(stLoc, 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, neptuneTexture);

			// Passing MVP matrix
			glUniformMatrix4fv(mLoc, 1, GL_FALSE, &modelMatrix[AST][0][0]);
			glUniformMatrix4fv(vLoc, 1, GL_FALSE, &inverse(viewMatrix)[0][0]);
			glUniformMatrix4fv(pLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
			glUniformMatrix4fv(nLoc, 1, GL_FALSE,
				&transpose(inverse(modelMatrix[AST]))[0][0]);

			// Passing up additional information
			glUniform3fv(cLoc, 1, &cameraPosition[0]);

			Primitive::DrawSphere();    // Moon

										// Unbinding textures
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, GL_NONE);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, GL_NONE);
		}
		//----------------------------------------------------------- THE SUN (see above for comments) ----------------------------------------------------

		glUseProgram(emissiveProgram);

		mLoc = glGetUniformLocation(emissiveProgram, "model");
		vLoc = glGetUniformLocation(emissiveProgram, "view");
		pLoc = glGetUniformLocation(emissiveProgram, "proj");
		GLuint etLoc = glGetUniformLocation(emissiveProgram, "emissiveTex");

		// Binding emissive texture
		glUniform1i(etLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sunTexture);

		// Passing MVP matrix
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, &modelMatrix[SUN][0][0]);
		glUniformMatrix4fv(vLoc, 1, GL_FALSE, &inverse(viewMatrix)[0][0]);
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, &projectionMatrix[0][0]);

		Primitive::DrawSphere();    // Sun

									// Unbinding textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		// unbinding the shader program
		glUseProgram(GL_NONE);



	}
}

void Cleanup()
{
	// Cleanup the shader programs here
	glDeleteProgram(skyboxProgram);
	glDeleteProgram(phongProgram);

	// Cleanup the textures here
	glDeleteTextures(1, &skyboxTexture);
	glDeleteTextures(1, &diffuseTexture);
	glDeleteTextures(1, &specularTexture);
}

void GUI()
{
	ImGui::Begin("Lab 8");
	{
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);

		ImGui::Spacing();
		ImGui::DragFloat("Simulation Speed", &simulationSpeed, 0.01f, 100.0f); simulationSpeed = clamp(simulationSpeed, 0.01f, 100.0f);
		ImGui::RadioButton("View 1", &viewMode, 0); ImGui::SameLine();
		ImGui::RadioButton("View 2", &viewMode, 1); ImGui::SameLine();
		ImGui::RadioButton("View 3", &viewMode, 2); ImGui::SameLine();
		ImGui::RadioButton("View 4", &viewMode, 3);

		ImGui::RadioButton("Mouse/keyboard movement", &viewMode, 4);
	}
	ImGui::End();
}

void OnWindowResized(GLFWwindow* win, int w, int h)
{
	// Set the viewport incase the window size changed
	width = w; height = h;
	glViewport(0, 0, width, height);
	float ratio = width / (float)height;
	projectionMatrix = perspective(radians(40.0f), ratio, 0.1f, 1000.0f);
}


int main()
{
	// start GL context and O/S window using the GLFW helper library
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	window = glfwCreateWindow(width, height, "Laboratory 8", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwSetWindowSizeCallback(window, OnWindowResized);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0); // essentially turn off vsync

						 // start GL3W
	gl3wInit();

	// Resize at least once
	OnWindowResized(window, width, height);

	// Setup ImGui binding
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsLight();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

	Initialize();

	float oldTime = 0.0f, currentTime = 0.0f, deltaTime = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		do { currentTime = (float)glfwGetTime(); } while (currentTime - oldTime < 1.0f / 120.0f);

		//FreeCam(deltaTime);
		// update other events like input handling 
		glfwPollEvents();
		
		// Clear the screen
		glClearColor(0.96f, 0.97f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplGlfwGL3_NewFrame();

		deltaTime = currentTime - oldTime; // Difference in time
		oldTime = currentTime;

		// Call the helper functions
		Update(deltaTime);
		Render();
		GUI();

		// Finish by drawing the GUI
		ImGui::Render();
		glfwSwapBuffers(window);
	}

	// close GL context and any other GLFW resources
	glfwTerminate();
	ImGui_ImplGlfwGL3_Shutdown();
	Cleanup();
	return 0;
}


