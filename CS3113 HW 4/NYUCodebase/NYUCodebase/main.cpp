#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "Matrix.h"
#include "ShaderProgram.h"
using namespace std;

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

#define TILE_SIZE 0.5f
bool readHeader(std::ifstream& stream, int& mapWidth, int& mapHeight, unsigned char**& levelData)
{
	string line;
	mapWidth = -1;
	mapHeight = -1;
	while(getline(stream, line)) 
	{
		if (line == "") 
		{ 
			break; 
		}
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		
		if (key == "width") 
		{
			mapWidth = atoi(value.c_str());
		}
		else if (key == "height")
		{
			mapHeight = atoi(value.c_str());
		}
	}

	if(mapWidth == -1 || mapHeight == -1) 
	{
		return false;
	}
	else 
	{ // allocate our map data
		levelData = new unsigned char*[mapHeight];
		for(int i = 0; i < mapHeight; ++i) 
		{
			levelData[i] = new unsigned char[mapWidth];
		}
		return true;
	}
}

bool readLayerData(std::ifstream& stream, int& mapWidth, int& mapHeight, unsigned char**& levelData)
{
	string line;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "data") 
		{
			for (int y = 0; y < mapHeight; y++) 
			{
				getline(stream, line);
				istringstream lineStream(line);
				string tile;
				for (int x = 0; x < mapWidth; x++) 
				{
					getline(lineStream, tile, ',');
					unsigned char val = (unsigned char)atoi(tile.c_str());
					if (val > 0) 
					{
						// be careful, the tiles in this format are indexed from 1 not 0
						levelData[y][x] = val - 1;
					}
					else 
					{
						levelData[y][x] = 0;
					}
				}
			}
		}
	}
	return true;
}

bool readEntityData(std::ifstream&stream) 
{
	string line;
	string type;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "type") {
			type = value;
		}
		else if (key == "location") 
		{
			istringstream lineStream(value);
			string xPosition, yPosition;
			getline(lineStream, xPosition, ',');
			getline(lineStream, yPosition, ',');
			float placeX = atoi(xPosition.c_str()) / 16 * TILE_SIZE;
			float placeY = atoi(yPosition.c_str()) / 16 * -TILE_SIZE;
			//placeEntity(type, placeX, placeY);
		}
	}
	return true;
}

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

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("HW 4", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	int	mapHeight = 0;
	int mapWidth = 0;
	unsigned char** levelData = new unsigned char*[1];
	ifstream infile("myMap.txt");
	string line;
	while (getline(infile, line)) 
	{
		if (line == "[header]") 
		{
			if (!readHeader(infile, mapWidth, mapHeight, levelData))
			{
				return NULL;
			}
		}
		else if (line == "[layer]") 
		{
			readLayerData(infile, mapWidth, mapHeight, levelData);
		}
		/*else if (line == "[Object Layer 1]") 
		{
			readEntityData(infile);
		}*/
	}

	//setup
	ShaderProgram program = ShaderProgram(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;


	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	GLuint map = LoadTexture("spritesheet_rgba.png");

	SDL_Event event;
	bool done = false;
	while (!done) 
	{
		while (SDL_PollEvent(&event)) 
		{
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) 
			{
				done = true;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		vector<float> vertexData;
		vector<float> texCoordData;
		//Spritesheet is 30 X 30
		int SPRITE_COUNT_X = 30;
		int SPRITE_COUNT_Y = 30;
		for (int y = 0; y < mapHeight; y++) 
		{
			for (int x = 0; x < mapWidth; x++) 
			{
				float u = (float)(((int)levelData[y][x]) % SPRITE_COUNT_X) / (float)SPRITE_COUNT_X;
				float v = (float)(((int)levelData[y][x]) / SPRITE_COUNT_X) / (float)SPRITE_COUNT_Y;

				float spriteWidth = 1.0f / (float)SPRITE_COUNT_X;
				float spriteHeight = 1.0f / (float)SPRITE_COUNT_Y;

				vertexData.insert(vertexData.end(), 
				{
					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,

					TILE_SIZE * x, -TILE_SIZE * y,
					(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
				});

				texCoordData.insert(texCoordData.end(), {
					u, v,
					u, v + (spriteHeight),
					u + spriteWidth, v + (spriteHeight),

					u, v,
					u + spriteWidth, v + (spriteHeight),
					u + spriteWidth, v
				});
			}
		}
		//doesnt display tile map 
		glUseProgram(program.programID);
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
		glEnableVertexAttribArray(program.positionAttribute);
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
		glEnableVertexAttribArray(program.texCoordAttribute);
		glUseProgram(program.programID);
		glBindTexture(GL_TEXTURE_2D, map);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
