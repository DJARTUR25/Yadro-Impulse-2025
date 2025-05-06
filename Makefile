CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

all: task.exe

task.exe: main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	del /F /Q task.exe

.PHONY: all clean