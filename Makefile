CXX=g++
CFLAGS=-std=c++17 -g
LIBS= -lmingw32 -mwindows -mconsole -lSDL2main -lSDL2
VPATH=src
OBJ = main.o

all: $(OBJ)
	$(CXX) $(CFLAGS) $(OBJ) -o multibox $(LIBS)

clean:
	rm *.o && rm multibox

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $<