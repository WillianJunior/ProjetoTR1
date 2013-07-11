# Build project.
all: main

# Clean everything.
clean:
	-rm main

main: main.cpp trans_protocols.h basic_types.h
	g++ -Wall -g main.cpp -o main