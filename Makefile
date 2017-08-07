
#Entrance-room-video-looper : Entrance-room-video-looper.cpp
#	g++ -o Entrance-room-video-looper -lwiringPi Entrance-room-video-looper.cpp

PROGRAM_NAME = Video_looper
SRC = $(wildcard *.cpp) 
OBJ = $(SRC:.cpp=.o) 
CC = g++

LDFLAGS = -O0 -g3 -lwiringPi -I.

$(PROGRAM_NAME): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
	rm -f $(OBJ)

.PHONY: clean
clean:
	rm -f $(OBJ) $(PROGRAM_NAME)
