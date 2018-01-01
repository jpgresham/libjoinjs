//
// Created by Jacob Gresham on 12/22/17.
//

#ifndef JOINJS_EXCEPTIONS_H
#define JOINJS_EXCEPTIONS_H

#include <iostream>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <execinfo.h>

using namespace std;

namespace joinjs {

    void stackTraceHandler() {
        void *trace_elems[20];
        int trace_elem_count(backtrace(trace_elems, 20));
        char **stack_syms(backtrace_symbols(trace_elems, trace_elem_count));
        for (int i = 0; i < trace_elem_count; ++i) {
            std::cout << stack_syms[i] << "\n";
        }
        free(stack_syms);

        exit(1);
    }

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

    class NullPointerException {
    private:
        string msg_;

    public:
        NullPointerException(const char* msg) : msg_(msg) {}
        NullPointerException() : msg_("We tried accessing a null pointer.") {}
    };

}
#endif //JOINJS_EXCEPTIONS_H
