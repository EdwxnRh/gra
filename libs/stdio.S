# x1 - function argument
# x11-x20 - temporary registers
# x11 - mapping address for terminal
# x12 - local variable for current character
# x13 - local variable for saving return address
# x31 - return address

print:
    li x11, 0            # loads address for memory-mapped terminal output
    li x12, 1
    sub x11, x11, x12      # 0 - 1 = 0xFFFFFFFF

    li x14, 2
    jal x13, 1           # Store current address for the loop
    add x13, x13, x14     # Increment the address by 2 to get the starting address of the loop

loop:
    ld x12, x1           # loads first character from string into temporary register
    sd x12, x11           # stores character at memory-mapped terminal address
    beq x12, x0, 3       # checks if character is null terminator 0x0 - if so, jump to end
    inc x1, x1          # increment address by one to get next character
    j x13                # next loop iteration

end:
    j x31                # end, jump back to previous return address    
    