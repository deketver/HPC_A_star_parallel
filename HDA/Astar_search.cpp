#include "Astar_search.h"

#include <utility>

Astar_search::Astar_search(int width, int height, Coordinates start, Coordinates goal, vector<vector<unsigned short>> map) {
    this->width = width;
    this->height = height;
    this->start = start;
    this->goal = goal;
    this->map = std::move(map);
    this->visited = vector<vector<bool>>(this->height, vector<bool>(this->width, false));
    this->queue = priority_queue<Node>();
}

bool Astar_search::isGoal(Node& node) {
    return node.getCoordinates().x == this->goal.x && node.getCoordinates().y == this->goal.y;
}

vector<Action> Astar_search::getActions(Node& node) {
    vector<Action> actions;
    for (int i = 0; i < 4; i++) {
        Action action;
        switch (i) {
            case 0:
                // Up
                action = Action{ 0, -1 };
                break;
            case 1:
                // Down
                action = Action{ 0, 1 };
                break;
            case 2:
                // Left
                action = Action{ -1, 0 };
                break;
            case 3:
                // Right
                action = Action{ 1, 0 };
                break;
        }
        Coordinates new_coordinates = this->getResult(node.getCoordinates(), action);
        if (this->checkBounds(new_coordinates)) {
            actions.push_back(action);
        }
    }
    return actions;
}

Coordinates Astar_search::getResult(Coordinates coordinates, Action action) {
    return Coordinates{ coordinates.x + action.dx, coordinates.y + action.dy };
}


bool Astar_search::checkBounds(Coordinates new_coordinates) {
    return new_coordinates.x >= 0 &&
           new_coordinates.x < this->height&&
           new_coordinates.y >= 0 &&
           new_coordinates.y < this->width;
}

// cost of the action
int Astar_search::cost(Coordinates new_coordinates) {
    return this->map[new_coordinates.x][new_coordinates.y];
}

int Astar_search::estimate(Coordinates coordinates) {
    //return abs(coordinates.x - this->start.x) + abs(coordinates.y - this->start.y);
    return abs(goal.x - coordinates.x) + abs(goal.y - coordinates.y);
}


void Astar_search::initialize(){
    Node const start_node = Node(this->start, 0, 0, Action{ 0, 0 }, nullptr);
    this->queue.push(start_node);
}

Node Astar_search::take_first_from_queue(){
    Node current_node = this->queue.top();
    this->queue.pop();
    return current_node;
}

bool Astar_search::check_is_visited(int x, int y){
    return this->visited[x][y];
}


void Astar_search::mark_visited(int x, int y){
    this->visited[x][y] = true;
}


vector<shared_ptr<Node>> Astar_search::expand_problem(Node& current_node){
    vector<Action> actions = this->getActions(current_node);
    vector<shared_ptr<Node>> new_nodes;
    for (Action action : actions) {
        Coordinates new_coordinates = this->getResult(current_node.getCoordinates(), action);
        int new_cost = current_node.getCost() + this->cost(new_coordinates);
        int new_f_cost = new_cost + this->estimate(new_coordinates);
        Node new_node = Node(new_coordinates, new_cost, new_f_cost, action, make_shared<Node>(current_node)); //return Path(std::make_shared<Node>(current_node));
        // is this check here ok in this distributed problem?
        if (!this->check_is_visited(new_coordinates.x, new_coordinates.y)){
            new_nodes.push_back(make_shared<Node>(new_node));
        }
    }
    return new_nodes;
}


void Astar_search::try_match_parent_current_node(Node& current_node,int other_process_x, int other_process_y){
    if (current_node.getCoordinates().x == other_process_x && current_node.getCoordinates().y == other_process_y){
        cout << "Direct match" << endl;
    }
    while(current_node.getParent() != nullptr){
        shared_ptr<Node> parent = current_node.getParent();
        if (parent->getCoordinates().x == other_process_x && parent->getCoordinates().y == other_process_y){
            cout << "Found match with other process" << endl;
            return;
        }
        current_node = *parent;
    }
    cout << "No match found" << endl;
}

Path Astar_search::find_in_explored_nodes(int other_process_x, int other_process_y){
    for (shared_ptr<Node> node : this->explored_nodes){
        if (node->getCoordinates().x == other_process_x && node->getCoordinates().y == other_process_y){
            Path path = Path(*node); // Path path
            return path;
        }
    }
    Path path = Path();
    //cout << "No match found" << endl;
    return path;
}

Path Astar_search::search() {
    Node const start_node = Node(this->start, 0, 0, Action{ 0, 0 }, nullptr);
    this->queue.push(start_node);

    // keep track of visited nodes
    //vector<vector<bool>> visited(this->height, vector<bool>(this->width, false));


    while (!queue.empty()) {
        Node current_node = this->queue.top();
        this->queue.pop();

        // check if node has been visited, or better
        if (this->visited[current_node.getCoordinates().x][current_node.getCoordinates().y]) {
            continue;
        }

        this->visited[current_node.getCoordinates().x][current_node.getCoordinates().y] = true;

        if (this->isGoal(current_node)) {
            cout << "Found solution" << endl;
            return Path(current_node);
        }

        int f_cost = current_node.getF_cost();

        vector<Action> actions = this->getActions(current_node);
        //cout << actions;
        for (Action action : actions) {
            Coordinates new_coordinates = this->getResult(current_node.getCoordinates(), action);
            int new_cost = current_node.getCost() + this->cost(new_coordinates);
            int new_f_cost = new_cost + this->estimate(new_coordinates);
            Node new_node = Node(new_coordinates, new_cost, new_f_cost, action, make_shared<Node>(current_node)); //return Path(std::make_shared<Node>(current_node));
            this->queue.push(new_node);
        }
    }
    // no solution found
    Node node = Node(Coordinates{ -1, -1 }, -1, -1, Action{ 0, 0 }, nullptr);
    return Path(node);
}
