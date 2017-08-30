PROGRAM_NAME = VideoTemplate

 
SRC = VideoButtons.cpp VideoControls.cpp Fitting_Room.cpp 
OBJ = $(SRC:.cpp=.o) 
CC = g++

CXXFLAGS = -std=c++11 -pthread -O0 -g  -I. 
LDFLAGS = -lwiringPi -lmosquitto -pthread

$(PROGRAM_NAME): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ 
	rm -f $(OBJ)

.PHONY: clean
clean:
	rm $(OBJ) $(PROGRAM_NAME)
