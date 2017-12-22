#include "joinjs.h"

#include <iostream>
#include <spdlog/spdlog.h>
#include "mapper.h"

namespace jjn {
    int Map(char *inputJson) {
        try {
            auto console = spdlog::stdout_color_mt("console");
            console->info("Starting mapping for json string.");
            jjn::SchemaJsonHandler jsonHandler;
            Reader reader;
            StringStream ss(inputJson);
            reader.Parse(ss, jsonHandler);
            unordered_map<string, JsonSchema> jsonSchema = jsonHandler.jsonSchema();
            console->info("Completed parsing the schema for provided json input.");
        } catch (const spdlog::spdlog_ex &ex) {
            std::cout << "Log init failed: " << ex.what() << std::endl;
            return 1;
        } catch(const char* message) {
            std::cout << "We received the following error message: " << message << std::endl;
        }
        return 0;
    }
}