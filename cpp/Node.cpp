#include "Node.h"
#include <iostream>

using namespace std;

Node::Node(Coordinates coordinates, int cost, int f_cost, Action action, Node* parent) {
    this->coordinates = coordinates;
    this->cost = cost;
    this->f_cost = f_cost;
    this->action = action;
    this->parent = parent;
    if (parent != nullptr){
        cout << "Parrent was" << parent << endl;
        cout << "Parrant coordinates" << parent->getCoordinates().x << " " << parent->getCoordinates().y << endl;
        cout << "New position" << coordinates.x << " " << coordinates.y << endl;
    }

}

Coordinates Node::getCoordinates() const {
    return this->coordinates;
}

int Node::getCost() const {
    return this->cost;
}

int Node::getF_cost() const {
    return this->f_cost;
}

Action Node::getAction() const {
    return this->action;
}

Node* Node::getParent() {
    return this->parent;
}

// reverse logic of < operator for priority queue
bool Node::operator<(const Node& other) const {
    return this->getF_cost() > other.getF_cost();
}

// reverse logic of < operator for priority queue
bool Node::operator>(const Node& other) const {
    return this->getF_cost() < other.getF_cost();
}

bool Node::operator==(const Node& other) const {
    return this->getCoordinates().x == other.getCoordinates().x && this->getCoordinates().y == other.getCoordinates().y;
}

bool Node::operator!=(const Node& other) const {
    return !(*this == other);
}

std::iostream& operator<<(std::iostream& os, Node& node) {
    os << "Node: " << node.getCoordinates().x << ", " << node.getCoordinates().y << endl;
    os << "Cost: " << node.getCost() << endl;
    return os;
}

std::iostream& operator<<(std::iostream& os, Action& action) {
    os << action.dx << action.dy << endl;
    return os;
}

Node::~Node() {}
