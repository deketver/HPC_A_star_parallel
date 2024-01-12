#pragma once
#include "Node.h"
#include<vector>
#include<iostream>

using namespace std;

class Path {
    int total_cost;
    vector<shared_ptr<Node>> nodes;
public:
    explicit Path(Node& last_node); //Node& last_node
    int getTotalCost();
    vector<shared_ptr<Node>> getPath();
    void print();
    void print_path_len() const ;
    void print_total_cost() const;
};

