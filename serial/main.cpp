#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include "Astar_search.h"
#include "Path.h"
#include "Node.h"



using namespace std;
int main() {
    // create imput map
    int width = 5000;
    int height = 5000;
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
    Coordinates goal = Coordinates{ 4858, 4988 };

    cout << "start is " << map[start.x][start.y] << endl;
    cout << "finish is " << map[goal.x][goal.y] << endl;
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    Astar_search problem = Astar_search(width, height, start, goal, map);
    Path path = problem.search();
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    cout << "Time difference = " << chrono::duration_cast<chrono::seconds>(end - begin).count() << "[s]" << endl;
    path.print_path_len();
    path.print_total_cost();

    return 0;
}