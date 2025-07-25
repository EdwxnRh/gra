#ifndef REGISTER_FILE_HPP
#define REGISTER_FILE_HPP

#include <stdbool.h>
#include <stdint.h>
#include <systemc>
#include <systemc.h>
using namespace sc_core;

SC_MODULE(REGISTER_FILE) {
  sc_in<bool> clk;

  sc_in<sc_bv<5>> a1;
  sc_in<sc_bv<5>> a2;
  sc_in<sc_bv<5>> a3;
  sc_in<bool> we;
  sc_in<uint32_t> wdata;
  
  sc_out<uint32_t> r1;
  sc_out<uint32_t> r2;

  uint32_t registers_data[32];

  SC_CTOR(REGISTER_FILE) {
    for (int i = 0; i < 32; i++) {
      registers_data[i] = 0;
    }
    SC_THREAD(behaviour);
    sensitive << clk.pos();
  }

  void behaviour() {
    while(true) {
      wait();
      uint8_t reg1 = a1.read().to_uint();
      uint8_t reg2 = a2.read().to_uint();
      uint8_t reg3 = a3.read().to_uint();

      r1.write(reg1 != 0 ? registers_data[reg1] : 0);
      //_debug("\033[0;32m(Registerfile writing  %u on r1)\033[0m\n", registers_data[reg1]);
      r2.write(reg2 != 0 ? registers_data[reg2] : 0);
      //_debug("\033[0;32m(Registerfile writing  %u on r2)\033[0m\n", registers_data[reg2]);

      if(we.read()) {
        registers_data[reg3] = wdata.read();
      _debug("\033[0;32m(Registerfile wrote %u in Reg:%u)\033[0m\n", wdata.read(), reg3);
      }
      // _debug("\033[0;32m(Registerfile: r1: %u, r2, %u)\033[0m\n", r1.read(), r2.read());
      //  if (DEBUG) {
      //    printRegisters(registers_data);
      //  }
    }
  }


  void printRegisters(const uint32_t r[32]) {
    std::cout << "\033[0;32m";
    for (int i = 0; i < 6; ++i)
      std::cout << "R" << i << ":" << r[i] << (i < 31 ? " | " : "");
    std::cout << "\033[0m\n";
  }
};

#endif // REGISTER_FILE_HPP