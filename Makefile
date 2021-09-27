
CC = g++

main.exe: main.cpp
	$(CC) -o $@ $^

clean:
	rm main.exe
