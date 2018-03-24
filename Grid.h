//
// Created by giannis on 24/03/18.
//

#ifndef BULKLOADRTREE_GRID_H
#define BULKLOADRTREE_GRID_H

#include <bitset>
#include "Interval.h"
#include "Rectangle.h"

class Grid {

public:
    static int const GRID_SIZE = 10000;
    //max 46e3
    /*these are the same for all dimensions*/

    static int const LOW = 0;
    static int const HIGH = 4;
    constexpr static int const ROOT_LENGTH = (HIGH - LOW);
    constexpr static double const CELL_INTERVAL = (double)ROOT_LENGTH / GRID_SIZE;
    /**************************************/

    void markInterval(Interval::typrect rec);

    Grid() = default;

    bool queryGrid2(Interval::typrect);


    double percentageOccupied(){
        return (double) Grid::r2.count() / std::pow(GRID_SIZE,Interval::DIMENSIONALITY);
    }

private:
    uint64_t getIndex(uint32_t dimensionality, uint32_t *coordinates);
    void mark3DCells(Interval::typinterval *dim_range);
    void query2DCells(Interval::typinterval *dim_range, int *count);
    void mark2DCells(Interval::typinterval *dim_range);
    int findPositionModulo(double number);
    static std::bitset<GRID_SIZE * GRID_SIZE> r2;
};


#endif //BULKLOADRTREE_GRID_H
