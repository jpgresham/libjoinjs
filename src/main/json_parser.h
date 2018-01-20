//
// Created by Jacob Gresham on 12/28/17.
//

#ifndef JOINJS_JSON_PARSER_H
#define JOINJS_JSON_PARSER_H

#include <rapidjson/encodings.h>
#include <string>
#include <rapidjson/stringbuffer.h>
#include <map>
#include <rapidjson/prettywriter.h>
#include "schema_parser.h"
#include "types.h"
#include "object_scaffold.h"
#include <pcrecpp.h>

using namespace std;
using namespace rapidjson;

namespace joinjs {

    class JsonMappingsHandler : BaseReaderHandler<UTF8<>, JsonMappingsHandler> {
    private:

        void pcre_match(const char *aStrRegex, const char* inputStr) {
            //pcrecpp::RE re(string(aStrRegex));
        }

        struct ObjProps {
            const char* data;

            nest_type type;
            const char* idProperty;
            const char* name;
        };

        struct id_types {
            const char* type;
            int i_32;
            unsigned int ui_32;
            long i_64;
            unsigned long ui_64;
            const char* str;
            bool bl;
            float flt;
            double dbl;
        };

        ObjectScaffoldHandler *scaffold;
        char* documentString;
        unique_ptr<map<const char*, ObjProps>> nestedObjects; // Key is map name,
        vector<const char*> *documentsArray;
        vector<id_types*> *spentIds;
        string schemaMapId;
        char* currentKey;
        int currentKeyLength;
        JSON_SCHEMA_MAP *jsonSchemaMap;
        JsonSchema *currentSchema;

        JsonSchema *getCurrentSchema() {
            for (auto itr = jsonSchemaMap->begin(); itr != jsonSchemaMap->end(); itr++) {
                if (strcmp(itr->second.mapId.c_str(), schemaMapId.c_str()) == 0) {
                    return &itr->second;
                }
            }
            return nullptr;
        }

        struct get_by_map_id
        {
            get_by_map_id( string mapId ) : _map_id(mapId) {}
            bool operator()( const std::pair<std::string, joinjs::JsonSchema>& v ) const
            {
                return v.second.mapId == _map_id;
            }
        private:
            string _map_id;
        };

    public:

        JsonMappingsHandler(const char *json, joinjs::JSON_SCHEMA_MAP *jsonSchemaMap, const char *schemaMapId) {
            this->schemaMapId = string(schemaMapId);
            this->jsonSchemaMap = jsonSchemaMap;
            this->currentSchema = getCurrentSchema();
            this->scaffold = new ObjectScaffoldHandler(json, jsonSchemaMap, schemaMapId);
            this->documentsArray = new vector<const char*>();
            this->spentIds = new vector<id_types*>();
            Reader reader;
            StringStream ss(json);
            reader.Parse(ss, *scaffold);
        }

        char* getResultString() {
            delete this->documentsArray;
            delete this->spentIds;
            delete this->scaffold;
            return this->documentString;
        }


        bool Null() {
            return true;
        }

        bool Bool(bool b) {
            return true;
        }

        bool Int(int i) {
            return true;
        }

        bool Uint(unsigned u) {
            return true;
        }

        bool Int64(int64_t i) {
            return true;
        }

        bool Uint64(uint64_t u) {
            return true;
        }

        bool Double(double d) {
            return true;
        }

        bool RawNumber(const char *str, rapidjson::SizeType length, bool copy) {

            return true;
        }

        bool String(const char *str, SizeType length, bool copy) {
            auto itr = find(currentSchema->properties.begin(), currentSchema->properties.end(), currentKey);
            if (itr != currentSchema->properties.end()) {

            } else {
                if (strcmp(currentSchema->idPropertyKey.c_str(), currentKey) == 0) {

                } else {
                    // Now look in associations and collections for the object
                    for (auto itr =  currentSchema->associations.begin(); itr != currentSchema->associations.end(); itr++) {
                        auto item = find_if(jsonSchemaMap->begin(), jsonSchemaMap->end(), get_by_map_id(itr.base()->mapId));
                        for (auto it = item->second.properties.begin(); it != item->second.properties.end(); it++) {
                            if (strcmp(currentKey, it.base()->data()) == 0) {
                                if (this->nestedObjects.get() != NULL && this->nestedObjects.get()->count(itr.base()->name.c_str()) != 0) {

                                    break;
                                } else {
                                    // Add the object to the nested objects list
                                    ObjProps props = ObjProps();
                                    props.name = itr.base()->name.c_str();
                                    props.idProperty = item->second.idPropertyKey.c_str();
                                    props.type = ASSOCIATION;
                                    props.data = this->scaffold->getScaffoldObjectSet()->at(itr->name)->data;
                                    this->nestedObjects.get()->insert(make_pair(itr->name.c_str(), props));
                                }
                            }
                        }
                    }
                }
            }

            return true;
        }

        bool StartObject() {
            this->nestedObjects = unique_ptr<map<const char*, ObjProps>>(new map<const char*, ObjProps>());
            auto it = this->scaffold->getScaffoldObjectSet()->find(schemaMapId);
            if (it != this->scaffold->getScaffoldObjectSet()->end()) {
                this->documentString = static_cast<char *>(calloc(sizeof(char), strlen(this->scaffold->getScaffoldObjectSet()->at(schemaMapId)->data)));
                this->documentString = this->scaffold->getScaffoldObjectSet()->at(schemaMapId)->data;
            } else {
                throw "Could not find the schemamapid in the scaffold.";
            }
            return true;
        }

        bool Key(const char *str, SizeType length, bool copy) {
            this->currentKeyLength = length;
            this->currentKey = reinterpret_cast<char *>(calloc(length, sizeof(char)));
            strcpy(currentKey, str);
            return true;
        }

        bool EndObject(SizeType memberCount) {
            if (this->nestedObjects.get()->size() > 0) {
                for (auto it = this->nestedObjects.get()->begin(); it != this->nestedObjects.get()->end(); it++) {
                    const char* idProperty = this->scaffold->getScaffoldObjectSet()->find(it->first)->second->IdProperty.c_str();
                    const char* regex = "";
                    pcre_match(regex, this->nestedObjects.get()->at(it->second.name).data);
                }
            }
            this->nestedObjects.release();
            return true;
        }

        bool StartArray() {
            return true;
        }

        bool EndArray(SizeType elementCount) {
            return true;
        }

        const char *GetJsonText(Value *value)
        {
            return "";
        }

    };
}

/**
 * This class creates a schema from the keys in the object. We just iterate over and object in the provided json string once
 * */

#endif //JOINJS_JSON_PARSER_H
