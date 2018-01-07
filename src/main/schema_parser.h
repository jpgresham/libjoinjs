//
// Created by Jacob Gresham on 12/15/17.
//

#ifndef JOINJS_MAPPER_H
#define JOINJS_MAPPER_H

#include <iostream>
#include <rapidjson/rapidjson.h>
#include <rapidjson/encodings.h>
#include <rapidjson/reader.h>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <vector>
#include "exceptions.h"

using namespace std;
using namespace rapidjson;

namespace joinjs {

    struct Associations {
        string name;
        string mapId;
    };

    struct Collections {
        string name;
        string mapId;
    };

    struct JsonSchema {
        string schemaKey;
        string mapId;
        string idPropertyKey;
        vector<string> properties;
        vector<Associations> associations;
        vector<Collections> collections;

    };

    class SchemaJsonHandler  : public BaseReaderHandler<UTF8<>, SchemaJsonHandler> {
    private:
        unordered_map<string, JsonSchema> jsonSchemaUnorderedMap;
        shared_ptr<spdlog::logger> console = spdlog::stdout_color_mt("schema_mapper");
        string lastKey;
        string currentSchemaKey;
        int objectDepth = 0;
        int arrayDepth = 0;
        bool insidePropertiesArray = false;
        bool insideAssociationsArray = false;
        bool insideCollectionsArray = false;
        int associationsObjCount = 0;
        int collectionsObjCount = 0;

        SchemaJsonHandler() {
            jsonSchemaUnorderedMap = unordered_map<string, JsonSchema>();
        }

    public:

        static SchemaJsonHandler& getInstance() {
            static SchemaJsonHandler instance;
            return instance;
        }

        unordered_map<string, JsonSchema>* getJsonSchema() {
            unordered_map<string, JsonSchema> *schema = &jsonSchemaUnorderedMap;
            return schema;
        }

        bool Null() { return true; }
        bool Bool(bool b) { return true; }
        bool Int(int i) { return true; }
        bool Uint(unsigned u) { return true; }
        bool Int64(int64_t i) { return true; }
        bool Uint64(uint64_t u) { return true; }
        bool Double(double d) { return true; }
        bool RawNumber(const char* str, rapidjson::SizeType length, bool copy) {
            return true;
        }
        bool String(const char* str, rapidjson::SizeType length, bool copy) {
            try {
                if (!lastKey.empty()) {
                    auto find = jsonSchemaUnorderedMap.find(currentSchemaKey);
                    if (find != jsonSchemaUnorderedMap.end()) {
                        if (strcmp(lastKey.c_str(), "mapId") == 0) { // MAPPING FOR MAP ID KEY
                            if (objectDepth == 2 && arrayDepth == 0) {
                                jsonSchemaUnorderedMap.at(currentSchemaKey).mapId = (char *) malloc(length);
                                jsonSchemaUnorderedMap.at(currentSchemaKey).mapId = const_cast<char *>(str);
                            } else if (objectDepth == 3 && arrayDepth == 1) {
                                if (insideAssociationsArray) {
                                    if (associationsObjCount > jsonSchemaUnorderedMap.at(currentSchemaKey).associations.size()) {
                                         Associations associations = Associations();
                                         jsonSchemaUnorderedMap.at(currentSchemaKey).associations.insert(jsonSchemaUnorderedMap.at(currentSchemaKey).associations.end(), associations);
                                     }
                                    jsonSchemaUnorderedMap.at(currentSchemaKey).associations.back().mapId = string(str);
                                } else if (insideCollectionsArray) {
                                    if (collectionsObjCount > jsonSchemaUnorderedMap.at(currentSchemaKey).collections.size()) {
                                        Collections collections = Collections();
                                        jsonSchemaUnorderedMap.at(currentSchemaKey).collections.insert(jsonSchemaUnorderedMap.at(currentSchemaKey).collections.end(), collections);
                                    }
                                    jsonSchemaUnorderedMap.at(currentSchemaKey).collections.back().mapId = string(str);
                                } else {
                                    throw "ERROR CREATING MAP ID";
                                }
                            }
                        } else if (strcmp(lastKey.c_str(), "idProperty") == 0) {
                            jsonSchemaUnorderedMap.at(currentSchemaKey).idPropertyKey = string(str);
                        } else if (strcmp(lastKey.c_str(), "name") == 0) { // MAPPING FOR NAME KEY
                            // The name key only exists in foreign key mappings, etc collections and associations
                            if (arrayDepth == 1 && objectDepth == 3) {
                                if (insideAssociationsArray) {
                                    if (associationsObjCount > jsonSchemaUnorderedMap.at(currentSchemaKey).associations.size()) {
                                        Associations associations = Associations();
                                        jsonSchemaUnorderedMap.at(currentSchemaKey).associations.insert(jsonSchemaUnorderedMap.at(currentSchemaKey).associations.end(), associations);
                                    }
                                    jsonSchemaUnorderedMap.at(currentSchemaKey).associations.back().name = string(str);
                                } else if (insideCollectionsArray) {
                                    if (collectionsObjCount > jsonSchemaUnorderedMap.at(currentSchemaKey).collections.size()) {
                                        Collections collections = Collections();
                                        jsonSchemaUnorderedMap.at(currentSchemaKey).collections.insert(jsonSchemaUnorderedMap.at(currentSchemaKey).collections.end(), collections);
                                    }
                                    jsonSchemaUnorderedMap.at(currentSchemaKey).collections.back().name = string(str);
                                } else {
                                    throw "ERROR CREATING MAP ID";
                                }
                            } else {
                                throw "Schema error detected. The \"name\" key must appear inside an array in either a \"properties\", \"collections\", or \"associations\" object.";
                            }
                        } else if (strcmp(lastKey.c_str(), "collections") == 0) {
                            jsonSchemaUnorderedMap.at(currentSchemaKey).collections = vector<Collections>();
                        } else if (strcmp(lastKey.c_str(), "associations") == 0) {
                            vector<Associations> *associationsPtr = &jsonSchemaUnorderedMap.at(currentSchemaKey).associations;
                            if (associationsPtr == NULL) {
                                jsonSchemaUnorderedMap.at(currentSchemaKey).associations = vector<Associations>();
                            }

                        } else if ((objectDepth == 2 && arrayDepth == 1) && insidePropertiesArray == true) {
                            vector<string> *propertiesPtr = &jsonSchemaUnorderedMap.at(currentSchemaKey).properties;
                            if (propertiesPtr == NULL) {
                                jsonSchemaUnorderedMap.at(currentSchemaKey).properties = vector<string>();
                            }
                            jsonSchemaUnorderedMap.at(currentSchemaKey).properties.insert(jsonSchemaUnorderedMap.at(currentSchemaKey).properties.end(), str);
                        } else if (objectDepth == 1) {

                        } else {
                            // throw error
                            throw SchemaParseException("Unable to find schema representation for ");
                        }
                    } else {
                        // TODO
                    }
                }
            } catch(...) {
                auto expPtr = current_exception();

                try
                {
                    if(expPtr) rethrow_exception(expPtr);
                }
                catch(const exception& e) //it would not work if you pass by value
                {
                    cout << e.what();
                }
            }
            return true;
        }
        bool StartObject() {
            this->objectDepth++;
            if (objectDepth == 3 && arrayDepth == 1) {
                if (insideAssociationsArray) {
                    associationsObjCount++;
                } else if (insideCollectionsArray) {
                    collectionsObjCount++;
                }
            }
            return true;
        }
        bool Key(const char* str, rapidjson::SizeType length, bool copy) {
            lastKey = str;

            if (strcmp(str,"mapId") == 0) {
                cout << "";
            } else if (strcmp(str,"idProperty") == 0) {
                cout << "";
            } else if (strcmp(str, "properties") == 0) {
                if (arrayDepth == 0 && objectDepth == 2) {
                    insidePropertiesArray = true;
                } else {
                    throw  "Schema error detected. The \"properties\" key must appear inside an array and schema object";
                }
            } else if (strcmp(str,"name") == 0) {
                if (arrayDepth != 1 || objectDepth != 3) {
                    throw "Schema error detected. The name key must appear inside an array and schema object.";
                }
            } else if (strcmp(str,"collections") == 0) {
                vector<Collections> *collectionsPtr = &jsonSchemaUnorderedMap.at(currentSchemaKey).collections;
                if (collectionsPtr == NULL) {
                    jsonSchemaUnorderedMap.at(currentSchemaKey).collections = vector<Collections>();
                }
                this->insideCollectionsArray = true;

            } else if (strcmp(str, "associations") == 0) {
                vector<Associations> *associationsPtr = &jsonSchemaUnorderedMap.at(currentSchemaKey).associations;
                if (associationsPtr == NULL) {
                    jsonSchemaUnorderedMap.at(currentSchemaKey).associations = vector<Associations>();
                }
                this->insideAssociationsArray = true;
            } else if (objectDepth == 1) { // We are in at the schema definition level.
                currentSchemaKey = str;
                JsonSchema schema = JsonSchema();
                schema.schemaKey = string(str);
                jsonSchemaUnorderedMap.insert(make_pair(str, schema));
            } else {
                cout << "We do not have a mapping for the provided key " << str << endl;
            }
            return true;
        }
        bool EndObject(rapidjson::SizeType memberCount) {
            this->objectDepth--;
            return true;
        }
        bool StartArray() {
            this->arrayDepth++;
            return true;
        }
        bool EndArray(rapidjson::SizeType elementCount) {
            this->arrayDepth--;
            insideAssociationsArray = false;
            insidePropertiesArray = false;
            insideCollectionsArray = false;
            associationsObjCount = 0;
            collectionsObjCount = 0;
            return true;
        }

        unordered_map<string, JsonSchema> jsonSchema() {
            return jsonSchemaUnorderedMap;
        }

    };

}

#endif //JOINJS_MAPPER_H
