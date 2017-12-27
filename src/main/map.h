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
        Value *domObjects;

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
            StringBuffer stringBuffer;
            this->dom = Document();
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
            domObjects->AddMember(Value(currentKey.c_str(), dom.GetAllocator()).Move(), Value(str, dom.GetAllocator()).Move(), dom.GetAllocator());
            const char* resultStr = getResultString();
            return true;
        }
        bool StartObject() {
            domObjects = new Value(kObjectType);
            return true;
        }
        bool Key(const char* str, SizeType length, bool copy) {
            currentKey = str;
            currentKeyLength = length;
            return true;
        }
        bool EndObject(SizeType memberCount) {
            dom.GetArray().PushBack(*domObjects, dom.GetAllocator());
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
