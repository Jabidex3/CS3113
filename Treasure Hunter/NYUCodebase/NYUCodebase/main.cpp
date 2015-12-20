/*
	Jabid Methun
	Treasure Hunter
*/

/*
	Gameplay:
	- 2 player local multiplayer
	- Player One (Top Paddle) uses Left & Right arrow keys to move
	- Player Two (Bottom Paddle) uses A & D keys to move
	- Exit Game by pressing ESC at anytime during or after the game
	- Press Space to deploy ball

	Objective of the game is to destroy obstacles(bricks) in order to get to the treasure!
*/

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
		v.push_back(new Entity(x, y, 1.5, 2.0));
		x += 3.0f;
	}
	x = -6.0f;
	y = -3.5f;
	for (int i = 0; i < 5; i++)
	{
		v.push_back(new Entity(x, y, 1.5, 2.0));
		x += 3.0f;
	}
	x = -6.0f;
	y = 4.0f;
	for (int i = 0; i < 3; i++)
	{
		v.push_back(new Entity(x, y, 1.5, 2.0));
		y -= 2.5f;
	}

	x = 9.0f;
	y = 4.0f;
	for (int i = 0; i < 4; i++)
	{
		v.push_back(new Entity(x, y, 1.5, 2.0));
		y -= 2.5f;
	}
}

void renderLevelTwo(std::vector<Entity*>& v)
{
	float x = -8.0f;
	float y = 4.0f;
	for (int i = 0; i <9 ; i++)
	{
		v.push_back(new Entity(x, y, .75, 1.5));
		x += 2.0f;
	}

	x = -8.0f;
	y = 3.0f;
	for (int i = 0; i <9; i++)
	{
		v.push_back(new Entity(x, y, .75, 1.5));
		x += 2.0f;
	}

	x = -8.0f;
	y = 2.0f;
	for (int i = 0; i <5; i++)
	{
		v.push_back(new Entity(x, y, .75, 1.5));
		y -= 1.0f;
	}
	x = -6.0f;
	y = 2.0f;
	for (int i = 0; i <5; i++)
	{
		v.push_back(new Entity(x, y, .75, 1.5));
		y -= 1.0f;
	}

	x = -4.0f;
	y = 2.0f;
	for (int i = 0; i <5; i++)
	{
		v.push_back(new Entity(x, y, .75, 1.5));
		y -= 1.0f;
	}

	x = 4.0f;
	y = 2.0f;
	for (int i = 0; i <5; i++)
	{
		v.push_back(new Entity(x, y, .75, 1.5));
		y -= 1.0f;
	}

	x = 6.0f;
	y = 2.0f;
	for (int i = 0; i <5; i++)
	{
		v.push_back(new Entity(x, y, .75, 1.5));
		y -= 1.0f;
	}

	x = 8.0f;
	y = 2.0f;
	for (int i = 0; i <5; i++)
	{
		v.push_back(new Entity(x, y, .75, 1.5));
		y -= 1.0f;
	}

	x = -2.0f;
	y = -1.0f;
	for (int i = 0; i <3; i++)
	{
		v.push_back(new Entity(x, y, .75, 1.5));
		x += 2.0f;
	}

	x = -2.0f;
	y = -2.0f;
	for (int i = 0; i <3; i++)
	{
		v.push_back(new Entity(x, y, .75, 1.5));
		x += 2.0f;
	}
}

void renderLevelThree(std::vector<Entity*>& v)
{
	float x = -2.0f;
	float y = 1.0f;
	for (int i = 0; i <2; i++)
	{
		v.push_back(new Entity(x, y, .5, 1.0));
		x += 4.0f;
	}

	x = -2.0f;
	y = -1.0f;
	for (int i = 0; i <2; i++)
	{
		v.push_back(new Entity(x, y, .5, 1.0));
		x += 4.0f;
	}

	x = -4.0f;
	y = -2.0f;
	for (int i = 0; i <3; i++)
	{
		v.push_back(new Entity(x, y, .5, 1.0));
		x += 4.0f;
	}
	x = -4.0f;
	y = 2.0f;
	for (int i = 0; i <3; i++)
	{
		v.push_back(new Entity(x, y, .5, 1.0));
		x += 4.0f;
	}
	x = -6.0f;
	y = 3.0f;
	for (int i = 0; i <4; i++)
	{
		v.push_back(new Entity(x, y, .5, 1.0));
		x += 4.0f;
	}
	x = -6.0f;
	y = -3.0f;
	for (int i = 0; i <4; i++)
	{
		v.push_back(new Entity(x, y, .5, 1.0));
		x += 4.0f;
	}
}

void renderBg(GLuint textureID, ShaderProgram& program, Matrix& modelMatrix)
{
	float bg[] = { -16.0, -9.0, 16.0, 9.0, -16.0, 9.0, 16.0, 9.0, -16.0, -9.0, 16.0, -9.0 };
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, bg);
	glEnableVertexAttribArray(program.positionAttribute);

	float bgTex[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, bgTex);
	glEnableVertexAttribArray(program.texCoordAttribute);
	glUseProgram(program.programID);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);
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
		GLuint open = LoadTexture("openChest.png");
		GLuint dig = LoadTexture("shovel.png");
		GLuint rip = LoadTexture("grave.png");
		GLuint tri = LoadTexture("b1.png");
		GLuint tri2 = LoadTexture("b2.png");
		GLuint bg1 = LoadTexture("bg5.png");
		GLuint bg2 = LoadTexture("bg3.png");
		GLuint bg3 = LoadTexture("bg2.png");
		GLuint bg4 = LoadTexture("bg4.png");
		Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
		Mix_Music* music = Mix_LoadMUS("gameMusic.mp3");
		Mix_Music* music2 = Mix_LoadMUS("game2.mp3");
		Mix_Music* music3 = Mix_LoadMUS("gameOver.mp3");
		Mix_Music* music4 = Mix_LoadMUS("win.mp3");

		Mix_Chunk* paddleHit;
		paddleHit = Mix_LoadWAV("hitPaddle.wav");
		
		Mix_Chunk* boom = Mix_LoadWAV("boom.wav");
		Mix_Chunk* blockHit = Mix_LoadWAV("blockHit.wav");

		Mix_PlayMusic(music, -1);
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
		bool start =false;
		bool winGame;
		bool winLevelOne;
		bool winLevelTwo;
		bool gameDone;
		bool reset = false;
		bool reset2 = false;
		float ball_x = -.50f;
		float ball_y = -6.0f;
		float velocityX = -0.005f;
		float velocityY = -0.005f;
		float angle = 0.0f;
		std::vector<Entity*> blocks;
		std::vector<Entity*> blocks2;
		std::vector<Entity*> blocks3;
		float screenShakeValue= 0.0;
		float screenShakeSpeed =5.0f;
		float screenShakeIntensity = .070f;
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
		angle += elapsed;
		screenShakeValue += elapsed;
		
		const Uint8* keys = SDL_GetKeyboardState(NULL);
		switch (state)
		{
			case STATE_TITLE_SCREEN:
			{
				  if (keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_RETURN2])
				  {
					   state = STATE_LEVEL_ONE;
					   Mix_PlayMusic(music2, -1);
					   renderLevelOne(blocks);
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
						   positionX_p1 += elapsed * 6;
						   playerOneX1 += elapsed * 6;
						   playerOneX2 += elapsed * 6;
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
						   positionX_p1 += -elapsed * 6;
						   playerOneX1 += -elapsed * 6;
						   playerOneX2 += -elapsed * 6;
					   }
				  }

			//Player 2 Controls A and D Keys)
				  if (keys[SDL_SCANCODE_D])
				  {
					   if (positionX_p2 > 13.5f)
					   {
						   positionX_p2 += 0;

					   }
					   else
					   {
						   positionX_p2 += elapsed * 6;
						   playerTwoX1 += elapsed * 6;
						   playerTwoX2 += elapsed * 6;
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
						  positionX_p2 += -elapsed * 6;
						  playerTwoX1 += -elapsed * 6;
						  playerTwoX2 += -elapsed * 6;
					  }
				  }

				  //skip level
				/*  if (keys[SDL_SCANCODE_LCTRL])
				  {
					 winLevelOne = true;
					 state = STATE_LEVEL_TWO;
				  }*/

				 if (keys[SDL_SCANCODE_SPACE])
				 {
					 start = true;
				 }
				 if (keys[SDL_SCANCODE_ESCAPE])
				 {
					 done = true;
				 }
				  break;
				}
			case STATE_LEVEL_TWO:
			{
				//Player 1 Controls (Left and Right Keys)
				if (keys[SDL_SCANCODE_RIGHT])
				{
					if (positionX_p1 > 14.0f)
					{
						positionX_p1 += 0;
					}
					else
					{
						positionX_p1 += elapsed * 9;
						playerOneX1 += elapsed * 9;
						playerOneX2 += elapsed * 9;
					}
				}
				else if (keys[SDL_SCANCODE_LEFT])
				{
					if (positionX_p1 < -14.0f)
					{
						positionX_p1 += 0;
					}
					else
					{
						positionX_p1 += -elapsed * 9;
						playerOneX1 += -elapsed * 9;
						playerOneX2 += -elapsed * 9;
					}
				}

				//Player 2 Controls (A and D Keys)
				if (keys[SDL_SCANCODE_D])
				{
					if (positionX_p2 > 14.0f)
					{
						positionX_p2 += 0;

					}
					else
					{
						positionX_p2 += elapsed * 9;
						playerTwoX1 += elapsed * 9;
						playerTwoX2 += elapsed * 9;
					}

				}
				else if (keys[SDL_SCANCODE_A])
				{
					if (positionX_p2 < -14.0f)
					{
						positionX_p2 += 0;
					}
					else
					{
						positionX_p2 += -elapsed * 9;
						playerTwoX1 += -elapsed * 9;
						playerTwoX2 += -elapsed * 9;
					}
				}

				if (keys[SDL_SCANCODE_SPACE])
				{
					start = true;
				}

				//skip level
			/*	if (keys[SDL_SCANCODE_TAB])
				{
					winLevelTwo = true;
					state = STATE_LEVEL_THREE;
				}*/

				if (keys[SDL_SCANCODE_ESCAPE])
				{
					done = true;
				}
				break;
			}
			case STATE_LEVEL_THREE:
			{
				 // Player 1 Controls(Left and Right Keys)
				 if (keys[SDL_SCANCODE_RIGHT])
				 {
					  if (positionX_p1 > 14.5f)
					  {
						  positionX_p1 += 0;
					  }
					  else
					  {
						  positionX_p1 += elapsed * 12;
						  playerOneX1 += elapsed * 12;
						  playerOneX2 += elapsed * 12;
					  }
				 }
				 else if (keys[SDL_SCANCODE_LEFT])
				 {
					  if (positionX_p1 < -14.5f)
					  {
						  positionX_p1 += 0;
					  }
					  else
					  {
						  positionX_p1 += -elapsed * 12;
						  playerOneX1 += -elapsed * 12;
						  playerOneX2 += -elapsed * 12;
					  }
				 }

				 //Player 2 Controls (A and D Keys)
				 if (keys[SDL_SCANCODE_D])
				 {
					  if (positionX_p2 > 14.5f)
					  {
						  positionX_p2 += 0;

					  }
					  else
					  {
						  positionX_p2 += elapsed * 12;
						  playerTwoX1 += elapsed * 12;
						  playerTwoX2 += elapsed * 12;
					  }

				 }
				 else if (keys[SDL_SCANCODE_A])
				 {
					  if (positionX_p2 < -14.5f)
					  {
						  positionX_p2 += 0;
					  }
					  else
					  {
						  positionX_p2 += -elapsed * 12;
						  playerTwoX1 += -elapsed * 12;
						  playerTwoX2 += -elapsed * 12;
					  }
				 }

				 if (keys[SDL_SCANCODE_SPACE])
				 {
					  start = true;
				 }

				 if (keys[SDL_SCANCODE_ESCAPE])
				 {
					 done = true;
				 }

				break;
			}
			case STATE_FINAL:
			{
				if (keys[SDL_SCANCODE_ESCAPE])
				{
					done = true;
				}

				if (keys[SDL_SCANCODE_SPACE])
				{
					Mix_PlayMusic(music, -1);
					state = STATE_TITLE_SCREEN;
					 positionX_p1 = 0.0f;
					 positionX_p2 = 0.0f;

					 playerOneX1 = 3.0f;
					 playerOneX2 = -3.0f;
					 playerOneY1 = 8.0f;
					 playerOneY2 = 8.9f;

					 playerTwoX1 = 3.0f;
					 playerTwoX2 = -3.0f;
					 playerTwoY1 = -8.0f;
					 playerTwoY2 = -8.9f;
					 start = false;
					 winGame = false;
					 winLevelOne = false;
					 winLevelTwo = false;
					 gameDone = false;
					 reset = false;
					 reset2 = false;
					 ball_x = -.50f;
					 ball_y = -6.0f;
					 velocityX = -0.005f;
					 velocityY = -0.005f;
					 angle = 0.0f;
					 blocks.clear();
					 blocks2.clear();
					 blocks3.clear();
					 //screenShakeValue = 0.0;
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
				if (screenShakeValue < 6.1f)
				{
				  viewMatrix.Translate(0.0f, sin(screenShakeValue * screenShakeSpeed)* screenShakeIntensity, 0.0f);
				}
				else
					viewMatrix.identity();
				
				//glClearColor(0.839216, 0.739216, 0.639216, 1.0f);
				//glClear(GL_COLOR_BUFFER_BIT);
				modelMatrix.identity();
				program.setModelMatrix(modelMatrix);
				float bg[] = { -16.0, -9.0, 16.0, 9.0, -16.0, 9.0, 16.0, 9.0, -16.0, -9.0, 16.0, -9.0 };
				glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, bg);
				glEnableVertexAttribArray(program.positionAttribute);

				float bgTex[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
				glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, bgTex);
				glEnableVertexAttribArray(program.texCoordAttribute);
				glUseProgram(program.programID);

				glBindTexture(GL_TEXTURE_2D, bg1);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(program.positionAttribute);
				glDisableVertexAttribArray(program.texCoordAttribute);

				modelMatrix.identity();
				modelMatrix.Translate(-12.0f, 5.0f, 1.0f);
				program.setModelMatrix(modelMatrix);
				DrawText(fontTex, "Treasure Hunter", 1.9f, -0.195f, program);
				modelMatrix.identity();
				modelMatrix.Translate(-11.0f, 4.0f, 1.0f);
				program.setModelMatrix(modelMatrix);
				DrawText(fontTex, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~", .6f, -0.09f, program);
				

				modelMatrix.identity();
				modelMatrix.Translate(-8.30f, -5.50f, 1.0f);
				modelMatrix.Scale(.65, .65, 1);
				program.setModelMatrix(modelMatrix);
				DrawText(fontTex, "Press Enter To Start", 1.6f, -0.29f, program);

				modelMatrix.identity();
				modelMatrix.Translate(-5.8f, -7.00f, 1.0f);
				modelMatrix.Scale(.65, .65, 1);
				program.setModelMatrix(modelMatrix);
				DrawText(fontTex, "Press ESC To Exit", 1.2f, -0.11f, program);

				modelMatrix.identity();
				modelMatrix.Translate(0.0, -1.00f, 1.0f);
				modelMatrix.Rotate(angle);
				program.setModelMatrix(modelMatrix);
				float vertices[] = { -3.5f, -3.5f, 3.5f, 3.5f, -3.5f, 3.5f, 3.5f, 3.5f, -3.5f, -3.5f, 3.5f, -3.5f };
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

				modelMatrix.identity();
				modelMatrix.Translate(-7.0, -1.00f, 1.0f);
				program.setModelMatrix(modelMatrix);
				float vertices2[] = { -3.0f, -3.0f, 3.0f, 3.0f, -3.0f, 3.0f, 3.0f, 3.0f, -3.0f, -3.0f, 3.0f, -3.0f };
				glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
				glEnableVertexAttribArray(program.positionAttribute);

				float texCoords2[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
				glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
				glEnableVertexAttribArray(program.texCoordAttribute);
				glUseProgram(program.programID);

				glBindTexture(GL_TEXTURE_2D, dig);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(program.positionAttribute);
				glDisableVertexAttribArray(program.texCoordAttribute);
				
				modelMatrix.identity();
				modelMatrix.Translate(7.0, -1.00f, 1.0f);
				modelMatrix.Rotate(1.5f);
				program.setModelMatrix(modelMatrix);
				float vertices3[] = { -3.0f, -3.0f, 3.0f, 3.0f, -3.0f, 3.0f, 3.0f, 3.0f, -3.0f, -3.0f, 3.0f, -3.0f };
				glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
				glEnableVertexAttribArray(program.positionAttribute);

				float texCoords3[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
				glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords3);
				glEnableVertexAttribArray(program.texCoordAttribute);
				glUseProgram(program.programID);

				glBindTexture(GL_TEXTURE_2D, dig);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(program.positionAttribute);
				glDisableVertexAttribArray(program.texCoordAttribute);

				
			
				break;
			}
			case STATE_LEVEL_ONE:
			{
				viewMatrix.identity();
				modelMatrix.identity();
				program.setModelMatrix(modelMatrix);
				renderBg(bg2, program, modelMatrix);

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
					Mix_PlayChannel(-1, paddleHit, 0);

				}

				if (ball_x <= playerTwoX1 && ball_x >= playerTwoX2 && ball_y <= playerTwoY1 && ball_y >= playerTwoY2)
				{
					ball_y += .250f;
					velocityY *= -1.0f;
					Mix_PlayChannel(-1, paddleHit, 0);
				}

				if (ball_x >= 15.4f || ball_x <= -15.4f)
				{
					velocityX *= -1.0f;
					Mix_PlayChannel(-1, paddleHit, 0);
				}
				
				if (ball_y >= 9.0f || ball_y <= -9.0f)
				{
					winGame = false;
					gameDone = true;
					Mix_PlayMusic(music3, 1);
					state = STATE_FINAL;
					
				}

				if (start == false)
				{
					modelMatrix.identity();
					modelMatrix.Translate(-12.0f, 7.0f, 1.0f);
					modelMatrix.Scale(.65, .65, 1);
					program.setModelMatrix(modelMatrix);
					DrawText(fontTex, "Press Space To Deploy Ball", 1.6f, -0.09f, program);
				}
				if (start == true)
				{
					ball_x += velocityX;
					ball_y += velocityY;
				}
					

				modelMatrix.identity();
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
				modelMatrix.Translate(1.0f, 0.0f, 0.0f);
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


				if (ball_x >= 0.5f && ball_x <= 2.5f && ball_y <= 1.5f && ball_y >= -1.5f)
				{
					winLevelOne = true;
					Mix_PlayChannel(-1, boom, 0);
					state = STATE_LEVEL_TWO;
				}

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
						Mix_PlayChannel(-1, blockHit, 0);
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
					if (reset == false)
					{
						positionX_p1 = 0.0f;
						positionX_p2 = 0.0f;
						playerOneX1 = 2.25f;
						playerOneX2 = -2.25f;
						playerTwoX1 = 2.25f;
						playerTwoX2 = -2.25f;
						start = false;
						ball_x = -1.0f;
						ball_y = 6.0f;
						velocityX = 0.008f;
						velocityY = 0.005f;
						renderLevelTwo(blocks2);
						reset = true;
					}
					modelMatrix.identity();
					program.setModelMatrix(modelMatrix);
					renderBg(bg2, program, modelMatrix);

					modelMatrix.identity();
					modelMatrix.Translate(positionX_p1, 0.0f, 0.0f);

					program.setModelMatrix(modelMatrix);
					program.setProjectionMatrix(projectionMatrix);
					program.setViewMatrix(viewMatrix);

					float player1[] = { -2.25f, 8.0f, 2.25f, 8.9f, -2.25f, 8.9f, 2.25f, 8.9f, -2.25f, 8.0f, 2.25f, 8.0f };
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

					float player2[] = { -2.25f, -8.0f, 2.25f, -8.9f, -2.25f, -8.9f, 2.25f, -8.9f, -2.25f, -8.0f, 2.25f, -8.0f };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, player2);
					glEnableVertexAttribArray(program.positionAttribute);

					float texCoords2[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
					glEnableVertexAttribArray(program.texCoordAttribute);

					glBindTexture(GL_TEXTURE_2D, pla2);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);

					if (start == true)
					{
						ball_x += velocityX;
						ball_y += velocityY;
					}
				
					modelMatrix.identity();
					modelMatrix.Translate(ball_x, ball_y, 0.0f);
					program.setModelMatrix(modelMatrix);

					float dot[] = { -.4f, -.4f, .4f, .4f, -.4f, .4f, .4f, .4f, -.4f, -.4f, .4f, -.4f };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, dot);
					glEnableVertexAttribArray(program.positionAttribute);

					float texCoords3[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords3);
					glEnableVertexAttribArray(program.texCoordAttribute);

					glBindTexture(GL_TEXTURE_2D, ball);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);

					if (ball_x <= playerOneX1 && ball_x >= playerOneX2 && ball_y >= playerOneY1 && ball_y <= playerOneY2)
					{
						ball_y -= .250f;
						velocityY *= -1.0f;
						Mix_PlayChannel(-1, paddleHit, 0);

					}

					if (ball_x <= playerTwoX1 && ball_x >= playerTwoX2 && ball_y <= playerTwoY1 && ball_y >= playerTwoY2)
					{
						ball_y += .250f;
						velocityY *= -1.0f;
						Mix_PlayChannel(-1, paddleHit, 0);
					}

					if (ball_x >= 15.4f || ball_x <= -15.4f)
					{
						velocityX *= -1.0f;
						Mix_PlayChannel(-1, paddleHit, 0);
					}

					if (ball_y >= 9.0f || ball_y <= -9.0f)
					{
						winGame = false;
						gameDone = true;
						Mix_PlayMusic(music3, 1);
						state = STATE_FINAL;

					}

					for (size_t i = 0; i < blocks2.size(); i++)
					{
						if (blocks2[i]->isAlive() == true)
						{
							blocks2[i]->draw(block, program, modelMatrix);
						}
					}
					for (size_t i = 0; i < blocks2.size(); i++)
					{
						if (blocks2[i]->collidesWith(ball_x, ball_y) == true && blocks2[i]->isAlive() == true)
						{
							Mix_PlayChannel(-1, blockHit, 0);
							velocityX *= -1;
							blocks2[i]->dies();
						}
					}

					modelMatrix.identity();
					modelMatrix.Translate(0.0f, 1.0f, 0.0f);
					program.setModelMatrix(modelMatrix);
					float vertices4[] = { -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f };
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

					if (ball_x >= -1.0f && ball_x <= 1.0f && ball_y <= 2.0f && ball_y >= 0.0f)
					{
						Mix_PlayChannel(-1, boom, 0);
						winLevelTwo = true;
						state = STATE_LEVEL_THREE;
					}
				}
				break;
			}
			case STATE_LEVEL_THREE:
			{
				if (winLevelTwo == true)
				{
					if (reset2 == false)
					{
						positionX_p1 = 0.0f;
						positionX_p2 = 0.0f;
						playerOneX1 = 1.5f;
						playerOneX2 = -1.5f;
						playerTwoX1 = 1.5f;
						playerTwoX2 = -1.5f;
						start = false;
						ball_x = 1.0f;
						ball_y = -6.0f;
						velocityX = -0.0005f*12 ;
						velocityY = -0.0005f*12 ;
						renderLevelThree(blocks3);
						reset2 = true;
					}

					modelMatrix.identity();
					program.setModelMatrix(modelMatrix);
					renderBg(bg2, program, modelMatrix);

					modelMatrix.identity();
					modelMatrix.Translate(positionX_p1, 0.0f, 0.0f);

					program.setModelMatrix(modelMatrix);
					program.setProjectionMatrix(projectionMatrix);
					program.setViewMatrix(viewMatrix);

					float player1[] = { -1.5f, 8.0f, 1.5f, 8.9f, -1.5f, 8.9f, 1.5f, 8.9f, -1.5f, 8.0f, 1.5f, 8.0f };
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

					float player2[] = { -1.5f, -8.0f, 1.5f, -8.9f, -1.5f, -8.9f, 1.5f, -8.9f, -1.5f, -8.0f, 1.5f, -8.0f };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, player2);
					glEnableVertexAttribArray(program.positionAttribute);

					float texCoords2[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
					glEnableVertexAttribArray(program.texCoordAttribute);

					glBindTexture(GL_TEXTURE_2D, pla2);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);

					modelMatrix.identity();
					program.setModelMatrix(modelMatrix);
					float vertices4[] = { -.65f, -.65f, .65f, .65f, -.65f, .65f, .65f, .65f, -.65f, -.65f, .65f, -.65f };
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

					if (ball_x >= -.65f && ball_x <= .65f && ball_y <= .65f && ball_y >= -.65f)
					{
						Mix_PlayChannel(-1, boom, 0);
						winGame = true;
						gameDone = true;
						Mix_PlayMusic(music4, -1);
						state = STATE_FINAL;
					}

					for (size_t i = 0; i < blocks3.size(); i++)
					{
						if (blocks3[i]->isAlive() == true)
						{
							blocks3[i]->draw(block, program, modelMatrix);
						}
					}
					for (size_t i = 0; i < blocks3.size(); i++)
					{
						if (blocks3[i]->collidesWith(ball_x, ball_y) == true && blocks3[i]->isAlive() == true)
						{
							Mix_PlayChannel(-1, blockHit, 0);
							velocityX *= -1;
							blocks3[i]->dies();
						}
					}

					if (start == true)
					{
						ball_x += velocityX;
						ball_y += velocityY;
					}

					modelMatrix.identity();
					modelMatrix.Translate(ball_x, ball_y, 0.0f);
					program.setModelMatrix(modelMatrix);

					float dot[] = { -.3f, -.3f, .3f, .3f, -.3f, .3f, .3f, .3f, -.3f, -.3f, .3f, -.3f };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, dot);
					glEnableVertexAttribArray(program.positionAttribute);

					float texCoords3[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords3);
					glEnableVertexAttribArray(program.texCoordAttribute);

					glBindTexture(GL_TEXTURE_2D, ball);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);

					if (ball_x <= playerOneX1 && ball_x >= playerOneX2 && ball_y >= playerOneY1 && ball_y <= playerOneY2)
					{
						ball_y -= .150f;
						velocityY *= -1.0f;
						Mix_PlayChannel(-1, paddleHit, 0);
					}

					if (ball_x <= playerTwoX1 && ball_x >= playerTwoX2 && ball_y <= playerTwoY1 && ball_y >= playerTwoY2)
					{
						ball_y += .150f;
						velocityY *= -1.0f;
						Mix_PlayChannel(-1, paddleHit, 0);
					}

					if (ball_x >= 15.4f || ball_x <= -15.4f)
					{
						velocityX *= -1.0f;
						Mix_PlayChannel(-1, paddleHit, 0);
					}

					if (ball_y >= 9.0f || ball_y <= -9.0f)
					{
						winGame = false;
						gameDone = true;
						Mix_PlayMusic(music3, 1);
						state = STATE_FINAL;

					}

					//Used for testing
					/*if (ball_y >= 9.0f || ball_y <= -9.0f)
					{
						velocityY *= -1.0f;

					}*/
				}
				break;
			}
			case STATE_FINAL:
			{
				if (winGame == true && gameDone == true)
				{
					glClearColor(0.1f, 0.5f, 0.2f, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT);

					modelMatrix.identity();
					program.setModelMatrix(modelMatrix);
					float bg[] = { -16.0, -9.0, 16.0, 9.0, -16.0, 9.0, 16.0, 9.0, -16.0, -9.0, 16.0, -9.0 };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, bg);
					glEnableVertexAttribArray(program.positionAttribute);

					float bgTex[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, bgTex);
					glEnableVertexAttribArray(program.texCoordAttribute);
					glUseProgram(program.programID);

					glBindTexture(GL_TEXTURE_2D, bg4);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);
					modelMatrix.identity();
					program.setModelMatrix(modelMatrix);
					float border[] = { 14.0, 7.0, 16.0, 9.0, 14.0, 9.0, 16.0, 9.0, 14.0, 7.0, 16.0, 7.0 };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, border);
					glEnableVertexAttribArray(program.positionAttribute);

					float borderTex[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, borderTex);
					glEnableVertexAttribArray(program.texCoordAttribute);
					glUseProgram(program.programID);

					glBindTexture(GL_TEXTURE_2D, tri);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);

					modelMatrix.identity();

					modelMatrix.Translate(-.08, 0.05, 1.0);
					program.setModelMatrix(modelMatrix);
					float border2[] = { -16.0, 7.0, -14.0, 9.0, -16.0, 9.0, -14.0, 9.0, -16.0, 7.0, -14.0, 7.0 };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, border2);
					glEnableVertexAttribArray(program.positionAttribute);

					float borderTex2[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, borderTex2);
					glEnableVertexAttribArray(program.texCoordAttribute);
					glUseProgram(program.programID);

					glBindTexture(GL_TEXTURE_2D, tri2);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);

					modelMatrix.identity();
					modelMatrix.Rotate(3.14);
					program.setModelMatrix(modelMatrix);
					float border3[] = { 14.0, 7.0, 16.0, 9.0, 14.0, 9.0, 16.0, 9.0, 14.0, 7.0, 16.0, 7.0 };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, border3);
					glEnableVertexAttribArray(program.positionAttribute);

					float borderTex3[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, borderTex3);
					glEnableVertexAttribArray(program.texCoordAttribute);
					glUseProgram(program.programID);

					glBindTexture(GL_TEXTURE_2D, tri);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);

					modelMatrix.identity();
					modelMatrix.Rotate(3.14);
					modelMatrix.Translate(-.1, 0.08, 1.0);
					program.setModelMatrix(modelMatrix);
					float border4[] = { -16.0, 7.0, -14.0, 9.0, -16.0, 9.0, -14.0, 9.0, -16.0, 7.0, -14.0, 7.0 };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, border4);
					glEnableVertexAttribArray(program.positionAttribute);

					float borderTex4[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, borderTex4);
					glEnableVertexAttribArray(program.texCoordAttribute);
					glUseProgram(program.programID);

					glBindTexture(GL_TEXTURE_2D, tri2);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);

					modelMatrix.identity();
					modelMatrix.Translate(-7.0f, 5.0f, 1.0f);
					program.setModelMatrix(modelMatrix);
					DrawText(fontTex, "You Win", 2.5f, 0.001f, program);

					modelMatrix.identity();
					modelMatrix.Translate(-12.0f, 3.0f, 1.0f);
					program.setModelMatrix(modelMatrix);
					DrawText(fontTex, "Press ESC To Exit", 1.5f, 0.001f, program);

					modelMatrix.identity();
					modelMatrix.Translate(0.0f, -2.0f, 0.0f);
					modelMatrix.Scale(3.0, 3.0, 1.0);
					program.setModelMatrix(modelMatrix);
					float vertices4[] = { -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices4);
					glEnableVertexAttribArray(program.positionAttribute);

					float texCoords4[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords4);
					glEnableVertexAttribArray(program.texCoordAttribute);
					glUseProgram(program.programID);

					glBindTexture(GL_TEXTURE_2D, open);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);

					modelMatrix.identity();
					modelMatrix.Translate(-8.0f, -7.0f, 1.0f);
					program.setModelMatrix(modelMatrix);
					DrawText(fontTex, "Press Spacebar To Play Again", 1.0f, -.41f, program);
				}
				else if (winGame == false && gameDone == true)
				{
					glClearColor(0.85f, 0.0f, 0.0f, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT);
					
					modelMatrix.identity();
					program.setModelMatrix(modelMatrix);
					float bg[] = { -16.0, -9.0, 16.0, 9.0, -16.0, 9.0, 16.0, 9.0, -16.0, -9.0, 16.0, -9.0 };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, bg);
					glEnableVertexAttribArray(program.positionAttribute);

					float bgTex[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, bgTex);
					glEnableVertexAttribArray(program.texCoordAttribute);
					glUseProgram(program.programID);

					glBindTexture(GL_TEXTURE_2D, bg3);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);

					modelMatrix.identity();
					program.setModelMatrix(modelMatrix);
					float border[] = { 14.0, 7.0, 16.0, 9.0, 14.0, 9.0, 16.0, 9.0, 14.0, 7.0, 16.0, 7.0 };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, border);
					glEnableVertexAttribArray(program.positionAttribute);

					float borderTex[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, borderTex);
					glEnableVertexAttribArray(program.texCoordAttribute);
					glUseProgram(program.programID);

					glBindTexture(GL_TEXTURE_2D, tri);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);

					modelMatrix.identity();

					modelMatrix.Translate(-.08, 0.05, 1.0);
					program.setModelMatrix(modelMatrix);
					float border2[] = { -16.0, 7.0, -14.0, 9.0, -16.0, 9.0, -14.0, 9.0, -16.0, 7.0, -14.0, 7.0 };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, border2);
					glEnableVertexAttribArray(program.positionAttribute);

					float borderTex2[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, borderTex2);
					glEnableVertexAttribArray(program.texCoordAttribute);
					glUseProgram(program.programID);

					glBindTexture(GL_TEXTURE_2D, tri2);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);

					modelMatrix.identity();
					modelMatrix.Rotate(3.14);
					program.setModelMatrix(modelMatrix);
					float border3[] = { 14.0, 7.0, 16.0, 9.0, 14.0, 9.0, 16.0, 9.0, 14.0, 7.0, 16.0, 7.0 };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, border3);
					glEnableVertexAttribArray(program.positionAttribute);

					float borderTex3[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, borderTex3);
					glEnableVertexAttribArray(program.texCoordAttribute);
					glUseProgram(program.programID);

					glBindTexture(GL_TEXTURE_2D, tri);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);

					modelMatrix.identity();
					modelMatrix.Rotate(3.14);
					modelMatrix.Translate(-.1, 0.08, 1.0);
					program.setModelMatrix(modelMatrix);
					float border4[] = { -16.0, 7.0, -14.0, 9.0, -16.0, 9.0, -14.0, 9.0, -16.0, 7.0, -14.0, 7.0 };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, border4);
					glEnableVertexAttribArray(program.positionAttribute);

					float borderTex4[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, borderTex4);
					glEnableVertexAttribArray(program.texCoordAttribute);
					glUseProgram(program.programID);

					glBindTexture(GL_TEXTURE_2D, tri2);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);

					modelMatrix.identity();
					modelMatrix.Translate(-10.0f, 5.0f, 1.0f);
					program.setModelMatrix(modelMatrix);
					DrawText(fontTex, "GAME OVER", 2.5f, 0.001f, program);

					modelMatrix.identity();
					modelMatrix.Translate(-12.0f, 3.0f, 1.0f);
					program.setModelMatrix(modelMatrix);
					DrawText(fontTex, "Press ESC To Exit", 1.5f, 0.001f, program);

					modelMatrix.identity();
					modelMatrix.Scale(3.0, 3.0, 1.0);
					modelMatrix.Translate(0.0, -0.50, 0.0);
					program.setModelMatrix(modelMatrix);
					float vertices4[] = { -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices4);
					glEnableVertexAttribArray(program.positionAttribute);

					float texCoords4[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords4);
					glEnableVertexAttribArray(program.texCoordAttribute);
					glUseProgram(program.programID);

					glBindTexture(GL_TEXTURE_2D,rip);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);

					modelMatrix.identity();
					modelMatrix.Translate(-8.0f, -7.0f, 1.0f);
					program.setModelMatrix(modelMatrix);
					DrawText(fontTex, "Press Spacebar To Play Again", 1.0f, -.41f, program);
				}
				
				break;
			}
		}

				SDL_GL_SwapWindow(displayWindow);
	}
	Mix_FreeChunk(paddleHit);
	Mix_FreeChunk(boom);
	Mix_FreeMusic(music);
	Mix_FreeMusic(music2);
	Mix_FreeMusic(music3);
	Mix_FreeMusic(music4);
	SDL_Quit();
	return 0;
}
