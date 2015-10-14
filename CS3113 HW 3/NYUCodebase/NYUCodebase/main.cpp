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

class Entity
{
public:
	Entity(float x, float y) : x(x), y(y), height(1.0f), width(1.0f) {}
	//bool collidesWith(Entity *entity);
	float x;
	float y;
	float width;
	float height;

};

void DrawSheetSprite(ShaderProgram program, Matrix modelMatrix, int index, int textureID, float x, float y, float size)
{
	//My spritesheet was 6 images  by 6 images
	int numInRow = 6;
	int numInColumn = 6;
	float width = 1.0 / (float)numInRow;
	float height = 1.0 / (float)numInColumn;
	float u = (float)(((int)index) % numInRow) / (float)numInRow;
	float v = (float)(((int)index) / numInRow) / (float)numInColumn;
	

	GLfloat texCoords[] = {
		u, v + height,
		u + width, v,
		u, v,
		u + width, v,
		u, v + height,
		u + width, v + height,
	};

	modelMatrix.identity();
	modelMatrix.Translate(x, y, 0.0);
	program.setModelMatrix(modelMatrix);

	float aspect = width / height;
	float vertices[] = {
		-0.5f * size * aspect, -0.5f * size,
		0.5f * size * aspect, 0.5f * size,
		-0.5f * size * aspect, 0.5f * size,
		0.5f * size * aspect, 0.5f * size,
		-0.5f * size * aspect, -0.5f * size,
		0.5f * size * aspect, -0.5f * size };

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);

	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);

	
	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);
}

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
		glViewport(0, 0, 640, 360);
		ShaderProgram program = ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

		Matrix projectionMatrix;
		Matrix modelMatrix;
		Matrix viewMatrix;
		GLuint fontTex = LoadTexture("font1.png");
		GLuint home = LoadTexture("si1.png");
		GLuint ship = LoadTexture("ship.png");
		GLuint enemy = LoadTexture("SpaceInvaders.png");
		projectionMatrix.setOrthoProjection(-16.0, 16.0, -9.0, 9.0, -1.0, 1.0);
		
		enum GameState { STATE_TITLE_SCREEN, STATE_GAME, STATE_GAME_OVER };
		GameState state = STATE_TITLE_SCREEN;
		float lastFrameTicks = 0.0f;
		float positionX = 0.0f;

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
				if (keys[SDL_SCANCODE_SPACE])
					state = STATE_GAME;
				break;
			}
			case STATE_GAME:
			{
				 if (keys[SDL_SCANCODE_LEFT])
				 {
					if (positionX < -7.0)
					{
						positionX += 0;
					}
					else
					{
						positionX += -.0005*2;
					}
						
				 }
				 else if (keys[SDL_SCANCODE_RIGHT])
				 {
					if (positionX > 7.0)
					{
						positionX += 0;
					}
					else
					{
						positionX += .0005*2;
					}			
				 }

				 /*
					//For Shooting
				 if (keys[SDL_SCANCODE_SPACE])
				 {
	
				 }
				 */

				 break;
			}
			case STATE_GAME_OVER:
			{
				break;
			}
				
		}
		
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		switch (state)
		{
			case STATE_TITLE_SCREEN:
			{
				modelMatrix.identity();
				modelMatrix.Translate(-11.0f, 3.0f, 1.0f);
				program.setModelMatrix(modelMatrix);
				DrawText(fontTex, "Galaxy Invaders", 1.5f, 0.001f, program);

				modelMatrix.identity();
				modelMatrix.Translate(-10.50f, 1.50f, 1.0f);
				modelMatrix.Scale(.5, .5, 1);
				program.setModelMatrix(modelMatrix);

				DrawText(fontTex, "...Press Space To Begin...", 1.6f, 0.001f, program);

				modelMatrix.identity();
				modelMatrix.Translate(-1.0f, -1.5f, 1.0f);
				modelMatrix.Scale(4.0, 4.0, 1);
				program.setModelMatrix(modelMatrix);
				float vertices[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
				glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
				glEnableVertexAttribArray(program.positionAttribute);

				float texCoords[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
				glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
				glEnableVertexAttribArray(program.texCoordAttribute);
				glUseProgram(program.programID);

				glBindTexture(GL_TEXTURE_2D, home);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(program.positionAttribute);
				glDisableVertexAttribArray(program.texCoordAttribute);

				break;
			}
			case STATE_GAME:
			{
			   glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
			   glClear(GL_COLOR_BUFFER_BIT);
			   modelMatrix.identity();
			   modelMatrix.Translate(0.0f, -7.0f, 0.0f);
			   modelMatrix.Scale(2.0f, 2.0f, 1.0f);
			   modelMatrix.Translate(positionX, 0.0f, 0.0f);
			   program.setModelMatrix(modelMatrix);

			   float vertices[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
			   glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
			   glEnableVertexAttribArray(program.positionAttribute);

			   float texCoords[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
			   glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
			   glEnableVertexAttribArray(program.texCoordAttribute);

			   glBindTexture(GL_TEXTURE_2D, ship);
			   glDrawArrays(GL_TRIANGLES, 0, 6);

			   std::vector<Entity*> enemies;
			   float x = -8.0f;
			   float y = 7.0f;
			   float x_change = 5.0f;
			   float y_change = 3.0f;
			   for (int i = -8; i < 12; i+=5)
			   {
				   for (int j = 7; j>0; j -= 3)
				   {
					   enemies.push_back(new Entity(x, y));
				   }
			   }
			
			   for (int i = 0; i < enemies.size(); i++)
			   {
					   if (i == 0)
						   DrawSheetSprite(program, modelMatrix, 0, enemy, x , y, 1.5f);
					   else if (i == 1)
						   DrawSheetSprite(program, modelMatrix, 0, enemy, x + x_change, y, 1.5f);
					   else if (i == 2)
						   DrawSheetSprite(program, modelMatrix, 0,  enemy, x + 2*x_change, y, 1.5f);
					   else if (i == 3)
						   DrawSheetSprite(program, modelMatrix, 0,  enemy, x + 3*x_change, y, 1.5f);
					   else if (i == 4)
							DrawSheetSprite(program, modelMatrix, 8, enemy, x, y-y_change, 1.5f);
					   else if (i == 5)
						   DrawSheetSprite(program, modelMatrix, 8, enemy, x+ x_change, y - y_change, 1.5f);
					   else if (i == 6)
						   DrawSheetSprite(program, modelMatrix, 8, enemy, x + 2 * x_change, y - y_change, 1.5f);
					   else if (i == 7)
						   DrawSheetSprite(program, modelMatrix, 8, enemy, x + 3 * x_change, y - y_change, 1.5f);
					   else if (i == 8)
							DrawSheetSprite(program, modelMatrix, 15,  enemy, x, y- 2*y_change, 1.5f);
					   else if (i == 9)
						   DrawSheetSprite(program, modelMatrix, 15, enemy, x + x_change , y - 2 * y_change, 1.5f);
					   else if (i == 10)
						   DrawSheetSprite(program, modelMatrix, 15, enemy, x + 2 * x_change, y - 2 * y_change, 1.5f);
					   else if (i == 11)
						   DrawSheetSprite(program, modelMatrix, 15, enemy, x + 3 * x_change, y - 2 * y_change, 1.5f);
			   }

			   //win check. Game ends if u cant kill aliens within 30 seconds
			   if (ticks >= 30)
			   {
				   state = STATE_GAME_OVER;
			   }

			   break;
			}
			case STATE_GAME_OVER:
			{
				  glClear(GL_COLOR_BUFFER_BIT);
				  modelMatrix.identity();
				  modelMatrix.Translate(-10.0f, 3.0f, 1.0f);
				  program.setModelMatrix(modelMatrix);
				  DrawText(fontTex, "GAME OVER", 2.5f, 0.001f, program);

				  modelMatrix.identity();
				  modelMatrix.Translate(-13.50f, 1.50f, 1.0f);
				  modelMatrix.Scale(.5, .5, 1);
				  program.setModelMatrix(modelMatrix);
				  DrawText(fontTex, "The Aliens Have Taken Over The Galaxy", 1.5f, 0.001f, program);

				  break;
			}
				
		}

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
