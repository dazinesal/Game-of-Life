RLES = beehive.rle glider.rle grower.rle
HEADERS = $(RLES:.rle=.h)
JUNK = $(TAR_FILE)
EXPENSIVE_JUNK = $(HEADERS)
TAR_FILES = $(HEADERS) $(RLES) convert_rle.py Makefile
TAR_FILE = game-of-life-assignment.tar.gz

IMPLEMENTATION = main.o
SOURCES = main.c
EXECUTABLE = $(SOURCES:.c=.o)
CC = mpiCC
CFLAGS = -openmp
PROCESSES = 1

all: $(HEADERS) $(EXECUTABLE)

tar: $(TAR_FILE)

$(TAR_FILE): $(TAR_FILES)
	tar cvfz $(TAR_FILE) $(TAR_FILES)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)

run: $(EXECUTABLE)
	mpirun -np $(PROCESSES) $(EXECUTABLE)

clean:
	rm -rf $(JUNK)
	rm -f $(EXECUTABLE)

empty:
	rm -rf $(JUNK) $(EXPENSIVE_JUNK)

%.h: %.rle
	python3 convert_rle.py $<
