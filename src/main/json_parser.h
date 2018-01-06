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

using namespace std;
using namespace rapidjson;

namespace joinjs {

    class JsonMappingsHandler : BaseReaderHandler<UTF8<>, JsonMappingsHandler> {
    private:
        ObjectScaffoldHandler *scaffold;
        Document document;
        string schemaMapId;

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

        void setString(const char* scaffoldKey, const char* valueKey, const char* string, int length) {

        }

        void setInt(const char* scaffoldKey, const char* valueKey, int value) {

        }

        void setInt64(const char* scaffoldKey, const char* valueKey, long value) {

        }

        void setUInt(const char* scaffoldKey, const char* valueKey, unsigned int value) {

        }

        void setUInt64(const char* scaffoldKey, const char* valueKey, unsigned long value) {

        }

        void setFloat(const char* scaffoldKey, const char* valueKey, float value) {

        }

        void setDouble(const char* scaffoldKey, const char* valueKey, double value) {

        }

        void setBool(const char* scaffoldKey, const char* valueKey, bool value) {

        }

        void setNull(const char* scaffoldKey, const char* valueKey) {

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
            return true;
        }

        bool StartObject() {
            return true;
        }

        bool Key(const char *str, SizeType length, bool copy) {
            return true;
        }

        bool EndObject(SizeType memberCount) {
            return true;
        }

        bool StartArray() {
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
