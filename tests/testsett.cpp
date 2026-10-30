// Testsett for Laboppgave 2 (test 1-5 fra oppgaven + ekstra tester for retta grafer)
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>
#include "Graf.h"

namespace {

int antallOK = 0;
int antallFeil = 0;

void overskrift(const std::string& tekst) {
    std::cout << '\n' << tekst << '\n';
}

void sjekk(bool betingelse, const std::string& beskrivelse) {
    if (betingelse) {
        ++antallOK;
        std::cout << "  [OK]   " << beskrivelse << '\n';
    } else {
        ++antallFeil;
        std::cout << "  [FEIL] " << beskrivelse << '\n';
    }
}

std::vector<int> nodeIDer(const Graf& g) {
    std::vector<int> ider;
    for (Node* node : g.hentNoder()) {
        ider.push_back(node->hentID());
    }
    std::sort(ider.begin(), ider.end());
    return ider;
}

using Kantesett = std::set<std::tuple<int, int, int>>;

// Uretta kanter normaliseres slik at (1,2) og (2,1) blir samme kant
Kantesett kantesett(const Graf& g) {
    Kantesett resultat;
    for (Kant* kant : g.hentKanter()) {
        int a = kant->hentFra()->hentID();
        int b = kant->hentTil()->hentID();
        if (!g.erRetta() && a > b) {
            std::swap(a, b);
        }
        resultat.insert(std::make_tuple(a, b, kant->hentVekt()));
    }
    return resultat;
}

// Noder A-E har ID 1-5
constexpr int A = 1, B = 2, C = 3, D = 4, E = 5;

Graf lagMstGraf() {
    Graf g;
    for (int id : {A, B, C, D, E}) g.leggTilNode(id);
    g.leggTilKant(A, B, 1);
    g.leggTilKant(A, C, 3);
    g.leggTilKant(B, C, 1);
    g.leggTilKant(B, D, 4);
    g.leggTilKant(C, D, 5);
    g.leggTilKant(C, E, 6);
    g.leggTilKant(D, E, 2);
    return g;
}

Graf lagDijkstraGraf(bool retta = false) {
    Graf g(retta);
    for (int id : {A, B, C, D, E}) g.leggTilNode(id);
    g.leggTilKant(A, B, 4);
    g.leggTilKant(A, C, 2);
    g.leggTilKant(B, C, 5);
    g.leggTilKant(B, D, 10);
    g.leggTilKant(C, E, 3);
    g.leggTilKant(D, E, 4);
    return g;
}

Graf lagGrafMedFemNoder() {
    Graf g;
    for (int id = 1; id <= 5; ++id) g.leggTilNode(id);
    return g;
}

using IDer = std::vector<int>;

void test1() {
    overskrift("Test 1: Legge til og fjerne noder");

    Graf g;
    for (int id = 1; id <= 5; ++id) g.leggTilNode(id);
    sjekk(g.antallNoder() == 5, "1.1 grafen har fem noder");
    sjekk(nodeIDer(g) == IDer({1, 2, 3, 4, 5}), "1.1 nodene har ID 1, 2, 3, 4, 5");

    g.slettNode(3);
    sjekk(g.antallNoder() == 4, "1.2 grafen har fire noder etter sletting");
    sjekk(g.finnNode(3) == nullptr, "1.2 node 3 er ikke til stede");
    sjekk(nodeIDer(g) == IDer({1, 2, 4, 5}), "1.2 gjenværende noder er 1, 2, 4, 5");
}

void test2() {
    overskrift("Test 2: Legge til og fjerne kanter");

    Graf g = lagGrafMedFemNoder();
    g.leggTilKant(1, 2);
    g.leggTilKant(1, 3);
    g.leggTilKant(2, 4);
    g.leggTilKant(3, 5);
    sjekk(g.naboer(1) == IDer({2, 3}), "2.1 node 1 har naboene 2 og 3");
    sjekk(g.naboer(2) == IDer({1, 4}), "2.1 node 2 har naboene 1 og 4");
    sjekk(g.naboer(3) == IDer({1, 5}), "2.1 node 3 har naboene 1 og 5");
    sjekk(g.antallKanter() == 4, "2.1 grafen har fire kanter");

    g.slettKant(1, 3);
    sjekk(g.naboer(1) == IDer({2}), "2.2 node 1 har nå bare naboen 2");
    sjekk(g.naboer(3) == IDer({5}), "2.2 node 3 har nå bare naboen 5");
    sjekk(g.antallKanter() == 3, "2.2 grafen har tre kanter");
}

void test3() {
    overskrift("Test 3: Minimalt spenntre");

    Kantesett forventet = {std::make_tuple(A, B, 1), std::make_tuple(B, C, 1),
                           std::make_tuple(D, E, 2), std::make_tuple(B, D, 4)};

    Graf g = lagMstGraf();
    Graf prim = g.minimaltSpenntrePrim(A);
    sjekk(kantesett(prim) == forventet, "3.1 Prim: kantene er A-B(1), B-C(1), D-E(2), B-D(4)");
    sjekk(prim.totalVekt() == 8, "3.1 Prim: total vekt er 8");
    sjekk(prim.antallNoder() == 5 && prim.antallKanter() == 4, "3.1 Prim: 5 noder og 4 kanter");

    Graf prim2 = g.minimaltSpenntrePrim(C);
    sjekk(kantesett(prim2) == forventet, "3.1 Prim fra C gir samme spenntre");

    Graf kruskal = g.minimaltSpenntreKruskal();
    sjekk(kantesett(kruskal) == forventet, "3.2 Kruskal: samme kanter som Prim");
    sjekk(kruskal.totalVekt() == 8, "3.2 Kruskal: total vekt er 8");

    sjekk(g.antallNoder() == 5 && g.antallKanter() == 7 && g.totalVekt() == 22,
          "3.x originalgrafen er uendret (5 noder, 7 kanter, vekt 22)");
}

void test4() {
    overskrift("Test 4: Korteste vei (Dijkstra)");

    Kantesett forventet = {std::make_tuple(A, C, 2), std::make_tuple(C, E, 3),
                           std::make_tuple(D, E, 4), std::make_tuple(A, B, 4)};

    Graf g = lagDijkstraGraf();
    std::map<int, int> avstand;
    Graf tre = g.kortesteVeiTre(A, &avstand);

    sjekk(kantesett(tre) == forventet, "4.1 treet har kantene A-C(2), C-E(3), E-D(4), A-B(4)");
    sjekk(avstand[B] == 4, "4.1 avstand til B er 4");
    sjekk(avstand[C] == 2, "4.1 avstand til C er 2");
    sjekk(avstand[D] == 9, "4.1 avstand til D er 9 (via C og E)");
    sjekk(avstand[E] == 5, "4.1 avstand til E er 5 (via C)");
    sjekk(g.antallKanter() == 6 && g.totalVekt() == 28, "4.x originalgrafen er uendret");
}

void test5() {
    overskrift("Test 5: Robusthet");

    Graf g;
    bool forste = g.leggTilNode(6);
    bool andre = g.leggTilNode(6);
    g.leggTilNode(6);
    sjekk(forste && !andre, "5.1 andre forsøk på å legge til ID 6 avvises");
    sjekk(g.antallNoder() == 1, "5.1 grafen har bare én node med ID 6");

    bool slettet1 = g.slettNode(6);
    bool slettet2 = g.slettNode(6);
    bool slettet3 = g.slettNode(6);
    sjekk(slettet1 && !slettet2 && !slettet3, "5.1 sletting av ikke-eksisterende node gir ingen feil");
    sjekk(g.erTom(), "5.1 grafen er tom");
    sjekk(!g.slettKant(1, 2), "5.1 sletting av ikke-eksisterende kant gir ingen feil");

    Graf h = lagMstGraf();
    for (Kant* kant : std::vector<Kant*>(h.hentKanter())) {
        h.slettKant(kant->hentFra()->hentID(), kant->hentTil()->hentID());
    }
    sjekk(h.antallKanter() == 0 && h.antallNoder() == 5, "5.2 alle kanter slettet, nodene er igjen");
    for (int id : {A, B, C, D, E}) h.slettNode(id);
    sjekk(h.erTom(), "5.2 alle noder slettet, grafen er tom");

    Graf k = lagMstGraf();
    for (int id : {A, B, C, D, E}) k.slettNode(id);
    sjekk(k.erTom(), "5.2 sletting av noder fjerner også kantene (tom graf, 0 kanter)");
}

void testRetta() {
    overskrift("Ekstra: retta grafer (Del 2 og 3c)");

    Graf g(true);
    for (int id = 1; id <= 3; ++id) g.leggTilNode(id);
    g.leggTilKant(1, 2, 1);
    g.leggTilKant(2, 3, 1);
    sjekk(g.naboer(1) == IDer({2}) && g.naboer(2) == IDer({3}) && g.naboer(3).empty(),
          "R.1 kantene går bare én vei");
    sjekk(g.harKant(1, 2) && !g.harKant(2, 1), "R.2 kanten 1->2 finnes, 2->1 finnes ikke");
    sjekk(!g.slettKant(2, 1), "R.3 sletting av kant i feil retning avvises");
    sjekk(g.leggTilKant(2, 1, 1) && g.antallKanter() == 3, "R.4 motsatt retning kan legges til som egen kant");

    g.slettNode(2);
    sjekk(g.antallKanter() == 0 && g.naboer(1).empty(), "R.5 sletting av node fjerner både inngående og utgående kanter");

    Graf d(true);
    for (int id = 1; id <= 4; ++id) d.leggTilNode(id);
    d.leggTilKant(1, 2, 1);
    d.leggTilKant(2, 3, 1);
    d.leggTilKant(1, 3, 5);
    d.leggTilKant(4, 1, 1);
    std::map<int, int> avstand;
    Graf tre = d.kortesteVeiTre(1, &avstand);
    sjekk(tre.erRetta(), "R.6 Dijkstra-treet er også retta");
    sjekk(kantesett(tre) == Kantesett({std::make_tuple(1, 2, 1), std::make_tuple(2, 3, 1)}),
          "R.6 Dijkstra følger kantretningen (1->2->3 er kortere enn 1->3)");
    sjekk(avstand[3] == 2 && avstand.count(4) == 0, "R.6 node 4 kan ikke nås fra 1 og er ikke med");

    bool kastet = false;
    try {
        d.minimaltSpenntrePrim(1);
    } catch (const std::invalid_argument&) {
        kastet = true;
    }
    sjekk(kastet, "R.7 minimalt spenntre på retta graf avvises med unntak");

    Graf neg;
    neg.leggTilNode(1);
    neg.leggTilNode(2);
    neg.leggTilKant(1, 2, -1);
    kastet = false;
    try {
        neg.kortesteVeiTre(1);
    } catch (const std::invalid_argument&) {
        kastet = true;
    }
    sjekk(kastet, "R.8 negative vekter avvises i Dijkstra");

    Graf kilde = lagMstGraf();
    Graf kopi = kilde.minimaltSpenntrePrim(A);
    kilde.slettNode(A);
    sjekk(kopi.antallNoder() == 5 && kopi.antallKanter() == 4,
          "R.9 resultatgrafen er uavhengig av originalen");
}

}  // namespace

int main() {
    test1();
    test2();
    test3();
    test4();
    test5();
    testRetta();

    std::cout << "\nResultat: " << antallOK << " OK, " << antallFeil << " feil\n";
    return antallFeil == 0 ? 0 : 1;
}
