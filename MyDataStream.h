//
// Created by giannis on 01/11/16.
//

#ifndef BULKLOADRTREE_MYDATASTREAM_H
#define BULKLOADRTREE_MYDATASTREAM_H

#include <unistd.h>
#include <fcntl.h>
#include "Interval.h"

class MyDataStream : public SpatialIndex::IDataStream {
private:
    int input_file;
    SpatialIndex::RTree::Data *m_pNext;
    uint32_t point_counter = 0;
public:

    MyDataStream(std::string inputFile, int numof_queries) : input_file(0), m_pNext(nullptr) {
        input_file = open(inputFile.c_str(), O_RDONLY);

        if (input_file == -1){
            std::cerr<<"File not found"<<std::endl;
            exit(9);
        }
        readNextEntry();
    }

    virtual ~MyDataStream() {
        if (m_pNext != nullptr)
            delete m_pNext;
    }

    virtual SpatialIndex::IData *getNext() {

        if (m_pNext == nullptr) {
            return nullptr;
        }
        SpatialIndex::RTree::Data* ret = m_pNext;
        m_pNext = nullptr;
        readNextEntry();
        return ret;
    }


    static SpatialIndex::Region convertRectangleRegion(Interval::typrect rectangle) {
        double low[Interval::DIMENSIONALITY], high[Interval::DIMENSIONALITY];
        for (int i = 0; i < Interval::DIMENSIONALITY; ++i) {
            low[i] = rectangle[i].l;
            high[i] = rectangle[i].h;
        }
        //first parameter is the lows at each dimension
        //2nd parameter is the highs at each dimension
        SpatialIndex::Region region(low, high, Interval::DIMENSIONALITY);

        return region;
    }
    void readNextEntry() {
        SpatialIndex::id_type id(point_counter++);
        Interval::typrect my_record{};

        double low[Interval::DIMENSIONALITY]{};
        ssize_t nbytes = read(input_file, my_record, sizeof(Interval::typrect));


        if (nbytes == 0){
            m_pNext = nullptr;
            return;
        }


        SpatialIndex::Region region = convertRectangleRegion(my_record);
        //constructor only takes regions
        m_pNext = new SpatialIndex::RTree::Data(sizeof(double), reinterpret_cast<byte *>(low), region, id);
    }

    virtual bool hasNext() {

        return (m_pNext != nullptr);
    }

    virtual uint32_t size() {
        throw Tools::NotSupportedException("Operation not supported.");
    }

    virtual void rewind() {
        throw Tools::NotSupportedException("Operation not supported.");
    }


};

#endif //BULKLOADRTREE_MYDATASTREAM_H
