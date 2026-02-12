# The triangle
> The triangle

![Sample console and graphical output.](./media/triangle.png)

## What
- A [Catppuccin Mauve-colored](https://catppuccin.com/palette/) triangle in the
  middle of the screen.

## Build prerequisites
- You need an operating system that supports OpenGL 4.6 (aka, anything but
  Mac). And a driver supporting OpenGL 4.6 (e.g. `mesa` on Linux).
- Other tools needed:
  - A C compiler supporting C23 (for reference, I use `gcc` version 15).
  - GNU Make (might work on `nmake`; untested).
  - GLFW and glad.
  - pkgconf.

## Building
- Make sure the `glad` command can be run from your terminal (by adding its
  path to environment variable, or making an alias, or whatever).
- Make sure `pkgconf` can find GLFW. Run `pkgconf --libs glfw3` to test.
- Then run `make`.

## Running
- `./build/main`

## License
- This project is under GPLv3. I figured that, if I don't attach a license,
  no one can really touch my code, even if it's on a forge. So, I picked the
  sane default.
