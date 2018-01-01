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

using namespace std;
using namespace rapidjson;

namespace joinjs {

    class JsonMappingsHandler : BaseReaderHandler<UTF8<>, JsonMappingsHandler> {
    private:
        joinjs::JsonSchema rootSchema; // The schema object grabbed from the mapid passed in
        JSON_SCHEMA_MAP *jsonSchemaMap;
        StringBuffer jsonStringBuffer;
        Document dom;
        const char *schemaMapId;

        /**
        * This map holds the schema for the input that will be generated.
        * The first value is the key to be put in; the second is the mapId
        * */
        map<const char *, const char *> *propertyMapIdAssociationMap;

        /**
         * This map determines whether the schema for a specified mapId is to be contained in a single object (an association)
         * or an array of objects (a collection)
         * */
        map<const char *, nested_map_type> *mapIdObjectTypeMap;

        /**
         * The current key value
         * */
        char *currentKey;

        /**
        * Keeps track of the string lengtho of the current key
        * */
        int currentKeyLength;

        /*
         * If true, the key that is currently set is the map id of the root object
         * */
        bool isCurrentKeyRootIdProperty;

        /*
         * This holds all of the objects in the array to be returned
         * */
        Value *domObjects;

        /*
         * This keeps a record of the root id properties placed in the list of objects.
         * */
        Value rootIdPropertyValueMap;

        /*
         *
         **/
        bool doesObjectWithRootIdPropertyExist = false;

        void setRootSchema() {
            for (auto it = jsonSchemaMap->begin(); it != jsonSchemaMap->end(); it++) {
                if (strcmp(it->second.mapId.c_str(), schemaMapId) == 0) {
                    rootSchema = it->second; // set the root schema object
                    break;
                }
            }
        }


        void setValues(Value *value) {
            /**
             * This needs to be changed. The root id property map should be changed to a set, and we should keep a variable around with the root id property
             * Or just reference it from the root schema map. Then if the current key is the root id property, do a check against the set to see if we have
             * already inserted the value.
             * */

            if (isCurrentKeyRootIdProperty) {
                if (!doesIdPropertyValueExist(value)) {
                    Value v;
                    if (kNumberType == value->GetType()) {
                        if (value->IsInt()) {
                            v.SetInt(value->GetInt());
                        } else if (value->IsInt64()) {
                            v.SetInt64(value->GetInt64());
                        } else if (value->IsUint()) {
                            v.SetUint(value->GetUint());
                        } else if (value->IsUint64()) {
                            v.SetUint64(value->GetUint64());
                        } else if (value->IsFloat() || value->IsLosslessFloat()) {
                            v.SetFloat(value->GetFloat());
                        } else if (value->IsDouble() || value->IsLosslessDouble()) {
                            v.SetDouble(value->GetDouble());
                        } else {
                            throw "ERROR ON NUMBER COMPARISON";
                        }
                    } else if (kStringType == value->GetType()) {
                        //v.SetString(value->GetString());
                    } else if (kTrueType == value->GetType()) {
                        v.SetBool(value->GetBool());
                    } else if (kFalseType == value->GetType()) {
                        v.SetBool(value->GetBool());
                    } else if (kNullType == value->GetType()) {
                        v.SetNull();
                    } else {
                        throw "Unable to find the proper type for the value";
                    }
                    rootIdPropertyValueMap.GetArray().PushBack(v, dom.GetAllocator());
                } else {
                    doesObjectWithRootIdPropertyExist = true;
                }
            }
            Value key(currentKey, dom.GetAllocator());
            Type t = value->GetType();
            bool includeInRoot = false;
            if (find(rootSchema.properties.begin(), rootSchema.properties.end(), currentKey) != rootSchema.properties.end()) {
                includeInRoot = true;
            } else {
                if (!rootSchema.associations.empty()) {
                    for (auto itr = rootSchema.associations.begin(); itr != rootSchema.associations.end(); itr++) {
                        Associations *associations = itr.base();
                        if (find(jsonSchemaMap->at(associations->mapId).properties.begin(), jsonSchemaMap->at(associations->mapId).properties.end(), currentKey) != jsonSchemaMap->at(associations->mapId).properties.end()) {
                            cout << "Key " << currentKey << " was found in " << associations->mapId << endl;
                        }
                    }
                }

                if (!rootSchema.collections.empty()) {
                    for (auto itr = rootSchema.collections.begin(); itr != rootSchema.collections.end(); itr++) {
                        Collections *collections = itr.base();
                    }
                }
            }
            if (value->GetType() == kStringType) {
                if (includeInRoot) {
                    this->domObjects->AddMember(key, *value, dom.GetAllocator());
                }
            } else if (value->GetType() == kNumberType) {
                if (includeInRoot) {
                    this->domObjects->AddMember(key, *value, dom.GetAllocator());
                }
            } else if (value->GetType() == kTrueType) {
                if (includeInRoot) {
                    this->domObjects->AddMember(key, *value, dom.GetAllocator());
                }
            } else if (value->GetType() == kFalseType) {
                if (includeInRoot) {
                    this->domObjects->AddMember(key, *value, dom.GetAllocator());
                }
            } else if (value->GetType() == kNullType) {
                if (includeInRoot) {
                    this->domObjects->AddMember(key, *value, dom.GetAllocator());
                }
            } else {
                char* buffer;
                snprintf(buffer, sizeof(buffer), "Could not find mapping for value %s", value->GetString());
                std::string buffAsStdStr = buffer;
                throw buffAsStdStr;
            }
        }

        bool doesIdPropertyValueExist(Value *value) {
            if (!rootIdPropertyValueMap.GetArray().Empty()) {
                for (auto itr = rootIdPropertyValueMap.GetArray().Begin(); itr != rootIdPropertyValueMap.GetArray().End(); itr++) {
                    Type type = value->GetType();
                    Value *_v = itr;
                    Type t = _v->GetType();
                    if (type == _v->GetType()) {
                        if (kNumberType == type) {
                            if (value->IsInt() && _v->IsInt()) {
                                return value->GetInt() == _v->GetInt();
                            } else if (value->IsInt64() && _v->IsInt64()) {
                                return value->GetInt64() == _v->GetInt64();
                            } else if (value->IsUint() && _v->IsUint()) {
                                return value->GetUint() == _v->GetUint();
                            } else if (value->IsUint64() && _v->IsUint64()) {
                                return value->GetUint64() == _v->GetUint64();
                            } else if (value->IsFloat() && _v->IsFloat()) {
                                return value->GetFloat() == _v->GetFloat();
                            } else if (value->IsDouble() && _v->IsDouble()) {
                                return  value->GetDouble() == _v->GetDouble();
                            } else if (value->IsLosslessFloat() && _v->IsLosslessFloat()) {
                                return value->GetFloat() == _v->GetFloat();
                            } else if (value->IsLosslessDouble() && _v->IsLosslessDouble()) {
                                return value->GetDouble() == _v->GetDouble();
                            } else {
                                throw "ERROR ON NUMBER COMPARISON";
                            }
                        } else if (kStringType == type) {
                            if (strcmp(_v->GetString(), value->GetString()) == 0) {
                                return true;
                            }
                        } else if (kTrueType == type || kFalseType == type) {
                            return value->GetBool() == _v->GetBool();
                        } else if (kNullType == type) {
                            return value->IsNull() == _v->IsNull();
                        } else {

                        }
                    }
                }
            }
            return false;
        }


    public:

        JsonMappingsHandler(joinjs::JSON_SCHEMA_MAP *jsonSchemaMap, const char *schemaMapId) {
            this->jsonSchemaMap = jsonSchemaMap;
            this->schemaMapId = schemaMapId;
            Reader reader;
            StringBuffer stringBuffer;
            this->jsonStringBuffer = StringBuffer();
            this->dom = Document();
            this->rootIdPropertyValueMap = Value(kArrayType);
            setRootSchema();
        }

        const char *getResultString() {
            rapidjson::StringBuffer buffer;

            buffer.Clear();

            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
            dom.Accept(writer);

            return strdup( buffer.GetString() );
        }


        bool Null() {
            Value value(kNullType);
            value.SetNull();
            setValues(&value);
            return true;
        }

        bool Bool(bool b) {
            if (b) {
                Value value(kTrueType);
                value.SetBool(b);
                setValues(&value);
            } else {
                Value value(kFalseType);
                value.SetBool(b);
                setValues(&value);
            }
            return true;
        }

        bool Int(int i) {
            Value value(kNumberType);
            value.SetInt(i);
            setValues(&value);
            return true;
        }

        bool Uint(unsigned u) {
            Value value(kNumberType);
            value.SetUint(u);
            Type t = value.GetType();
            setValues(&value);
            return true;
        }

        bool Int64(int64_t i) {
            Value value(kNumberType);
            value.SetInt64(i);
            setValues(&value);
            return true;
        }

        bool Uint64(uint64_t u) {
            Value value(kNumberType);
            value.SetUint64(u);
            setValues(&value);
            return true;
        }

        bool Double(double d) {
            Value value(kNumberType);
            value.SetDouble(d);
            setValues(&value);
            return true;
        }

        bool RawNumber(const char *str, rapidjson::SizeType length, bool copy) {
            // Treat the raw number like a string
            Value value(kStringType);
            value.SetString(str, length, dom.GetAllocator());
            setValues(&value);
            return true;
        }

        bool String(const char *str, SizeType length, bool copy) {
            Value value(kStringType);
            value.SetString(str, length, dom.GetAllocator());
            setValues(&value);
            return true;
        }

        bool StartObject() {
            domObjects = new Value(kObjectType);
            return true;
        }

        bool Key(const char *str, SizeType length, bool copy) {
            currentKey = static_cast<char *>(calloc(length, sizeof(char)));
            strncpy(currentKey, str, length+1);
            currentKeyLength = length;
            isCurrentKeyRootIdProperty = false;
            if (strcmp(rootSchema.idPropertyKey.c_str(), str) == 0) {
                isCurrentKeyRootIdProperty = true;
            };
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

}

/**
 * This class creates a schema from the keys in the object. We just iterate over and object in the provided json string once
 * */

#endif //JOINJS_JSON_PARSER_H
