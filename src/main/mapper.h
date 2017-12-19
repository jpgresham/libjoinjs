//
// Created by Jacob Gresham on 12/15/17.
//

#ifndef JOINJS_MAPPER_H
#define JOINJS_MAPPER_H

#include <iostream>
#include <rapidjson/rapidjson.h>
#include <rapidjson/encodings.h>
#include <rapidjson/reader.h>
#include <set>
#include <unordered_map>
#include <spdlog/spdlog.h>

using namespace std;
using namespace rapidjson;

namespace jjn {

    struct Associations {
        char *name;
        char *mapId;
    };

    struct Collections {
        char *name;
        char *mapId;
    };

    struct JsonSchema {
        char *schemaKey;
        char *mapId;
        char *idPropertyKey;
        set<string> *properties;
        set<Associations> *associations;
        set<Collections> *collections;

    };

    class SchemaJsonHandler  : public BaseReaderHandler<UTF8<>, SchemaJsonHandler> {
    private:
        unordered_map<string, JsonSchema> jsonSchemaSetPtr;
        shared_ptr<spdlog::logger> console = spdlog::stdout_color_mt("mapper");
        string lastKey;
        string currentObjectKey;
        int objectDepth = 0;
        int arrayDepth = 0;
        bool insidePropertiesArr = false;
        bool insideAssociationsObj = false;
        bool insideCollectionsObj = false;
    public:

        SchemaJsonHandler() {
            std::cout << "In the Constructor" << std::endl;
            jsonSchemaSetPtr = unordered_map<string, JsonSchema>();
        }

        bool Null() { std::cout << "Null()" << std::endl; return true; }
        bool Bool(bool b) { std::cout << "Bool(" << std::boolalpha << b << ")" << std::endl; return true; }
        bool Int(int i) { std::cout << "Int(" << i << ")" << std::endl; return true; }
        bool Uint(unsigned u) { std::cout << "Uint(" << u << ")" << std::endl; return true; }
        bool Int64(int64_t i) { std::cout << "Int64(" << i << ")" << std::endl; return true; }
        bool Uint64(uint64_t u) { std::cout << "Uint64(" << u << ")" << std::endl; return true; }
        bool Double(double d) { std::cout << "Double(" << d << ")" << std::endl; return true; }
        bool RawNumber(const char* str, rapidjson::SizeType length, bool copy) {
            std::cout << "Number(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
            return true;
        }
        bool String(const char* str, rapidjson::SizeType length, bool copy) {
            if (lastKey.empty()) {
                cout << "Setting up the JSON schema" << endl;
            } else {
                auto find = jsonSchemaSetPtr.find(currentObjectKey);
                if (find != jsonSchemaSetPtr.end()) {
                    JsonSchema *jsonSchema = &jsonSchemaSetPtr.at(currentObjectKey);
                    if (strcmp(lastKey.c_str(), "mapId") == 0) {
                        jsonSchema->mapId = (char *) malloc(length);
                        jsonSchema->mapId = const_cast<char *>(str);
                    }
                }
            }
            std::cout << "String(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
            return true;
        }
        bool StartObject() {
            lastKey = "";
            this->objectDepth++;
            std::cout << "StartObject()" << std::endl; return true;
        }
        bool Key(const char* str, rapidjson::SizeType length, bool copy) {
            lastKey = str;

            if (strcmp(str,"mapId") == 0) {
                cout << "";
            } else if (strcmp(str,"idProperty") == 0) {
                cout << "";
            } else if (strcmp(str, "properties") == 0) {

            } else if (strcmp(str,"name") == 0) {

            } else if (strcmp(str,"collections") == 0) {
                insideCollectionsObj = true;
            } else if (objectDepth == 1) { // We are in at the schema definition level.
                currentObjectKey = str;
                JsonSchema jsonSchema = JsonSchema();
                jsonSchemaSetPtr.insert(make_pair(str, jsonSchema));
            } else {
                cout << "We do not have a mapping for the provided key " << str << endl;
            }

            std::cout << "Key(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
            return true;
        }
        bool EndObject(rapidjson::SizeType memberCount) {
            this->objectDepth--;
            std::cout << "EndObject(" << memberCount << ")" << std::endl; return true;
        }
        bool StartArray() {
            this->arrayDepth++;
            std::cout << "StartArray()" << std::endl; return true;
        }
        bool EndArray(rapidjson::SizeType elementCount) {
            this->arrayDepth--;
            std::cout << "EndArray(" << elementCount << ")" << std::endl; return true;
        }
    };

}

#endif //JOINJS_MAPPER_H
