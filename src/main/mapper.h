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
        unordered_map<string, JsonSchema> *jsonSchemaSetPtr;
        string lastKey;
        int objectDepth = 0;
        int arrayDepth = 0;
    public:

        SchemaJsonHandler() {
            std::cout << "In the Constructor" << std::endl;
            jsonSchemaSetPtr = new unordered_map<string, JsonSchema>();
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
                auto find = jsonSchemaSetPtr->find(lastKey);
                if (find == jsonSchemaSetPtr->end()) {
                    jsonSchemaSetPtr->insert(make_pair(lastKey, *new JsonSchema()));
                    //this->jsonSchemaSetPtr->mapId = (char *) malloc(length);
                    //this->jsonSchemaSetPtr->mapId = const_cast<char *>(str);
                }
            }
            std::cout << "String(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
            return true;
        }
        bool StartObject() {
            lastKey = "";
            std::cout << "StartObject()" << std::endl; return true;
            objectDepth++;
        }
        bool Key(const char* str, rapidjson::SizeType length, bool copy) {
            lastKey = str;

            std::cout << "Key(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
            return true;
        }
        bool EndObject(rapidjson::SizeType memberCount) {
            std::cout << "EndObject(" << memberCount << ")" << std::endl; return true;
            objectDepth--;
        }
        bool StartArray() { std::cout << "StartArray()" << std::endl; return true; }
        bool EndArray(rapidjson::SizeType elementCount) { std::cout << "EndArray(" << elementCount << ")" << std::endl; return true; }
    };

}

#endif //JOINJS_MAPPER_H
