#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DEAD 0
#define ALIVE 1

void populate_grid(uint8_t* grid, int height, int width, uint8_t* pattern, int patternHeight, int patternWidth);
void process_grid(uint8_t* grid, int height, int width);
int count_live_neighbors(int x, int y, uint8_t* grid, int height, int width);
void print_grid(uint8_t* grid, int height, int width);
