#include "SDL.h"
#define GL_GLEXT_PROTOTYPES
#include "SDL_opengl.h"

const int	SLEEP	= 10;

SDL_Window		*gwin;
SDL_GLContext	gcontext;

bool
gfxinit(int width, int height)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	// SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 1);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	gwin = SDL_CreateWindow("OpenGL 3.1 test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                       width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!gwin) {
		fprintf(stderr, "SDL_CreateWindow() failed: %s\n", SDL_GetError());
		return false;
	}

	gcontext = SDL_GL_CreateContext(gwin);
	if (!gcontext) {
		fprintf(stderr, "SDL_GL_CreateContext() failed: %s\n", SDL_GetError());
		SDL_DestroyWindow(gwin);
		return false;
	}

	SDL_GL_SetSwapInterval(1);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, width, height);

	return true;
}

void
printlog(GLuint sh)
{
	GLint	len;

	if (!glIsShader(sh) && !glIsProgram(sh)) {
		fprintf(stderr, "printlog: not a shader or program\n");
		return;
	}

	bool isprog = glIsProgram(sh);
	if (isprog)
		glGetProgramiv(sh, GL_INFO_LOG_LENGTH, &len);
	else
		glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		char *serr = (char*) malloc(sizeof(char) * len);
		if (isprog)
			glGetProgramInfoLog(sh, len, NULL, serr);
		else
			glGetShaderInfoLog(sh, len, NULL, serr);
		fprintf(stderr, "%s\n", serr);
		free(serr);
	}
}

GLuint
gfxloadshader(GLenum type, const char *src)
{
	GLuint sh;

	sh = glCreateShader(type);
	if (sh == 0)
		return 0;

	glShaderSource(sh, 1, &src, NULL);
	glCompileShader(sh);

	GLint ret;
	glGetShaderiv(sh, GL_COMPILE_STATUS, &ret);
	if (ret == GL_FALSE) {
		fprintf(stderr, "error compiling shader:\n");
		printlog(sh);
		glDeleteShader(sh);
		return 0;
	}

	return sh;
}

GLuint
gfxmakeprogram(GLuint shvert, GLuint shfrag)
{
	GLuint pr;

	pr = glCreateProgram();
	glAttachShader(pr, shvert);
	glAttachShader(pr, shfrag);
	glDeleteShader(shvert);
	glDeleteShader(shfrag);
	glLinkProgram(pr);

	GLint ret;
	glGetProgramiv(pr, GL_LINK_STATUS, &ret);
	if (ret == GL_FALSE) {
		fprintf(stderr, "error linking shader program:\n");
		printlog(pr);
		glDeleteProgram(pr);
		return 0;
	}

	return pr;
}

void
gfxswap()
{
	SDL_GL_SwapWindow(gwin);
	// SDL_Delay(SLEEP);
}

void
gfxfree()
{
	SDL_GL_DeleteContext(gcontext);
	SDL_DestroyWindow(gwin);
	SDL_Quit();
}

