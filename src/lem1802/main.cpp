#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <SDL/SDL.h>
#include "font.h"

#define SDLHZ 30

SDL_Surface *screen;
SDL_sem *loopsem = NULL;

void DrawPixel(SDL_Surface *screen, int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
    Uint32 color = SDL_MapRGB(screen->format, R, G, B);
	x*=4;
	y*=4;

	Uint32 *bufp;

	for (int u=0;u<3;++u)
		for (int v=0;v<3;++v) {
			bufp = (Uint32 *)screen->pixels + (y+v)*screen->pitch/4 + (x+u);
			*bufp = color;
		}
}

void PutChar(char c, int x, int y) {
	int row = int(c/32)*8;
	int col = (c%32)*4;
	x *= 4;
	y *= 8;
	
	for (int v=0;v<8;++v)
		for (int u=0;u<4;++u) {
			int c = font[col+u + ((row+v)*128)]*255;
			DrawPixel(screen, x+u, y+v, c,c,c);
		}
}

void UpdateScreen(void) {
	PutChar('H',2,2);
	PutChar('e',3,2);
	PutChar('l',4,2);
	PutChar('l',5,2);
	PutChar('o',6,2);
	PutChar(' ',7,2);
	PutChar('W',8,2);
	PutChar('o',9,2);
	PutChar('r',10,2);
	PutChar('l',11,2);
	PutChar('d',12,2);
	PutChar('!',13,2);
	SDL_UpdateRect(screen, 0,0,512,384);
}

void InnerLoop(void) {
	SDL_Event event;

	if (SDL_SemTryWait(loopsem) == 0) {
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
			case SDL_KEYDOWN:
				printf("The %s key was pressed!\n", SDL_GetKeyName(event.key.keysym.sym));
				break;
			case SDL_KEYUP:
				printf("The %s key was released!\n", SDL_GetKeyName(event.key.keysym.sym));
				break;
			case SDL_QUIT:
				printf("SDL_QUIT event\n");
				exit(0);
			default:
				printf("Event: %d\n", event.type);
			}
		}
		UpdateScreen();
		SDL_SemPost(loopsem);
	}
}

void onexit() {
	if (loopsem) SDL_DestroySemaphore(loopsem);
	SDL_Quit();
}

int main(int argc, char **argv) {
	// Initialize SDL & set cleanup
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(onexit);


	// Configure display
	screen = SDL_SetVideoMode(512, 384, 32, SDL_SWSURFACE);
	if ( screen == NULL ) {
		fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
		exit(1);
	}

	// Set-up Inner Loop attached to a timer, to run at SDLHZ frequency
	loopsem = SDL_CreateSemaphore(1);
	struct itimerval it_val;
	if (signal(SIGALRM, (void (*)(int)) InnerLoop) == SIG_ERR) {
		perror("Unable to catch SIGALRM");
		exit(1);
	}
	it_val.it_value.tv_sec = 0;
	it_val.it_value.tv_usec = 1000000 / SDLHZ;
	it_val.it_interval = it_val.it_value;
	if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
		perror("error calling setitimer()");
		exit(1);
	}
	// Handle signals
	while (1) pause();
}
