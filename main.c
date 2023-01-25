#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "main.h"
#define ITERATIONS 5000
#define N_THREADS 14

#include "beehive.h"
extern uint8_t beehive[BEEHIVE_HEIGHT][BEEHIVE_WIDTH];

#include "glider.h"
extern uint8_t glider[GLIDER_HEIGHT][GLIDER_WIDTH];

#include "grower.h"
extern uint8_t grower[GROWER_HEIGHT][GROWER_WIDTH];

/**
 * Initializes the grid.
 * @param grid the grid.
 * @param height the height of the grid.
 * @param width the width of the grid.
 * @param pattern the pattern.
 * @param patternHeight the height of the pattern.
 * @param patternWidth the width of the pattern.
 */
void populate_grid(uint8_t* grid, int height, int width, uint8_t* pattern, int patternHeight, int patternWidth) {
    for (int i = 0; i < patternHeight; i++) {
        for (int j = 0; j < patternWidth; j++) {
            // grid[i][j] == grid[i*width+j] = value;
            grid[((height/2)+i) * width + ((width / 2)+j)] = pattern[i * patternWidth + j];
        }
    }
}

/**
 * Processes the grid.
 * @param grid the grid.
 * @param height the height of the grid.
 * @param width the width of the grid.
 */
void update_grid(int iterations, uint8_t* grid, int height, int width) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows_per_process = height / size;
    int start = rank * rows_per_process;
    int end = start + rows_per_process;

    if (rank == size - 1) {
        end = height; // last process gets the remaining rows.
    }

    uint8_t* new_grid;
    int alive_neighbours;
    for(int iteration = 0; iteration < iterations; iteration++) {
        new_grid = malloc(height * width * sizeof(uint8_t));
        for (int i = start; i < end; i++) {
            for (int j = 0; j < width; j++) {
                alive_neighbours = count_live_neighbors(i, j, grid, height, width);

                // Update cell
                switch (grid[i * width + j]) {
                    case ALIVE:
                        if (alive_neighbours < 2 || alive_neighbours > 3) {
                            new_grid[i * width + j] = DEAD;
                        } else {
                            new_grid[i * width + j] = grid[i * width + j];
                        }
                        break;

                    case DEAD:
                        if (alive_neighbours == 3) { 
                            new_grid[i * width + j] = ALIVE;
                        } else {
                            new_grid[i * width + j] = grid[i * width + j];
                        }
                        break;
                } 
            }
        }
        print_grid((uint8_t*)grid, height, width);
        MPI_Allgather(new_grid + start * width, rows_per_process * width, MPI_UNSIGNED_CHAR, grid, rows_per_process * width, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
        free(new_grid);
    }
}

/**
 * Counts the number of live neighbors for cell i, j.
 * @param x the targeted cell's row.
 * @param y the targeted cell's column.
 * @param grid the cell's grid.
 * @param height the height of the grid.
 * @param width the width of the grid.
 * @returns the number of live neighbors.
 */
int count_live_neighbors(int x, int y, uint8_t* grid, int height, int width) {
    int count = 0;
    
    // Get all cells from -1 to 1 both X and Y direction.
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) {
                continue;
            }

            int neighborX = x + i;
            int neighborY = y + j;

            if (
                neighborX < 0 || neighborX >= height || 
                neighborY < 0 || neighborY >= width
            ) {
                continue;
            }

            if (grid[neighborX * width + neighborY] == ALIVE) {
                count++;
            }
        }
    }

    return count;
}

/**
 * Prints the grid.
 * @param grid the grid.
 * @param height the height of the grid.
 * @param width the width of the grid.
 */
void print_grid(uint8_t* grid, int height, int width) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%d ", grid[i * width + j]);
        }
        printf("\n");
    }
}

/**
 * Main Method
 */
int main(int argc, char* argv[]) {
    int height = 3000;
    int width = 3000;
    uint8_t grid[3000][3000] = {0};

    MPI_Init(&argc, &argv);

    // --
    // Beehive
    // --
    // int patternHeight = BEEHIVE_HEIGHT;
    // int patternWidth = BEEHIVE_WIDTH;
    // uint8_t* pattern = (uint8_t*)beehive;

    // --
    // Glider
    // --
    // int patternHeight = GLIDER_HEIGHT;
    // int patternWidth = GLIDER_WIDTH;
    // uint8_t* pattern = (uint8_t*)glider;

    // --
    // Grower
    // --
    int patternHeight = GROWER_HEIGHT;
    int patternWidth = GROWER_WIDTH;
    uint8_t* pattern = (uint8_t*)grower;

    populate_grid((uint8_t*) grid, height, width, pattern, patternHeight, patternWidth);
    print_grid((uint8_t*)grid, height, width);
    update_grid(ITERATIONS, (uint8_t*)grid, height, width);
    MPI_Finalize();
    return 0;
}