
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../include/helper_messages.h"
//#include "../include/assembler/assembler.h"
#include "../include/tinyRiscMain.hpp"
#include "../include/debug_logger.h"
bool DEBUG = false;

unsigned long parseToUL(const char *optarg, const char *operation) //TODO checken ob alles geclosed wird bei Fehler!
{
    if (optarg[0] == '-')
    {
        fprintf(stderr, "Error: Invalid Argument %s for %s, which requires a positive Input number.\n", optarg, operation);
        exit(EXIT_FAILURE);
    }
    char *endptr;
    errno = 0;
    uint32_t result = strtoul(optarg, &endptr, 10);

    if (errno == ERANGE)
    {
        fprintf(stderr, "Error: invalid number for %s : %s\n Check if the number is in range\n", operation, optarg);
        exit(EXIT_FAILURE);
    }
    if (errno == EINVAL || endptr == optarg)
    {
        fprintf(stderr, "No valid input number specified for %s\n", operation);
        exit(EXIT_FAILURE);
    }
    if (*endptr != '\0')
    {
        printf("Further characters after number: \"%s for %s\n", endptr, operation);
        exit(EXIT_FAILURE);
    }
    return result;
}

uint32_t parseBinToUL(const char *inst) {
    if (inst == NULL || *inst == '\0') {
        fprintf(stderr, "Error: Instruction is empty or null\n");
        return UINT32_MAX;
    }
    char* endptr;
    errno = 0;
    uint32_t result = strtoul(inst, &endptr, 2);
    if (errno == ERANGE) {
        fprintf(stderr, "Error: Instruction '%s' is out of range for a 32-bit unsigned integer.\n", inst);
        return UINT32_MAX;
    }
    if (errno == EINVAL || endptr == inst) {
        fprintf(stderr, "Error: Invalid instruction found %s\n", inst);
        return UINT32_MAX;
    }
    return result;
}

void convertInstructions() {
    //TODO
}

int parse_cli(int argc, char **argv, char **input_file, uint32_t *cycles, char **trace_file, const char **terminal_file, uint32_t *latency)
{

    static struct option long_options[] = {
        {"cycles", required_argument, 0, 'c'},
        {"tf", required_argument, 0, 't'},
        {"terminal-file", required_argument, 0, 'f'},
        {"latency", required_argument, 0, 'l'},
        {"help", no_argument, 0, 'h'},
        {"debug", no_argument, 0, 'd'},
        {0, 0, 0, 0}
    };

    // Standartwerte
    *cycles = 20;
    *latency = 1;

    int opt;
    while ((opt = getopt_long(argc, argv, "c:t:f:l:hd", long_options, NULL)) != -1)
    {
        switch (opt)
        {
        case 'h':
            print_help_message();
            return EXIT_SUCCESS;
        case 't':
            *trace_file = optarg;
            break;
        case 'l':
            *latency = parseToUL(optarg, "latency");
            break;
        case 'c':
            *cycles = parseToUL(optarg, "cycles");
            break;
        case 'f':
            *terminal_file = "terminal.out";
            break;
        case 'd':
            DEBUG = true;
            break;
        case '?':
            // Unknown option
            fprintf(stderr, "Unknown option: -%c\n", optopt);
            return 1;
        default:
            // This should not happen
            fprintf(stderr, "Unexpected error while parsing options.\n");
            return 1;
        }
    }

    if (optind >= argc)
    {
        fprintf(stderr, "No input file specified.\n");
        //print_help_message();
        return 1;
    }

    // TODO Checken ob das passt: (ist noch aus Musterlösung)
    while (optind < argc)
    {
        if (*input_file != NULL)
        {
            fprintf(stderr, "Too many input files specified.\n");
            return 1;
        }
        *input_file = argv[optind];
        optind++;
    }

    return 0;
}

FILE *validate_and_open_read(const char *path)
{
    FILE *file;
    if (!((file = fopen(path, "r"))))
    {
        return NULL;
    }

    struct stat statbuf;
    if (fstat(fileno(file), &statbuf))
    {
        fclose(file);
        return NULL;
    }
    if (!S_ISREG(statbuf.st_mode) || statbuf.st_size <= 0)
    {
        fclose(file);
        return NULL;
    }
    return file;
}

char validate_write(const char *path)
{
    FILE *file;
    if (!((file = fopen(path, "w"))))
    {
        return 0;
    }
    fclose(file);
    return 1;
}

void to_binary(uint32_t num, char *str)
{
    for (int i = 31; i >= 0; i--)
    {
        str[i] = (num & 1) ? '1' : '0';
        num >>= 1;
    }
    str[32] = '\0';
}

int main(int argc, char **argv)
{

    // char* output_file = NULL;
    char *input_file = NULL;
    char *trace_file = NULL;
    const char *terminal_file = NULL;
    u_int32_t cycles = 0;
    u_int32_t latency = 0;
    char terminalToStdout = false;

    int result = parse_cli(argc, argv, &input_file, &cycles, &trace_file, &terminal_file, &latency);
    if (result != 0 || input_file == NULL)
    {
        return result;
    }

    if (trace_file != NULL) {
        if(!validate_write(trace_file)) {
            fprintf(stderr, "The provided trace file path ('%s') is not writable\n", trace_file);
            return EXIT_FAILURE;
        }
    }


    // Decides if the Output is the Terminal File then validate it or else set terminalToStdout to true
    if (terminal_file != NULL)
    {
        if (!validate_write(terminal_file))
        {
            fprintf(stderr, "Terminal file '%s' was not accessible.\n", terminal_file); // Maybe wäre Could not write to terminal file besser
            return EXIT_FAILURE;
        }
        terminalToStdout = false;
    }
    else
    {
        terminalToStdout = true;
    }

    FILE *fileInputInstructions = validate_and_open_read(input_file);
    if (fileInputInstructions == NULL)
    {
        fprintf(stderr, "Failed to open input file '%s'.\n", input_file);
        return 1;
    }
    fseek(fileInputInstructions, 0, SEEK_END);
    long file_size = ftell(fileInputInstructions);
    fseek(fileInputInstructions, 0, SEEK_SET);

    char *inputInstructions = (char *)malloc(file_size + 1);
    if (inputInstructions == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for input file.\n");
        fclose(fileInputInstructions);
        return 1;
    }
    size_t numberOfInstructions = (file_size + 1) / 33; // Dateigröße/33 Byte ergibt numberOfInstructions
    uint32_t numberOfInstructionsInt = (file_size + 1) / 33; // Dateigröße/4 Byte ergibt numberOfInstructions
    uint32_t *instructions = (uint32_t *)malloc((numberOfInstructions + 1) * sizeof(uint32_t));
    if (instructions == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for instructions.\n");
        fclose(fileInputInstructions);
        free(inputInstructions);
        free(instructions);
        return EXIT_FAILURE;
    }

    char *instruction = (char*) malloc(34);
    if (instruction == NULL) {
        fprintf(stderr, "Failed to allocate memory for instruction.\n");
        fclose(fileInputInstructions);
        free(inputInstructions);
        return EXIT_FAILURE;
    }

    int i = 1;
    while (i <= numberOfInstructions) {
        instruction[32] = 'f'; //change the pre to last character in order to find short instructions
        if (fgets(instruction, 34, fileInputInstructions) == NULL) {
            fprintf(stderr, "Failed to read line in input file. \n");
        }

        _debug("Instruction Number %i: %s", i, instruction);

        uint32_t instUint = parseBinToUL(instruction);
        if ((instruction[32] != '\n' && i < numberOfInstructions)
            || (!(instruction[32] == 0x00 || instruction[32] == '\n') && i == numberOfInstructions)) {
            fprintf(stderr, "Failed to read instruction. Length of instruction %i is not 32 bit.\n", i);
            fclose(fileInputInstructions);
            free(inputInstructions);
            free(instruction);
            return EXIT_FAILURE;
        }

        _debug("Instruction Number %i in uint: 0x%x\n", i, instUint);

        if (instUint == UINT32_MAX) {
            fclose(fileInputInstructions);
            free(inputInstructions);
            free(instruction);
            return EXIT_FAILURE;
        }
        instructions[i] = instUint;
        i++;
    }
    *instructions = numberOfInstructionsInt; // Write the number of Instructions into the first element of instructions (Pascal Instrucions :P)

    // For Debugging
    _debug("First char in first Instruction: %u\n", inputInstructions[0]);
    _debug("Cycles: %u\n", cycles);
    _debug("Latency: %u\n", latency);
    _debug("Number of Instructions: %lu\n", numberOfInstructions);
    _debug("st\n");
    _debug("end of main, start of simulation:\n");


    struct Result resultStruct = run_simulation(cycles, trace_file, instructions, terminalToStdout, latency);

    if (!terminalToStdout) {
        _debug("Terminal to stdout: writing in File\n");
        if (validate_write(terminal_file)) {
            FILE *terFile = fopen(terminal_file, "w");
            fprintf(terFile, "%s\n", resultStruct.output);
            fclose(terFile);
        }
    }

    printf("\n========= TinyRISC Simulation Results =========\n");
    printf("Total cycles executed:   %u\n", resultStruct.cycles);
    printf("Terminal output blocks:  %u\n", resultStruct.outs);
    printf("===============================================\n");

    free(resultStruct.output);
    free(inputInstructions);
    free(instructions);
    fclose(fileInputInstructions);

    return EXIT_SUCCESS;
}
