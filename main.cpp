#include <yarp/os/all.h>
#include <yarp/os/Network.h>
#include <yarp/os/impl/NameConfig.h>
#include <stdio.h>
#include "SDL.h"

yarp::os::BufferedPort<yarp::os::Bottle> port;

SDL_Window* gWindow = NULL;

SDL_Surface* gScreenSurface = NULL;

SDL_Surface* gHelloWorld = NULL;

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

bool init();

bool loadMedia();

void close();

bool init()
{
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) 
	{
		printf("error: sdl did not initialize! SDL_ERROR: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) 
		{
			printf("Window could not be created! SDL_ERROR: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}


	//yarp init

	//the following lines are necessary for iOS; on "desktop" OS's you can set the
	// yarp conf the normal way (i.e. changing the global yarp.conf text file, or run "yarp conf X.X.X.X PORT")
	yarp::os::impl::NameConfig nc;
	nc.setManualConfig("10.0.1.32", 10000);
	port.open("/SDLPort2");
	return success;
}

bool loadMedia()
{
	bool success = true;
	gHelloWorld = SDL_LoadBMP("helloworld.bmp");
	if (gHelloWorld == NULL)
	{
		printf("unable to load img! %s\n", SDL_GetError());
		success = false;
	}
	return success;
}

void close()
{

	port.close();

	SDL_FreeSurface(gHelloWorld);
	gHelloWorld = NULL;

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	SDL_Quit();
}

int main (int argc, char* args[])
{
	yarp::os::Network yarp;
	if (!init()) 
	{
		printf("failed to init!\n");
	}
	else
	{
		if (!loadMedia()) 
		{
			printf("failed to load media!\n");
		}
		else
		{
			SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
		}

		bool quit = false;
		SDL_Event e;


		

		while (!quit) 
		{
			SDL_UpdateWindowSurface(gWindow);
			while (SDL_PollEvent(&e) != 0) 
			{
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}

			}
			if (port.getPendingReads()) {
				yarp::os::Bottle* bot = port.read();
				printf("read bot = %s\n",bot->toString().c_str());
			}
		}
	}

	close();

	return 0;
}