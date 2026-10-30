#include <iostream>
#include <map>
#include "Graf.h"

int main() {
    // Eksempelgraf fra Test 3 (A-E = 1-5)
    Graf g;
    for (int id = 1; id <= 5; ++id) {
        g.leggTilNode(id);
    }
    g.leggTilKant(1, 2, 1);
    g.leggTilKant(1, 3, 3);
    g.leggTilKant(2, 3, 1);
    g.leggTilKant(2, 4, 4);
    g.leggTilKant(3, 4, 5);
    g.leggTilKant(3, 5, 6);
    g.leggTilKant(4, 5, 2);

    std::cout << "Original graf:\n";
    g.skrivUt(std::cout);

    Graf mst = g.minimaltSpenntrePrim(1);
    std::cout << "\nMinimalt spenntre (Prim fra node 1), total vekt " << mst.totalVekt() << ":\n";
    mst.skrivUt(std::cout);

    std::map<int, int> avstand;
    Graf tre = g.kortesteVeiTre(1, &avstand);
    std::cout << "\nKorteste vei-tre (Dijkstra fra node 1):\n";
    tre.skrivUt(std::cout);
    for (const auto& par : avstand) {
        std::cout << "  avstand til " << par.first << ": " << par.second << '\n';
    }
    return 0;
}
