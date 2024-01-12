#pragma once

#include <memory>
using namespace std;

struct Coordinates {
public:
    int x;
    int y;
};

//struct Action {
//    struct Up {
//        int dx = 0;
//        int dy = -1;
//    };
//    struct Down {
//        int dx = 0;
//        int dy = 1;
//    };
//    struct Left {
//        int dx = -1;
//        int dy = 0;
//    };
//    struct Right {
//        int dx = 1;
//        int dy = 0;
//    };
//public:
//    Up up;
//    Down down;
//    Left left;
//    Right right;
//};

enum Direction {
    Up,
    Down,
    Left,
    Right
};

struct Action {
    //Direction direction;
    int dx;
    int dy;
};

class Node {
    Coordinates coordinates{};
    int cost;
    int f_cost;
    Action action{};
    shared_ptr<Node> parent;
public:
    Node(Coordinates coordinates, int cost, int f_cost, Action action, shared_ptr<Node> parent);
    ~Node();
    Coordinates getCoordinates() const;
    int getCost() const;
    int getF_cost() const;
    Action getAction() const;
    shared_ptr<Node> getParent() const;
    bool operator<(const Node& other) const;
    bool operator>(const Node& other) const;
    bool operator==(const Node& other) const;
    bool operator!=(const Node& other) const;

};
