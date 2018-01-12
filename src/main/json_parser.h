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
#include <pcre.h>

using namespace std;
using namespace rapidjson;

namespace joinjs {

    class JsonMappingsHandler : BaseReaderHandler<UTF8<>, JsonMappingsHandler> {
    private:
        ObjectScaffoldHandler *scaffold;
        map<string, Value> *nestedObject;
        unique_ptr<ObjectProperties> currentScaffold;
        Document document;
        string schemaMapId;
        char* currentKey;
        int currentKeyLength;

    public:

        JsonMappingsHandler(const char *json, joinjs::JSON_SCHEMA_MAP *jsonSchemaMap, const char *schemaMapId) {
            this->schemaMapId = string(schemaMapId);
            this->scaffold = new ObjectScaffoldHandler(json, jsonSchemaMap, schemaMapId);
            Reader reader;
            StringStream ss(json);
            reader.Parse(ss, *scaffold);
            document = Document();
        }

        const char* getResultString() {
            return scaffold->getResultString(schemaMapId.c_str());
        }

        void setString(const char* scaffoldKey, const char* string, int length) {
            if (this->currentScaffold.get()->domObject.HasMember(currentKey)) {
                this->currentScaffold.get()->domObject.FindMember(currentKey)->value.SetString(string, length, document.GetAllocator());
            } else {
                bool didFind = false;
                for (auto itr = this->scaffold->getScaffoldObjectSet()->begin(); itr != this->scaffold->getScaffoldObjectSet()->end(); itr++) {
                    const char* key = itr->second->name.c_str();
                    auto it = this->nestedObject->find(key);
                    if (it != this->nestedObject->end()) {
                        didFind = true;
                        if (itr->second->Placement == ASSOCIATION) {
                            this->nestedObject->find(key)->second.AddMember(Value(kStringType).SetString(currentKey, currentKeyLength, document.GetAllocator()), Value(kStringType).SetString(string, length, document.GetAllocator()), document.GetAllocator());
                        } else if (itr->second->Placement == COLLECTION) {

                        }
                        break;
                    } else {
                        if (itr->second->Placement == ASSOCIATION) {
                            this->nestedObject->insert(make_pair(key, Value(kObjectType)));
                            this->nestedObject->find(key)->second.AddMember(Value(kStringType).SetString(currentKey, currentKeyLength, document.GetAllocator()), Value(kStringType).SetString(string, length, document.GetAllocator()), document.GetAllocator());
                        } else if (itr->second->Placement == COLLECTION) {
                            this->nestedObject->insert(make_pair(key, Value(kArrayType)));
                        }

                    }
                }
                if (!didFind) {
                    // TODO throw "ERROR";
                }
            }
        }

        void setInt(const char* scaffoldKey, int value) {
            if (this->currentScaffold.get()->domObject.HasMember(currentKey)) {
                this->currentScaffold.get()->domObject.FindMember(currentKey)->value.SetInt(value);
            } else {

            }
        }

        void setInt64(const char* scaffoldKey, long value) {
            if (this->currentScaffold.get()->domObject.HasMember(currentKey)) {
                this->currentScaffold.get()->domObject.FindMember(currentKey)->value.SetInt64(value);
            } else {

            }
        }

        void setUInt(const char* scaffoldKey, unsigned int value) {
            if (this->currentScaffold.get()->domObject.HasMember(currentKey)) {
                this->currentScaffold.get()->domObject.FindMember(currentKey)->value.SetUint(value);
            } else {

            }
        }

        void setUInt64(const char* scaffoldKey, unsigned long value) {
            if (this->currentScaffold.get()->domObject.HasMember(currentKey)) {
                this->currentScaffold.get()->domObject.FindMember(currentKey)->value.SetUint64(value);
            } else {

            }
        }

        void setFloat(const char* scaffoldKey, float value) {
            if (this->currentScaffold.get()->domObject.HasMember(currentKey)) {
                this->currentScaffold.get()->domObject.FindMember(currentKey)->value.SetFloat(value);
            } else {

            }
        }

        void setDouble(const char* scaffoldKey, double value) {
            if (this->currentScaffold.get()->domObject.HasMember(currentKey)) {
                this->currentScaffold.get()->domObject.FindMember(currentKey)->value.SetDouble(value);
            } else {

            }
        }

        void setBool(const char* scaffoldKey, bool value) {
            if (this->currentScaffold.get()->domObject.HasMember(currentKey)) {
                this->currentScaffold.get()->domObject.FindMember(currentKey)->value.SetBool(value);
            } else {

            }
        }

        void setNull(const char* scaffoldKey) {
            if (this->currentScaffold.get()->domObject.HasMember(currentKey)) {
                this->currentScaffold.get()->domObject.FindMember(currentKey)->value.SetNull();
            } else {

            }
        }


        bool Null() {
            setNull("");
            return true;
        }

        bool Bool(bool b) {
            setBool("", b);
            return true;
        }

        bool Int(int i) {
            setInt("", i);
            return true;
        }

        bool Uint(unsigned u) {
            setUInt("", u);
            return true;
        }

        bool Int64(int64_t i) {
            setInt64("", i);
            return true;
        }

        bool Uint64(uint64_t u) {
            setUInt64("", u);
            return true;
        }

        bool Double(double d) {
            setDouble("", d);
            return true;
        }

        bool RawNumber(const char *str, rapidjson::SizeType length, bool copy) {

            return true;
        }

        bool String(const char *str, SizeType length, bool copy) {
            setString("", str, length);
            return true;
        }

        bool StartObject() {
            auto it = this->scaffold->getScaffoldObjectSet()->find(schemaMapId);
            if (it != this->scaffold->getScaffoldObjectSet()->end()) {
                this->currentScaffold = unique_ptr<ObjectProperties>(it->second);
                this->nestedObject = new map<string, Value>();
            } else {
                throw "Could not find the schemamapid in the scaffold.";
            }
            return true;
        }

        bool Key(const char *str, SizeType length, bool copy) {
            currentKeyLength = length;
            currentKey = reinterpret_cast<char *>(calloc(length, sizeof(char)));
            strcpy(currentKey, str);
            return true;
        }

        bool EndObject(SizeType memberCount) {
            if (nestedObject->size() > 0) {
                for (auto it = nestedObject->begin(); it != nestedObject->end(); it++) {
                    const char* idProperty = this->scaffold->getScaffoldObjectSet()->find(it->first)->second->IdProperty.c_str();
                    if (it->second.HasMember(idProperty)) {
                        this->currentScaffold.get()->domObject.AddMember(
                                Value(kStringType).SetString(it->first.c_str(), it->first.length(),
                                                             document.GetAllocator()).Move(), it->second.Move(),
                                document.GetAllocator());
                        this->currentScaffold.get()->domObject.RemoveMember(
                                this->currentScaffold.get()->domObject.FindMember(it->first.c_str()));
                    }
                }
            }
            cout << GetJsonText(&this->currentScaffold.get()->domObject) << endl;
            this->currentScaffold.release();
            delete nestedObject;
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
            Document doc;
            doc.CopyFrom(*value, document.GetAllocator());
            rapidjson::StringBuffer buffer;

            buffer.Clear();

            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
            doc.Accept(writer);

            return strdup( buffer.GetString() );
        }

    };
}

/**
 * This class creates a schema from the keys in the object. We just iterate over and object in the provided json string once
 * */

#endif //JOINJS_JSON_PARSER_H
