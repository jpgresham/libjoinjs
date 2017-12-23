//
// Created by Jacob Gresham on 12/22/17.
//

#ifndef JOINJS_EXCEPTIONS_H
#define JOINJS_EXCEPTIONS_H

#include <iostream>

using namespace std;

class SchemaParseException {
private:
    string msg_;
public:
    SchemaParseException(const string &msg) : msg_(msg) {}
    SchemaParseException() : msg_("An error occurred while parsing the schema") {}
};

class JsonMapException {
private:
    string msg_;
public:
    JsonMapException(const string &msg) : msg_(msg) {}
    JsonMapException() : msg_("An error occurred while mapping the json.") {}
};

#endif //JOINJS_EXCEPTIONS_H
