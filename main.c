#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <mpi.h>
#include <omp.h>
// or for MacOSx
// #include "/usr/local/opt/libomp/include/omp.h"

#include "main.h"

// #include "beehive.h"
// extern uint8_t beehive[BEEHIVE_HEIGHT][BEEHIVE_WIDTH];

// #include "glider.h"
// extern uint8_t glider[GLIDER_HEIGHT][GLIDER_WIDTH];

#include "grower.h"
extern uint8_t grower[GROWER_HEIGHT][GROWER_WIDTH];

/**
 * Initializes the grid.
 * @param grid the grid.
 * @param pattern the pattern.
 * @param patternHeight the height of the pattern.
 * @param patternWidth the width of the pattern.
 */
void populate_grid(bool grid[HEIGHT][WIDTH], bool* pattern, int patternHeight, int patternWidth) {
    for (int row = 0; row < patternHeight; row++) {
        for (int col = 0; col < patternWidth; col++) {
            grid[(HEIGHT/2) + row][(WIDTH/2) + col] = pattern[row * patternWidth + col];
        }
    }
}

/**
 * Processes the grid.
 * @param iterations the amount of iterations.
 * @param grid the grid.
 */
void update_grid(bool grid[HEIGHT][WIDTH], bool new_grid[HEIGHT][WIDTH], int* population) {
    int local_population = 0;
    #pragma omp parallel for collapse(2) reduction(+:local_population)
    {
        for (int row = 0; row < HEIGHT; row++) {
            for (int col = 0; col < WIDTH; col++) {
                int live_neighbours = count_live_neighbors(row, col, grid);

                switch (grid[row][col]) {
                    case ALIVE:
                        if (live_neighbours < 2 || live_neighbours > 3) {
                            new_grid[row][col] = DEAD;
                        } else {
                            new_grid[row][col] = ALIVE;
                            local_population++;
                        }
                        break;

                    case DEAD:
                        if (live_neighbours == 3) { 
                            new_grid[row][col] = ALIVE;
                            local_population++;
                        } else {
                            new_grid[row][col] = DEAD;
                        }
                        break;
                } 
            }
        }
    }
    MPI_Allreduce(&local_population, population, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
}

/**
 * Counts the number of live neighbors for cell i, j.
 * @param x the targeted cell's row.
 * @param y the targeted cell's column.
 * @param grid the cell's grid.
 * @returns the number of live neighbors.
 */
int count_live_neighbors(int row, int col, bool grid[HEIGHT][WIDTH]) {
    int count = 0;
    
    // Get all cells from -1 to 1 both X and Y direction.
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            if (x == 0 && y == 0) {
                continue;
            }

            int neighborX = row + x;
            int neighborY = col + y;

            if (
                neighborX < 0 || neighborX >= HEIGHT || 
                neighborY < 0 || neighborY >= WIDTH
            ) {
                continue;
            }

            if (grid[neighborX][neighborY] == ALIVE) {
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
void print_grid(bool grid[HEIGHT][WIDTH]) {
    for (int row = 0; row < HEIGHT; row++) {
        for (int col = 0; col < WIDTH; col++) {
            printf("%d", grid[row][col] ? 1 : 0);
        }
        printf("\n");
    }
}

/**
 * Main Method
 */
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    bool grid[HEIGHT][WIDTH], new_grid[HEIGHT][WIDTH];
    if (rank == 0) {
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
        bool* pattern = (bool*)grower;

        populate_grid(grid, pattern, patternHeight, patternWidth);
    }
    MPI_Bcast(grid, HEIGHT * WIDTH, MPI_C_BOOL, 0, MPI_COMM_WORLD);

    int population;
    for(int iteration = 0; iteration < ITERATIONS; iteration++) {
        double start = MPI_Wtime();

        update_grid(grid, new_grid, &population);
        MPI_Allreduce(MPI_IN_PLACE, new_grid, HEIGHT * WIDTH, MPI_C_BOOL, MPI_LOR, MPI_COMM_WORLD);
        memcpy(grid, new_grid, sizeof(new_grid));

        double end = MPI_Wtime();

        if (rank == 0) {
            printf("Generation: %d, population count: %d, obtained in %f seconds\n", iteration, population, end-start);
        }
    }

    MPI_Finalize();
    return 0;
}