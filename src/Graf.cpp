#include "Graf.h"
#include <algorithm>
#include <functional>
#include <queue>
#include <set>
#include <stdexcept>
#include <utility>

namespace {

// Hjelpeklasse for Kruskal: holder styr på hvilke noder som allerede henger sammen
class UnionFind {
private:
    std::map<int, int> forelder;

public:
    void lagMengde(int x) { forelder[x] = x; }

    int finn(int x) {
        while (forelder[x] != x) {
            forelder[x] = forelder[forelder[x]];
            x = forelder[x];
        }
        return x;
    }

    // Returnerer false hvis a og b allerede er i samme mengde (kanten ville laget en syklus)
    bool slaaSammen(int a, int b) {
        int rotA = finn(a);
        int rotB = finn(b);
        if (rotA == rotB) {
            return false;
        }
        forelder[rotA] = rotB;
        return true;
    }
};

}  // namespace

// ---------- Konstruktører og destruktør ----------

Graf::Graf(bool retta) : retta(retta) {}

Graf::~Graf() {
    ryddOpp();
}

Graf::Graf(Graf&& other) noexcept
    : retta(other.retta), noder(std::move(other.noder)), kanter(std::move(other.kanter)) {
    other.noder.clear();
    other.kanter.clear();
}

Graf& Graf::operator=(Graf&& other) noexcept {
    if (this != &other) {
        ryddOpp();
        retta = other.retta;
        noder = std::move(other.noder);
        kanter = std::move(other.kanter);
        other.noder.clear();
        other.kanter.clear();
    }
    return *this;
}

void Graf::ryddOpp() {
    for (Kant* kant : kanter) {
        delete kant;
    }
    for (Node* node : noder) {
        delete node;
    }
    kanter.clear();
    noder.clear();
}

// ---------- Legge til ----------

bool Graf::leggTilNode(int ID) {
    if (finnNode(ID) != nullptr) {
        return false;   // ingen duplikater
    }
    noder.push_back(new Node(ID));
    return true;
}

bool Graf::leggTilKant(int fraID, int tilID, int vekt) {
    if (fraID == tilID) {
        return false;   // ingen løkker
    }
    Node* fra = finnNode(fraID);
    Node* til = finnNode(tilID);
    if (fra == nullptr || til == nullptr || finnKant(fraID, tilID) != nullptr) {
        return false;
    }

    Kant* kant = new Kant(fra, til, vekt);
    kanter.push_back(kant);
    fra->leggTilKant(kant);
    if (!retta) {
        til->leggTilKant(kant);   // uretta: samme kant nås fra begge ender
    }
    return true;
}

// ---------- Slette ----------

void Graf::slettKantObjekt(Kant* kant) {
    kant->hentFra()->fjernKant(kant);
    if (!retta) {
        kant->hentTil()->fjernKant(kant);
    }
    kanter.erase(std::remove(kanter.begin(), kanter.end(), kant), kanter.end());
    delete kant;
}

bool Graf::slettKant(int fraID, int tilID) {
    Kant* kant = finnKant(fraID, tilID);
    if (kant == nullptr) {
        return false;
    }
    slettKantObjekt(kant);
    return true;
}

bool Graf::slettNode(int ID) {
    Node* node = finnNode(ID);
    if (node == nullptr) {
        return false;
    }

    // Fjern alle kanter som er knyttet til noden (både inn og ut)
    std::vector<Kant*> kopi = kanter;
    for (Kant* kant : kopi) {
        if (kant->hentFra() == node || kant->hentTil() == node) {
            slettKantObjekt(kant);
        }
    }

    noder.erase(std::remove(noder.begin(), noder.end(), node), noder.end());
    delete node;
    return true;
}

// ---------- Oppslag ----------

Node* Graf::finnNode(int ID) const {
    for (Node* node : noder) {
        if (node->hentID() == ID) {
            return node;
        }
    }
    return nullptr;
}

Kant* Graf::finnKant(int fraID, int tilID) const {
    for (Kant* kant : kanter) {
        int a = kant->hentFra()->hentID();
        int b = kant->hentTil()->hentID();
        if (a == fraID && b == tilID) {
            return kant;
        }
        if (!retta && a == tilID && b == fraID) {
            return kant;
        }
    }
    return nullptr;
}

bool Graf::harKant(int fraID, int tilID) const {
    return finnKant(fraID, tilID) != nullptr;
}

std::vector<int> Graf::naboer(int ID) const {
    std::vector<int> resultat;
    Node* node = finnNode(ID);
    if (node == nullptr) {
        return resultat;
    }
    for (Kant* kant : node->hentKanter()) {
        resultat.push_back(kant->annenEnde(node)->hentID());
    }
    std::sort(resultat.begin(), resultat.end());
    return resultat;
}

bool Graf::erRetta() const { return retta; }
std::size_t Graf::antallNoder() const { return noder.size(); }
std::size_t Graf::antallKanter() const { return kanter.size(); }
bool Graf::erTom() const { return noder.empty() && kanter.empty(); }
const std::vector<Node*>& Graf::hentNoder() const { return noder; }
const std::vector<Kant*>& Graf::hentKanter() const { return kanter; }

int Graf::totalVekt() const {
    int sum = 0;
    for (Kant* kant : kanter) {
        sum += kant->hentVekt();
    }
    return sum;
}

void Graf::skrivUt(std::ostream& ut) const {
    for (Node* node : noder) {
        ut << node->hentID() << ":";
        for (Kant* kant : node->hentKanter()) {
            ut << ' ' << kant->annenEnde(node)->hentID() << '(' << kant->hentVekt() << ')';
        }
        ut << '\n';
    }
}

// ---------- Del 3: algoritmer ----------

Graf Graf::minimaltSpenntrePrim(int startID) const {
    if (retta) {
        throw std::invalid_argument("Minimalt spenntre er kun definert for uretta grafer");
    }

    Graf mst(false);
    Node* start = finnNode(startID);
    if (start == nullptr) {
        return mst;
    }

    struct Kandidat {
        int vekt;
        Kant* kant;
        Node* til;   // noden utenfor treet som kanten fører til
    };
    auto storre = [](const Kandidat& a, const Kandidat& b) { return a.vekt > b.vekt; };
    std::priority_queue<Kandidat, std::vector<Kandidat>, decltype(storre)> ko(storre);

    std::set<int> iTreet;
    iTreet.insert(startID);
    mst.leggTilNode(startID);
    for (Kant* kant : start->hentKanter()) {
        ko.push({kant->hentVekt(), kant, kant->annenEnde(start)});
    }

    while (!ko.empty()) {
        Kandidat valgt = ko.top();
        ko.pop();

        int tilID = valgt.til->hentID();
        if (iTreet.count(tilID) > 0) {
            continue;   // noden er allerede med
        }

        iTreet.insert(tilID);
        mst.leggTilNode(tilID);
        mst.leggTilKant(valgt.kant->hentFra()->hentID(), valgt.kant->hentTil()->hentID(),
                        valgt.kant->hentVekt());

        for (Kant* kant : valgt.til->hentKanter()) {
            Node* annen = kant->annenEnde(valgt.til);
            if (iTreet.count(annen->hentID()) == 0) {
                ko.push({kant->hentVekt(), kant, annen});
            }
        }
    }
    return mst;
}

Graf Graf::minimaltSpenntreKruskal() const {
    if (retta) {
        throw std::invalid_argument("Minimalt spenntre er kun definert for uretta grafer");
    }

    Graf mst(false);
    UnionFind grupper;
    for (Node* node : noder) {
        mst.leggTilNode(node->hentID());
        grupper.lagMengde(node->hentID());
    }

    std::vector<Kant*> sortert = kanter;
    std::stable_sort(sortert.begin(), sortert.end(),
                     [](const Kant* a, const Kant* b) { return a->hentVekt() < b->hentVekt(); });

    for (Kant* kant : sortert) {
        int a = kant->hentFra()->hentID();
        int b = kant->hentTil()->hentID();
        if (grupper.slaaSammen(a, b)) {
            mst.leggTilKant(a, b, kant->hentVekt());
        }
    }
    return mst;
}

Graf Graf::kortesteVeiTre(int startID, std::map<int, int>* avstandUt) const {
    Graf tre(retta);
    Node* start = finnNode(startID);
    if (start == nullptr) {
        return tre;
    }
    for (Kant* kant : kanter) {
        if (kant->hentVekt() < 0) {
            throw std::invalid_argument("Dijkstra støtter ikke negative vekter");
        }
    }

    std::map<int, int> avstand;       // beste kjente avstand fra start
    std::map<int, Kant*> forelder;    // kanten som ga beste avstand
    std::set<int> ferdig;

    using Par = std::pair<int, int>;  // (avstand, node-ID)
    std::priority_queue<Par, std::vector<Par>, std::greater<Par>> ko;
    avstand[startID] = 0;
    ko.push({0, startID});

    while (!ko.empty()) {
        Par topp = ko.top();
        ko.pop();
        int d = topp.first;
        int id = topp.second;
        if (ferdig.count(id) > 0) {
            continue;
        }
        ferdig.insert(id);

        Node* u = finnNode(id);
        // I en retta graf ligger bare utgående kanter i hentKanter(), så retningen respekteres
        for (Kant* kant : u->hentKanter()) {
            Node* v = kant->annenEnde(u);
            int nyAvstand = d + kant->hentVekt();
            auto it = avstand.find(v->hentID());
            if (it == avstand.end() || nyAvstand < it->second) {
                avstand[v->hentID()] = nyAvstand;
                forelder[v->hentID()] = kant;
                ko.push({nyAvstand, v->hentID()});
            }
        }
    }

    for (const auto& par : avstand) {
        tre.leggTilNode(par.first);
    }
    for (const auto& par : forelder) {
        Kant* kant = par.second;
        tre.leggTilKant(kant->hentFra()->hentID(), kant->hentTil()->hentID(), kant->hentVekt());
    }

    if (avstandUt != nullptr) {
        *avstandUt = avstand;
    }
    return tre;
}
