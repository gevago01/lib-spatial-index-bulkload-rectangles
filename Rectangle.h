//
// Created by giannis on 23/03/18.
//

#ifndef BULKLOADRTREE_RECTANGLE_H
#define BULKLOADRTREE_RECTANGLE_H

#include <vector>
#include <tuple>
#include <cassert>
#include <spatialindex/Region.h>
#include "Interval.h"

class MyRectangle{


    std::vector<std::tuple<double, double>> rec;

    void addInterval(std::tuple<double, double> interval){
        rec.push_back(interval);
    }

public:
    MyRectangle(Interval::typrect rectangle){
        for (int i = 0; i < Interval::DIMENSIONALITY; ++i) {
            std::tuple<double, double> interval{rectangle[i].l,rectangle[i].h};
            rec.push_back(interval);
        }
    }


    SpatialIndex::Region convertRectangleRegion() {
        double low[Interval::DIMENSIONALITY], high[Interval::DIMENSIONALITY];
        assert(Interval::DIMENSIONALITY == rec.size());
        for (int i = 0; i < Interval::DIMENSIONALITY; ++i) {
            low[i] = std::get<0>(rec.at(i));
            high[i] =std::get<1>(rec.at(i));
        }
        //first parameter is the lows at each dimension
        //2nd parameter is the highs at each dimension
        SpatialIndex::Region region(low, high, Interval::DIMENSIONALITY);

        return region;
    }

};


#endif //BULKLOADRTREE_RECTANGLE_H
