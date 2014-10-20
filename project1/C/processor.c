
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define CPU_READ read_pipe[0]
#define CPU_WRITE write_pipe[1]
#define MEMORY_READ write_pipe[0]
#define MEMORY_WRITE read_pipe[1]

const int USER_MODE = 0;
const int SYSTEM_MODE = 1;

const int MEMORY_SIZE = 2000;
const int SYSTEM_CODE_START = 1000;
const int TIMER_INTERRUPT_START = 1000;
const int INT_INTERRUPT_START = 1500;

const int READ_COMMAND = 1;
const int WRITE_COMMAND = 0;
const int CRASH_COMMAND = -1;

/* reads memory from file and stores in an integer array */
int initialize(int memory[], char* filename) {
    FILE *file = fopen(filename, "r");
    char line[128];
    if (file) {
        int counter = 0;
        while (fgets(line, sizeof(line), file) != NULL) {
            int data = 0;
            if (line[0] == '.') {
                sscanf(line, ". %d", &data);
                counter = data;
            } else if (line[0] == ' ' || line[0] == '\n') {
                continue;
            } else {
                sscanf(line, "%d", &data);
                memory[counter++] = data;
                char c = (char) data;
            }
        } // END while
        fclose(file);
        return 0;
    } else {
        // file not exist
        fclose(file);
        return 1;
    }
}

/* Checks validity of a memory addres */
int validate_address(int mode, int address) {
    if (address < 0 || address >= MEMORY_SIZE) return 1;    // address does not exist!
    if (mode == USER_MODE) {
        // permission denied!
        if (SYSTEM_CODE_START <= address && address < MEMORY_SIZE) return 2;
    }
    return 0;
}

int main(int argc, char *argv[]) {

    int read_pipe[2];
    int write_pipe[2];

    if (argc != 3) {
        printf("error: missing argument(s)\n");
        exit(1);
    }
    char *filename = argv[1];

    // make pipe to communicate
    if (pipe(read_pipe) < 0 || pipe(write_pipe) < 0) {
        printf("Pipe failed!\n");
        exit(1);
    }

    int pid = fork();
    if (pid == -1) {
        /* Here pid is -1, the fork failed */
        /* Some possible reasons are that you're */
        /* out of process slots or virtual memory */
        perror("The fork failed!\n");
        exit(2);
    }
    if (pid == 0) {
        /********** Memory **********/

        // close parent's pipes
        close(read_pipe[0]);
        close(write_pipe[1]);

        // initialize memory from file
        int memory[MEMORY_SIZE];
        int initialize_error = initialize(memory, filename);
        write(MEMORY_WRITE, &initialize_error, sizeof(int));
        if (initialize_error == 1) _exit(0);

        // read from parent
        while (1) {
            int address = -1;
            int command = -2;

            if (read(MEMORY_READ, &command, sizeof(int)) >= 0) {
                if (command == CRASH_COMMAND) {
                    // crash signal from the parent process
                    _exit(1);
                } else if (command == READ_COMMAND) {
                    // returns the value at address
                    read(MEMORY_READ, &address, sizeof(int));
                    write(MEMORY_WRITE, &memory[address], sizeof(int));
                } else if (command == WRITE_COMMAND) {
                    // writes a value to address
                    int data = -1;
                    read(MEMORY_READ, &address, sizeof(int));
                    read(MEMORY_READ, &data, sizeof(int));
                    memory[address] = data;
                }
            }
        } // END while
        _exit(0);
    }

    /********** CPU **********/

    // close child's pipes
    close(write_pipe[0]);
    close(read_pipe[1]);

    // check whether child process fails to initialize
    int initialize_error = -1;
    while (initialize_error == -1) {
        read(CPU_READ, &initialize_error, sizeof(int));
        if (initialize_error == 1) {
            printf("error: input file does not exist!\n");
            waitpid(-1, NULL, 0);
            exit(1);
        }
    }

    int TIMEOUT = -1;
    int timer = 0;
    int is_timer_interrupt = false;
    int interrupted = false;

    // initialize CPU's variables
    int program_counter = 0;
    int stack_pointer = SYSTEM_CODE_START;
    int instruction_register = 0;
    int accumulator = 0;
    int x = 0;
    int y = 0;
    int mode = USER_MODE;

    sscanf(argv[2], "%d", &TIMEOUT);
    if (TIMEOUT < 5) {
        printf("timeout too short!\n");
        goto ERROR_EXIT;
    }

    int address = -1;
    int flag = -2;
    while (1) {
        /* Fetch new instruction */
        write(CPU_WRITE, &READ_COMMAND, sizeof(int));
        write(CPU_WRITE, &program_counter, sizeof(int));
        program_counter++;
        if (!interrupted) timer ++;

        // read the returned data/instruction from memory
        // then execute
        if (read(CPU_READ, &instruction_register, sizeof(int)) >= 0) {
            switch (instruction_register) {
                case 1:
                    /* Loads the value into AC */
                    // fetch the value from the memory
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &program_counter, sizeof(int));
                    program_counter++;

                    // reads the value to the AC
                    read(CPU_READ, &accumulator, sizeof(int));
                    if (!interrupted) timer++;
                    break;

                case 2:
                    /* Loads the value in address into AC */
                    address = -1;
                    // fetch the address from memory
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &program_counter, sizeof(int));
                    program_counter++;
                    read(CPU_READ, &address, sizeof(int));
                    if (!interrupted) timer++;

                    // check validity of the address
                    flag = validate_address(mode, address);
                    if (flag != 0) {
                        if (flag == 1) {
                            // address does not exist!
                            printf("error: address does not exist!\n");
                        } else if (flag == 2) {
                            // invalid write operation!
                            printf("error: permission denied! Cannot access system code in user mode\n");
                        }
                        goto ERROR_EXIT;
                    }
                    // read the value at address to the AC
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &address, sizeof(int));
                    read(CPU_READ, &accumulator, sizeof(int));
                    if (!interrupted) timer++;
                    break;

                case 3:
                    /* Loads the value from the address found in the address into the AC */
                    address = -1;
                    // fetch the address from the memory
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &program_counter, sizeof(int));
                    program_counter++;
                    read(CPU_READ, &address, sizeof(int));
                    if (!interrupted) timer++;

                    // check validity of the address
                    flag = validate_address(mode, address);
                    if (flag != 0) {
                        if (flag == 1) {
                            // address does not exist!
                            printf("error: address does not exist!\n");
                        } else if (flag == 2) {
                            // invalid write operation!
                            printf("error: permission denied! Cannot access system code in user mode\n");
                        }
                        goto ERROR_EXIT;
                    }
                    // read the address at address
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &address, sizeof(int));
                    read(CPU_READ, &address, sizeof(int));
                    if (!interrupted) timer++;

                    // check validity of the address
                    flag = validate_address(mode, address);
                    if (flag != 0) {
                        if (flag == 1) {
                            // address does not exist!
                            printf("error: address does not exist!\n");
                        } else if (flag == 2) {
                            // invalid write operation!
                            printf("error: permission denied! Cannot access system code in user mode\n");
                        }
                        goto ERROR_EXIT;
                    }
                    // read the value at adress to the AC
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &address, sizeof(int));
                    read(CPU_READ, &accumulator, sizeof(int));
                    if (!interrupted) timer++;
                    break;

                case 4: case 5:
                    /* Loads the value at address (address + X) or (address + Y) into the AC */
                    address = -1;
                    // read the address
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &program_counter, sizeof(int));
                    program_counter++;
                    read(CPU_READ, &address, sizeof(int));
                    if (!interrupted) timer++;

                    // read the value at (address + X) or (address + Y) into the AC
                    if (instruction_register == 4) {
                        address += x;
                    } else if (instruction_register == 5) {
                        address += y;
                    }

                    // check validity of the address
                    flag = validate_address(mode, address);
                    if (flag != 0) {
                        if (flag == 1) {
                            // address does not exist!
                            printf("error: address does not exist!\n");
                        } else if (flag == 2) {
                            // invalid write operation!
                            printf("error: permission denied! Cannot access system code in user mode\n");
                        }
                        goto ERROR_EXIT;
                    }
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &address, sizeof(int));
                    read(CPU_READ, &accumulator, sizeof(int));
                    if (!interrupted) timer++;
                    break;

                case 6:
                    /* Loads from (stack_pointer + X) into the AC */
                    address = stack_pointer + x;
                    // check validity of the address
                    flag = validate_address(mode, address);
                    if (flag != 0) {
                        if (flag == 1) {
                            // address does not exist!
                            printf("error: address does not exist!\n");
                        } else if (flag == 2) {
                            // invalid write operation!
                            printf("error: permission denied! Cannot access system code in user mode\n");
                        }
                        goto ERROR_EXIT;
                    }
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &address, sizeof(int));
                    read(CPU_READ, &accumulator, sizeof(int));
                    if (!interrupted) timer++;
                    break;

                case 7:
                    /* Stores the value in AC to the address */
                    address = -1;
                    // fetch the address from memory
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &program_counter, sizeof(int));
                    program_counter++;
                    read(CPU_READ, &address, sizeof(int));
                    if (!interrupted) timer++;

                    // check validity of the address
                    flag = validate_address(mode, address);
                    if (flag != 0) {
                        if (flag == 1) {
                            // address does not exist!
                            printf("error: address does not exist!\n");
                        } else if (flag == 2) {
                            // invalid write operation!
                            printf("error: permission denied! Cannot access system code in user mode\n");
                        }
                        goto ERROR_EXIT;
                    }

                    // writes the AC to the address
                    write(CPU_WRITE, &WRITE_COMMAND, sizeof(int));
                    write(CPU_WRITE, &address, sizeof(int));
                    write(CPU_WRITE, &accumulator, sizeof(int));
                    break;

                case 8:
                    // gets a random integer from 1 to 100 into the AC
                    srand(time(NULL));
                    accumulator = rand() % 99 + 1;
                    break;

                case 9: ;
                    /* Writes AC as an int or char into the screen */
                    // fetch port number from the memory
                    int port = -1;
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &program_counter, sizeof(int));
                    program_counter++;
                    read(CPU_READ, &port, sizeof(int));
                    if (!interrupted) timer++;

                    if (port == 1) {
                        // writes as an integer
                        printf("%d", accumulator);
                    } else if (port == 2) {
                        // writes as a character
                        write(1, &accumulator, sizeof(int));
                    } else {
                        printf("error: print to invalid port!\n");
                        goto ERROR_EXIT;
                    }
                    break;

                case 10:
                    // add the value from X to AC
                    accumulator += x;
                    break;

                case 11:
                    // add the value from Y to AC
                    accumulator += y;
                    break;

                case 12:
                    // subtract the value from X from AC
                    accumulator -= x;
                    break;

                case 13:
                    // subtract the value from Y from AC
                    accumulator -= y;
                    break;

                case 14:
                    // copy the value from AC to X
                    x = accumulator;
                    break;

                case 15:
                    // copy the value from X to AC
                    accumulator = x;
                    break;

                case 16:
                    // copy the value from AC to Y
                    y = accumulator;
                    break;

                case 17:
                    // copy the value from Y to AC
                    accumulator = y;
                    break;

                case 18:
                    // copy the value from AC to SP
                    stack_pointer = accumulator;
                    break;

                case 19:
                    // copy the value from SP to AC
                    accumulator = stack_pointer;
                    break;

                case 20:
                    /* Jumps to the address */
                    // fetch the address
                    address = -1;
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &program_counter, sizeof(int));
                    program_counter++;
                    read(CPU_READ, &address, sizeof(int));
                    if (!interrupted) timer++;

                    // jump
                    JUMP:
                    // check validity of the address
                    flag = validate_address(mode, address);
                    if (flag != 0) {
                        if (flag == 1) {
                            // address does not exist!
                            printf("error: address does not exist!\n");
                        } else if (flag == 2) {
                            // invalid write operation!
                            printf("error: permission denied! Cannot access system code in user mode\n");
                        }
                        goto ERROR_EXIT;
                    }
                    program_counter = address;  // jump
                    break;

                case 21: case 22:
                    /*  case 21: Jumps to the address only if the value in the AC is zero 
                        case 22: Jumps if the value in the AC is not zero */
                    // fetch the address from memory
                    address = -1;
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &program_counter, sizeof(int));
                    program_counter++;
                    read(CPU_READ, &address, sizeof(int));
                    if (!interrupted) timer++;

                    if (instruction_register == 21) {
                        if (accumulator == 0) {
                            goto JUMP;
                        }
                    } else if (instruction_register == 22) {
                        if (accumulator != 0) {
                            goto JUMP;
                        }
                    }
                    break;

                case 23:
                    /* Push return address (i.e. program counter) onto stack, jump to the address */

                    // read address
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &program_counter, sizeof(int));
                    program_counter++;
                    read(CPU_READ, &address, sizeof(int));
                    if (!interrupted) timer++;

                    // push return address to user's stack
                    stack_pointer --;
                    write(CPU_WRITE, &WRITE_COMMAND, sizeof(int));
                    write(CPU_WRITE, &stack_pointer, sizeof(int));
                    write(CPU_WRITE, &program_counter, sizeof(int));

                    goto JUMP;
                    break;

                case 24:
                    /* Pop return address from the stack, jump to the address */
                    if (stack_pointer == SYSTEM_CODE_START || stack_pointer == MEMORY_SIZE) {
                        // nothing to pop
                        printf("error: invalid stack-popping operation!\n");
                        goto ERROR_EXIT;
                    }
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &stack_pointer, sizeof(int));
                    read(CPU_READ, &address, sizeof(int));
                    stack_pointer++;
                    if (!interrupted) timer++;

                    goto JUMP;
                    break;

                case 25:
                    // Increment the value in X
                    x ++;
                    break;

                case 26:
                    // Increment the value in Y
                    x --;
                    break;

                case 27:
                    // Push AC onto stack
                    stack_pointer --;
                    write(CPU_WRITE, &WRITE_COMMAND, sizeof(int));
                    write(CPU_WRITE, &stack_pointer, sizeof(int));
                    write(CPU_WRITE, &accumulator, sizeof(int));
                    break;

                case 28:
                    // Pop from stack into AC
                    if (stack_pointer == SYSTEM_CODE_START || stack_pointer == MEMORY_SIZE) {
                        // nothing to pop
                        printf("error: invalid stack-popping operation!\n");
                        goto ERROR_EXIT;
                    }
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &stack_pointer, sizeof(int));
                    read(CPU_READ, &accumulator, sizeof(int));
                    stack_pointer ++;
                    if (!interrupted) timer++;
                    break;

                case 29:
                    /* System call: interrupt */
                    // Set system mode, switch stack, push stack pointer and program counter
                    // Set new stack pointer and program counter
                    if (!interrupted) {
                        interrupted = true;
                        mode = SYSTEM_MODE;

                        // push stack pointer to system stack
                        address = MEMORY_SIZE - 1;
                        write(CPU_WRITE, &WRITE_COMMAND, sizeof(int));
                        write(CPU_WRITE, &address, sizeof(int));
                        write(CPU_WRITE, &stack_pointer, sizeof(int));
                        stack_pointer = MEMORY_SIZE - 1;    // switch stack

                        // push program counter
                        stack_pointer --;
                        write(CPU_WRITE, &WRITE_COMMAND, sizeof(int));
                        write(CPU_WRITE, &stack_pointer, sizeof(int));
                        write(CPU_WRITE, &program_counter, sizeof(int));
                        program_counter = INT_INTERRUPT_START;
                    }
                    break;

                case 30:
                    /* Restores registers, set user mode */
                    if (mode == USER_MODE) {
                        // invalid instruction
                        printf("error: call instruction 30 in user mode!\n");
                        goto ERROR_EXIT;
                    }

                    /* SYSTEM MODE */
                    // pop program counter
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &stack_pointer, sizeof(int));
                    read(CPU_READ, &program_counter, sizeof(int));
                    stack_pointer ++;

                    // pop user's stack pointer
                    write(CPU_WRITE, &READ_COMMAND, sizeof(int));
                    write(CPU_WRITE, &stack_pointer, sizeof(int));
                    read(CPU_READ, &stack_pointer, sizeof(int));

                    // get out of the interrupt handler
                    interrupted = false;
                    is_timer_interrupt = false;
                    mode = USER_MODE;
                    break;

                case 50:
                    // end of program
                    goto NORMAL_EXIT;

                default:
                    /* Invalid instruction */
                    printf("error: invalid instruction at %d\n", (--program_counter));
                    goto ERROR_EXIT;
            }
        }
        if (timer >= TIMEOUT)  {
            // cause interrupt
            timer = 0;
            interrupted = true;
            is_timer_interrupt = true;

            mode = SYSTEM_MODE;

            // push stack pointer to system stack
            address = MEMORY_SIZE - 1;
            write(CPU_WRITE, &WRITE_COMMAND, sizeof(int));
            write(CPU_WRITE, &address, sizeof(int));
            write(CPU_WRITE, &stack_pointer, sizeof(int));
            stack_pointer = MEMORY_SIZE - 1;    // switch stack

            // push program counter
            stack_pointer --;
            write(CPU_WRITE, &WRITE_COMMAND, sizeof(int));
            write(CPU_WRITE, &stack_pointer, sizeof(int));
            write(CPU_WRITE, &program_counter, sizeof(int));
            program_counter = TIMER_INTERRUPT_START;
        }
    } // END while

    NORMAL_EXIT:
    // wait for child process to end
    write(CPU_WRITE, &CRASH_COMMAND, sizeof(int));
    waitpid(-1, NULL, 0);
    return 0;

    ERROR_EXIT:
    write(CPU_WRITE, &CRASH_COMMAND, sizeof(int));
    waitpid(-1, NULL, 0);
    exit(1);
}