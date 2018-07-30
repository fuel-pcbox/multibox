CXX=g++
CFLAGS=-std=c++17 -g
LIBS=
VPATH=src
OBJ = cpu.o \
main.o

all: $(OBJ)
	$(CXX) $(CFLAGS) $(OBJ) -o multibox $(LIBS)

clean:
	rm *.o && rm multibox

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $<