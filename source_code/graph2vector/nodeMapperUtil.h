#ifndef nodeMapperUtil_h_
#define nodeMapperUtil_h_
#include "../main/function.h"
enum string_code {
    pin,
    query,
    term,
};

string_code hashit (std::string const& inString) {
    if (inString == "pin") return pin;
    if (inString == "query") return query;
    if (inString == "term") return term;
}

#endif