//
// Created by giannis on 24/03/18.
//

#include <cassert>
#include <cstdint>
#include <cmath>
#include <iostream>
#include "Grid.h"


/**
 * This function takes as an input a multidimensional index and
 * returns an one dimensional index. It assumes that GRID_SIZE
 * is the same along all dimensions
 */
std::bitset <Grid::GRID_SIZE * Grid::GRID_SIZE> Grid::r2{0};

uint64_t Grid::getIndex(uint32_t dimensionality, uint32_t *coordinates) {
    uint32_t power = dimensionality - 1;
    uint64_t index = 0;
    for (uint32_t i = 0; i < dimensionality; i++) {
//		assert(coordinates[i]<GRID_SIZE && coordinates[i] >= 0);
        index += (coordinates[i] * std::pow(Grid::GRID_SIZE, power));

        --power;
    }

    return index;
}

int Grid::findPositionModulo(double number) {
    double pos;
    if (number <= Grid::LOW) {
        //value is out of root's rectangle
        pos = 0;
    } else if (number >= Grid::HIGH) {
        //value is out of root's rectangle
        pos = Grid::GRID_SIZE - 1;
    } else {
        double length = 0;

        length = number - Grid::LOW;

        pos = length / Grid::CELL_INTERVAL;
    }

    if (std::fmod(pos, Grid::GRID_SIZE) != pos) {
        std::cout << "\npos shouldn't be different" <<std::endl;
        exit(9);
    }
    return (static_cast < int > (pos));
}

void Grid::mark2DCells(Interval::typinterval *dim_range) {

    for (auto j = dim_range[0].l; j <= dim_range[0].h; ++j) {
        for (auto k = dim_range[1].l; k <= dim_range[1].h; ++k) {
            uint32_t coordinates[Interval::DIMENSIONALITY];
            coordinates[0] = static_cast<uint32_t>(j);
            coordinates[1] = static_cast<uint32_t>(k);
            uint64_t index = getIndex(Interval::DIMENSIONALITY, coordinates);
            Grid::r2.set(index, true);
        }
    }

}


void Grid::markInterval(Interval::typrect rec) {

    Interval::typinterval dim_range[Interval::DIMENSIONALITY];

    int low, high;

    for (auto dim = 0; dim < Interval::DIMENSIONALITY; dim++) {
        low = findPositionModulo(rec[dim].l);
        high = findPositionModulo(rec[dim].h);
        assert(low <= high);
        dim_range[dim].l = low;
        dim_range[dim].h = high;

    }

    mark2DCells(dim_range);

}

void Grid::query2DCells(Interval::typinterval *dim_range, int *count) {
//int nofcells = 0;
    uint32_t coordinates[Interval::DIMENSIONALITY];

    for (auto j = dim_range[0].l; j <= dim_range[0].h; ++j) {

        for (auto k = dim_range[1].l; k <= dim_range[1].h; ++k) {

            coordinates[0] = j;
            coordinates[1] = k;

            uint64_t index = getIndex(Interval::DIMENSIONALITY, coordinates);

            if (Grid::r2.test(index)) {
                ++(*count);
                return;
            }

        }
    }

}

bool Grid::queryGrid2(Interval::typrect rec) {
    Interval::typinterval dim_range[Interval::DIMENSIONALITY];
    int count = 0;

    int low, high;

    for (int dim = 0; dim < Interval::DIMENSIONALITY; dim++) {
        low = findPositionModulo(rec[dim].l);
        high = findPositionModulo(rec[dim].h);

        assert(low <= high);
        dim_range[dim].l = low;
        dim_range[dim].h = high;

    }

    query2DCells(dim_range, &count);

    return count != 0;
}


