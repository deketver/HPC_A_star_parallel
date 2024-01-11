#include "Path.h"

Path::Path(Node& last_node) {
    this->total_cost = last_node.getF_cost();
    if (this->total_cost == -1)
    {
        this->nodes = {};
    }
    else {
        Node* current_node = &last_node;
        while (current_node != nullptr) {
            this->nodes.push_back(current_node);
            current_node = current_node->getParent();
        }
    }

}

int Path::getTotalCost() {
    return this->total_cost;
}

vector<Node*> Path::getPath() {
    return this->nodes;
}

void Path::print() {
    if (this->nodes.empty()) {
        cout << "No path found" << endl;
        return;
    }
    cout << "Path: " << endl;
    for (int i = this->nodes.size() - 1; i >= 0; i--) {
        cout << this->nodes[i];
    }
    cout << "Total cost: " << this->total_cost << endl;
}