#include "tree.h"

#include <stdio.h>
#include <stdlib.h>

typedef enum CellType {
    CELL_EMPTY = 0,
    CELL_LEAF,
    CELL_TRUNK
} CellType;

typedef enum CellOrnament {
    ORNAMENT_NONE = 0,
    ORNAMENT_RED
} CellOrnament;

typedef struct Cell {
    CellType type;
    CellOrnament ornament;
} Cell;

#define CELL(y, x) map[(y) * width + (x)]

int printTree(int height, int trunkHeight, int trunkWidth, int color, int dense, int ornaments)
{
    if (height < 1) height = 1;
    int width = 2 * height - 1;

    int total_height = height + trunkHeight;

    Cell* map = (Cell*)calloc(total_height * width, sizeof(Cell));
    if (!map) return 1;

    
    // Set types
    for (int i = 0; i < height; i++) {
        int leaves = 2 * i + 1;
        int start = (width - leaves) / 2;
        for (int j = 0; j < leaves; j++)
            CELL(i, start + j).type = CELL_LEAF;
    }
    int trunkStart = (width - trunkWidth) / 2;
    for (int i = 0; i < trunkHeight; i++) {
        for (int j = 0; j < trunkWidth; j++)
            CELL(height + i, trunkStart + j).type = CELL_TRUNK;
    }


    // Ornaments
    if (ornaments) {
        unsigned int start_chance = 2;
        unsigned int chance = start_chance;
        for (int row = 0; row < total_height; row++) {
            for (int col = 0; col < width; col++) {
                Cell* cell = &CELL(row, col);
                if (cell->type == CELL_LEAF) {
                    if (rand() % chance == 0) {
                        cell->ornament = ORNAMENT_RED;
                        chance = start_chance;
                    } else {
                        chance /= 2;
                        if (chance < 2) chance = 2;
                    }
                }
            }
            start_chance *= 2;
            if (start_chance > 4096 || start_chance <= 0) start_chance = 4096;
        }
    }


    // Draw
    for (int row = 0; row < total_height; row++) {
        for (int col = 0; col < width; col++) {
            Cell* cell = &CELL(row, col);

            switch (cell->type) {
                case CELL_EMPTY:
                    fputc(' ', stdout);
                    break;

                case CELL_LEAF:
                    if (cell->ornament != ORNAMENT_NONE) {
                        if (color) {
                            if (dense) fputs("\033[48;5;22m"  "\033[38;5;196m" "●" "\033[0m", stdout);
                            else       fputs("\033[38;5;196m" "o" "\033[0m", stdout);
                        }
                        else {
                            fputs(dense ? "●" : "o", stdout);
                        }
                    } else {
                        if (color) {
                            if (dense) fputs("\033[48;5;22m" " " "\033[0m", stdout);
                            else       fputs("\033[32m"      "*" "\033[0m", stdout);
                        }
                        else {
                            fputs(dense ? "▓" : "*", stdout);
                        }
                    }
                    break;

                case CELL_TRUNK:
                    if (color) {
                        if (dense) fputs("\033[48;5;95m" " " "\033[0m", stdout);
                        else       fputs("\033[38;5;95m" "|" "\033[0m", stdout);
                    }
                    else {
                        fputs(dense ? "█" : "|", stdout);
                    }
                    break;
            }

        }
        fputc('\n', stdout);
    }

    free(map);

    return 0;
}
