#ifndef PC_HPP
#define PC_HPP

#include <systemc>
#include "systemc.h"
using namespace sc_core;

SC_MODULE(PC)
{
  sc_in<bool> clk;
  sc_in<bool> enable;
  sc_in<sc_bv<2>> j;
  sc_in<u_int32_t> next;
  sc_in<sc_bv<12>> offset;

  sc_out<uint32_t> pc;

  uint32_t pcValue;

  uint32_t totalClks;

  bool lastInstructionReached = false;

  uint32_t lastInstruction;

  SC_CTOR(PC);
  PC(sc_module_name name, uint32_t lastInstruction)
      : sc_module(name), lastInstruction(lastInstruction)
  {
    pcValue = 0x00001000;
    SC_THREAD(behaviour);
    sensitive << clk.pos();
  }

  void behaviour()
  {
    while (true)
    {
      //_debug(">>CLK<<\n")
      if (pcValue > lastInstruction)
      {
        _debug("\033[0;36m ---- Last Input Instruction passed ----\n\033[0m")
            lastInstructionReached = true;
      }
      if (!lastInstructionReached)
      {
        totalClks++;
      }
      wait();
      if (enable.read())
      {
        _debug("\n------------- New Instruction -------------\n")
            _debug("\033[0;36mPc Value: %u. Enable: %u, Jump: %u \n\033[0m", pcValue, enable.read(), j.read().to_uint());
        switch (j.read().to_uint())
        {
        case 0:
          pcValue = pcValue + 4;
          break;
        case 1:
          pcValue = next.read();
          break;
        case 2:
          pcValue = pcValue + offset.read().to_uint();
          break;
        default:
          break;
        }
      }
      pc.write(pcValue);
    }
  }
};

#endif // PC_HPP