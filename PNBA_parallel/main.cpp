#include <iostream>
#include <mpi.h>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
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
    // create imput map
    int width = 10;
    int height = 200;
    
    int world_size;
    int rank;

    MPI_Init(NULL, NULL);

        // get number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    // get  common rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    Coordinates start = Coordinates{ 0, 5 };
    Coordinates goal = Coordinates{ 4, 4 };

    vector<vector<unsigned short>> map;

    std::ostringstream filename;
    filename << "map_" << width << ".txt";
    read_matrix(filename.str(), map);

    if( rank == 0){

        Astar_search problem = Astar_search(width, height, start, goal, map);
        Path path = problem.search();

        std::ostringstream path_filename;
        path_filename << "solution_" << width << ".txt";
        Path official_solution = Path();
        read_path(path_filename.str(), official_solution);
        cout << "Paths are same: " << (path == official_solution) << endl;
    }
    else{
        Astar_search problem = Astar_search(width, height, goal, start, map);
        Path path = problem.search();

        std::ostringstream path_filename;
        path_filename << "solution_" << width << ".txt";
        Path official_solution = Path();
        read_path(path_filename.str(), official_solution);
        cout << "Paths are same: " << (path == official_solution) << endl;
    }

    MPI_Finalize();
    return 0;
}