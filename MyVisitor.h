//
// Created by giannis on 01/11/16.
//

#ifndef BULKLOADRTREE_MYVISITOR_H
#define BULKLOADRTREE_MYVISITOR_H
#include <spatialindex/SpatialIndex.h>

class MyVisitor : public SpatialIndex::IVisitor {

private:
    uint32_t data_length;
    std::vector<size_t> _indices;

public:

    explicit MyVisitor(uint32_t const max_cluster_id) {
//        data_length = static_cast<uint32_t>(std::to_string(max_cluster_id).length());
    }

    void visitNode(const SpatialIndex::INode &in) override {
    }


    void visitData(std::vector<const SpatialIndex::IData *> &data) override {
        std::cout<<"size:"<<data.size()<<std::endl;
    }

    void visitData(const SpatialIndex::IData &d) override {
        //_indices.push_back(d.getIdentifier());

//        byte *bytearray = nullptr;
        //plus one for the a null terminated string
//        char char_cluster_id[data_length + 1];
//
//        d.getData(data_length, &bytearray);
//        std::copy(&bytearray[0], &bytearray[data_length], char_cluster_id);
//        char_cluster_id[data_length] = '\0';


    }



};

#endif //BULKLOADRTREE_MYVISITOR_H
