# Untitled Wizard Game

## Installation
```bash
git clone --recursive <repo-url>
```
cd <Folder name>

# Generate the build files
```
cmake -B build
# or
cmake -B build -G "MinGW Makefiles" `
  -DVCPKG_TARGET_TRIPLET=x64-mingw-static `
  -DVCPKG_HOST_TRIPLET=x64-mingw-static `
  -DCMAKE_MAKE_PROGRAM="mingw32-make.exe"
```


# Compile the executable
cmake --build build

## Resources
[SDL3 Hello World Tutorial](https://lazyfoo.net/tutorials/SDL3/01-hello-sdl3/index2.php)
[Tiled Global Tile IDs](https://doc.mapeditor.org/en/stable/reference/global-tile-ids/)