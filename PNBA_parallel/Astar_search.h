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
    vector<vector<bool>> visited;
    vector<vector<bool>> other_process_visited;
public:
    priority_queue<Node> queue;
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
    void initialize(); // initializes the search with start end, pushes start node to queue
    Node take_first_from_queue(); // takes the first node from the queue with the lowest value
    bool check_is_visited(int x, int y); // checks if the node is visited
    bool check_is_other_process_visited(int x, int y); // checks if the node is visited
    void mark_visited(int x, int y); // marks the node as visited
    void mark_other_process_visited(int x, int y); // marks the node as visited
    void expand_problem(Node& current_node); // expands the problem by adding new nodes to the queue

};