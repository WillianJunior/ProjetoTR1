# Build project.
all: main

# Clean everything.
clean: clean_ipc
	-rm main
	-rm receiver

clean_ipc:
	-ipcrm -Q 0x1927
	-ipcrm -Q 0x1928

main: main.cpp trans_protocols.cpp stop_n_wait.cpp go_back_n.cpp selective_repeat.cpp trans_protocols.h util.cpp util.h basic_types.h
	g++ -Wall -g main.cpp trans_protocols.cpp stop_n_wait.cpp go_back_n.cpp selective_repeat.cpp util.cpp -o main
	g++ -Wall -g receiver.cpp trans_protocols.cpp stop_n_wait.cpp go_back_n.cpp selective_repeat.cpp util.cpp -o receiver