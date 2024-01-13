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
    int height = 10;
    
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
        problem.initialize();
        Coordinates last_coordinates = Coordinates{0, 0};

        while(!problem.queue.empty()) {
            Node current_node = problem.take_first_from_queue();

            if (problem.check_is_visited(current_node.getCoordinates().x, current_node.getCoordinates().y)) {
                continue;
            }
            // mark state visited
            problem.mark_visited(current_node.getCoordinates().x, current_node.getCoordinates().y);

            // now we should send a message of new visited state to the other process
            // we should send the coordinates of the visited state
            // we will use tags in order to diferentiate between message sending just
            // coordinates for updates and between found match in search space

            int coordinates[2] = {current_node.getCoordinates().x, current_node.getCoordinates().y};
            if (problem.check_is_other_process_visited(current_node.getCoordinates().x, current_node.getCoordinates().y)) {
                MPI_Send(coordinates, 2, MPI_INT, 1, 1, MPI_COMM_WORLD);
                cout << "Found match with other process" << endl;
                last_coordinates = current_node.getCoordinates();
                break;
            }
            else{
                MPI_Send(coordinates, 2, MPI_INT, 1, 0, MPI_COMM_WORLD);
            }


            int other_process_coordinates[2];
            MPI_Status status;
            MPI_Recv(other_process_coordinates, 2, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if(status.MPI_TAG == 1){
                cout << "Got message about match, my process" << rank << endl;
                break;
            }

            problem.mark_other_process_visited(other_process_coordinates[0], other_process_coordinates[1]);

//            if (problem.check_is_other_process_visited(current_node.getCoordinates().x, current_node.getCoordinates().y)) {
//                cout << "Found match with other process, my process" << rank << endl;
//                break;
//            }
            problem.expand_problem(current_node);

        }
        if (last_coordinates.x != 0 && last_coordinates.y != 0){
            cout << "Last coordinates from process " << rank << " were " << last_coordinates.x << " "
            << last_coordinates.y << endl;
        }

//        std::ostringstream path_filename;
//        path_filename << "solution_" << width << ".txt";
//        Path official_solution = Path();
//        read_path(path_filename.str(), official_solution);
//        cout << "Paths are same: " << (path == official_solution) << endl;
    }
    else{
        Astar_search problem = Astar_search(width, height, goal, start, map);
        problem.initialize();
        Coordinates last_coordinates = Coordinates{0, 0};

        while(!problem.queue.empty()) {
            Node current_node = problem.take_first_from_queue();
            if (problem.check_is_visited(current_node.getCoordinates().x, current_node.getCoordinates().y)) {
                continue;
            }
            // mark state visited
            problem.mark_visited(current_node.getCoordinates().x, current_node.getCoordinates().y);

            int other_process_coordinates[2];
            MPI_Status status;
            MPI_Recv(other_process_coordinates, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if(status.MPI_TAG == 1){
                cout << "Got message about match, my process" << rank << endl;
                break;
            }

            problem.mark_other_process_visited(other_process_coordinates[0], other_process_coordinates[1]);


            int coordinates[2] = {current_node.getCoordinates().x, current_node.getCoordinates().y};
            if (problem.check_is_other_process_visited(current_node.getCoordinates().x, current_node.getCoordinates().y)) {
                MPI_Send(coordinates, 2, MPI_INT, 0, 1, MPI_COMM_WORLD);
                cout << "Sending message about match, my process" << rank << endl;
                last_coordinates = current_node.getCoordinates();
                break;
            }
            else{
                MPI_Send(coordinates, 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }


//            if (problem.check_is_other_process_visited(current_node.getCoordinates().x, current_node.getCoordinates().y)) {
//                cout << "Found match with other process, my process" << rank << endl;
//                break;
//            }
            problem.expand_problem(current_node);

        }

//        std::ostringstream path_filename;
//        path_filename << "solution_" << width << ".txt";
//        Path official_solution = Path();
//        read_path(path_filename.str(), official_solution);
//        cout << "Paths are same: " << (path == official_solution) << endl;
    }

    MPI_Finalize();
    return 0;
}