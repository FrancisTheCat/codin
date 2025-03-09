CC = cc
CFLAGS = -nostdlib -fno-builtin -I. -fno-stack-protector -O3 -march=native
OBJS = codin.o strings.o runtime_linux.o allocators.o os.o os_linux.o log.o time.o fmt.o context.o amd64.o bit_array.o net.o thread_linux.o xml.o image.o test.o mem.o math.o

all: minimal generator compiler qr server main

minimal: minimal.o $(OBJS)
	$(CC) $^ -o minimal $(CFLAGS)

generator: generator.o $(OBJS)
	$(CC) $^ -o generator $(CFLAGS)

compiler: compiler.o $(OBJS)
	$(CC) $^ -o compiler $(CFLAGS)

qr: qr_gen.o $(OBJS)
	$(CC) $^ -o qr_gen $(CFLAGS)

server: server.o $(OBJS)
	$(CC) $^ -o server $(CFLAGS)

main: main.o $(OBJS)
	$(CC) $^ -o main $(CFLAGS)

lib: $(OBJS)
	ar rcs libcodin.a $(OBJS)

install: lib
	sudo cp libcodin.a /usr/lib/libcodin.a
	sudo ldconfig

uninstall:
	sudo rm /usr/lib/libcodin.a
	sudo ldconfig

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o minimal generator compiler qr_gen server main libcodin.a
