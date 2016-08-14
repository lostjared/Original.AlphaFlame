#include<SDL.h>
#include "mxfont.h"
#include<stdlib.h>
#include<time.h>
#include<SDL_stretch.h>


SDL_Color back_buffer[480][272] = { 0 };
SDL_Surface *front = 0, *video_surface = 0;
struct SDL_Font *font = 0;
int fade_r = 1, fade_g = 0, fade_b = 0;

SDL_Surface *img[4] = { 0 };


SDL_Color *slow_get(SDL_Surface *surf, int x, int y) {
	void *buf = lock(surf, surf->format->BitsPerPixel);
	static SDL_Color c;
	getpixel(surf, x,y,surf->format->BitsPerPixel, surf->pitch, &c);
	unlock(surf);
	return &c;
}


void Init() {

	unsigned int i,z;
//	scePowerSetClockFrequency(333, 333, 166); //# overclocked
	img[0] = SDL_LoadBMP("img1.bmp");
	img[1] = SDL_LoadBMP("img2.bmp");
	img[2] = SDL_LoadBMP("img3.bmp");
	img[3] = SDL_LoadBMP("img4.bmp");

	
	srand((unsigned int)time(0));


	for(i = 0; i < 480; i++)
		for(z = 0; z < 272; z++) {

			back_buffer[i][z].r = slow_get(img[rand()%4], i,z)->r;
			back_buffer[i][z].g = slow_get(img[rand()%4], i,z)->g;
			back_buffer[i][z].b = slow_get(img[rand()%4], i,z)->b;
		}

}

void Blend(SDL_Surface *surf) {
	static float alpha = 1.0f;
	static unsigned int i,z;
	void *buf = lock(surf, surf->format->BitsPerPixel);

	for(i = 0; i < 480; i++)
		for(z = 0; z < 272; z++) {
			// get pixel
			struct SDL_Color col ;
			getpixel(surf, i,z,surf->format->BitsPerPixel, surf->pitch, &col);

			back_buffer[i][z].r = back_buffer[i][z].r + ( alpha * col.r );
			if( back_buffer[i][z].r >= 0xFE ) back_buffer[i][z].r = 255;
			back_buffer[i][z].g = back_buffer[i][z].g + ( alpha * col.g );
			if(back_buffer[i][z].g >= 0xFE) back_buffer[i][z].g = 255;
			back_buffer[i][z].b = back_buffer[i][z].b + ( alpha * col.b );
			if(back_buffer[i][z].b >= 0xFE) back_buffer[i][z].b = 255;
			
		}


	unlock(surf);
	

}

void Morph(SDL_Surface *surf) {
	static unsigned int i,z;
	for(i = 0; i < 439; i++) {
		for(z = 0; z < 271; z++) {
			int col[3];
			col[0] = back_buffer[i][z].r + back_buffer[i+1][z].r + back_buffer[i][z+1].r + back_buffer[i+1][z+1].r;
			col[1] = back_buffer[i][z].g + back_buffer[i+1][z].g + back_buffer[i][z+1].g + back_buffer[i+1][z+1].g;
			col[2] = back_buffer[i][z].b + back_buffer[i+1][z].b + back_buffer[i][z+1].b + back_buffer[i+1][z+1].b;
			col[0] = col[0] / 4;
			col[1] = col[1] / 4;
			col[2] = col[2] / 4;
			if(fade_r == 1) {
				back_buffer[i][z].r = ++col[0];
				back_buffer[i][z].g = --col[1];
				back_buffer[i][z].b = --col[2];
			}
			else if(fade_g == 1) {
				back_buffer[i][z].r = --col[0];
				back_buffer[i][z].g = ++col[1];
				back_buffer[i][z].b = --col[2];
			}
			else if(fade_b == 1) {
				back_buffer[i][z].r = --col[0];
				back_buffer[i][z].g = --col[1];
				back_buffer[i][z].b = ++col[2];
			}
		}
	}

}

void CopyBuffer(SDL_Surface *surf) {
	
	int i,z;
	void *buf;

	buf = lock(surf,surf->format->BitsPerPixel);

	for(i = 0; i < surf->w; i++)
		for(z = 0; z < surf->h; z++)
			setpixel(buf, i,z,SDL_MapRGB(front->format,back_buffer[i][z].r, back_buffer[i][z].g, back_buffer[i][z].b),surf->format->BitsPerPixel, surf->pitch);


	unlock(surf);
}


void JaredBruniSaysAlphaFlameIt(SDL_Surface *surf) {


	Morph(surf);
	Blend(img[rand()%4]);
	CopyBuffer(front);

}

void save_shot() {
	char buf[1024];
	static int counter = 0;
	sprintf(buf,"afss%d.bmp", ++counter);
	SDL_SaveBMP(front, buf);
	SDL_FillRect(front, 0,0);
	SDL_PrintText(front, font, 10,10,SDL_MapRGB(front->format, 255,255,255), "Snap Shot Saved.");
	SDL_UpdateRect(front,0,0, 640,480);
	SDL_Delay(5);
}

int main(int argc, char **argv) {

	SDL_Event e;
	int active = 1;
	SDL_Joystick *joy = 0;
	int width=480, height=272;
	
	if(argc == 3) {
	
		width = atoi(argv[1]);
		height = atoi(argv[2]);
		
	}
	

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
		return -1;

	if(!(video_surface = SDL_SetVideoMode(width,height,0,0)))
		return -1;
	
	static int rmask, gmask, bmask, amask;
	
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif
	
	front =  SDL_CreateRGBSurface(SDL_SWSURFACE,480,272,32, rmask, gmask, bmask, amask);
	
	SDL_WM_SetCaption("AlphaFlame..", 0);

	joy = SDL_JoystickOpen(0);
	SDL_JoystickEventState(SDL_ENABLE);
	font = SDL_InitFont("font.mxf");
	if(font != 0) {
	SDL_FillRect(front, 0, 0);
	SDL_PrintText(front, font, 10,10, SDL_MapRGB(front->format, 255,0,0), "Loading...");
	SDL_UpdateRect(front, 0,0,480,272);
	}

	Init();


	while(active == 1) {

		JaredBruniSaysAlphaFlameIt(front);

		while(SDL_PollEvent(&e)) {

			switch(e.type) {
				case SDL_QUIT:
					active = 0;
					break;
				case SDL_JOYBUTTONDOWN:
					{
						switch(e.jbutton.button) 
						{
							case 0:
								fade_r = 1;
								fade_g = 0;
								fade_b = 0;
								break;
							case 1:
								fade_r = 0;
								fade_g = 1;
								fade_b = 0;
								break;
							case 2:
								fade_r = 0;
								fade_g = 0;
								fade_b = 1;
								break;
							
						}
					}
					break;
					case SDL_KEYDOWN:
					{

						switch(e.key.keysym.sym) 
						{
							case SDLK_LEFT:
								fade_r = 1;
								fade_g = 0;
								fade_b = 0;
								break;
							case SDLK_RIGHT:
								fade_r = 0;
								fade_g = 1;
								fade_b = 0;
								break;
							case SDLK_DOWN:
								fade_r = 0;
								fade_g = 0;
								fade_b = 1;
								break;
							
						}
					
					}
					break;
			}
		}

		
		//SDL_UpdateRect(front, 0,0,480,272);
		SDL_StretchSurfaceRect(front ,0, video_surface,0);	
		SDL_Flip(video_surface);

	}

	for(active = 0; active < 4; active++)
		SDL_FreeSurface(img[active]);


	SDL_FreeSurface(front);
	SDL_FreeFont(font);
	SDL_JoystickClose(joy);
	SDL_Quit();
	return 0;
}

