#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <stdlib.h>
#include <SDL_mixer.h>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	///setup
	ShaderProgram program = ShaderProgram(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;


	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
	float lastFrameTicks = 0.0f;
	float positionY_p1 = 0.0f;
	float positionY_p2 = 0.0f;
	float velocityX = 0.0005f;
	float velocityY = 0.0005f;
	float ball_x = 0.0f;
	float ball_y = 0.0f;

	float playerOneX1 = 3.4f;
	float playerOneX2 = 3.5f;
	float playerOneY1 = 0.5f;
	float playerOneY2 = -0.5f;

	float playerTwoX1 = -3.4f;
	float playerTwoX2 = -3.5f;
	float playerTwoY1 = 0.5f;
	float playerTwoY2 = -0.5f;

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Chunk* paddleHit;
	paddleHit = Mix_LoadWAV("hitPaddle.wav");
	
	Mix_Music* music;
	music = Mix_LoadMUS("sandstorm.mp3");
	Mix_PlayMusic(music, -1);

	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		//Player 1 Controls (UP and Down Keys)
		if (keys[SDL_SCANCODE_UP])
		{
			if (positionY_p1 > 1.5)
			{
				positionY_p1 += 0;
			}
			else
			{
				positionY_p1 += 0.003f;
				playerOneY1 += 0.003f;
				playerOneY2 += 0.003f;
			}
				
		}
		else if (keys[SDL_SCANCODE_DOWN])
		{
			if (positionY_p1 < -1.5)
			{
				positionY_p1 += 0;
			}
			else
			{
				positionY_p1 += -0.003f;
				playerOneY1 += -0.003f;
				playerOneY2 += -0.003f;
			}
		}

		//Player 2 Controls (W and S Keys)
		if (keys[SDL_SCANCODE_W])
		{
			if (positionY_p2 > 1.5)
			{
				positionY_p2 += 0;

			}
			else
			{
				positionY_p2 += 0.003f;
				playerTwoY1 += 0.003f;
				playerTwoY2 += 0.003f;
			}
				
		}
		else if (keys[SDL_SCANCODE_S])
		{
			if (positionY_p2 < -1.5)
			{
				positionY_p2 += 0;
			}
			else
			{
				positionY_p2 += -0.003f;
				playerTwoY1 += -0.003f;
				playerTwoY2 += -0.003f;
			}
				
		}

		modelMatrix.identity();
		modelMatrix.Translate(0.0f, positionY_p1, 0.0f);

		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		glUseProgram(program.programID);


		float player1[] = { 3.5f, -0.5f, 3.4f, 0.5f, 3.5f, 0.5f, 3.4f, 0.5f, 3.5f, -0.5f, 3.4f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, player1);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		modelMatrix.identity();
		modelMatrix.Translate(0.0f, positionY_p2, 0.0f);
		program.setModelMatrix(modelMatrix);

		float player2[] = { -3.5f, -0.5f, -3.4f, 0.5f, -3.5f, 0.5f, -3.4f, 0.5f, -3.5f, -0.5f, -3.4f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, player2);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		if (ball_x >= playerOneX1 && ball_x <= 3.65 && ball_y <= playerOneY1 && ball_y >= playerOneY2)
		{
			ball_x += -0.2f;
			velocityX *= -1;
			Mix_PlayChannel(-1, paddleHit, 0);
		}

		if (ball_x <= playerTwoX1 && ball_x >= -3.65  && ball_y <= playerTwoY1 && ball_y >= playerTwoY2)
		{
			ball_x += .2f;
			velocityX *= -1;
			Mix_PlayChannel(-1, paddleHit, 0);
		}

		if (ball_y >= 2 || ball_y <= -2)
			velocityY *= -1.0;

		//Win Check. Changes Screen Color
		if (ball_x >= 3.55 || ball_x <= -3.55)
		{
			glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			
		}
		ball_x += velocityX;
		ball_y += velocityY;


	
		modelMatrix.identity();
		modelMatrix.Translate(ball_x, ball_y, 0.0f);
		program.setModelMatrix(modelMatrix);

		float dot[] = { -0.1f, -0.1f, 0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 0.1f, -0.1f, -0.1f, 0.1f, -0.1f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, dot);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		SDL_GL_SwapWindow(displayWindow);
	}

	Mix_FreeChunk(paddleHit);
	Mix_FreeMusic(music);
	SDL_Quit();
	return 0;
}
