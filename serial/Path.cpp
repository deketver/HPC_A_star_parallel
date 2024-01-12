#include "Path.h"

Path::Path(Node& last_node) {
    this->total_cost = last_node.getCost();
    if (this->total_cost == -1)
    {
        this->nodes = {};
    }
    else {
        this->nodes.push_back(make_shared<Node>(last_node));
        shared_ptr<Node> new_node = last_node.getParent();
        while (new_node != nullptr) {
            this->nodes.push_back(new_node);
            new_node = new_node->getParent();
        }
    }

}

int Path::getTotalCost() {
    return this->total_cost;
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
        //cout << "Action " << (*this->nodes[i]).getAction().dx << " " << (*this->nodes[i]).getAction().dy << endl;
        cout << "[" <<(*this->nodes[i]).getCoordinates().x << "," << (*this->nodes[i]).getCoordinates().y << "] ";
    }

    cout << "\nTotal cost: " << this->total_cost << endl;
    cout << "Len of the path: " << this->nodes.size() << endl;
}