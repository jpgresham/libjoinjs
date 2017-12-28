//
// Created by Jacob Gresham on 12/15/17.
//

#include <spdlog/spdlog.h>
#include <iostream>
#include "../main/schema_parser.h"
#include "../main/map.h"
#include <rapidjson/filereadstream.h>
#include <sstream>
#include <fstream>
#include <rapidjson/istreamwrapper.h>

using namespace std;

class JoinJSTest {
public:
private:
};

char* readFileToString() {
    size_t length_;
    char *json_;
    FILE* fp = fopen("/Users/jacobgresham/test/testjson-large.js", "r");

    if (!fp) {
        cout << "Unable to open test file" << endl;
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    length_ = static_cast<size_t>(ftell(fp));
    fseek(fp, 0, SEEK_SET);
    json_ = static_cast<char*>(malloc(length_ + 1));
    size_t readLength = fread(json_, 1, length_, fp);
    json_[readLength] = '\0';
    fclose(fp);
    return json_;
}

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

        jjn::SchemaJsonHandler handler = handler.getInstance();
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

        FILE* fp2 = fopen("/Users/jacobgresham/test/schema.js", "r");

        if (!fp2) {
            console->error("Unable to open test file");
            exit(1);
        }

        console->info("Mapping the json to parsed schema. . .");

        fseek(fp2, 0L, SEEK_END);
        long sz2 = ftell(fp);

        rewind(fp2);

        char readBuffer2[sz2 + 1];

        // now test parsing the json data

        rapidjson::FileReadStream frs(fp2, readBuffer2, sizeof(readBuffer2));



        size_t tell = frs.Tell();
        char take = frs.Take();
        char take2 = frs.Take();
        char take3 = frs.Take();
        char take4 = frs.Take();

        size_t tell2 = frs.Tell();

        console->info("JSON test file loaded. Mapping. . .");

        char* json = readFileToString();

        JsonMapper jsonMapper(json, "eventMembersMap", &jsonSchema);

        const char* result = jsonMapper.getResult();

        cout << "THE RESULT JSON IS: " << endl << result << endl;


    } catch (const spdlog::spdlog_ex &ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
        return 1;
    } catch(const char* message) {
        std::cout << "We received the following error message: " << message << std::endl;
    }
    return 0;
}