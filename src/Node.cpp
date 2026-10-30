#include "Node.h"
#include <algorithm>

void Node::leggTilKant(Kant* kant) {
    kanter.push_back(kant);
}

void Node::fjernKant(Kant* kant) {
    kanter.erase(std::remove(kanter.begin(), kanter.end(), kant), kanter.end());
}
