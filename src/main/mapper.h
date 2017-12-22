//
// Created by Jacob Gresham on 12/15/17.
//

#ifndef JOINJS_MAPPER_H
#define JOINJS_MAPPER_H
#define MAP_ID = "mapId"
#define ID_PROPERTY = "idProperty"
#define PROPERTIES = "properties"
#define ASSOCIATIONS = "associations"
#define NAME = "name"
#define COLLECTIONS = "collections"

#include <iostream>
#include <rapidjson/rapidjson.h>
#include <rapidjson/encodings.h>
#include <rapidjson/reader.h>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <vector>

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
        vector<string> *properties;
        vector<Associations> *associations;
        vector<Collections> *collections;

    };

    class SchemaJsonHandler  : public BaseReaderHandler<UTF8<>, SchemaJsonHandler> {
    private:
        unordered_map<string, JsonSchema> jsonSchemaUnorderedMap;
        shared_ptr<spdlog::logger> console = spdlog::stdout_color_mt("mapper");
        string lastKey;
        string currentSchemaKey;
        int objectDepth = 0;
        int arrayDepth = 0;
        bool insidePropertiesArr = false;
        bool insideAssociationsObj = false;
        bool insideCollectionsObj = false;
        int associationsObjCount = 0;
        int collectionsObjCount = 0;
    public:

        SchemaJsonHandler() {
            std::cout << "In the Constructor" << std::endl;
            jsonSchemaUnorderedMap = unordered_map<string, JsonSchema>();
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
                auto find = jsonSchemaUnorderedMap.find(currentSchemaKey);
                if (find != jsonSchemaUnorderedMap.end()) {
                    if (strcmp(lastKey.c_str(), "mapId") == 0) {
                        if (objectDepth == 2 && arrayDepth == 0) {
                            jsonSchemaUnorderedMap.at(currentSchemaKey).mapId = (char *) malloc(length);
                            jsonSchemaUnorderedMap.at(currentSchemaKey).mapId = const_cast<char *>(str);
                        } else if (objectDepth == 3 && arrayDepth == 1) {
                            if (strcmp(lastKey.c_str(), "associations") == 0) {
                               /* if () {
                                    Associations *associations = new Associations();
                                    jsonSchemaUnorderedMap.at(currentSchemaKey).associations->insert(jsonSchemaUnorderedMap.at(currentSchemaKey).associations->end(), *associations);
                                }*/
                                jsonSchemaUnorderedMap.at(currentSchemaKey).associations->back().mapId = (char *) malloc(length);
                                jsonSchemaUnorderedMap.at(currentSchemaKey).associations->back().mapId = const_cast<char *>(str);
                            } else if (strcmp(lastKey.c_str(),"collections") == 0) {
                                jsonSchemaUnorderedMap.at(currentSchemaKey).collections->back().mapId = (char *) malloc(length);
                                jsonSchemaUnorderedMap.at(currentSchemaKey).collections->back().mapId = const_cast<char *>(str);
                            } else {
                                throw "ERROR CREATING MAP ID";
                            }
                        }
                    } else if (strcmp(lastKey.c_str(), "idProperty") == 0) {
                        jsonSchemaUnorderedMap.at(currentSchemaKey).idPropertyKey = (char*) malloc(length);
                        jsonSchemaUnorderedMap.at(currentSchemaKey).idPropertyKey = const_cast<char *>(str);
                    } else if (strcmp(lastKey.c_str(), "name") == 0) {
                        // The name key only exists in foreign key mappings, etc collections and associations
                        if (arrayDepth == 1 && objectDepth == 3) {
                            // TODO
                        } else {
                            throw "Schema error detected. The \"name\" key must appear inside an array in either a \"properties\", \"collections\", or \"associations\" object.";
                        }
                    } else if (strcmp(lastKey.c_str(), "collections") == 0) {
                        jsonSchemaUnorderedMap.at(currentSchemaKey).collections = new vector<Collections>();
                    } else if (strcmp(lastKey.c_str(), "associations") == 0) {

                        if (jsonSchemaUnorderedMap.at(currentSchemaKey).associations == NULL) {
                            jsonSchemaUnorderedMap.at(currentSchemaKey).associations = new vector<Associations>();
                        }

                    } else if ((objectDepth == 2 && arrayDepth == 1) && insidePropertiesArr == true) {
                        if (jsonSchemaUnorderedMap.at(currentSchemaKey).properties == NULL) {
                            jsonSchemaUnorderedMap.at(currentSchemaKey).properties = new vector<string>();
                        }
                        jsonSchemaUnorderedMap.at(currentSchemaKey).properties->insert(jsonSchemaUnorderedMap.at(currentSchemaKey).properties->end(), str);
                    } else if (objectDepth == 1) {

                    } else {
                        // throw error
                        throw "Unable to find schema representation for ";
                    }
                }
            }
            std::cout << "String(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
            return true;
        }
        bool StartObject() {
            //lastKey = "";
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
                if (arrayDepth == 0 && objectDepth == 2) {
                    insidePropertiesArr = true;
                } else {
                    throw  "Schema error detected. The \"properties\" key must appear inside an array and schema object";
                }
            } else if (strcmp(str,"name") == 0) {
                if (arrayDepth != 1 || objectDepth != 3) {
                    throw "Schema error detected. The name key must appear inside an array and schema object.";
                }
            } else if (strcmp(str,"collections") == 0) {
                if (jsonSchemaUnorderedMap.at(currentSchemaKey).collections == NULL) {
                    jsonSchemaUnorderedMap.at(currentSchemaKey).collections = new vector<Collections>;
                }
                this->insideCollectionsObj = true;

            } else if (strcmp(str, "associations") == 0) {
                if (jsonSchemaUnorderedMap.at(currentSchemaKey).associations == NULL) {
                    jsonSchemaUnorderedMap.at(currentSchemaKey).associations = new vector<Associations>;
                }
                this->insideAssociationsObj = true;
            } else if (objectDepth == 1) { // We are in at the schema definition level.
                currentSchemaKey = str;
                jsonSchemaUnorderedMap.insert(make_pair(str, JsonSchema()));
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
            insideAssociationsObj = false;
            insidePropertiesArr = false;
            insideCollectionsObj = false;
            std::cout << "EndArray(" << elementCount << ")" << std::endl; return true;
        }

        unordered_map<string, JsonSchema> jsonSchema() {
            return jsonSchemaUnorderedMap;
        }

    };

}

#endif //JOINJS_MAPPER_H
