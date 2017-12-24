#include "joinjs.h"

#include <iostream>
#include <spdlog/spdlog.h>
#include "schema_parser.h"

using namespace std;

namespace jjn {
    int Map(char *inputJson) {
        try {
            auto console = spdlog::stdout_color_mt("console");
            console->info("Starting mapping for json string.");
            jjn::SchemaJsonHandler jsonHandler = jsonHandler.getInstance();
            Reader reader;
            StringStream ss(inputJson);
            reader.Parse(ss, jsonHandler);
            unordered_map<string, JsonSchema> jsonSchema = jsonHandler.jsonSchema();
            console->info("Completed parsing the schema for provided json input.");
        } catch (const spdlog::spdlog_ex &ex) {
            cout << "Log init failed: " << ex.what() << endl;
            return 1;
        } catch(const char* message) {
            cout << "We received the following error message: " << message << endl;
        }
        return 0;
    }
}