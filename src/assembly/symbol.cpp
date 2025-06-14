#include "symbol.hpp"

void resolveParsed(Parsed& parsed)
{
    for (const auto& sym : parsed.globals) {
        bool found = false;

        for (auto& section : parsed.sections) {
            auto it = section.labels.find(sym);
            if (it != section.labels.end()) {
                it->second.isGlobal = true;
                found = true;
                break;
            }
        }

        if (!found) {
            std::cerr << "Error: global symbol '" << sym << "' not defined\n";
        }
    }
}