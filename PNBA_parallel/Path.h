#pragma once
#include "Node.h"
#include<vector>
#include<sstream>
#include<iostream>
#include<algorithm>

using namespace std;

class Path {
    int total_cost;
    int path_len = 0;
    vector<shared_ptr<Node>> nodes;
    vector<Coordinates> coordinates = {};
public:
    explicit Path(Node& last_node); //Node& last_node
    Path();
    int getTotalCost();
    int getPathLen();
    vector<int> getPathSend();
    vector<shared_ptr<Node>> getPath();
    void print();
    void printCoordinates();
    void print_path_len() const ;
    void print_total_cost() const;
    friend ostream& operator<<(ostream& os, Path& path);
    friend istream& operator>>(istream& is, Path& path);
    bool operator==(const Path& other) const;
};

