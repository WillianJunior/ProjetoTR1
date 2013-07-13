# Build project.
all: main

# Clean everything.
clean:
	-rm main
	-rm receiver

main: main.cpp trans_protocols.cpp stop_n_wait.cpp trans_protocols.h util.cpp util.h basic_types.h
	g++ -Wall -g main.cpp trans_protocols.cpp stop_n_wait.cpp util.cpp -o main
	g++ -Wall -g receiver.cpp trans_protocols.cpp stop_n_wait.cpp util.cpp -o receiver