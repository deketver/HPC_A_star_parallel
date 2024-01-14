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

public:
    priority_queue<Node> queue;
    vector<shared_ptr<Node>> explored_nodes;
    Astar_search(int width, int height, Coordinates start, Coordinates goal, vector<vector<unsigned short>> map);
    Path search();
    bool isGoal(Node& node);
    bool checkBounds(Coordinates new_coordinates);
    vector<Action> getActions(Node& node);
    Coordinates getResult(Coordinates coordinates, Action action);
    int cost(Coordinates coordinates);
    int estimate(Coordinates coordinates);
    void initialize(); // initializes the search with start end, pushes start node to queue
    Node take_first_from_queue(); // takes the first node from the queue with the lowest value
    bool check_is_visited(int x, int y); // checks if the node is visited
    void mark_visited(int x, int y); // marks the node as visited
    vector<shared_ptr<Node>> expand_problem(Node& current_node);
    void try_match_parent_current_node(Node& current_node,int other_process_x, int other_process_y);
    Path find_in_explored_nodes(int other_process_x, int other_process_y);

};