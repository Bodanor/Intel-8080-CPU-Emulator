CC=gcc
CFLAGS=-Wall -O3

intel-8080-CPU-Emulator: *.c *.h
	$(CC) $(CFLAGS) *.c -o $@

clean:
	rm -rf *.out
	rm -rf intel-8080-CPU-Emulator