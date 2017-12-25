//
// Created by Jacob Gresham on 12/23/17.
//

#ifndef JOINJS_MAP_H
#define JOINJS_MAP_H

#include <iostream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include "schema_parser.h"

using namespace rapidjson;
using namespace std;

class JsonMapper {
private:
    const char* schemaMapId;
    const char* columnPrefix;
    jjn::JsonSchema rootSchema;
    unordered_map<std::string, jjn::JsonSchema>* jsonSchemaMap;
    StringBuffer jsonStringBuffer;
    Writer<StringBuffer> *writer;

    void initialize(char* json, const char* schemaMapId, const char* columnPrefix, unordered_map<std::string, jjn::JsonSchema>* jsonSchemaMap) {
        try {
            auto console = spdlog::stdout_color_mt("json_mapper");
            this->schemaMapId = schemaMapId;
            this->columnPrefix = columnPrefix;
            this->jsonSchemaMap = jsonSchemaMap;
            writer = new Writer<StringBuffer>(jsonStringBuffer);
            console->info("JSON Mapper initialized");
            Reader reader;
            JsonMappingsHandler handler(this->jsonSchemaMap, schemaMapId);
            StringStream ss(json);
            reader.Parse(ss, handler);
            delete writer;
        } catch (const spdlog::spdlog_ex &ex) {
            std::cout << "Log init failed: " << ex.what() << std::endl;
            exit(1);
        }
    }

    class JsonMappingsHandler : BaseReaderHandler<UTF8<>, JsonMappingsHandler> {
    private:
        jjn::JsonSchema rootSchema; // The schema object grabbed from the mapid passed in
        unordered_map<std::string, jjn::JsonSchema>* jsonSchemaMap;
        const char *schemaMapId;

        void setRootSchema() {
            for (auto it = jsonSchemaMap->begin(); it != jsonSchemaMap->end(); it++) {
                if (strcmp(it->second.mapId.c_str(), schemaMapId) == 0) {
                    rootSchema = it->second; // set the root schema object
                    break;
                }
            }
        }

    public:

        JsonMappingsHandler(unordered_map<string, jjn::JsonSchema> *jsonSchemaMap, const char *schemaMapId) {
            this->jsonSchemaMap = jsonSchemaMap;
            this->schemaMapId = schemaMapId;
            setRootSchema();
        }

        bool Null() { cout << "Null()" << endl; return true; }
        bool Bool(bool b) { cout << "Bool(" << boolalpha << b << ")" << endl; return true; }
        bool Int(int i) { cout << "Int(" << i << ")" << endl; return true; }
        bool Uint(unsigned u) { cout << "Uint(" << u << ")" << endl; return true; }
        bool Int64(int64_t i) { cout << "Int64(" << i << ")" << endl; return true; }
        bool Uint64(uint64_t u) { cout << "Uint64(" << u << ")" << endl; return true; }
        bool Double(double d) { cout << "Double(" << d << ")" << endl; return true; }
        bool RawNumber(const char* str, rapidjson::SizeType length, bool copy) {
            cout << "Number(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;
            return true;
        }
        bool String(const char* str, SizeType length, bool copy) {
            cout << "String(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;
            return true;
        }
        bool StartObject() {
            cout << "StartObject()" << endl;

            return true;
        }
        bool Key(const char* str, SizeType length, bool copy) {
            cout << "Key(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;
            if (strcmp(rootSchema.mapId.c_str(), str) == 0) {

            } else {

            }
            return true;
        }
        bool EndObject(SizeType memberCount) { cout << "EndObject(" << memberCount << ")" << endl; return true; }
        bool StartArray() { cout << "StartArray()" << endl; return true; }
        bool EndArray(SizeType elementCount) { cout << "EndArray(" << elementCount << ")" << endl; return true; }
    };

public:

    JsonMapper(char* json, const char* schemaMapId, const char* columnPrefix, unordered_map<std::string, jjn::JsonSchema>* jsonSchemaMap) {
        initialize(json, schemaMapId, columnPrefix, jsonSchemaMap);
    }

    JsonMapper(char* json, const char* schemaMapId, unordered_map<std::string, jjn::JsonSchema>* jsonSchemaMap) {
        initialize(json, schemaMapId, NULL, jsonSchemaMap);
    }

    const char *getResult() {
        return jsonStringBuffer.GetString();
    }
};

#endif //JOINJS_MAP_H
