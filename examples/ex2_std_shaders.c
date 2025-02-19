#define PORTABLEGL_IMPLEMENTATION
#include "portablegl.h"


#include <stdio.h>


#define SDL_MAIN_HANDLED
#include <SDL.h>

#define WIDTH 640
#define HEIGHT 480


SDL_Window* window;
SDL_Renderer* ren;
SDL_Texture* tex;

u32* bbufpix;

glContext the_Context;

void cleanup();
void setup_context();


int main(int argc, char** argv)
{
	setup_context();

	float points_n_colors[] = {
		-0.5, -0.5, 0.0,
		 1.0,  0.0, 0.0,

		 0.5, -0.5, 0.0,
		 0.0,  1.0, 0.0,

		 0.0,  0.5, 0.0,
		 0.0,  0.0, 1.0 };

	GLuint triangle;
	glGenBuffers(1, &triangle);
	glBindBuffer(GL_ARRAY_BUFFER, triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points_n_colors), points_n_colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(PGL_ATTR_VERT);
	glVertexAttribPointer(PGL_ATTR_VERT, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, 0);
	glEnableVertexAttribArray(PGL_ATTR_COLOR);
	glVertexAttribPointer(PGL_ATTR_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)(sizeof(float)*3));

	// create standard programs and select one for use
	GLuint std_shaders[PGL_NUM_SHADERS];
	pgl_init_std_shaders(std_shaders);
	glUseProgram(std_shaders[PGL_SHADER_SHADED]);

	// Even though we're not transforming the verts, this standard shader
	// does expect an mvp matrix so we have to set the uniforms
	pgl_uniforms the_uniforms = { .mvp_mat = IDENTITY_MAT4() };
	pglSetUniform(&the_uniforms);

	glClearColor(0, 0, 0, 1);


	SDL_Event e;
	int quit = 0;

	unsigned int old_time = 0, new_time=0, counter = 0;
	unsigned int last_frame = 0;
		
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				quit = 1;
			if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				quit = 1;
			if (e.type == SDL_MOUSEBUTTONDOWN)
				quit = 1;
		}

		new_time = SDL_GetTicks();
		last_frame = new_time;
		
		counter++;
		if (!(counter % 300)) {
			printf("%d  %f FPS\n", new_time-old_time, 300000/((float)(new_time-old_time)));
			old_time = new_time;
			counter = 0;
		}

		
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		SDL_UpdateTexture(tex, NULL, bbufpix, WIDTH * sizeof(u32));
		//Render the scene
		SDL_RenderCopy(ren, tex, NULL, NULL);
		SDL_RenderPresent(ren);
	}

	cleanup();

	return 0;
}

void setup_context()
{
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("SDL_init error: %s\n", SDL_GetError());
		exit(0);
	}

	window = SDL_CreateWindow("c_ex2", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	if (!window) {
		printf("Failed to create window\n");
		SDL_Quit();
		exit(0);
	}

	ren = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

	if (!init_glContext(&the_Context, &bbufpix, WIDTH, HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000)) {
		puts("Failed to initialize glContext");
		exit(0);
	}
}

void cleanup()
{
	free_glContext(&the_Context);

	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(window);

	SDL_Quit();
}



