#include <iostream>
#include <mpi.h>
#include <vector>
#include <cstdlib>
#include "Astar_search.h"
#include "Path.h"
#include "Node.h"

using namespace std;

/*
 * This algorithm approach uses only parallelization on 2 processes.
 * Bidirectional search heuristic search.
 * */

int main() {
    // create imput map
    int width = 300;
    int height = 200;
    
    int world_size;
    int rank;

    MPI_Init(NULL, NULL);

        // get number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    // get  common rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if( rank == 0){
        vector<vector<unsigned short>> map;
        for (int i = 0; i < height; i++) {
            vector<unsigned short> row;
            row.reserve(width);
            for (int j = 0; j < width; j++) {
                row.push_back(static_cast<unsigned short>(rand() % 10));
            }
            map.push_back(row);
        }
    //    // print map
    //    cout << "Map: " << endl;
    //    for (unsigned short i = 0; i < map.size(); i++) {
    //        for (unsigned short j = 0; j < map[i].size(); j++)
    //            cout << map[i][j] << "\t";
    //        cout << endl;
    //    }
        Coordinates start = Coordinates{ 0, 5 };
        Coordinates goal = Coordinates{ 155, 258 };

        cout << "start is " << map[start.x][start.y] << endl;
        cout << "finish is " << map[goal.x][goal.y] << endl;

        Astar_search problem = Astar_search(width, height, start, goal, map);
        Path path = problem.search();

        path.print_path_len();
        path.print_total_cost();
    }
    else{
        cout << "My rank is " << rank << endl;
    }

    MPI_Finalize();
    return 0;
}