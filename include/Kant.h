#pragma once

class Node;

// En kant mellom to noder, med vekt.
// I en retta graf går kanten fra 'fra' til 'til'.
// I en uretta graf er rekkefølgen uten betydning.
class Kant {
private:
    Node* fra;
    Node* til;
    int vekt;

public:
    Kant(Node* fra, Node* til, int vekt = 1) : fra(fra), til(til), vekt(vekt) {}

    Node* hentFra() const { return fra; }
    Node* hentTil() const { return til; }
    int hentVekt() const { return vekt; }

    // Returnerer endepunktet som ikke er 'node'
    Node* annenEnde(const Node* node) const { return node == fra ? til : fra; }
};
