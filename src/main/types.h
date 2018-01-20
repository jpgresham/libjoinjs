//
// Created by Jacob Gresham on 12/29/17.
//

#ifndef JOINJS_TYPES_H
#define JOINJS_TYPES_H

#include <string>
#include "schema_parser.h"

namespace joinjs {
    enum nest_type{ROOT, ASSOCIATION, COLLECTION};
    typedef unordered_map<std::string, joinjs::JsonSchema> JSON_SCHEMA_MAP;
}

#endif //JOINJS_TYPES_H
