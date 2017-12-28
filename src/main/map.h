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

using namespace rapidjson;
using namespace std;

class JsonMapper {
private:
    const char* schemaMapId;
    const char* columnPrefix;
    jjn::JsonSchema rootSchema;
    unordered_map<std::string, jjn::JsonSchema>* jsonSchemaMap;
    const char *resultString;
    set<string> redBlackTree;

    void initialize(char* json, const char* schemaMapId, const char* columnPrefix, unordered_map<std::string, jjn::JsonSchema>* jsonSchemaMap) {
        try {
            auto console = spdlog::stdout_color_mt("json_mapper");
            this->schemaMapId = schemaMapId;
            this->columnPrefix = columnPrefix;
            this->jsonSchemaMap = jsonSchemaMap;
            console->info("JSON Mapper initialized");
            Reader reader;
            JsonMappingsHandler handler(this->jsonSchemaMap, schemaMapId);
            StringStream ss(json);
            reader.Parse(ss, handler);
            resultString = handler.getResultString();
        } catch (const spdlog::spdlog_ex &ex) {
            std::cout << "Log init failed: " << ex.what() << std::endl;
            exit(1);
        }
    }

    class JsonMappingsHandler : BaseReaderHandler<UTF8<>, JsonMappingsHandler> {
    private:
        jjn::JsonSchema rootSchema; // The schema object grabbed from the mapid passed in
        unordered_map<std::string, jjn::JsonSchema>* jsonSchemaMap;
        Document dom;
        const char *schemaMapId;
        string currentKey;
        int currentKeyLength;
        bool isCurrentKeyRootIdProperty; // If true, the key that is currently set is the map id of the root object
        Value *domObjects;
        map<const char*, unsigned long> idPropertyMap; // This keeps a record of the root id properties placed in the list of objects.
        bool doesObjectWithRootIdPropertyExist = false;

        void setRootSchema() {
            for (auto it = jsonSchemaMap->begin(); it != jsonSchemaMap->end(); it++) {
                if (strcmp(it->second.mapId.c_str(), schemaMapId) == 0) {
                    rootSchema = it->second; // set the root schema object
                    break;
                }
            }
        }

        template <typedef const char*>
        bool idPropertyCheck() {
            if (isCurrentKeyRootIdProperty) {
                if (idPropertyMap.find(currentKey.c_str()) == idPropertyMap.end()) {
                    unsigned long val = idPropertyMap.size()+1;
                    idPropertyMap.insert(make_pair(currentKey.c_str(), val));
                } else {
                    doesObjectWithRootIdPropertyExist = true;
                }
            }
            return true;
        }

    public:

        JsonMappingsHandler(unordered_map<string, jjn::JsonSchema> *jsonSchemaMap, const char *schemaMapId) {
            this->jsonSchemaMap = jsonSchemaMap;
            this->schemaMapId = schemaMapId;
            StringBuffer stringBuffer;
            this->dom = Document();
            this->idPropertyMap = map<const char*, unsigned long>();
            setRootSchema();
        }

        const char *getResultString() {
            rapidjson::StringBuffer buffer;

            buffer.Clear();

            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
            dom.Accept(writer);

            return strdup( buffer.GetString() );
        }

        bool Null() { return true; }
        bool Bool(bool b) {
            Value value;
            value.SetBool(b);
            domObjects->AddMember(Value(currentKey.c_str(), dom.GetAllocator()).Move(), value, dom.GetAllocator());
            return true;
        }
        bool Int(int i) { return true; }
        bool Uint(unsigned u) { return true; }
        bool Int64(int64_t i) { return true; }
        bool Uint64(uint64_t u) { return true; }
        bool Double(double d) { return true; }
        bool RawNumber(const char* str, rapidjson::SizeType length, bool copy) {
            return true;
        }
        bool String(const char* str, SizeType length, bool copy) {
            // If true then the field/value belong in the root schema. Otherwise it belongs in a nested object
            if (find(rootSchema.properties.begin(), rootSchema.properties.end(), currentKey) != rootSchema.properties.end()) {
                domObjects->AddMember(Value(currentKey.c_str(), dom.GetAllocator()).Move(), Value(str, dom.GetAllocator()).Move(), dom.GetAllocator());
            } else {
                if (!rootSchema.collections.empty()) {
                    for (auto itr = rootSchema.collections.begin(); itr != rootSchema.collections.end(); itr++) {

                    }
                } else if (!rootSchema.associations.empty()) {
                    for (auto itr = rootSchema.associations.begin(); itr != rootSchema.associations.end(); itr++) {
                        if (domObjects->HasMember(itr.base()->name.c_str())) {
                            Value::MemberIterator member = domObjects->FindMember(itr.base()->name.c_str());
                            member->value.AddMember(Value(currentKey.c_str(), dom.GetAllocator()).Move(), Value(str, dom.GetAllocator()).Move(), dom.GetAllocator());
                        } else {
                            Value obj(kObjectType);
                            obj.AddMember(Value(currentKey.c_str(), dom.GetAllocator()).Move(), Value(str, dom.GetAllocator()).Move(), dom.GetAllocator());
                            domObjects->AddMember(Value(itr.base()->name.c_str(), dom.GetAllocator()).Move(), obj, dom.GetAllocator());
                        }
                    }
                } else {
                    throw "UNABLE TO FIND A MATCHING SCHEMA FOR THE FIELD";
                }
            }
            if (isCurrentKeyRootIdProperty) {
                if (idPropertyMap.find(currentKey.c_str()) == idPropertyMap.end()) {
                    unsigned long val = idPropertyMap.size()+1;
                    idPropertyMap.insert(make_pair(currentKey.c_str(), val));
                } else {
                    doesObjectWithRootIdPropertyExist = true;
                }
            }
            return true;
        }
        bool StartObject() {
            domObjects = new Value(kObjectType);
            return true;
        }
        bool Key(const char* str, SizeType length, bool copy) {
            currentKey = str;
            currentKeyLength = length;
            isCurrentKeyRootIdProperty = false;
            if (strcmp(rootSchema.idPropertyKey.c_str(), str) == 0) {
                isCurrentKeyRootIdProperty = true;
            }
            return true;
        }
        bool EndObject(SizeType memberCount) {
            if (!doesObjectWithRootIdPropertyExist) {
                dom.GetArray().PushBack(*domObjects, dom.GetAllocator());
            }
            doesObjectWithRootIdPropertyExist = false;
            delete domObjects;
            return true;
        }
        bool StartArray() {
            dom.SetArray();
            return true;
        }
        bool EndArray(SizeType elementCount) {
            return true;
        }
    };

public:

    JsonMapper(char* json, const char* schemaMapId, const char* columnPrefix, unordered_map<std::string, jjn::JsonSchema>* jsonSchemaMap) {
        initialize(json, schemaMapId, columnPrefix, jsonSchemaMap);
    }

    JsonMapper(char* json, const char* schemaMapId, unordered_map<std::string, jjn::JsonSchema>* jsonSchemaMap) {
        initialize(json, schemaMapId, NULL, jsonSchemaMap);
    }

    const char *getResult() {
        return resultString;
    }
};

#endif //JOINJS_MAP_H
