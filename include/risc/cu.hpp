#ifndef CU_HPP
#define CU_HPP

#include <systemc>
#include <systemc.h>
using namespace sc_core;

SC_MODULE(CU) {
  sc_in<bool> clk;
  sc_in<uint32_t> pcin;
  sc_in<uint32_t> datain;
  sc_in<bool> memready;
  sc_in<uint32_t> ro;
  sc_in<bool> overflow;
  sc_in<uint32_t> memAddr; // new input port to control the select output

  sc_out<bool> pcenable;
  sc_out<sc_bv<2>> pcj;
  sc_out<sc_bv<12>> pcoffset;
  sc_out<uint32_t> addr;
  sc_out<bool> rmem;
  sc_out<bool> wmem;
  sc_out<sc_bv<3>> op;
  sc_out<sc_bv<5>> a1;
  sc_out<sc_bv<5>> a2;
  sc_out<sc_bv<5>> a3;
  sc_out<bool> we;
  sc_out<uint8_t> rsource;
  sc_out<uint8_t> msource;
  sc_out<uint8_t> select; // New output signal Select to
  sc_out<uint32_t> imm; // New output for Imm

  uint8_t lead;
  uint8_t rs1;
  uint8_t rs2;
  uint8_t rd;
  uint8_t funct;
  uint8_t opcode;
  uint32_t immediate;

  bool lastOverflow;

  SC_CTOR(CU) {
    SC_THREAD(behaviour);
    sensitive << clk.pos();
  }

  void behaviour() {
    while(true) {
      wait();
      wait(SC_ZERO_TIME);
      we.write(false);
      pcenable.write(false);
      pcj.write(false);

      instructionFetch();
      instructionDecode();
      execution();
      memoryAccess();
      writeBack();
    }
  }

  void instructionFetch() {
    uint32_t address = pcin.read();
    addr.write(address);

    _debug("\033[0;34mInstruction Fetch: Addr for instr in CU: %u\033[0m\n", addr.read());

    select.write(0);
    rmem.write(true);
    msource.write(1);

    do {
      wait();
      wait(SC_ZERO_TIME);
    } while(!memready.read());

    rmem.write(false);
  }

  void instructionDecode() {
    wait(SC_ZERO_TIME);
    uint32_t instruction = datain.read();

    _debug("\033[0;34mInstruction decode: Inst in CU from Memory: 0x%x\033[0m\n", instruction);

    immediate = instruction & 0xFFFFF000;
    immediate = immediate >> 12;
    opcode = instruction & 0b1111111;
    rd = (instruction >> 7) & 0b11111;
    funct = (instruction >> 12) & 0b111;
    rs1 = (instruction >> 15) & 0b11111;
    rs2 = (instruction >> 20) & 0b11111;
    lead = (instruction >> 25) & 0b1111111;

    _debug("\033[0;34mInstruction decode: rs1: %u, rs2: %u\n\033[0m", rs1, rs2);

    a1.write(rs1);
    a2.write(rs2);
    if(opcode == 0b1100011 && funct == 0) {
      a1.write(rd);
    }
    wait();
  }

  void execution() {
    if(opcode == 0b110011) {
      op.write(funct);
    }
    else if(opcode == 0b1100011) {
      op.write(5);
    }

    wait();
  }

  void memoryAccess() {
    if(opcode == 0b100011) {
      msource.write(0);
      rmem.write(funct == 0);
      wmem.write(funct == 1);

      _debug("\033[0;34mMemory/Terminal Access: MemAddr: %u, Rmem: %u\n\033[0m", memAddr.read(), rmem.read() );

      if (memAddr == 0xFFFFFFFF) {
        _debug("\033[0;34mMemAddr is Terminaladdr: Setting select to 1\n\033[0m");
        select.write(1);
      }
      else {
        select.write(0);
      }

      do {
        wait();
        wait(SC_ZERO_TIME);
      } while(!memready.read());

      rmem.write(0);
      wmem.write(0);
    }
  }

  void writeBack() {
    if(opcode == 0b110011) {
      _debug("\033[0;34mWriteback R-Type: rd: %u\n\033[0m", rd);
      we.write(true);
      a3.write(rd);
      rsource.write(0);
      lastOverflow = overflow.read();
    }
    else if (opcode == 0b0100010) {
      _debug("\033[0;34mWriteback U-Type: rd: %u, imm: %u\n\033[0m", rd, immediate);
      a3.write(rd);
      we.write(true);
      imm.write(immediate);
      rsource.write(2);
    }
    else if(opcode == 0b1100011) {
      _debug("\033[0;34mWriteback B-Type: rd: %u\n\033[0m", rd);
      uint32_t result = ro.read();
      if(funct == 0) {
        pcoffset.write(0);
        pcj.write(1);
      } else if((funct == 1 && result == 0) || 
                (funct == 2 && result != 0) || 
                (funct == 3 && result > 0x80000000) ||
                (funct == 4 && result < 0x80000000)) {
        pcoffset.write(rd | (lead << 5));
        pcj.write(2);
      } else {
        pcoffset.write(0);
        pcj.write(0);
      }
    } else if(opcode == 0b100011 && funct == 0) {
      _debug("\033[0;34mWriteback S-Type: rd: %u\n\033[0m", rd);
      we.write(true);
      a3.write(rd);
      rsource.write(1);
    }
    pcenable.write(true);

    // wait happens in while loop.
  }
};

#endif // CU_HPP