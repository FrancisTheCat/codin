## Codin - An alternative standard library for C inspired by Odin

### Warning! Still in early development, only linux with gcc is somewhat supported

### Installation
Linux: `$ make PLATFORM=linux MODE=release install`
this will build and install the library as well as the headers

### Usage
Once installed you can build a codin C program by using the linker flag `-lcodin`.
You will also have to specify a few more compiler flags: `-nostdlib -nostdinc -fno-stack-protector -fno-builtin`.
As this is a bit tedious to type out, I like to have a command for it in my bashrc/zshrc:
```zsh
ccodin () {
        cc -nostdlib -nostdinc -fno-stack-protector -fno-builtin "$@" -lcodin -I/usr/include/codin
}
```
By default you can include codin's headers with `import "codin/codin.h"`, but you can add `-I/usr/include/codin` to omit the `codin` prefix.
