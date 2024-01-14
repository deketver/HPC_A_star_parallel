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

    std::vector<int> input_size = {500, 1000, 1500, 2000, 2250, 2500, 2750, 3000, 3250, 3500, 3750, 4000};
    for (auto& size: input_size) {
        cout << endl;
        cout << "Size is " << size << endl;

        // create imput map
        int width = size;
        int height = size;

        Coordinates start = Coordinates{0, 5}; // 0, 5
        Coordinates goal = Coordinates{size-16, size-6}; 

        vector<vector<unsigned short>> map;

        std::ostringstream filename;
        filename << "/home/veronika.deketova/A_star_parralel/HPC_A_star_parallel/datasets/map_" << width << ".txt";
        read_matrix(filename.str(), map);

        if (rank == 0) {

            int best_solution = std::numeric_limits<int>::max();
            int F_2 = 0;
            Coordinates best_solution_coordinates = Coordinates{0, 0};
            //unsigned int g_2 = 0;

            cout << "start is " << map[start.x][start.y] << endl;
            cout << "finish is " << map[goal.x][goal.y] << endl;
            double t1 = MPI_Wtime();
            Astar_search problem = Astar_search(width, height, start, goal, map);
            problem.initialize();
            Coordinates last_coordinates = Coordinates{0, 0};

            while (!problem.queue.empty()) {
                Node current_node = problem.take_first_from_queue();

                if (problem.check_is_visited(current_node.getCoordinates().x, current_node.getCoordinates().y)) {
                    continue;
                }

                if (current_node.getF_cost() >= best_solution) {
                    continue;
                }

                if(current_node.getCost() + F_2 - problem.estimate_other_process(current_node.getCoordinates()) >= best_solution){
                    continue;
                }

                // mark state visited
                problem.mark_visited(current_node.getCoordinates().x, current_node.getCoordinates().y);
                problem.explored_nodes.push_back(make_shared<Node>(current_node));

                // now we should send a message of new visited state to the other process
                // we should send the coordinates of the visited state
                // we will use tags in order to diferentiate between message sending just
                // coordinates for updates and between found match in search space


                if (problem.check_is_other_process_visited(current_node.getCoordinates().x,
                                                           current_node.getCoordinates().y)) {
                    // send message that you have found a match and wait for data from the other process

                    // get g2 for given coordinates
                    int g_2 = problem.other_process_costs[current_node.getCoordinates().x][current_node.getCoordinates().y];
                    int final_cost = current_node.getCost() + g_2 + map[goal.x][goal.y];
                    if (final_cost < best_solution) {
                        best_solution = final_cost;
                        best_solution_coordinates = current_node.getCoordinates();
                    }
                    // x, y, fcost, cost, best solution so far
                    int coordinates_and_costs[5] = {current_node.getCoordinates().x, current_node.getCoordinates().y,
                                                    current_node.getF_cost(), current_node.getCost(), best_solution};

                    MPI_Send(coordinates_and_costs, 5, MPI_INT, 1, 1, MPI_COMM_WORLD);

                    //last_coordinates = current_node.getCoordinates();

                    /* previous logic

                    // received data about other process search
                    //get len of the data vector first
                    int other_process_path_len;
                    MPI_Recv(&other_process_path_len, 1, MPI_INT, 1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    //cout << "Path length is " << other_process_path_len/2 << endl;

                    // get the whole path
                    std::vector<int> other_process_path(other_process_path_len);
                    MPI_Recv(&other_process_path[0], other_process_path_len, MPI_INT, 1, 2, MPI_COMM_WORLD,
                             MPI_STATUS_IGNORE);

                    // cost of this part of the solution
                    Path path = Path(current_node);
                    int cost =
                            path.getTotalCost() - map[current_node.getCoordinates().x][current_node.getCoordinates().y];
                    int path_len = path.getPathLen();
                    int total_path_len = path_len + other_process_path_len/2 - 1;

                    int other_process_cost;
                    MPI_Recv(&other_process_cost, 1, MPI_INT, 1, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    int final_cost = cost + other_process_cost + map[goal.x][goal.y];

                    cout << "Total final cost was: " << final_cost << endl;
                    cout << "Path len is " << total_path_len << endl;

                    break;
                     */
                } else {
                    int coordinates_and_costs[5] = {current_node.getCoordinates().x, current_node.getCoordinates().y,
                                                    current_node.getF_cost(), current_node.getCost(), best_solution};
                    MPI_Send(coordinates_and_costs, 5, MPI_INT, 1, 0, MPI_COMM_WORLD);

                    int other_process_coordinates_and_cost[4];
                    MPI_Status status;
                    MPI_Recv(other_process_coordinates_and_cost, 4, MPI_INT, 1, 0, MPI_COMM_WORLD, &status); //MPI_ANY_TAG
                    problem.mark_other_process_visited(other_process_coordinates_and_cost[0], other_process_coordinates_and_cost[1]);
                    F_2 = other_process_coordinates_and_cost[2];
                    problem.other_process_costs[other_process_coordinates_and_cost[0]][other_process_coordinates_and_cost[1]] = other_process_coordinates_and_cost[3];
                    //g_2 = other_process_coordinates_and_cost[3];
                    problem.expand_problem(current_node);
                }

            }
//            if (last_coordinates.x != 0 && last_coordinates.y != 0) {
//                cout << "Last coordinates from process " << rank << " were " << last_coordinates.x << " "
//                     << last_coordinates.y << endl;
//            }

            // now ask for the path from the best solution
            // and send the coordinates you want first
            int coordinates[2] = {best_solution_coordinates.x, best_solution_coordinates.y};
            MPI_Send(coordinates, 2, MPI_INT, 1, 2, MPI_COMM_WORLD); // tag 2 for solution sending

            // now wait to receive the path
            int other_process_path_len;
            MPI_Recv(&other_process_path_len, 1, MPI_INT, 1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // get the whole path
            std::vector<int> other_process_path(other_process_path_len);
            MPI_Recv(&other_process_path[0], other_process_path_len, MPI_INT, 1, 2, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);

            // cost of this part of the solution
            Path path = problem.find_in_explored_nodes(best_solution_coordinates.x, best_solution_coordinates.y);
            int cost =
                    path.getTotalCost() - map[best_solution_coordinates.x][best_solution_coordinates.y];
            int path_len = path.getPathLen();
            int total_path_len = path_len + other_process_path_len/2 - 1;

            int other_process_cost;
            MPI_Recv(&other_process_cost, 1, MPI_INT, 1, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            int final_cost = cost + other_process_cost + map[goal.x][goal.y];

            cout << "Total final cost was: " << final_cost << endl;
            cout << "Path len is " << total_path_len << endl;


            double t2 = MPI_Wtime();
            cout << "Time difference " << t2 - t1 << endl;

        } else {
            Astar_search problem = Astar_search(width, height, goal, start, map);
            problem.initialize();
            unsigned int F_1 = 0;
            //unsigned int g_1 = 0;

            unsigned int best_solution = std::numeric_limits<unsigned int>::max();

            while (!problem.queue.empty()) {
                Node current_node = problem.take_first_from_queue();

                if (problem.check_is_visited(current_node.getCoordinates().x, current_node.getCoordinates().y)) {
                    continue;
                }
                if (problem.check_is_other_process_visited(current_node.getCoordinates().x,
                                                           current_node.getCoordinates().y)) {
                    continue;
                }

                int other_process_coordinates_costs[5];
                MPI_Status status;
                MPI_Recv(other_process_coordinates_costs, 5, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

                if (status.MPI_TAG == 1) {

                    // now try to match node to the other process coordinates, so you can reconstruct the path
                    // and send the message back to the other process
//                    Path path = problem.find_in_explored_nodes(other_process_coordinates_costs[0],
//                                                               other_process_coordinates_costs[1]);
                    F_1 = other_process_coordinates_costs[2];
                    problem.other_process_costs[other_process_coordinates_costs[0]][other_process_coordinates_costs[1]] = other_process_coordinates_costs[3];
                    best_solution = other_process_coordinates_costs[4];

//                    int cost = path.getTotalCost();
//
//                    int path_len = 2 * path.getPathLen();
//                    vector<int> path_send = path.getPathSend();
//
//                    MPI_Send(&path_len, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
//                    MPI_Send(&path_send[0], path_len, MPI_INT, 0, 2, MPI_COMM_WORLD);
//                    MPI_Send(&cost, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
//                    break;
                } else {

                    problem.mark_other_process_visited(other_process_coordinates_costs[0], other_process_coordinates_costs[1]);
                    F_1 = other_process_coordinates_costs[2];

                    // assign g1 to appropriate place
                    problem.other_process_costs[other_process_coordinates_costs[0]][other_process_coordinates_costs[1]] = other_process_coordinates_costs[3];

                    // we got here, so now we know it is ok to explore current solution

                    // mark state visited
                    problem.mark_visited(current_node.getCoordinates().x, current_node.getCoordinates().y);
                    problem.explored_nodes.push_back(make_shared<Node>(current_node));

                    int coordinates_and_costs[4] = {current_node.getCoordinates().x, current_node.getCoordinates().y,
                                                    current_node.getF_cost(), current_node.getCost()};

                    MPI_Send(coordinates_and_costs, 4, MPI_INT, 0, 0, MPI_COMM_WORLD);

                    problem.expand_problem(current_node);
                }
            }
            // now we got our branch empty, so we need to wait for the other process
            // to send us final matching coordinates to recreate the path

            int best_solution_coordinates[0];
            MPI_Recv(&best_solution_coordinates, 2, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // now we have the coordinates, so we can recreate the path
            Path path = problem.find_in_explored_nodes(best_solution_coordinates[0],
                                                       best_solution_coordinates[1]);
            int cost = path.getTotalCost();

            int path_len = 2 * path.getPathLen();
            vector<int> path_send = path.getPathSend();

            MPI_Send(&path_len, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
            MPI_Send(&path_send[0], path_len, MPI_INT, 0, 2, MPI_COMM_WORLD);
            MPI_Send(&cost, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}