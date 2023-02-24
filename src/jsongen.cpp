#include <parser.h>
#include <common.h>
#include <json_generator.h>

#include <fstream>

#include <cassert>

using namespace alien;

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: jsongen script file(output)";
        return EX_USAGE;
    }
    std::string script(argv[1]), jsonfile(argv[2]);
    std::ifstream iscript(script);
    std::ofstream ojsonfile(jsonfile);
    if (!iscript) {
        std::cerr << "Couldn't open file " << std::quoted(script);
        return EX_UNAVAILABLE;
    }
    if (!ojsonfile) {
        std::cerr << "Couldn't open file " << std::quoted(jsonfile);
        return EX_UNAVAILABLE;
    }
    std::string source(std::istreambuf_iterator<char>(iscript.rdbuf()),
                     std::istreambuf_iterator<char>());
    Parser parser(source);
    auto program = parser.parse();
    if (parser.hadError()) {
        return EX_UNAVAILABLE;
    }
    JsonGenerator jsongen;
    jsongen.generate_json(ojsonfile, program);
    return 0;
}