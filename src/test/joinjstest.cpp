//
// Created by Jacob Gresham on 12/15/17.
//

#include <spdlog/spdlog.h>
#include <iostream>
#include "../main/mapper.h"
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <sstream>
#include <fstream>

using namespace std;

class JoinJSTest {
public:
private:
};

int main(int argc,  char *argv[]) {
    try {
        auto console = spdlog::stdout_color_mt("console");

        console->info("Testing mapping for JSON");

        FILE* fp = fopen("/Users/jacobgresham/test/schema.js", "r");

        if (!fp) {
            console->error("Unable to open file");
            exit(1);
        }

        console->info("Parsing the json schema. . .");

        fseek(fp, 0L, SEEK_END);
        long sz = ftell(fp);

        rewind(fp);

        char readBuffer[sz + 1];

        jjn::SchemaJsonHandler handler;
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

        rapidjson::Reader reader;
        reader.Parse(is, handler);


        unordered_map<string, jjn::JsonSchema> jsonSchema = handler.jsonSchema();
        console->info("Completed parsing the schema for provided json input.");

        for (auto it : jsonSchema) {
            jjn::JsonSchema schema = it.second;
            cout << it.first << endl;
            cout << "Map Id: " << schema.mapId << endl;
            cout << "Schema Key: " << schema.schemaKey << endl;
            cout << "Id Property Key: " << schema.idPropertyKey << endl;
            cout << "First property is: " << schema.properties.at(0) << endl;
            cout << endl << endl;
        }

        fclose(fp);


    } catch (const spdlog::spdlog_ex &ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
        return 1;
    } catch(const char* message) {
        std::cout << "We received the following error message: " << message << std::endl;
    }
    return 0;
}