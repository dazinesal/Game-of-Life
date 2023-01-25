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
 * @param grid the grid.
 * @param height the height of the grid.
 * @param width the width of the grid.
 */
void process_grid(bool *grid, int height, int width) {
    // Copy the current grid to a temporary grid
    bool temp_grid[height][width];
    #pragma omp parallel for collapse(2)
    {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                temp_grid[i][j] = grid[i*width + j];
            }
        }
    }

    // Update the grid based on the Game of Life rules
    #pragma omp parallel for collapse(2)
    {
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                int live_neighbours = count_live_neighbors(row, col, (bool *)temp_grid, height, width);

                switch (temp_grid[row][col]) {
                    case ALIVE:
                        if (live_neighbours < 2 || live_neighbours > 3) {
                            grid[row * width + col] = DEAD;
                        } 
                        break;

                    case DEAD:
                        if (live_neighbours == 3) { 
                            grid[row * width + col] = ALIVE;
                        }
                        break;
                } 
            }
        }
    }
}

/**
 * Counts the number of live neighbors for cell i, j.
 * @param x the targeted cell's row.
 * @param y the targeted cell's column.
 * @param grid the cell's grid.
 * @returns the number of live neighbors.
 */
int count_live_neighbors(int row, int col, bool *grid, int height, int width) {
    int count = 0;
    
    // Get all cells from -1 to 1 both X and Y direction.
    #pragma omp parallel for reduction(+:count)
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            // Skip the current cell.
            if (x == 0 && y == 0) {
                continue;
            }

            int neighborX = row + x;
            int neighborY = col + y;

            // Check if the neighbor is within the grid boundaries
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
void print_grid(bool *grid, int height, int width) {
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            printf("%c ", grid[row * width + col] ? '*' : '.');
        }
        printf("\n");
    }
}

/**
 * Counts the population.
 * @param grid the grid.
 * @param height the height of the grid.
 * @param width the width of the grid.
 */
int count_population(bool *grid, int height, int width) {
    int population = 0;
    #pragma omp parallel for reduction(+:population)
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (grid[i * width + j]) {
                population++;
            }
        }
    }
    return population;
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

    // Split the grid into chunks.
    int chunk_size = (HEIGHT * WIDTH) / size;
    bool chunk[chunk_size][WIDTH];
    MPI_Scatter(grid, chunk_size * WIDTH, MPI_C_BOOL, chunk, chunk_size * WIDTH, MPI_C_BOOL, 0, MPI_COMM_WORLD);

    // Perform the Game of Life.
    for(int iteration = 0; iteration < ITERATIONS; iteration++) {
        int local_population = count_population((bool*)chunk, chunk_size, WIDTH);
        int total_population;

        double start_time = MPI_Wtime();
        process_grid((bool*)chunk, chunk_size, WIDTH);
        double end_time = MPI_Wtime();

        MPI_Reduce(&local_population, &total_population, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            printf("Generation: %d, population count: %d, obtained in %f seconds\n", iteration, total_population, end_time-start_time);
        }
    }

    // Gather the processed chunks back to the grid
    MPI_Gather(chunk, chunk_size*WIDTH, MPI_C_BOOL, grid, chunk_size*WIDTH, MPI_C_BOOL, 0, MPI_COMM_WORLD);

    // Print the final grid
    if (rank == 0) {
        print_grid((bool*)grid, HEIGHT, WIDTH);
    }

    MPI_Finalize();
    return 0;
}