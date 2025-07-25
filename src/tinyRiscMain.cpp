#include "../include/tinyRiscMain.hpp"
#include "../include/debug_logger.h"
#include <systemc>

#include "../include/risc/tinyRISC.hpp"

void initializeMemory(std::map<uint32_t, uint32_t> &initial_memory, const uint32_t* instructions) {
    uint32_t number_of_instructions = *instructions;
    _debug("Number of Instructions: %i \n", number_of_instructions);
    int j = 1;
    for (uint32_t i = 0 ; j <= number_of_instructions; i = i + 4) {
        uint32_t currentPos = i + 0x00001000;
        initial_memory[currentPos] = instructions[j];
        _debug("Instruction %i: 0x%x at %u.\n", j, instructions[j], currentPos);
        j++;
    }
}

Result run_simulation(uint32_t cycles, const char* tracefile, uint32_t* instructions, char terminalToStdout, uint32_t latency) {
    std::map<uint32_t, uint32_t> initial_memory;
    initializeMemory(initial_memory, instructions);
    uint32_t period = 1; //TODO checken, was ist period??
    TINY_RISC tiny_risc("tiny_risc", initial_memory, period, latency, terminalToStdout);

    // Tracefile setup
    sc_trace_file *tf = nullptr;
    if(tracefile && tracefile[0] != '\0') {
        tf = sc_create_vcd_trace_file(tracefile);
        sc_trace(tf, tiny_risc.clk, "clk");
        sc_trace(tf, tiny_risc.pc.pc, "pc_value");
        sc_trace(tf, tiny_risc.terminal.end, "terminal_end");
        sc_trace(tf, tiny_risc.terminal.addr, "terminal_addr");
        sc_trace(tf, tiny_risc.terminal.w, "terminal_w");
        sc_trace(tf, tiny_risc.cu.select, "cu_select");
        sc_trace(tf, tiny_risc.rf.wdata, "rf_wData");

        //TODO: Add more signals to trace if needed
    }

    // Bind signals
    sc_signal<bool> end;
    sc_signal<bool> reset;
    tiny_risc.reset.bind(reset);
    tiny_risc.terminal.end(end);

    // Run simulation with the provided cycles
    printf("\n============ Starting Simulation ============\n");
    sc_start(static_cast<int>(cycles * period), SC_NS);
    _debug("Finished simulation.\n");

    if(tf) {
        sc_close_vcd_trace_file(tf);
    }

    // Prepare collected results from simulation
    char *terminal_file_content = (char*)malloc(tiny_risc.terminal.s.size() + 1);
    if (!terminal_file_content) {
        fprintf(stdout, "Failed to alloc memory for terminal file content");
        exit(EXIT_FAILURE);
    }
    std::memcpy(terminal_file_content, tiny_risc.terminal.s.c_str(), tiny_risc.terminal.s.size() + 1); // Idee von LLM mit memcpy damit char* nicht const ist

    uint32_t output_operations = tiny_risc.terminal.output_operations;
    _debug("Output operations: %u\n", output_operations);

    _debug("clks in Terminal Module: %i\n", tiny_risc.terminal.clks)

    //uint32_t elapsed_cycles = static_cast<uint32_t>(sc_time_stamp().to_seconds()) / period;
    uint32_t elapsed_cycles = tiny_risc.pc.totalClks;
    _debug("Cycles ran: %u\n", elapsed_cycles);

    return Result {elapsed_cycles, terminal_file_content, output_operations}; //
};

int sc_main(int argc, char* argv[]) {
    std::cout << "ERROR. Please run through the project executable." << std::endl;
    return 1;
}

