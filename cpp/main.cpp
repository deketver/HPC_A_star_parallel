#include <iostream>
#include <vector>
#include <cstdlib>
#include "Astar_search.h"
#include "Path.h"
#include "Node.h"


using namespace std;
int main() {
    // create imput map
    int width = 10;
    int height = 10;
    vector<vector<unsigned short>> map;
    for (int i = 0; i < height; i++) {
        vector<unsigned short> row;
        row.reserve(width);
        for (int j = 0; j < width; j++) {
            row.push_back(static_cast<unsigned short>(rand() % 100));
        }
        map.push_back(row);
    }
    cout << "Map: " << endl;

    for (unsigned short i = 0; i < map.size(); i++) {
        for (unsigned short j = 0; j < map[i].size(); j++)
            cout << map[i][j] << "\t";
        cout << endl;
    }

    Astar_search problem = Astar_search(width, height, Coordinates{ 1, 1 }, Coordinates{ 2, 3 }, map);
    cout << "start is" << map[1][1] << endl;
    cout << "finish is" << map[2][3] << endl;
    Path path = problem.search();
    // got over constructor
    cout << "Solllution:";
    path.print();

    return 0;
}