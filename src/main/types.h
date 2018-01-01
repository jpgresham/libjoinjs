//
// Created by Jacob Gresham on 12/29/17.
//

#ifndef JOINJS_TYPES_H
#define JOINJS_TYPES_H

#include <string>
#include "schema_parser.h"

namespace joinjs {
    enum nested_map_type {collection, associative};
    typedef unordered_map<std::string, joinjs::JsonSchema> JSON_SCHEMA_MAP;
}

#endif //JOINJS_TYPES_H
