#include <iostream>
#include <vector>
#include <cstdlib>
#include "Astar_search.h"
#include "Path.h"
#include "Node.h"


using namespace std;
int main() {
    // create imput map
    int width = 2000;
    int height = 1000;
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
    Coordinates goal = Coordinates{ 958, 1588 };

    Astar_search problem = Astar_search(width, height, start, goal, map);
    cout << "start is " << map[start.x][start.y] << endl;
    cout << "finish is " << map[goal.x][goal.y] << endl;
    Path path = problem.search();
    // got over constructor
    path.print();

    return 0;
}