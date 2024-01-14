#include <iostream>
#include <mpi.h>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <limits>
#include "Astar_search.h"
#include "Path.h"
#include "Node.h"

using namespace std;

/*
 * This algorithm approach uses only parallelization on 2 processes.
 * Bidirectional search heuristic search.
 * */
void read_matrix(const std::string& filename, std::vector<std::vector<unsigned short>>& matrix) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening the matrix file." << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        vector<unsigned short> row;

        int value;
        char comma;
        while (iss >> value >> comma) {
            row.push_back(value);
        }

        matrix.push_back(row);
    }
    file.close();
}

void read_path(const std::string& filename, Path& path) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error reading file." << std::endl;
        return;
    }
    file >> path;
    file.close();
}

int main() {
    int world_size;
    int rank;

    MPI_Init(NULL, NULL);
    // get number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // get  common rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::vector<int> input_size = {10, 100, 500, 1000, 1500, 2000, 2250, 2500, 2750, 3000, 3250, 3500, 3750, 4000};
    for (auto& size: input_size) {
        cout << endl;
        cout << "Size is " << size << endl;

        // create imput map
        int width = size;
        int height = size;

        // create start and goal coordinates
        Coordinates start = Coordinates{0, 0}; // 0, 5
        Coordinates goal = Coordinates{size - 1, size - 1};

        // read the map from the file
        vector<vector<unsigned short>> map;
        std::ostringstream filename;
        filename << "/home/veronika.deketova/A_star_parralel/HPC_A_star_parallel/datasets/map_" << width << ".txt";
        read_matrix(filename.str(), map);

        // now different logic based on the rank
        if (rank == 0){


        }
        else{

        }
    }
    MPI_Finalize();
    return 0;
}