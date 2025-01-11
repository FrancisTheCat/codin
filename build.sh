cc generator.c amd64.S -nostdlib -fno-builtin -O3 -march=native -o generator

./generator wayland-protcols/staging/cursor-shape/cursor-shape-v1.xml wayland-gen/cursor-shape.h
./generator wayland-protcols/stable/xdg-shell/xdg-shell.xml           wayland-gen/xdg-shell.h
./generator wayland/protocol/wayland.xml                              wayland-gen/wayland.h

cc main.c      amd64.S -nostdlib -fno-builtin -O3 -march=native -I.
