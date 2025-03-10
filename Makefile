CC         = cc
CFLAGS     = -nostdlib -fno-builtin -I. -fno-stack-protector -O3 -march=native
OBJS_LINUX = runtime_linux.o os_linux.o thread_linux.o mem_virtual_linux.o
OBJS       = codin.o strings.o allocators.o os.o log.o time.o fmt.o context.o amd64.o bit_array.o net.o xml.o image.o test.o mem.o math.o unicode.o mem_virtual.o unicode.o $(OBJS_LINUX)

all: lib qr server

qr: qr_gen.o $(OBJS)
	$(CC) $^ -o qr_gen $(CFLAGS)

server: server.o $(OBJS)
	$(CC) $^ -o server $(CFLAGS)

lib: $(OBJS)
	ar rcs libcodin.a $(OBJS)

install: lib
	sudo cp libcodin.a /usr/lib/libcodin.a
	sudo rm -rf /usr/include/codin
	sudo mkdir  /usr/include/codin
	sudo cp *.h /usr/include/codin
	sudo ldconfig

uninstall:
	sudo rm -rf /usr/include/codin
	sudo rm /usr/lib/libcodin.a
	sudo ldconfig

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o compiler qr_gen server libcodin.a
