# UML-klassediagram

```mermaid
classDiagram
    direction LR
    class Graf {
        -bool retta
        -vector~Node*~ noder
        -vector~Kant*~ kanter
        +Graf(bool retta = false)
        +~Graf()
        +leggTilNode(int ID) bool
        +leggTilKant(int fraID, int tilID, int vekt) bool
        +slettNode(int ID) bool
        +slettKant(int fraID, int tilID) bool
        +finnNode(int ID) Node∗
        +harKant(int fraID, int tilID) bool
        +naboer(int ID) vector~int~
        +minimaltSpenntrePrim(int startID) Graf
        +minimaltSpenntreKruskal() Graf
        +kortesteVeiTre(int startID, map* avstand) Graf
        -finnKant(int fraID, int tilID) Kant∗
        -slettKantObjekt(Kant* kant) void
    }
    class Node {
        -int ID
        -vector~Kant*~ kanter
        +Node(int ID)
        +hentID() int
        +hentKanter() vector~Kant*~
        +leggTilKant(Kant* kant) void
        +fjernKant(Kant* kant) void
    }
    class Kant {
        -Node* fra
        -Node* til
        -int vekt
        +Kant(Node* fra, Node* til, int vekt)
        +hentFra() Node∗
        +hentTil() Node∗
        +hentVekt() int
        +annenEnde(Node* node) Node∗
    }
    Graf "1" *-- "0..*" Node : eier
    Graf "1" *-- "0..*" Kant : eier
    Node "1" o-- "0..*" Kant : peker til
    Kant "1" --> "2" Node : forbinder
```

## Designvalg

- **Eierskap:** `Graf` eier alle `Node`- og `Kant`-objektene og sletter dem i destruktøren. `Node` og `Kant` har bare pekere som ikke eier. Dermed slettes ingenting dobbelt.
- **Uretta graf:** Én `Kant` lages, og pekeren legges i `kanter` hos *begge* noder.
- **Retta graf:** Kanten legges bare i `kanter` hos `fra`-noden. Retning styres av `Graf::retta`.
- **Del 1 til Del 2:** `Node` hadde først `vector<Node*> naboer`. I Del 2 ble denne erstattet av `vector<Kant*> kanter`. Naboer finnes nå via `Kant::annenEnde()`, eller `Graf::naboer(ID)`.
- **Kopiering:** `Graf` eier rå pekere, så kopiering er slått av (`= delete`). Grafen kan flyttes, og det brukes når algoritmene returnerer et nytt `Graf`-objekt.

## Endringer for retta grafer (krav i 3.2c)

| Funksjon | Uretta | Retta |
|---|---|---|
| `leggTilKant(a, b)` | Kanten legges hos både a og b | Kanten legges bare hos a |
| `slettKant(a, b)` | Finner kanten i begge retninger | Finner bare a→b |
| `slettNode` | Fjerner alle kanter som berører noden | Fjerner både inngående og utgående kanter (kantlisten i `Graf` brukes, siden inngående kanter ikke ligger hos noden) |
| Dijkstra | Følger alle kanter fra noden | Følger bare utgående kanter. Noder som ikke kan nås tas ikke med i treet. Resultatgrafen er retta |
| Minimalt spenntre | Prim og Kruskal | Støttes ikke (kaster `std::invalid_argument`). MST er definert for uretta grafer, mens retta grafer krever en annen algoritme (Chu-Liu/Edmonds) |
