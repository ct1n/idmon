#include <stdio.h>
#include <errno.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "SDL.h"
#define GL_GLEXT_PROTOTYPES
#include "SDL_opengl.h"

#include "gfx.h"

using namespace std;

const int	WIDTH	= 640;
const int	HEIGHT	= 480;

bool	grunning;
GLuint	gshvert;
GLuint	gshfrag;
GLuint	gshprog;
// GLuint	gvao;
GLuint	gvbovert;
GLuint	gvbocol;
GLuint	gibo;
GLint	gvarmvp;
GLint	gvarpos;
GLint	gvarcol;
glm::vec3	gcampos;
glm::vec3	gcamfront;
glm::vec3	gcamtop;
glm::vec3	gcamleft;
glm::mat4	gmvp;

bool	initglprogs();
string	readfile(const char *path);
void	creategeom();
void	updatemvp();
void	run();
void	checkevents();
void	rotatecam(glm::vec3 dir);
void	render();

int
main(int argc, char *argv[])
{

	if (!gfxinit(WIDTH, HEIGHT)) {
		return 1;
	}

	if (!initglprogs()) {
		gfxfree();
		return 1;
	}

	gcampos = glm::vec3(0.0, 2.0, 0.0);
	gcamfront = glm::vec3(0.0, 0.0, -1.0);
	gcamtop = glm::vec3(0.0, 1.0, 0.0);
	gcamleft = glm::vec3(-1.0, 0.0, 0.0);
	updatemvp();
	creategeom();

	run();

	gfxfree();
	return 0;
}

bool
initglprogs()
{
	string src = readfile("vert.glsl");
	if (src.size() == 0) {
		gfxfree();
		return false;
	}
	gshvert = gfxloadshader(GL_VERTEX_SHADER, src.c_str());
	if (gshvert == 0) {
		gfxfree();
		return false;
	}

	src = readfile("frag.glsl");
	if (src.size() == 0) {
		gfxfree();
		return false;
	}
	gshfrag = gfxloadshader(GL_FRAGMENT_SHADER, src.c_str());
	if (gshfrag == 0) {
		gfxfree();
		return false;
	}

	gshprog = gfxmakeprogram(gshvert, gshfrag);
	if (gshprog == 0) {
		gfxfree();
		return false;
	}
	glUseProgram(gshprog);

	gvarmvp = glGetUniformLocation(gshprog, "mvp");
	if (gvarmvp == -1) {
		fprintf(stderr, "Unable to get uniform location for mvp\n");
		return false;
	}
	gvarpos = glGetAttribLocation(gshprog, "pos");
	if (gvarpos == -1) {
		fprintf(stderr, "Unable to get attribute location for pos\n");
		return false;
	}
	gvarcol = glGetAttribLocation(gshprog, "col");
	if (gvarcol == -1) {
		fprintf(stderr, "Unable to get attribute location for col\n");
		return false;
	}
	return true;
}

string readfile(const char *path)
{
	string str;
	FILE *f;

	f = fopen(path, "r");
	if (!f) {
		perror(path);
		return "";
	}

	while (!feof(f)) {
		char buf[1024];
		size_t n = fread(buf, sizeof(char), sizeof(buf), f);
		buf[n] = '\0';
		str += buf;
	}

	fclose(f);
	return str;
}

void
creategeom()
{
	// glGenVertexArrays(1, &gvao);
	// glBindVertexArray(gvao);

	const float cubev[] = {
		// front
		-1.0, -1.0,  1.0,
		 1.0, -1.0,  1.0,
		 1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		// back
		-1.0, -1.0, -1.0,
		 1.0, -1.0, -1.0,
		 1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0
	};

	glGenBuffers(1, &gvbovert);
	glBindBuffer(GL_ARRAY_BUFFER, gvbovert);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubev), cubev, GL_STATIC_DRAW);
	glVertexAttribPointer(gvarpos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(gvarpos);

	const float cubec[] = {
		// front
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		0.0, 1.0, 1.0,
		// back
		0.0, 1.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 1.0, 0.0,
		1.0, 0.0, 0.0
		/*
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		0.0, 1.0, 1.0
		*/
	};

	glGenBuffers(1, &gvbocol);
	glBindBuffer(GL_ARRAY_BUFFER, gvbocol);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubec), cubec, GL_STATIC_DRAW);
	glVertexAttribPointer(gvarcol, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(gvarcol);

	const GLushort elems[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// bottom
		4, 0, 3,
		3, 7, 4,
		// left
		4, 5, 1,
		1, 0, 4,
		// right
		3, 2, 6,
		6, 7, 3
	};

	glGenBuffers(1, &gibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void
updatemvp()
{
	glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, -4.0));
	glm::mat4 view = glm::lookAt(gcampos, gcampos + gcamfront, gcamtop);
	glm::mat4 proj = glm::perspective(45.0f, 1.0f * WIDTH / HEIGHT, 0.1f, 10.0f);
	gmvp = proj * view * model;
}

void
run()
{
	grunning = true;
	while (grunning) {
		checkevents();
		render();
	}
}

#define	KEYCHECK(a, b)	((a & 0xff) == (b & 0xff))
#define UPDATEKEY(k, ev) if (ev == SDL_KEYDOWN) k = true; else if (ev == SDL_KEYUP) k = false
#define	SPEED	0.1f
#define	RSPEED	1.0f

void
checkevents()
{
	SDL_Event	evt;
	static bool kq = false;
	static bool kw = false, ka = false, ks = false, kd = false;
	static bool kup = false, kdown = false, kleft = false, kright = false;
	bool	newcam;

	newcam = false;
	while (SDL_PollEvent(&evt)) {
		if (KEYCHECK(evt.key.keysym.sym, SDLK_q)) {
			UPDATEKEY(kq, evt.type);
		} else if (KEYCHECK(evt.key.keysym.sym, SDLK_w)) {
			UPDATEKEY(kw, evt.type);
		} else if (KEYCHECK(evt.key.keysym.sym, SDLK_a)) {
			UPDATEKEY(ka, evt.type);
		} else if (KEYCHECK(evt.key.keysym.sym, SDLK_s)) {
			UPDATEKEY(ks, evt.type);
		} else if (KEYCHECK(evt.key.keysym.sym, SDLK_d)) {
			UPDATEKEY(kd, evt.type);
		} else if (KEYCHECK(evt.key.keysym.sym, SDLK_UP)) {
			UPDATEKEY(kup, evt.type);
		} else if (KEYCHECK(evt.key.keysym.sym, SDLK_DOWN)) {
			UPDATEKEY(kdown, evt.type);
		} else if (KEYCHECK(evt.key.keysym.sym, SDLK_LEFT)) {
			UPDATEKEY(kleft, evt.type);
		} else if (KEYCHECK(evt.key.keysym.sym, SDLK_RIGHT)) {
			UPDATEKEY(kright, evt.type);
		}
	}
	if (kq)
		grunning = false;
	if (kw) {
		gcampos += SPEED * gcamfront; // glm::vec3(0.0, 0.0, -SPEED);
		newcam = true;
	}
	if (ka) {
		gcampos += SPEED * gcamleft;
		newcam = true;
	}
	if (ks) {
		gcampos += (-SPEED) * gcamfront; // glm::vec3(0.0, 0.0, SPEED);
		newcam = true;
	}
	if (kd) {
		gcampos += (-SPEED) * gcamleft;
		newcam = true;
	}
	if (kup) {
		rotatecam(-1.0f * gcamleft);
		newcam = true;
	}
	if (kdown) {
		rotatecam(gcamleft);
		newcam = true;
	}
	if (kleft) {
		rotatecam(gcamtop);
		newcam = true;
	}
	if (kright) {
		rotatecam(-1.0f * gcamtop);
		newcam = true;
	}
	if (newcam)
		updatemvp();
}

void
rotatecam(glm::vec3 dir)
{
	gcamfront = glm::rotate(gcamfront, RSPEED, dir);
	gcamtop = glm::rotate(gcamtop, RSPEED, dir);
	gcamleft = glm::rotate(gcamleft, RSPEED, dir);
}

void
render()
{
	float move = sinf(SDL_GetTicks() / 1000.0 * (2 * 3.14) / 5);
	float angle = SDL_GetTicks() / 1000.0 * 45;
	glm::vec3 axisz(0, 0, 1);
	glm::mat4 anim = glm::translate(glm::mat4(1.0), glm::vec3(move, 0.0, 0.0)) *
		             glm::rotate(glm::mat4(1.0), angle, axisz);
	glm::mat4 mvp = gmvp; // * anim;
	glUniformMatrix4fv(gvarmvp, 1, GL_FALSE, glm::value_ptr(mvp));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gibo);

	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	gfxswap();
}

