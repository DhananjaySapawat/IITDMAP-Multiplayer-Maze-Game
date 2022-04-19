# COP290: A2

### TEAM
Dhananjay Sapawat | 2019cs10345 <br>
Burouj Armgaan | 2021vsn9003

### SYSTEM SPECIFICATION
- Ubuntu 20.04.4
- Ubuntu 20.04.4 using WSL on Windows 11

### LIBRARIES USED
- `SDL2/SDL.h`
- `SDL2/SDL_image.h`
- `SDL2/SDL_ttf.h`
- `SDL2/SDL_image.h`
- `sys/types.h`
- `sys/socket.h`
- `netinet/in.h`
- `arpa/inet.h`
- `unistd.h`
- `netdb.h`
- `pthread.h`

### LIST OF FILES
- `SDL_help.h`: Contains functions handling SDL rendering, SDL IO and sockets.

- `Player.cpp`: Contains class for players that run in the maze.

- `server.cpp`: Makes server side calls.

- `client.cpp`: Makes client side calls.

- `Image`: Directory containing background images and player images.

- `Audio`: Directory containing background audio.

### DEPENDENCIES
- Install SDL. Here is a good resource for installation instructions: https://lazyfoo.net/tutorials/SDL/01_hello_SDL/index.php
- Install SDL2_image, SDL2_ttf, SDL2_mixer: https://lazyfoo.net/tutorials/SDL/06_extension_libraries_and_loading_other_image_formats/index.php
- The above link has multiple steps spread across three pages. Follow till the end to finish the installation.

### USAGE

- Clone the repostitory on two devices. Assume one to be server and one to be client.
- Open terminal in the cloned repo's directory.
- On Linux, install above above mentioned dependencies as follows (for other OS, refer to the above given link):
		a) sudo apt-get install libsdl2-dev
		b) sudo apt-get install libsdl-image1.2-dev
		c) sudo apt install libsdl1.2-dev libsdl-mixer1.2-dev
		d) sudo apt-get install libsdl2-ttf-dev
- Connect both to the same hotspot.
- Run `make s` on the server.
- Run `make c` on the client.
- Run `./server <server's IP>` on server's machine.
- Run `./server <client's IP>` on client's machine.
- The game will launch automatically on both systems.

### RESOURCES USED
- Lazy Foo's Productions : https://lazyfoo.net/tutorials/SDL/
- maciejspychala/sdl-game : https://github.com/maciejspychala/sdl-game.git
