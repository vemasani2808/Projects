## Windows (MinGW and make in CMD)
CXXFLAGS += -std=c++11 -Wall -w -Wno-unused-local-typedefs

LDFLAGS +=

MAIN = infer

all: $(MAIN).exe generate_nets.exe

opt: CXXFLAGS += -O4
opt: LDFLAGS += -O4
opt: $(MAIN).exe generate_nets.exe

# COMPILE
$(MAIN).exe: $(MAIN).cpp cascinf.cpp Snap.o
	g++ -o $(MAIN).exe $(MAIN).cpp cascinf.cpp snap/kronecker.cpp Snap.o -I./glib -I./snap $(LDFLAGS)

generate_nets.exe: generate_nets.cpp cascinf.cpp Snap.o
	g++ -o generate_nets.exe generate_nets.cpp cascinf.cpp snap/kronecker.cpp Snap.o -I./glib -I./snap $(LDFLAGS)

Snap.o:
	g++ -c $(CXXFLAGS) -std=gnu++11 ./snap/Snap.cpp -I./glib -I./snap



clean:
	del /F $(MAIN) generate_nets generate_nets.exe Snap.o $(MAIN).exe generate_nets.exe
