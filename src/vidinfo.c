#include <SDL.h>
#include "vidinfo.h"

void PrintVideoInfo()
{
	char vidname[256];
	const SDL_VideoInfo *info = NULL;
	info = SDL_GetVideoInfo();
	SDL_VideoDriverName(vidname, 255);
	
	printf("Video-driver: %s\n", vidname);
	/* the members are printed */
	printf("Is it possible to create hardware surfaces: %d\n", info->hw_available);
	printf("Is there a window manager available: %d\n", info->wm_available);
	printf("Are hardware to hardware blits accelerated: %d\n", info->blit_hw);
	printf("Are hardware to hardware colorkey blits accelerated: %d\n", info->blit_hw_CC);
	printf("Are hardware to hardware alpha blits accelerated: %d\n", info->blit_hw_A);
	printf("Are software to hardware blits accelerated: %d\n", info->blit_sw);
	printf("Are software to hardware colorkey blits accelerated: %d\n", info->blit_sw_CC);
	printf("Are software to hardware alpha blits accelerated: %d\n", info->blit_sw_A);
	printf("Are color fills accelerated: %d\n", info->blit_fill);
	printf("Total amount of video memory in Kilobytes: %dKb\n", info->video_mem);
	
	return;
}

void PrintSurfaceInfo( SDL_Surface *surf, char *name )
{
	printf("Name of the surface: %s\n", name);
	
	printf("Surface is in ");
	if(surf->flags & SDL_HWSURFACE)
		printf("video-memory\n");
	else
		printf("system-memory\n");
	
	if(surf->flags & SDL_HWACCEL)
		printf("Blitting is hardware accelerated\n");
	else
		printf("Blitting is not hardware accelerated\n");
	
	return;
}
