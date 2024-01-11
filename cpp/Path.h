#pragma once
#include "Node.h"
#include<vector>
#include<iostream>

using namespace std;

class Path {
    int total_cost;
    vector<Node*> nodes;
public:
    Path(Node& last_node);
    int getTotalCost();
    vector<Node*> getPath();
    void print();
};

