#include "symbol.hpp"

#include "../Exception.hpp"

void resolveParsed(Parsed& parsed, Context& context)
{
    //TODO
    (void)context;
    for (const auto& sym : parsed.globals) {
        bool found = false;
        for (auto& section : parsed.sections)
        {
            auto it = section.labels.find(sym);
            if (it != section.labels.end())
            {
                it->second.isGlobal = true;
                found = true;
                break;
            }
        }
        if (!found)
            throw Exception::UndefinedSymbol("Error: global symbol '" + sym + "' not defined");
    }
}