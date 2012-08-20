#ifndef _gfx_h
#define _gfx_h

bool	gfxinit(int width, int height);
GLuint	gfxloadshader(GLenum type, const char *src);
GLuint	gfxmakeprogram(GLuint shvert, GLuint shfrag);
void	gfxswap();
void	gfxfree();

#endif

