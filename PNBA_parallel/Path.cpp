#include "Path.h"

Path::Path(Node& last_node) {
    this->total_cost = last_node.getCost();
    if (this->total_cost == -1)
    {
        this->nodes = {};
        this->coordinates = {};
    }
    else {
        this->nodes.push_back(make_shared<Node>(last_node));
        this->coordinates.push_back(last_node.getCoordinates());
        shared_ptr<Node> new_node = last_node.getParent();
        while (new_node != nullptr) {
            this->nodes.push_back(new_node);
            this->coordinates.push_back(new_node->getCoordinates());
            new_node = new_node->getParent();
        }
        std::reverse(this->coordinates.begin(), this->coordinates.end());
        this->path_len = this->nodes.size();
    }

}

Path::Path() {
    this->total_cost = 0;
    this->path_len = 0;
    this->nodes = {};
    this->coordinates = {};
}

int Path::getTotalCost() {
    return this->total_cost;
}

int Path::getPathLen() {
    return this->path_len;
}

vector<int> Path::getPathSend() {
    int len = this->path_len;
    std::vector<int> path_send(2 * len);
    for (int i = 0; i < this->path_len; i++) {
        path_send[2*i] = this->coordinates[i].x;
        path_send[2*i+1] = this->coordinates[i].y;
    }
    return path_send;
}

vector<shared_ptr<Node>> Path::getPath() {
    return this->nodes;
}

void Path::print() {
    if (this->nodes.empty()) {
        cout << "No path found" << endl;
        return;
    }
    cout << "Path: " << endl;
    // print the whole vector as a vector
    cout << "Coordinates " << endl;
    for (int i = this->nodes.size() - 1; i >= 0; i--) {
        cout << "[" <<(*this->nodes[i]).getCoordinates().x << "," << (*this->nodes[i]).getCoordinates().y << "] ";
    }
}

void Path::printCoordinates(){
    if (this->coordinates.empty()) {
        cout << "No path found" << endl;
        return;
    }
    for (int i = 0; i <= this->coordinates.size() - 1; i++) {
        cout << "[" <<this->coordinates[i].x << "," << this->coordinates[i].y << "] " << endl;
    }
}

void Path::print_path_len() const {
    cout << "Len of the path: " << this->nodes.size() << endl;
}

void Path::print_total_cost() const{
    cout << "Total cost: " << this->total_cost << endl;
}

ostream &operator<<(ostream &os, Path &path) {
    // print nicely path, so it can be easily saved and read from file
    for (int i = path.nodes.size() - 1; i >= 0; i--) {
        os  <<(*path.nodes[i]).getCoordinates().x << "," << (*path.nodes[i]).getCoordinates().y << endl;
    }
    return os;
}

istream &operator>>(istream &is, Path &path) {
    // read from input into a vector
    std::string line;
    while (std::getline(is, line)) {
        std::istringstream iss(line);
        Coordinates coordinates;

        char comma;
        if (iss >> coordinates.x >> comma >> coordinates.y) {
            path.coordinates.push_back(coordinates);
        }
    }
    return is;
}

bool Path::operator==(const Path &other) const {
    bool coordinates_equal = true;
    for (int i = 0; i < this->coordinates.size(); i++) {
        if (this->coordinates[i].x != other.coordinates[i].x ||
            this->coordinates[i].y != other.coordinates[i].y) {
            coordinates_equal = false;
            break;
        }
    }
    return coordinates_equal;
}


