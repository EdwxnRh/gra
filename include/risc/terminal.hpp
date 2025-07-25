#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include <systemc>
#include <systemc.h>
#include <string>

SC_MODULE(TERMINAL)
{
    // Input ports
    sc_in<bool> clk;
    sc_in<bool> w;
    sc_in<uint32_t> addr;
    sc_in<uint32_t> wdata;

    // Output ports
    sc_out<bool> ready;
    sc_out<bool> end;

    // Parameters
    int latency;
    bool use_stdout;

    // Cycle counter
    int left_cycles;

    // Terminal address
    static const uint32_t TERMINAL_ADDR = 0xFFFFFFFF;

    // String to print in Terminalfile
    std::string s;

    // Counter for completed output operations
    uint32_t output_operations = 0;

    // Debug test for clk counting
    int clks = 0;

    SC_CTOR(TERMINAL);
    TERMINAL(sc_module_name name, int latency = 1, bool use_stdout = true)
        : sc_module(name), latency(latency), use_stdout(use_stdout)
    {
        SC_THREAD(behaviour);
        sensitive << clk.pos();
        left_cycles = latency;
    }

    // TODO: What should output do? Description missing.
    void output(char value) {
        if (use_stdout) {
            _debug("\033[0;31m Terminal printing to stdout \n\033[0m")
            std::cout << value;
        } else {
            s += value;
        }
    }

    void behaviour()
    {
        bool prevWrite = false;
        while (true)
        {
            // if (addr.read() == TERMINAL_ADDR) {
            //     _debug("\033[0;31mAddr in Terminal: %u\n\033[0m", addr.read());
            //     _debug("\033[0;31mCLK in Terminal: %u\n\033[0m", clk.read());
            // }
            end.write(false);
            bool curWrite = w.read();

            if ((!prevWrite && curWrite)  && addr.read() == TERMINAL_ADDR) {

                ready.write(false);
                _debug("\033[0;31mTerminal printing.... \n\033[0m");
                uint32_t data = wdata.read();

                for (int i = 0; i < left_cycles; i++) {
                    _debug("\033[0;31m Terminal waiting\n\033[0m")
                    wait();
                }

                _debug("\033[0;31m WData to Print: <%u> \n\033[0m", data);

                char c = static_cast<char>(data & 0xFF); // Extract the character from the data - used Gemini
                _debug("\033[0;31m Char to Print: <%c> \n\033[0m", c);

                if (c == 0) {
                    end.write(true);
                    output_operations++;
                    if (use_stdout) {
                        std::cout.flush();
                    }
                    ready.write(true);
                }
                else {
                    output(c);
                }
                ready.write(true);
            }
            //_debug("Terminal: cycles left, %u\n", left_cycles)
            prevWrite = curWrite;
            clks++;
            wait();
        }
    }
};

#endif // TERMINAL_HPP