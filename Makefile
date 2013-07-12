# Build project.
all: main

# Clean everything.
clean:
	-rm main

main: main.cpp trans_protocols.cpp trans_protocols.h basic_types.h
	g++ -Wall -g main.cpp trans_protocols.cpp -o main
	g++ -Wall -g receiver.cpp trans_protocols.cpp -o receiver