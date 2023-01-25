#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ITERATIONS 5000

#define WIDTH 3000
#define HEIGHT 3000

#define DEAD 0
#define ALIVE 1

void populate_grid(bool grid[HEIGHT][WIDTH], bool* pattern, int patternHeight, int patternWidth);
void process_grid(bool grid[HEIGHT][WIDTH]);
int count_live_neighbors(int x, int y, bool grid[HEIGHT][WIDTH]);
void print_grid(bool grid[HEIGHT][WIDTH]);
