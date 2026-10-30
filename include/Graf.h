#pragma once
#include <cstddef>
#include <map>
#include <ostream>
#include <vector>
#include "Kant.h"
#include "Node.h"

// Graf eier alle Node- og Kant-objektene og sletter dem i destruktøren.
// Grafen er enten retta eller uretta (bestemmes når den lages).
class Graf {
private:
    bool retta;
    std::vector<Node*> noder;
    std::vector<Kant*> kanter;

    Kant* finnKant(int fraID, int tilID) const;
    void slettKantObjekt(Kant* kant);
    void ryddOpp();

public:
    explicit Graf(bool retta = false);
    ~Graf();

    // Grafen eier rå pekere, så den kan flyttes men ikke kopieres
    Graf(const Graf&) = delete;
    Graf& operator=(const Graf&) = delete;
    Graf(Graf&& other) noexcept;
    Graf& operator=(Graf&& other) noexcept;

    // Del 1 og 2: legge til og slette. Returnerer true hvis grafen ble endret.
    bool leggTilNode(int ID);
    bool leggTilKant(int fraID, int tilID, int vekt = 1);
    bool slettNode(int ID);
    bool slettKant(int fraID, int tilID);

    // Oppslag
    bool erRetta() const;
    Node* finnNode(int ID) const;
    bool harKant(int fraID, int tilID) const;
    std::vector<int> naboer(int ID) const;   // sorterte ID-er man kan gå til fra noden
    std::size_t antallNoder() const;
    std::size_t antallKanter() const;
    bool erTom() const;
    int totalVekt() const;
    const std::vector<Node*>& hentNoder() const;
    const std::vector<Kant*>& hentKanter() const;
    void skrivUt(std::ostream& ut) const;

    // Del 3: alle returnerer et nytt Graf-objekt og endrer ikke originalen.
    // Minimalt spenntre finnes bare for uretta grafer (kaster std::invalid_argument ellers).
    Graf minimaltSpenntrePrim(int startID) const;
    Graf minimaltSpenntreKruskal() const;
    // Fungerer for både retta og uretta grafer. Negative vekter kaster std::invalid_argument.
    // Hvis 'avstand' er gitt, fylles den med korteste avstand fra startnoden til hver nåbar node.
    Graf kortesteVeiTre(int startID, std::map<int, int>* avstand = nullptr) const;
};
