#pragma once
#include "Node.h"
#include "Path.h"
#include <vector>
#include <queue>
#include <iostream>


class Astar_search {
    int width;
    int height;
    Coordinates start;
    Coordinates goal;
    vector<vector<unsigned short>> map;
    //vector<vector<bool>> visited;
public:
    Astar_search(int width, int height, Coordinates start, Coordinates goal, vector<vector<unsigned short>> map);
    Path search();
    bool isGoal(Node& node);
    bool checkBounds(Coordinates new_coordinates);
    //    bool isOutOfBounds(Coordinates coordinates);
    //    bool isObstacle(Coordinates coordinates);
    //    bool isVisited(Coordinates coordinates);
    //    void printMap();
    //    void printVisited();
    //    void printPath(Path& path);
    //    void printNode(Node& node);
    //    void printCoordinates(Coordinates coordinates);
    //    void printSuccessors(vector<Node*> successors);
    //    void printQueue(priority_queue<Node*, vector<Node*>, CompareNodes> queue);
    vector<Action> getActions(Node& node);
    Coordinates getResult(Coordinates coordinates, Action action);
    int cost(Coordinates coordinates);
    int estimate(Coordinates coordinates);
};