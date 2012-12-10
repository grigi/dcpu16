#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <SDL/SDL.h>

#define SDLHZ 30

SDL_Surface *screen;

void InnerLoop(void) {
    SDL_Event event;
	
	while ( SDL_PollEvent(&event) ) {
		//printf("%d", event.type);
		switch (event.type) {
			case SDL_KEYDOWN:
				printf("The %s key was pressed!\n", SDL_GetKeyName(event.key.keysym.sym));
				break;
			case SDL_QUIT:
				printf("SDL_QUIT event");
				exit(0);
		}
	}
}

int main(int argc, char **argv)
{
	// Initialize SDL & set cleanup
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);
	

	// Configure display
    screen = SDL_SetVideoMode(640, 480, 16, SDL_SWSURFACE);
    if ( screen == NULL ) {
		fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
        exit(1);
    }

	// Set-up Inner Loop attached to a timer, to run at SDLHZ frequency
	struct itimerval it_val;
	if (signal(SIGALRM, (void (*)(int)) InnerLoop) == SIG_ERR) {
		perror("Unable to catch SIGALRM");
		exit(1);
	}
	it_val.it_value.tv_sec = 0;
	it_val.it_value.tv_usec =  1000000 / SDLHZ;	
	it_val.it_interval = it_val.it_value;
	if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
		perror("error calling setitimer()");
		exit(1);
	}
	// Handle signals
	while (1) pause();
}

