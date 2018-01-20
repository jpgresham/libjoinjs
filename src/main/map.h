//
// Created by Jacob Gresham on 12/23/17.
//

#ifndef JOINJS_MAP_H
#define JOINJS_MAP_H

#include <iostream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include "schema_parser.h"
#include <set>
#include "json_parser.h"

using namespace rapidjson;
using namespace std;
using namespace joinjs;

class JsonMapper {
private:
    const char* schemaMapId;
    const char* columnPrefix;
    joinjs::JsonSchema rootSchema;
    unordered_map<std::string, joinjs::JsonSchema>* jsonSchemaMap;
    const char* resultString;

    void initialize(char* json, const char* schemaMapId, const char* columnPrefix, unordered_map<std::string, joinjs::JsonSchema>* jsonSchemaMap) {
        try {
            auto console = spdlog::stdout_color_mt("json_mapper");
            this->schemaMapId = schemaMapId;
            this->columnPrefix = columnPrefix;
            this->jsonSchemaMap = jsonSchemaMap;
            console->info("JSON Mapper initialized");
            Reader reader;
            JsonMappingsHandler handler(json, this->jsonSchemaMap, schemaMapId);
            StringStream ss(json);
            reader.Parse(ss, handler);
            resultString = handler.getResultString();
        } catch (const spdlog::spdlog_ex &ex) {
            std::cout << "Log init failed: " << ex.what() << std::endl;
            exit(1);
        }
    }

public:

    JsonMapper(char* json, const char* schemaMapId, const char* columnPrefix, unordered_map<std::string, joinjs::JsonSchema>* jsonSchemaMap) {
        initialize(json, schemaMapId, columnPrefix, jsonSchemaMap);
    }

    JsonMapper(char* json, const char* schemaMapId, unordered_map<std::string, joinjs::JsonSchema>* jsonSchemaMap) {
        initialize(json, schemaMapId, NULL, jsonSchemaMap);
    }

    const char *getResult() {
        return resultString;
    }
};

#endif //JOINJS_MAP_H
