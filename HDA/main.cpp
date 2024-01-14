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
 * In this project, we are trying to use HDA*
 * to calculate the cheapest path from start to goal.
 * We are using MPI to parallelize the algorithm.
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

int calculate_hash(int x, int y, int width, int number_processes) {
    int position = x * width + y + 1;
    return position % number_processes + 1;
}

int main() {
    int world_size;
    int rank;

    MPI_Init(NULL, NULL);
    // get number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // get  common rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::vector<int> input_size = {10};//, 100, 500, 1000, 1500, 2000, 2250, 2500, 2750, 3000, 3250, 3500, 3750, 4000};
    for (auto& size: input_size) {

        // create imput map
        int WIDTH = size;
        int HEIGHT = size;

        // create start and goal coordinates
        Coordinates START = Coordinates{0, 0}; // 0, 5
        Coordinates GOAL = Coordinates{size - 1, size - 1};

        // read the map from the file
        vector<vector<unsigned short>> map;
        std::ostringstream filename;
        filename << "/home/veronika.deketova/A_star_parralel/HPC_A_star_parallel/datasets/map_" << WIDTH << ".txt";
        read_matrix(filename.str(), map);

        int BEST_SOLUTION = std::numeric_limits<int>::max();

        int initial_coordinates_array[4];

        if (rank == 0) {
            // process 0 initiates the search and then sends states to respective processes based on the hash function

            Astar_search problem = Astar_search(WIDTH, HEIGHT, START, GOAL, map);
            Node start_node = Node(START, 0, 0, Action{0, 0}, nullptr);
            auto new_nodes = problem.expand_problem(start_node);

            for( int i = 0; i < new_nodes.size(); i++){
                Node node = *new_nodes[i];
                Coordinates coordinates = node.getCoordinates();
                initial_coordinates_array[2 *i] = coordinates.x;
                initial_coordinates_array[2 *i +1] = coordinates.y;
                cout << " sending coordinate x " << coordinates.x << endl;
                cout << " sending coordinate y" << coordinates.y << endl;
            }
           

        }
        // broadcast value
        MPI_Bcast(initial_coordinates_array, sizeof(initial_coordinates_array), MPI_INT, 0, MPI_COMM_WORLD);
        if (rank == 0) {

                cout << "Values broadcasted" << endl;

        }
        else{
            for (int i = 0; i < 4; i++){
                int x = initial_coordinates_array[2 *i];
                int y = initial_coordinates_array[2 *i +1];
                int hash = calculate_hash(x, y, WIDTH, world_size);
                if (hash == rank){
                    cout << "I am process " << rank << " and I have coordinates " << x << " " << y << endl;
                }
                else{
                    cout << "I am process " << rank << " and those values were not for me" << endl;
                }
            }

        }
    }
    MPI_Finalize();
    return 0;
}