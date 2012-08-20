CFLAGS = -g -O0 -I/home/costin/code/SDL2/include/SDL2 -D_REENTRANT
LDFLAGS = -L/home/costin/code/SDL2/lib -Wl,-rpath,/home/costin/code/SDL2/lib
LIBS = -lSDL2 -lGL -lpthread
OBJS = main.o gfx.o

main: $(OBJS)
	g++ $(LDFLAGS) -o main $(OBJS) $(LIBS)

$(OBJS): %.o: %.cpp
	g++ -c $(CFLAGS) $< -o $@

