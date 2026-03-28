#ifndef REAGENT_H
#define REAGENT_H

#include <string>

// Tipe elemen kimia
enum class Element {
    HYDROGEN,   // H
    OXYGEN,     // O
    CARBON,     // C
    NITROGEN,   // N
    IRON,       // Fe
    SULFUR,     // S
    CALCIUM,    // Ca
    SODIUM      // Na
};

// Reagent adalah unit data — tidak butuh interface
struct Reagent {
    std::string symbol;   // "H", "O", "C", dst
    std::string name;     // "Hydrogen", "Oxygen", dst
    Element     element;
    int         potency;  // nilai dasar reagen
};

inline std::string elementSymbol(Element e) {
    switch (e) {
        case Element::HYDROGEN: return "H";
        case Element::OXYGEN:   return "O";
        case Element::CARBON:   return "C";
        case Element::NITROGEN: return "N";
        case Element::IRON:     return "Fe";
        case Element::SULFUR:   return "S";
        case Element::CALCIUM:  return "Ca";
        case Element::SODIUM:   return "Na";
        default:                return "?";
    }
}

#endif
