#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <string>
#include <vector>
#include <iostream>
#include <SDL_mixer.h>
#include "Entity.h"

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

GLuint LoadTexture(const char *image_path)
{
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);
	return textureID;
}

void DrawText(int fontTexture, std::string text, float size, float spacing, ShaderProgram program) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}
	glUseProgram(program.programID);
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program.positionAttribute);
	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program.texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);
}

void renderLevelOne(std::vector<Entity*>& v)
{
	float x = -6.0f;
	float y = 4.0f;
	for (int i = 0; i < 5; i++)
	{
		v.push_back(new Entity(x, y, 2.0, 2.0));
		x += 3.0f;
	}
	x = -6.0f;
	y = -3.5f;
	for (int i = 0; i < 5; i++)
	{
		v.push_back(new Entity(x, y, 2.0, 2.0));
		x += 3.0f;
	}
	x = -6.0f;
	y = 4.0f;
	for (int i = 0; i < 3; i++)
	{
		v.push_back(new Entity(x, y, 2.0, 2.0));
		y -= 2.5f;
	}

	x = 9.0f;
	y = 4.0f;
	for (int i = 0; i < 4; i++)
	{
		v.push_back(new Entity(x, y, 2.0, 2.0));
		y -= 2.5f;
	}
}
int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Treasure Hunter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif
	
	///setup
		glViewport(0, 0, 640, 360);
		ShaderProgram program = ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
		//ShaderProgram program = ShaderProgram(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
		Matrix projectionMatrix;
		Matrix modelMatrix;
		Matrix viewMatrix;
		GLuint fontTex = LoadTexture("font1.png");
		GLuint pla1 = LoadTexture("red.png");
		GLuint pla2 = LoadTexture("blue.png");
		GLuint pirate = LoadTexture("pirate.png");
		GLuint ball = LoadTexture("ball.png");
		GLuint chest = LoadTexture("treasure2.png");
		GLuint block = LoadTexture("brick.png");
		Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
		Mix_Music* music = Mix_LoadMUS("gameMusic.mp3");
		Mix_Music* music2 = Mix_LoadMUS("game2.mp3");
		Mix_Music* music3 = Mix_LoadMUS("gameOver.mp3");
		Mix_PlayMusic(music, -1);
		//projectionMatrix.setOrthoProjection(-2.0f, 2.0f, -8.95f, 8.95f, -1.0f, 1.0f);
		projectionMatrix.setOrthoProjection(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f);
		

		enum GameState { STATE_TITLE_SCREEN, STATE_LEVEL_ONE, STATE_LEVEL_TWO, STATE_LEVEL_THREE, STATE_FINAL };
		GameState state = STATE_TITLE_SCREEN;
		
		float lastFrameTicks = 0.0f;
		float positionX_p1 = 0.0f;
		float positionX_p2 = 0.0f;

		float playerOneX1 = 3.0f;
		float playerOneX2 = -3.0f;
		float playerOneY1 = 8.0f;
		float playerOneY2 = 8.9f;

		float playerTwoX1 = 3.0f;
		float playerTwoX2 = -3.0f;
		float playerTwoY1 = -8.0f;
		float playerTwoY2 = -8.9f;
		bool winGame;
		bool winLevelOne;
		bool winLevelTwo;
		bool gameDone;
		float ball_x = 0.0f;
		float ball_y = -6.0f;
		float velocityX = -0.0005f*2;
		float velocityY = -0.0005f*2;
		std::vector<Entity*> blocks;
		renderLevelOne(blocks);
		//Entity bob(4.0, 2.0, 2.5, 2.5);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;


		const Uint8* keys = SDL_GetKeyboardState(NULL);
		switch (state)
		{
			case STATE_TITLE_SCREEN:
			{
				  if (keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_RETURN2])
				  {
					   state = STATE_LEVEL_ONE;
					   Mix_PlayMusic(music2, -1);
				  }

				  if (keys[SDL_SCANCODE_ESCAPE])
				  {
					  done = true;
				  }
				 break;
			}
			case STATE_LEVEL_ONE:
			{

			//Player 1 Controls (Left and Right Keys)
				 if (keys[SDL_SCANCODE_RIGHT])
				 {
					   if (positionX_p1 > 13.5f)
					   {
						   positionX_p1 += 0;
					   }
					   else
					   {
						   positionX_p1 += 0.005f;
						   playerOneX1 += 0.005f;
						   playerOneX2 += 0.005f;
					   }
				  }
				  else if (keys[SDL_SCANCODE_LEFT])
				  {
					   if (positionX_p1 < -13.5f)
					   {
						   positionX_p1 += 0;
					   }
					   else
					   {
						   positionX_p1 += -0.005f;
						   playerOneX1 += -0.005f;
						   playerOneX2 += -0.005f;
					   }
				  }

			//Player 2 Controls (D and A Keys)
				  if (keys[SDL_SCANCODE_D])
				  {
					   if (positionX_p2 > 13.5f)
					   {
						   positionX_p2 += 0;

					   }
					   else
					   {
						   positionX_p2 += 0.005f;
						   playerTwoX1 += 0.005f;
						   playerTwoX2 += 0.005f;
					   }

				  }
				  else if (keys[SDL_SCANCODE_A])
				  {
					  if (positionX_p2 < -13.5f)
					  {
						  positionX_p2 += 0;
					  }
					  else
					  {
						  positionX_p2 += -0.005f;
						  playerTwoX1 += -0.005f;
						  playerTwoX2 += -0.005f;
					  }
				  }

				/*  if (keys[SDL_SCANCODE_SPACE])
				  {
					  Mix_PlayMusic(music3, 1);
					  state = STATE_FINAL;
				  }*/
				  break;
				}
				case STATE_FINAL:
				{
					if (keys[SDL_SCANCODE_ESCAPE])
					{
						done = true;
					}
					break;
				}
		}
		
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		glClear(GL_COLOR_BUFFER_BIT);
		switch (state)
		{
			case STATE_TITLE_SCREEN:
			{
				glClearColor(0.839216, 0.739216, 0.639216, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				modelMatrix.identity();
				modelMatrix.Translate(-11.5f, 5.0f, 1.0f);
				program.setModelMatrix(modelMatrix);
				DrawText(fontTex, "Treasure Hunter", 1.9f, -0.195f, program);
				modelMatrix.identity();
				modelMatrix.Translate(-11.0f, 4.0f, 1.0f);
				program.setModelMatrix(modelMatrix);
				DrawText(fontTex, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", .6f, -0.09f, program);
				

				modelMatrix.identity();
				modelMatrix.Translate(-8.50f, -3.50f, 1.0f);
				modelMatrix.Scale(.65, .65, 1);
				program.setModelMatrix(modelMatrix);
				DrawText(fontTex, "Press Enter To Start", 1.6f, -0.09f, program);

				modelMatrix.identity();
				modelMatrix.Translate(-5.50f, -5.50f, 1.0f);
				modelMatrix.Scale(.65, .65, 1);
				program.setModelMatrix(modelMatrix);
				DrawText(fontTex, "Press ESC To Exit", 1.2f, -0.001f, program);

				modelMatrix.identity();
				program.setModelMatrix(modelMatrix);
				float vertices[] = { -3.0f, -3.0f, 3.0f, 3.0f, -3.0f, 3.0f, 3.0f, 3.0f, -3.0f, -3.0f, 3.0f, -3.0f };
				glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
				glEnableVertexAttribArray(program.positionAttribute);

				float texCoords[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
				glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
				glEnableVertexAttribArray(program.texCoordAttribute);
				glUseProgram(program.programID);

				glBindTexture(GL_TEXTURE_2D, pirate);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(program.positionAttribute);
				glDisableVertexAttribArray(program.texCoordAttribute);
				
				break;
			}
			case STATE_LEVEL_ONE:
			{
				//glClearColor(0.419608, 0.556863, 0.137255, 1.0f);
				glClearColor(0.839216, 0.739216, 0.839216, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				modelMatrix.identity();
				modelMatrix.Translate(positionX_p1, 0.0f, 0.0f);

				program.setModelMatrix(modelMatrix);
				program.setProjectionMatrix(projectionMatrix);
				program.setViewMatrix(viewMatrix);

				float player1[] = { -3.0f, 8.0f, 3.0f, 8.9f, -3.0f, 8.9f, 3.0f, 8.9f, -3.0f, 8.0f, 3.0f, 8.0f };
				glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, player1);
				glEnableVertexAttribArray(program.positionAttribute);
				
				float texCoords[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
				glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
				glEnableVertexAttribArray(program.texCoordAttribute);
				
				glBindTexture(GL_TEXTURE_2D, pla1);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(program.positionAttribute);
				glDisableVertexAttribArray(program.texCoordAttribute);

				modelMatrix.identity();
				modelMatrix.Translate(positionX_p2, 0.0f, 0.0f);
				program.setModelMatrix(modelMatrix);

				float player2[] = { -3.0f, -8.0f, 3.0f, -8.9f, -3.0f, -8.9f, 3.0f, -8.9f, -3.0f, -8.0f, 3.0f, -8.0f };
				glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, player2);
				glEnableVertexAttribArray(program.positionAttribute);
				
				float texCoords2[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
				glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
				glEnableVertexAttribArray(program.texCoordAttribute);

				glBindTexture(GL_TEXTURE_2D, pla2);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(program.positionAttribute);
				glDisableVertexAttribArray(program.texCoordAttribute);


				if (ball_x <= playerOneX1 && ball_x >= playerOneX2 && ball_y >= playerOneY1 && ball_y <= playerOneY2)
				{
					ball_y -= .250f;
					velocityY *= -1.0f;

				}

				if (ball_x <= playerTwoX1 && ball_x >= playerTwoX2 && ball_y <= playerTwoY1 && ball_y >= playerTwoY2)
				{
					ball_y += .250f;
					velocityY *= -1.0f;
				}

				if (ball_x >= 15.4f || ball_x <= -15.4f)
				{
					//ball_x *= -1;
					velocityX *= -1.0f;
				}
				
				if (ball_y >= 9.0f || ball_y <= -9.0f)
				{
					winGame = false;
					gameDone = true;
					Mix_PlayMusic(music3, 1);
					state = STATE_FINAL;
					
				}

				ball_x += velocityX;
				ball_y += velocityY;

				modelMatrix.identity();
			//	modelMatrix.Translate(0.0f, -4.0f, 0.0f);
				modelMatrix.Translate(ball_x, ball_y, 0.0f);
				program.setModelMatrix(modelMatrix);

				float dot[] = { -.5f, -.5f, .5f, .5f, -.5f, .5f, .5f, .5f, -.5f, -.5f, .5f, -.5f };
				glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, dot);
				glEnableVertexAttribArray(program.positionAttribute);

				float texCoords3[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
				glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords3);
				glEnableVertexAttribArray(program.texCoordAttribute);

				glBindTexture(GL_TEXTURE_2D, ball);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(program.positionAttribute);
				glDisableVertexAttribArray(program.texCoordAttribute);

				modelMatrix.identity();
				modelMatrix.Translate(2.0f, 0.0f, 0.0f);
				program.setModelMatrix(modelMatrix);
				float vertices4[] = { -1.5f, -1.5f, 1.5f, 1.5f, -1.5f, 1.5f, 1.5f, 1.5f, -1.5f, -1.5f, 1.5f, -1.5f };
				glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices4);
				glEnableVertexAttribArray(program.positionAttribute);

				float texCoords4[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
				glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords4);
				glEnableVertexAttribArray(program.texCoordAttribute);
				glUseProgram(program.programID);

				glBindTexture(GL_TEXTURE_2D, chest);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(program.positionAttribute);
				glDisableVertexAttribArray(program.texCoordAttribute);


				if (ball_x >= 1.5f && ball_x <= 3.5f && ball_y <= 1.5f && ball_y >= -1.5f)
				{
					//winGame = true;
					winLevelOne = true;
					//gameDone = true;
					state = STATE_LEVEL_TWO;
				}

				//float x = 4.0;
				//float y = 2.0;

				
				/*if (bob.isAlive() == true)
				{
					bob.draw(block, program, modelMatrix);
				}
				if (bob.collidesWith(ball_x, ball_y) == true && bob.isAlive() == true)
				{
					velocityX *= -1;
					bob.dies();
				}
				*/
				for (size_t i = 0; i < blocks.size(); i++)
				{
					if (blocks[i]->isAlive() == true)
					{
						blocks[i]->draw(block, program, modelMatrix);
					}
				}
				for (size_t i = 0; i < blocks.size(); i++)
				{
					if (blocks[i]->collidesWith(ball_x, ball_y) == true && blocks[i]->isAlive() == true)
					{
						velocityX *= -1;
						blocks[i]->dies();
					}
				}
				
				break;
			}
			case STATE_LEVEL_TWO:
			{
				if (winLevelOne == true)
				{
					modelMatrix.identity();
					modelMatrix.Translate(-5.50f, 3.50f, 1.0f);
					modelMatrix.Scale(.65, .65, 1);
					program.setModelMatrix(modelMatrix);
					DrawText(fontTex, "Level Two", 2.0f, -0.09f, program);

					modelMatrix.identity();
					modelMatrix.Translate(-10.50f, 1.50f, 1.0f);
					modelMatrix.Scale(.65, .65, 1);
					program.setModelMatrix(modelMatrix);
					DrawText(fontTex, "Under Construction", 2.0f, -0.001f, program);
				}
				
			}
			case STATE_LEVEL_THREE:
			{

			}
			case STATE_FINAL:
			{
				if (winGame == true && gameDone == true)
				{
					glClearColor(0.1f, 0.5f, 0.2f, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT);
					modelMatrix.identity();
					modelMatrix.Translate(-7.0f, 3.0f, 1.0f);
					program.setModelMatrix(modelMatrix);
					DrawText(fontTex, "You Win", 2.5f, 0.001f, program);

					modelMatrix.identity();
					modelMatrix.Translate(-12.0f, 0.0f, 1.0f);
					program.setModelMatrix(modelMatrix);
					DrawText(fontTex, "Press ESC To Exit", 1.5f, 0.001f, program);
				}
				else if (winGame == false && gameDone == true)
				{
					glClearColor(0.7f, 0.0f, 0.0f, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT);
					modelMatrix.identity();
					modelMatrix.Translate(-10.0f, 3.0f, 1.0f);
					program.setModelMatrix(modelMatrix);
					DrawText(fontTex, "GAME OVER", 2.5f, 0.001f, program);

					modelMatrix.identity();
					modelMatrix.Translate(-12.0f, 0.0f, 1.0f);
					program.setModelMatrix(modelMatrix);
					DrawText(fontTex, "Press ESC To Exit", 1.5f, 0.001f, program);
				}
				
				break;
			}
		}

				SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
