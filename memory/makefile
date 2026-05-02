CXX = g++
CXXFLAGS = -Wall -Wextra -O2

all: memory_demo

memory_demo: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o memory_demo

run: memory_demo
	./memory_demo

clean:
	rm -f memory_demo