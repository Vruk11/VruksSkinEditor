An MDL file skin editor (currently only V48 files) made using:<br>
 [The Dear ImGui library](https://github.com/ocornut/imgui),<br>[Native File Dialog Extended](https://github.com/btzy/nativefiledialog-extended),<br> [Simple DirectMedia Layer 3 (SDL3)](https://github.com/libsdl-org/SDL),<br> [SDL_image 3.0](https://github.com/libsdl-org/SDL_image),<br> and referencing some code from [Crowbar](https://github.com/ZeqMacaw/Crowbar) (with permission to license this under zlib, very cool)

## Building
Tested on windows and linux, no guarantees though.

Make sure you clone with `git clone --recursive` or if you've already cloned do `git submodule init` then `git submodule update`

On windows, you should be able to open the root folder with visual studio and right click the CMakeList.txt file and build. You should also be able to build using typical CMake commands in terminal:<br>
`mkdir out`<br>
`cd out`<br>
`cmake ../`<br>
`make`

Feel free to tell me how bad it is in the issues, or if you really hate it open a PR<br>
You can also contact me at JustVruk@gmail.com
