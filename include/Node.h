#pragma once
#include <vector>

class Kant;

// En node i grafen. Noden eier ikke kantene sine, det gjør Graf.
class Node {
private:
    int ID;
    std::vector<Kant*> kanter;   // kanter som går ut fra (eller, i uretta graf, inn til) noden

public:
    explicit Node(int ID) : ID(ID) {}

    int hentID() const { return ID; }
    const std::vector<Kant*>& hentKanter() const { return kanter; }

    void leggTilKant(Kant* kant);
    void fjernKant(Kant* kant);
};
