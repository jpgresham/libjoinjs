//
// Created by Jacob Gresham on 1/1/18.
//

#ifndef JOINJS_OBJECT_SCAFFOLD_H
#define JOINJS_OBJECT_SCAFFOLD_H

#include <cstdint>
#include <rapidjson/rapidjson.h>
#include <rapidjson/encodings.h>
#include <rapidjson/reader.h>
#include <map>
#include <rapidjson/document.h>

using namespace rapidjson;
using namespace std;

namespace joinjs {

    enum nest_type{ROOT, ASSOCIATION, COLLECTION};

    struct ObjectProperties {
        const char* name;
        Value domObject;
        const char* MapId;
        const char* IdProperty;
        const char* ColumnID;
        nest_type Placement;
    };

    class ObjectScaffoldHandler : BaseReaderHandler<UTF8<>, ObjectScaffoldHandler> {
    private:
        map<const char*, ObjectProperties> *scaffoldObjectSet;
        JSON_SCHEMA_MAP *jsonSchemaMap;
        JsonSchema rootSchema;
        const char* schemaMapId;
        string scaffoldKey;
        bool scaffoldKeyCreated;
        Document document;

        void setRootSchema() {
            for (auto it = jsonSchemaMap->begin(); it != jsonSchemaMap->end(); it++) {
                if (strcmp(it->second.mapId.c_str(), schemaMapId) == 0) {
                    rootSchema = it->second; // set the root schema object
                    break;
                }
            }
        }
    public:

        ObjectScaffoldHandler(const char *json, joinjs::JSON_SCHEMA_MAP *jsonSchemaMap, const char *schemaMapId) {
            this->jsonSchemaMap = jsonSchemaMap;
            this->schemaMapId = schemaMapId;
            this->scaffoldKeyCreated = false;
            this->scaffoldKey = string(schemaMapId);
            this->scaffoldObjectSet = new map<const char*, ObjectProperties>();
            setRootSchema();
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

        bool RawNumber(const char *str, SizeType length, bool copy) {
            return true;
        }

        bool String(const char *str, SizeType length, bool copy) {
            return true;
        }

        bool StartObject() {
            return true;
        }

        bool Key(const char *str, SizeType length, bool copy) {
            if (find(rootSchema.properties.begin(), rootSchema.properties.end(), str) != rootSchema.properties.end()) {
                // We fpund this key in the root schema, so try to get that from the object thingy
                auto it = this->scaffoldObjectSet->find(schemaMapId);
                if (it != this->scaffoldObjectSet->end()) {
                    // We've already created a schema object so lets add this key to it
                    Value v(kNullType);
                    Value k(kStringType);
                    this->scaffoldObjectSet->at(schemaMapId).domObject.AddMember(k.Move(), v.Move(), document.GetAllocator());

                } else {
                    // Create a schema object and add the key to it
                    this->scaffoldObjectSet->insert(make_pair(schemaMapId, ObjectProperties()));
                    this->scaffoldObjectSet->at(schemaMapId).domObject = Value(kObjectType);
                    Value v(kNullType);
                    Value k(kStringType);
                    k.SetString(str, length);
                    this->scaffoldObjectSet->at(schemaMapId).domObject.AddMember(k.Move(), v.Move(), document.GetAllocator());
                    // Next, add a map to the associations and collections the schema has
                    if (!rootSchema.associations.empty()) {
                        for (auto itr = rootSchema.associations.begin(); itr != rootSchema.associations.end(); itr++) {
                            const char* name = itr.base()->name.c_str();
                            Value v(kNullType);
                            Value k(kStringType);
                            k.SetString(name, itr.base()->name.length());
                            this->scaffoldObjectSet->at(schemaMapId).domObject.AddMember(k, v.Move(), document.GetAllocator());
                            ObjectProperties valObj = ObjectProperties();
                            valObj.domObject = Value(kObjectType);
                            valObj.MapId = itr.base()->mapId.c_str();
                            valObj.Placement = ASSOCIATION;
                            valObj.name = name;
                            scaffoldObjectSet->insert(make_pair(name, ObjectProperties()));
                            scaffoldObjectSet->at(name).domObject = Value(kArrayType);
                            scaffoldObjectSet->at(name).MapId = itr.base()->mapId.c_str();
                            scaffoldObjectSet->at(name).Placement = ASSOCIATION;
                            scaffoldObjectSet->at(name).name = name;
                        }
                    } else if (!rootSchema.collections.empty()) {
                        for (auto itr = rootSchema.collections.begin(); itr != rootSchema.collections.end(); itr++) {
                            const char* name = itr.base()->name.c_str();
                            Value v(kNullType);
                            Value k(kStringType);
                            k.SetString(name, itr.base()->name.length());
                            this->scaffoldObjectSet->at(schemaMapId).domObject.AddMember(k, v.Move(), document.GetAllocator());
                            scaffoldObjectSet->insert(make_pair(name, ObjectProperties()));
                            scaffoldObjectSet->at(name).domObject = Value(kArrayType);
                            scaffoldObjectSet->at(name).MapId = itr.base()->mapId.c_str();
                            scaffoldObjectSet->at(name).Placement = COLLECTION;
                            scaffoldObjectSet->at(name).name = name;
                        }
                    }
                }
            } else {
                // Property probably exists as a collection or association
                if (!rootSchema.associations.empty()) {
                    for (auto itr = rootSchema.associations.begin(); itr != rootSchema.associations.end(); itr++) {

                    }
                } else if (!rootSchema.collections.empty()) {
                    for (auto itr = rootSchema.collections.begin(); itr != rootSchema.collections.end(); itr++) {

                    }
                }
            }
            return true;
        }

        bool EndObject(SizeType memberCount) {
            // The end of the first object is where we stop because we will have created the scaffold needed.
            for (auto itr = scaffoldObjectSet->begin(); itr != scaffoldObjectSet->end(); itr++) {

            }
            delete scaffoldObjectSet;
            return false;
        }

        bool StartArray() {
            return true;
        }

        bool EndArray(SizeType elementCount) {
            return true;
        }
    };
}

#endif //JOINJS_OBJECT_SCAFFOLD_H
