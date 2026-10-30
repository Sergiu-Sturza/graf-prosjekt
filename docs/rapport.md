# Laboppgave 2: Objektorientert implementasjon av grafer

**Språk:** C++17 · **Kompilator:** g++ · **Struktur:** `include/` (headere), `src/` (implementasjon), `tests/` (testsett), `docs/` (dokumentasjon)

## Oversikt og design

Grafen er implementert med tre klasser: `Graf`, `Node` og `Kant`. UML-klassediagram og designvalg ligger i [uml.md](uml.md).

- `Graf` eier alle `Node`- og `Kant`-objektene (`std::vector<Node*>` og `std::vector<Kant*>`) og sletter dem i destruktøren.
- `Node` har en ID og en liste med pekere til `Kant`-objekter. Den eier ikke kantene.
- `Kant` peker på de to nodene den forbinder og har en vekt.
- Grafen er enten retta eller uretta (`Graf(bool retta = false)`).

Siden `Graf` eier rå pekere, er kopiering slått av (`= delete`) og flytting implementert. Det gjør at algoritmene kan returnere et nytt `Graf`-objekt effektivt.

## Del 1: Grunnleggende grafimplementasjon

**1. Graf og Node.** `Node` har `int ID` og en liste med naboer. `Graf` har en `vector` med nodepekere. I første versjon var nabolisten `vector<Node*>`. I Del 2 ble den erstattet av `vector<Kant*>`.

**2. Legge til.**
- `bool Graf::leggTilNode(int ID)` oppretter noden med `new Node(ID)`. Duplikat-ID avvises (returnerer `false`).
- `bool Graf::leggTilKant(int fraID, int tilID, int vekt = 1)` finner begge nodene med `finnNode`. Avvises hvis en node mangler, hvis kanten finnes fra før, eller hvis den er en løkke (`fra == til`).

**3. Slette.**
- `bool Graf::slettKant(int fraID, int tilID)` fjerner kanten fra nodenes lister og fra grafens kantliste, og kaller `delete`.
- `bool Graf::slettNode(int ID)` fjerner først alle kanter som er knyttet til noden, og sletter så selve noden. Ikke-eksisterende node eller kant gir `false`, ikke feil.

## Del 2: Utvidet grafimplementasjon med kanter

**1. Kant.** `Kant` har pekere `fra` og `til`, `int vekt`, og `annenEnde(node)` som gir endepunktet som ikke er `node`.

**2. Oppdatert Node og Graf.** `Node` har nå `vector<Kant*> kanter` i stedet for direkte nabopekere. `leggTilKant` og `slettKant` jobber med `Kant`-objekter. Naboer finnes via `Graf::naboer(ID)`.

**3. Retta og uretta grafer.**

| | Uretta | Retta |
|---|---|---|
| `leggTilKant(a, b)` | Én `Kant`, pekeren legges hos både a og b | Pekeren legges bare hos a |
| `slettKant(a, b)` | Finner kanten uansett retning | Finner bare a→b |
| `slettNode` | Fjerner alle kanter som berører noden | Fjerner både inngående og utgående kanter. Inngående kanter ligger ikke hos noden, så grafens kantliste brukes til å finne dem |
| Dijkstra | Følger alle kanter | Følger bare utgående kanter |
| Minimalt spenntre | Prim og Kruskal | Støttes ikke (`std::invalid_argument`) |

## Del 3: Ekstraoppgaver

**1. Minimalt spenntre.** Både Prim (`minimaltSpenntrePrim(int startID)`, med prioritetskø) og Kruskal (`minimaltSpenntreKruskal()`, sortering og union-find) er implementert. Begge returnerer et nytt `Graf`-objekt og endrer ikke originalen. For en usammenhengende graf gir Prim spenntreet for startnodens komponent, og Kruskal en spenntreskog.

**2. Dijkstra.** `kortesteVeiTre(int startID, std::map<int,int>* avstand = nullptr)` bruker en prioritetskø og returnerer korteste vei-treet som nytt `Graf`-objekt. Avstandene kan hentes ut via `avstand`. Negative vekter avvises med `std::invalid_argument`. Noder som ikke kan nås fra startnoden er ikke med i treet.

**3. Retta grafer.** Dijkstra fungerer uten endringer i algoritmen. Det eneste som må til, er at hver node bare har utgående kanter i `kanter`-listen, og at resultatgrafen settes til retta. Minimalt spenntre er definert for uretta grafer. For retta grafer trengs en annen algoritme (Chu-Liu/Edmonds), og funksjonene kaster derfor `std::invalid_argument`.

## Testing

Testsettet fra oppgaven er implementert i `tests/testsett.cpp` (noder A–E har ID 1–5). Hele utskriften ligger i [testresultater.txt](testresultater.txt). Testene ble kompilert med `-Wall -Wextra -Wpedantic` og kjørt med AddressSanitizer og UBSan uten meldinger (ingen minnelekkasjer).

| Test | Scenario | Resultat |
|---|---|---|
| 1.1 | Legge til fem noder | OK, fem noder med ID 1–5 |
| 1.2 | Fjerne node 3 | OK, fire noder, ID 3 er borte |
| 2.1 | Kantene 1-2, 1-3, 2-4, 3-5 | OK, naboene stemmer for node 1, 2 og 3 |
| 2.2 | Fjerne kanten 1-3 | OK, node 1 har bare nabo 2, node 3 bare nabo 5 |
| 3.1 | Prim fra A | OK, kantene A-B(1), B-C(1), D-E(2), B-D(4), total vekt 8 |
| 3.2 | Kruskal | OK, samme kanter og vekt 8 |
| 4.1 | Dijkstra fra A | OK, kantene A-C(2), C-E(3), E-D(4), A-B(4). Avstander B=4, C=2, D=9, E=5 |
| 5.1 | Duplikat og sletting av ikke-eksisterende | OK, duplikat avvises, ingen feil |
| 5.2 | Slette alle noder og kanter | OK, grafen er tom |
| R.1–R.9 | Egne tester for retta grafer, feilhåndtering og uavhengighet mellom original og resultatgraf | OK |

**Totalt: 44 OK, 0 feil.**

## Kompilering og kjøring

```
g++ -std=c++17 -Iinclude src/Graf.cpp src/Node.cpp main.cpp -o graf
.\graf.exe
g++ -std=c++17 -Iinclude src/Graf.cpp src/Node.cpp tests/testsett.cpp -o testsett
.\testsett.exe
```
