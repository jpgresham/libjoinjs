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
        string name;
        Value domObject;
        string MapId;
        string IdProperty;
        string ColumnID;
        nest_type Placement;
    };

    class ObjectScaffoldHandler : BaseReaderHandler<UTF8<>, ObjectScaffoldHandler> {
    private:
        map<string, ObjectProperties> scaffoldObjectSet;
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
            this->scaffoldObjectSet = map<string, ObjectProperties>();
            setRootSchema();
        }

        map<string, ObjectProperties> *getScaffoldObjectSet() {
            return &scaffoldObjectSet;
        };

        const char *getResultString(const char* schemaMapId) {
            bool found = false;
            for (auto itr = scaffoldObjectSet.begin(); itr != scaffoldObjectSet.end(); itr++) {
                if (strcmp(itr->first.c_str(), schemaMapId) == 0) {
                    document.CopyFrom(scaffoldObjectSet.at(schemaMapId).domObject, document.GetAllocator());
                    found = true;
                    break;
                }
            }

            if (!found) {
                cout << "Could not find scaffold object for schema map id " << schemaMapId << endl;
            }
            rapidjson::StringBuffer buffer;

            buffer.Clear();

            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
            document.Accept(writer);

            return strdup( buffer.GetString() );
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
                auto it = this->scaffoldObjectSet.find(schemaMapId);
                if (it != this->scaffoldObjectSet.end()) {
                    // We've already created a schema object so lets add this key to it
                    Value v(kNullType);
                    Value k(kStringType);
                    k.SetString(str, length, document.GetAllocator());
                    this->scaffoldObjectSet.at(schemaMapId).domObject.AddMember(k.Move(), v.Move(), document.GetAllocator());
                } else {
                    // Create a schema object and add the key to it
                    this->scaffoldObjectSet.insert(make_pair(schemaMapId, ObjectProperties()));
                    this->scaffoldObjectSet.at(schemaMapId).domObject = Value(kObjectType);
                    Value v(kNullType);
                    Value k(kStringType);
                    k.SetString(str, length, document.GetAllocator());
                    this->scaffoldObjectSet.at(schemaMapId).domObject.AddMember(k.Move(), v.Move(), document.GetAllocator());
                    this->scaffoldObjectSet.at(schemaMapId).MapId = rootSchema.mapId;
                    this->scaffoldObjectSet.at(schemaMapId).IdProperty = rootSchema.idPropertyKey;
                    // Next, add a map to the associations and collections the schema has
                    if (!rootSchema.associations.empty()) {
                        for (auto itr = rootSchema.associations.begin(); itr != rootSchema.associations.end(); itr++) {
                            string name = itr.base()->name;
                            Value v(kNullType);
                            Value k(kStringType);
                            k.SetString(name.c_str(), itr.base()->name.length(), document.GetAllocator());
                            this->scaffoldObjectSet.at(schemaMapId).domObject.AddMember(k.Move(), v.Move(), document.GetAllocator());
                            ObjectProperties valObj = ObjectProperties();
                            valObj.domObject = Value(kObjectType);
                            valObj.MapId = itr.base()->mapId;
                            valObj.Placement = ASSOCIATION;
                            valObj.name = name;
                            scaffoldObjectSet.insert(make_pair(name, ObjectProperties()));
                            scaffoldObjectSet.at(name).domObject = Value(kObjectType);
                            scaffoldObjectSet.at(name).MapId = itr.base()->mapId.c_str();
                            scaffoldObjectSet.at(name).Placement = ASSOCIATION;
                            scaffoldObjectSet.at(name).name = name;
                        }
                    } else if (!rootSchema.collections.empty()) {
                        for (auto itr = rootSchema.collections.begin(); itr != rootSchema.collections.end(); itr++) {
                            string name = itr.base()->name;
                            Value v(kNullType);
                            Value k(kStringType);
                            k.SetString(name.c_str(), itr.base()->name.length(), document.GetAllocator());
                            this->scaffoldObjectSet.at(schemaMapId).domObject.AddMember(k.Move(), v.Move(), document.GetAllocator());
                            scaffoldObjectSet.insert(make_pair(name, ObjectProperties()));
                            scaffoldObjectSet.at(name).domObject = Value(kArrayType);
                            scaffoldObjectSet.at(name).MapId = itr.base()->mapId;
                            scaffoldObjectSet.at(name).Placement = COLLECTION;
                            scaffoldObjectSet.at(name).name = name;
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
            displayVars();
            for (auto itr = scaffoldObjectSet.begin(); itr != scaffoldObjectSet.end(); itr++) {

            }
            return false;
        }

        bool StartArray() {
            return true;
        }

        bool EndArray(SizeType elementCount) {
            return true;
        }

        void CleanUp() {
            //delete scaffoldObjectSet;
        }

        void displayVars() {
            for (auto itr = scaffoldObjectSet.begin(); itr != scaffoldObjectSet.end(); itr++) {
                ObjectProperties *obj = &itr->second;
                cout << "The name is: " << obj->name << endl;
                cout << "The MAPID is: " << obj->MapId << endl;
                cout << "The object is: " << obj->Placement << endl;
                cout << "The properties are: " << endl;
                if (obj->domObject.IsArray()) {
                    auto array = obj->domObject.GetArray();
                    for (auto it = array.begin(); it != array.end(); it++) {
                        cout << "Is object" << endl;
                    }
                } else if (obj->domObject.IsObject()) {
                    auto obj2 = obj->domObject.GetObject();
                    int capacity = obj2.MemberCapacity();
                    cout << obj->name << " has the following member: " << endl;
                    cout << obj2.MemberCount() << endl;
                    for (auto i = obj2.begin(); i != obj2.end(); i++) {
                        cout << i->name.GetString() << endl;
                    }
                }
            }
        }

    };
}

#endif //JOINJS_OBJECT_SCAFFOLD_H
