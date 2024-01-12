#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <fstream>
#include <sstream>
#include "Astar_search.h"
#include "Path.h"
#include "Node.h"

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

void save_matrix_file(const std::string& filename, std::vector<std::vector<unsigned short>>& matrix) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error creating matrix the file." << std::endl;
        return;
    }
    for (auto& row : matrix) {
        for (auto& value : row) {
            file << value << ",";
        }
        file << std::endl;
    }
    file.close();
}

void save_path(const std::string& filename, Path& path) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error creating solution file." << std::endl;
        return;
    }
    file << path;
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
    int width = 1500;
    int height = 1500;
    vector<vector<unsigned short>> map;
    for (int i = 0; i < height; i++) {
        vector<unsigned short> row;
        row.reserve(width);
        for (int j = 0; j < width; j++) {
            row.push_back(static_cast<unsigned short>(rand() % 10));
        }
        map.push_back(row);
    }
    std::ostringstream filename;
    filename << "map_" << width << ".txt";
    save_matrix_file(filename.str(), map);
    vector<vector<unsigned short>> second;
    read_matrix(filename.str(), second);


//    // print map
//    cout << "Map: " << endl;
//    for (unsigned short i = 0; i < map.size(); i++) {
//        for (unsigned short j = 0; j < map[i].size(); j++)
//            cout << map[i][j] << "\t";
//        cout << endl;
//    }
    Coordinates start = Coordinates{ 0, 5 };
    Coordinates goal = Coordinates{ 1484, 1494 };

    cout << "start is " << map[start.x][start.y] << endl;
    cout << "finish is " << map[goal.x][goal.y] << endl;
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    Astar_search problem = Astar_search(width, height, start, goal, map);
    Path path = problem.search();
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    cout << "Time difference = " << chrono::duration_cast<chrono::seconds>(end - begin).count() << "[s]" << endl;
    path.print_path_len();
    path.print_total_cost();

    // save path to file
    std::ostringstream path_filename;
    path_filename << "solution_" << width << ".txt";
    save_path(path_filename.str(), path);
    Path path2 = Path();
    read_path(path_filename.str(), path2);
    cout << "Paths are same: " << (path == path2) << endl;


    return 0;
}