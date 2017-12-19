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
        } catch (const spdlog::spdlog_ex &ex) {
            std::cout << "Log init failed: " << ex.what() << std::endl;
            return 1;
        }
        return 0;
    }
}