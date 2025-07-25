
#include "../include/helper_messages.h"
#include <stdio.h>
#include <stdlib.h>

void print_help_message(void)
{
  printf("\n======= WELCOME TO STDIO - Help Message =======\n");
  printf("Description: TinyRISC CPU Simulator\n\n");
  printf("Usage: ./project [OPTIONS] <input_file>\n\n");
  printf("DESCRIPTION:\n");
  printf("Simulates the TinyRISC CPU extended with a TERMINAL module.\n");
  printf("The simulator executes the program from the input file and handles\n");
  printf("terminal output according to the specified parameters.\n\n");
  printf("REQUIRED ARGUMENTS:\n");
  printf("  <input_file>                   Path to the binary input file to be executed\n\n");
  printf("OPTIONS:\n");
  printf("  -c, --cycles <count>           Number of clock cycles to simulate\n");
  printf("  -t, --tf <path>                Path for tracefile output (disabled by default)\n");
  printf("  -f, --terminal-file <path>     Redirect terminal output to specified file\n");
  printf("  -l, --latency <cycles>         Clock cycles needed for terminal output\n");
  printf("  -d, --debug                    Runs simulation in debug mode with extended console reports\n");
  printf("  -h, --help                     Display this help message and exit\n\n");
  printf("EXAMPLES:\n");
  printf("  Basic simulation:\n");
  printf("    ./project program.out\n\n");
  printf("  Simulation with custom cycles and terminal output:\n");
  printf("    ./project --cycles 5000 --terminal-file output.txt program.out\n\n");
  printf("  Simulation with tracefile:\n");
  printf("    ./project --tf trace.log --cycles 2000 program.out\n\n");
  printf("NOTES:\n");
  printf("  - The terminal latency specifies how many clock cycles are required\n");
  printf("    for each terminal output operation\n");
  printf("  - When --terminal-file is specified, all terminal output will be\n");
  printf("    captured and written to the specified file\n");
  printf("  - The tracefile will contain important signal traces when enabled\n\n");
  printf("===============================================\n");
}