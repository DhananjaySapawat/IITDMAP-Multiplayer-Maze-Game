# Todo
1. [done] create a blank window
2. render a dot onto it
3. make the dot move
4. render objects onto the window
5. make the dot respect collisions

# Rendering
1. Create function `loadTexture(path)` for rendering individual images as textures.
2. Instantiate the window we'll be rendering to (`gWindow`), the renderer (`gRenderer`), and the current displayed texture (`gTexture`). Here "g" stands for global.
3. Assign a window to gWindow using SDL_CreateWindow.
4. Assign a renderer to gRenderer using SDL_CreateRenderer.
5. Set a default rendering color.
6. Initialize PNG loading using IMG_INIT_PNG;
7. Define `loadTexture(path)`:
    - Declare a new texture `newTexture`.
    - Load image into surface using `IMG_LOAD`.
    - Create texture from surface.
    - Free the surface using SDL_FreeSurface.
    - Return `newTexture`
8. Define function `loadMedia()` which calls `loadTexture(path)`.
9. Define function `close()` which deallocates all memory and destroys SDL, Renderers and IMG stuff.
10. In the main loop:
    - After handling an event call `SDL_RenderClear`. This function fills the screen with the color that was last set with `SDL_SetRenderDrawColor`.
    - Render the new texture using `SDL_RenderCopy`.
    - Call `SDL_RenderPresent` to update the window.
11. Viewports are used to render images in specific parts of the window. Check how to integrate that with the above process. Viewports for each block will be overkill, no?

# Timing