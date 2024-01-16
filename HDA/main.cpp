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

        Astar_search problem = Astar_search(WIDTH, HEIGHT, START, GOAL, map);
        int count_sent = 0;
        int count_received = 0;
        bool FINISHED = false;

        if (rank == 0) {
            // process 0 initiates the search and then sends states to respective processes based on the hash function


            Node start_node = Node(START, 0, 0, nullptr);
            auto new_nodes = problem.expand_problem(start_node);
            cout << "Number of processes " << world_size << endl;

            for (int i = 0; i < new_nodes.size(); i++) {
                int x = new_nodes[i]->getCoordinates().x;
                int y = new_nodes[i]->getCoordinates().y;
                int hash = calculate_hash(x, y, WIDTH, world_size-1);
                int coordinates_cost[4] = {x, y, new_nodes[i]->getCost(), BEST_SOLUTION};
                MPI_Send(&coordinates_cost, 4, MPI_INT, hash, 0, MPI_COMM_WORLD);
                //cout << "Sent to process " << hash << endl;
                count_sent++;
            }
        }
        else{
            // inicialization step for other processes
            int coordinates_cost[4];
            MPI_Status status;
            MPI_Recv(&coordinates_cost, 4, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if(status.MPI_TAG == 2){
                // finish you search
                FINISHED = true;
                break;
            }
            Coordinates coordinates = Coordinates{coordinates_cost[0], coordinates_cost[1]};
            BEST_SOLUTION = coordinates_cost[3];
            int f_cost_estimate = problem.estimate(coordinates);
            Node node = Node(coordinates, coordinates_cost[2], f_cost_estimate, nullptr);
            if (problem.isGoal(node)){
                cout << "I am process " << rank << " and I found the goal for price " << node.getCost() <<endl;
                int message[3] = {node.getCoordinates().x, node.getCoordinates().y, node.getCost()};
                MPI_Send(&message, 3, MPI_INT, 0, 1, MPI_COMM_WORLD);
            }
            else{
                auto new_nodes = problem.expand_problem(node);
                for (int i = 0; i < new_nodes.size(); i++) {
                    int x = new_nodes[i]->getCoordinates().x;
                    int y = new_nodes[i]->getCoordinates().y;
                    int hash = calculate_hash(x, y, WIDTH, world_size-1);
                    if (hash != rank){
                        int send_coordinates_cost[3] = {x, y, new_nodes[i]->getCost()};
                        MPI_Send(&send_coordinates_cost, 3, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    }
                    else{
                        problem.queue.push(*new_nodes[i]);
                    }
                }
            }
        }
        while(!FINISHED){
            if(rank == 0){
                // try to switch receive and send messages
                // try to receive first any source with any tag and then proceed with the send
                int receive_coordinates_cost[3];
                MPI_Status status;
                MPI_Recv(&receive_coordinates_cost, 3, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                count_received++;
                //cout << "Received message from " << status.MPI_SOURCE << endl;
                while(status.MPI_TAG == 1){
                    // update best value
                    if(receive_coordinates_cost[2] < BEST_SOLUTION){
                        BEST_SOLUTION = receive_coordinates_cost[2];
                    }
                    if(count_sent == count_received){
                        int message[4] = {0, 0, 0, BEST_SOLUTION};
                        cout << "Finishing, the best solution was: " << BEST_SOLUTION << endl;
                        for(int i = 0; i < world_size; i++){
                            MPI_Send(&message, 4, MPI_INT, i, 2, MPI_COMM_WORLD);
                        }
                        FINISHED = true;
                        break;
                    }
                    //receive_coordinates_cost[3];
                    //MPI_Status status;
                    MPI_Recv(&receive_coordinates_cost, 3, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                    //cout << "Received message from " << status.MPI_SOURCE << endl;
                    count_received++;

                }
//                if(status.MPI_TAG == 1){
//                    // update best value
//                    if(receive_coordinates_cost[2] < BEST_SOLUTION){
//                        BEST_SOLUTION = receive_coordinates_cost[2];
//                    }
//                    if (count_received == count_sent){
//                        int message[3] = {0, 0, 0};
//                        MPI_Send(&message, 3, MPI_INT, status.MPI_SOURCE, 2, MPI_COMM_WORLD);
//                        break;
//                    }
//                    MPI_Recv(&receive_coordinates_cost, 3, MPI_INT, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
//                    count_received++;
//
//                }
                if(FINISHED){
                    break;
                }
                // calculate hash
                int x = receive_coordinates_cost[0];
                int y = receive_coordinates_cost[1];

                if(count_sent == count_received){
                   int message[4] = {0, 0, 0, BEST_SOLUTION};
                   cout << "Finishing, the best solution was: " << BEST_SOLUTION << endl;
                   for(int i = 0; i < world_size; i++){
                       MPI_Send(&message, 4, MPI_INT, i, 2, MPI_COMM_WORLD);
                   }
                   break;
                   FINISHED = true;
               }
               // forwrd data
                int send_data[4] = {x, y, receive_coordinates_cost[2], BEST_SOLUTION};
                int hash = calculate_hash(x, y, WIDTH, world_size-1);
                MPI_Send(&send_data, 4, MPI_INT, hash, 0, MPI_COMM_WORLD);
                //cout << "sent to process " << hash << endl;
                count_sent++;

            }
            else{
                while(!problem.queue.empty()){
                    Node current_node = problem.take_first_from_queue();
                    if(problem.check_is_visited(current_node.getCoordinates().x, current_node.getCoordinates().y)) {
                        continue;
                    }
                    problem.mark_visited(current_node.getCoordinates().x, current_node.getCoordinates().y);
                    if (problem.isGoal(current_node)){
                        if(current_node.getCost() > BEST_SOLUTION){
                            continue;
                        }
                        cout << "I am process " << rank << " and I found the goal for price " << current_node.getCost() <<endl;
                        int message[3] = {current_node.getCoordinates().x, current_node.getCoordinates().y, current_node.getCost()};
                        MPI_Send(&message, 3, MPI_INT, 0, 1, MPI_COMM_WORLD);
                    }
                    else{
                        auto new_nodes = problem.expand_problem(current_node);
                        for (int i = 0; i < new_nodes.size(); i++) {
                            int x = new_nodes[i]->getCoordinates().x;
                            int y = new_nodes[i]->getCoordinates().y;
                            int hash = calculate_hash(x, y, WIDTH, world_size-1);
                            if (hash != rank){
                                int send_coordinates_cost[3] = {x, y, new_nodes[i]->getCost()};
                                MPI_Send(&send_coordinates_cost, 3, MPI_INT, 0, 0, MPI_COMM_WORLD);
                            }
                            else{
                                problem.queue.push(*new_nodes[i]);
                            }
                        }
                    }
                }
                int receive_coordinates_cost[4];
                MPI_Status status;
                MPI_Recv(&receive_coordinates_cost, 4, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                if(status.MPI_TAG == 2){
                    // finish you search
                    FINISHED = true;
                    break;
                }
                BEST_SOLUTION = receive_coordinates_cost[3];
                Coordinates coordinates = Coordinates{receive_coordinates_cost[0], receive_coordinates_cost[1]};
                if (problem.check_is_visited(coordinates.x, coordinates.y)){
                    continue;
                }
                int f_cost_estimate = problem.estimate(coordinates);
                Node node = Node(coordinates, receive_coordinates_cost[2], f_cost_estimate, nullptr);
                auto new_nodes = problem.expand_problem(node);
                for (int i = 0; i < new_nodes.size(); i++) {
                    int x = new_nodes[i]->getCoordinates().x;
                    int y = new_nodes[i]->getCoordinates().y;
                    int hash = calculate_hash(x, y, WIDTH, world_size-1);
                    if (hash != rank){
                        int send_coordinates_cost[3] = {x, y, new_nodes[i]->getCost()};
                        MPI_Send(&send_coordinates_cost, 3, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    }
                    else{
                        problem.queue.push(*new_nodes[i]);
                    }
                }

            }
        }
    }
    MPI_Finalize();
    return 0;
}