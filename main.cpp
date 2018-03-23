/******************************************************************************
 * Project:  libspatialindex - A C++ library for spatial indexing
 * Author:   Marios Hadjieleftheriou, mhadji@gmail.com
 ******************************************************************************
 * Copyright (c) 2002, Marios Hadjieleftheriou
 *
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
******************************************************************************/

// NOTE: Please read README.txt before browsing this code.

// include library header file.
#include <spatialindex/SpatialIndex.h>
#include <chrono>
#include "MyVisitor.h"
#include "MyDataStream.h"
#include <sys/stat.h>
#include <fcntl.h>


#define MEASUREMENTS 600
#define CUT_MEASUREMENTS 100
#define GB 1000000000

using std::cout;
using std::endl;


void printInfo(SpatialIndex::ISpatialIndex *tree, SpatialIndex::StorageManager::IBuffer *file,SpatialIndex::id_type index_id);
void calculate_statistics(std::vector<double> &times, float file_size);
auto  call_lib_spatial(SpatialIndex::Point const &point, MyVisitor &visitor, SpatialIndex::ISpatialIndex *tree_);

void
printInfo(SpatialIndex::ISpatialIndex *tree, SpatialIndex::StorageManager::IBuffer *file,
          SpatialIndex::id_type index_id) {

    auto isIndexValid = tree->isIndexValid();
    if (!isIndexValid) {
        std::cerr << "ERROR: Structure is invalid!" << std::endl;
    } else {
        std::cerr << "The stucture seems O.K." << std::endl;
    }

    std::cerr << *tree;
    std::cerr << "Buffer hits: " << file->getHits() << std::endl;
    std::cerr << "Index ID: " << index_id << std::endl;
}


void calculate_statistics(std::vector<double> &times) {

    //sort times
    std::sort(times.begin(), times.end());

    times.resize(MEASUREMENTS - CUT_MEASUREMENTS);

    /*estimated mean*/
    double sum = std::accumulate(times.cbegin(), times.cend(), 0.0);
    double mean = sum / times.size();

    std::vector<double> times_minus_mean;

    /*subtract mean from every vector element*/
    std::transform(times.cbegin(), times.cend(), std::back_inserter(times_minus_mean),
                   [&mean](double const x) { return x - mean; });

    double innerProduct = std::inner_product(times_minus_mean.cbegin(), times_minus_mean.cend(),
                                             times_minus_mean.cbegin(), 0.0);

    double std = std::sqrt(innerProduct / times_minus_mean.size());

    double std_error = std / std::sqrt((MEASUREMENTS - CUT_MEASUREMENTS));


    cout << "#Time|Standard error" << endl;
    cout <<  mean << "\t" << std_error << "\t" << endl;
}

void  call_lib_spatial(SpatialIndex::Region query, SpatialIndex::ISpatialIndex *tree_) {
    MyVisitor visitor(1);
    tree_->intersectsWithQuery(query,visitor);
}


std::vector<Interval::typrect> readQueryPoints(std::string queryFile) {
    int datafile;
    Interval::typrect rectangle;
    std::vector<Interval::typrect > queryPoints{};
    int length, numbRects, nbytes, i=0;
    struct stat status;

    datafile= open(queryFile.c_str(),O_RDONLY);

    fstat(datafile,&status);
    length= status.st_size;
    numbRects= length / sizeof(Interval::typrect);


    do {
        nbytes= read(datafile,rectangle,sizeof(Interval::typrect));
        if (nbytes != sizeof(Interval::typrect)) {
            fprintf(stderr,"Error during read!\n");
            fprintf(stdout,"Error during read!\n");
            exit(-1);
        }


        i++;
    } while (i < numbRects);



    return queryPoints;
}

int main(int argc, char **argv) {

    std::string treeFile="tree_file.index";
    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    std::vector<double> times;

    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " input_file query_file capacity utilization." << std::endl;
        return -1;
    }


    auto utilization = std::stod(argv[4]);
    auto capacity = static_cast<uint32_t>(std::stoi(argv[3]));

    //create new in memory storage manager
    SpatialIndex::IStorageManager *in_mem_manager = SpatialIndex::StorageManager::createNewDiskStorageManager(treeFile, 4096);

//SpatialIndex::StorageManager::

    SpatialIndex::StorageManager::IBuffer *file = SpatialIndex::StorageManager::createNewRandomEvictionsBuffer(
            *in_mem_manager, 10, false);
    // applies a main memory random buffer on top of the persistent storage manager
    // (LRU buffer, etc can be created the same way).

    //pass the file name to create a stream
    cout << "----------------------------------------------------"<< endl;
    MyDataStream stream(argv[1], MEASUREMENTS);
    cout << "----------------------------------------------------"<< endl;

    // Create and bulk load a new RTree using STR and the  RSTAR splitting policy.
    SpatialIndex::id_type indexIdentifier = 1;
    std::cout<<utilization<<std::endl;
    std::cout<<capacity<<std::endl;
    SpatialIndex::ISpatialIndex *tree = nullptr;
    try {

        tree = SpatialIndex::RTree::createAndBulkLoadNewRTree(
                SpatialIndex::RTree::BLM_STR, stream, *file, 0.7, 100, 100,
                2,
                SpatialIndex::RTree::RV_RSTAR, indexIdentifier);
    }

    catch (Tools::IllegalArgumentException e){
        std::cerr<<e.what()<<std::endl;
        std::cout<<e.what()<<std::endl;
    }


    std::cout<<"Building the index done!"<<std::endl;
//exit(9);
    std::vector<Interval::typrect > query_points{};
    query_points = readQueryPoints(argv[2]);
    std::cout<<"size:"<<query_points.size()<<std::endl;
    for (auto &p:query_points) {
        SpatialIndex::Region queryRectangle = MyDataStream::convertRectangleRegion(p);

        t1 = std::chrono::high_resolution_clock::now();
        //do the point query and return the cluster the point belongs to
        call_lib_spatial(queryRectangle,  tree);
        t2 = std::chrono::high_resolution_clock::now();
        double slib_time = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
        times.push_back(slib_time);
        std::cout << "Retrieval time:" << slib_time << std::endl<<std::endl;
    }

    //print useful info & stats
    printInfo(tree, file, indexIdentifier);


    /*release memory. Order is important*/
    // delete the buffer first, then the storage manager
    // (otherwise the the buffer will fail trying to write the dirty entries).
    delete tree;
    delete file;
    delete in_mem_manager;

    calculate_statistics( times);

    return 0;
}

