CXX := g++ -std=c++11
CXXFLAGS := -g3 -pthread
LD := g++ -std=c++11
LDFLAGS := -lpthread -lglfw -lGLEW
OBJECTS := main.o

.PHONY: run debug clean

main : $(OBJECTS)
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run : main
	./main

debug : main
	gdb main

clean:
	rm $(OBJECTS)
