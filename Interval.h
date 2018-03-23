//
// Created by giannis on 23/03/18.
//

#ifndef BULKLOADRTREE_INTERVAL_H
#define BULKLOADRTREE_INTERVAL_H

class Interval {
public:
    static const int DIMENSIONALITY = 2;

    typedef struct {
        double l, h;
    } typinterval;


    typedef typinterval typrect[DIMENSIONALITY];
};
#endif //BULKLOADRTREE_INTERVAL_H
