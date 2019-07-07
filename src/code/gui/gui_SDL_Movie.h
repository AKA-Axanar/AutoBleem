#pragma once
#include "gui_sdl_wrapper.h"
#include "smpeg.h"

class SDL_Movie
{
private:
	// Surface for the main screen
	SDL_Shared<SDL_Surface> screen;

	// Surface for the movie
	SDL_Shared<SDL_Surface> movieSurface;

	// Holds the movie information
	SMPEG_Info movieInfo;

	// Load the movie
	SMPEG *movie = nullptr;

	// The max we can scale by
	int MaxScaleX = 0;
	int MaxScaleY = 0;
	int MaxScale = 0;

	// Locations on screen to draw at
	int X = 0, Y = 0;

	void DrawIMG(SDL_Shared<SDL_Surface> img, SDL_Shared<SDL_Surface> dst, int x, int y);
	
public:
	SDL_Movie();
	
	// Free our movie
	~SDL_Movie();

	void ClearScreen();

	// Set's the volume on a scale of 0 - 100
	void SetVolume( int vol );

	// Scale the movie by the desired factors
	void Scale( int w, int h );

	// Scale the movie by the desired factor
	void ScaleBy( int factor );

	// Sets the region of the video to be shown
	void SetDisplayRegion( int x, int y, int w, int h );

	// Set the position that the movie should be drawn at on the screen
	void SetPosition( int x, int y );

	// Check for any errors
	void CheckErrors();

	// Load the movie
	void Load( const char* fileName, SDL_Shared<SDL_Surface> s, int maxscalex = 1, int maxscaley = 1 );

	// Set the looping of hte movie
	void SetLoop( int val );

	// Play the movie
	void Play();

    // Stops the movie, but keeps current position
    void Stop();

#if 0
	// Pause the movie
	void Pause();

	// Rewind the movie back to 0:00:00
	void Rewind();

	// Seek a number of bytes into the movie
	void Seek( int bytes );

	// Skip a number of seconds
	void Skip( float seconds );
#endif

	// Render some frame of the movie
	void RenderFrame( int frame );

	// Render the final frame of the movie
	void RenderFinal();

	// Draw the movie surface to the main screen at x, y
	void DisplayAt( int x, int y );

	// Draw the movie surface to the main screen at x, y
	void Display();

	// Return the current info for the movie
	SMPEG_Info GetInfo();

	// Get the current status of the movie, can be SMPEG_ERROR = -1, SMPEG_STOPPED, SMPEG_PLAYING
	SMPEGstatus GetStatus();
};
