#ifndef TINYRISCMAIN_H
#define TINYRISCMAIN_H
#include <stdint.h>

extern "C" { //TODO Muss nicht extern weil alles zusammen compiliert wird: Compilierung ändern!!
    struct Result {
        uint32_t cycles;
        char* output;
        uint32_t outs;
    };
    struct Result run_simulation(uint32_t cycles,
                                 const char* tracefile,
                                 uint32_t* instructions,
                                 char terminalToStdout,
                                 uint32_t latency);
}
#endif //TINYRISCMAIN_H
