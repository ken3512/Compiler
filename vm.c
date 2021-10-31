
/**
 * @file vm.c
 * @author   Kenny Subrahmanyam
 * @date 2021-06-01
 * @copyright Copyright (c) 2021
 */


// Predefined libraries and constants
#include <stdlib.h>
#include <stdio.h>
#define MAX_PAS_LENGTH 500

// Variables
int length = 0;
int pas[MAX_PAS_LENGTH];
int OP, L, M;
int SP, BP, PC = 0, halt = 1;

// Base function from source file
int base(int L)
{
    int arb = BP; // Arb = activation record base
    while ( L > 0)     // Find base L levels down
    {
        arb = pas[arb];
        L--;
    }
    return arb;
}

// Main function
int main (int argV, char ** Filename)
{
    // Open file
    FILE * ifp = fopen(Filename[1], "r");
    
    // Scan information in until it reaches the end of file
    while (!feof(ifp))
    {
        // Scan in PAS values
        fscanf(ifp, "%d", &pas[length]);
        
        // Update length
        length++;    
    }

    // Intialize SP and BP
    SP = length - 1, BP = SP + 1;
    
    // Print the intial values of PC, BP, and SP
    printf("\t\t PC BP SP   stack\n");
    printf("Intitail values: 0  %2d %2d\n\n", BP, SP);

    // Continue reading through instructions while the halt flag is still 0
    while(halt == 1)
    {
        // Read information in chunks of 3
        OP = pas[PC];
        L = pas[PC + 1];
        M = pas[PC + 2];
        
        // Keep track of temporary PC value for later use
        int temp_PC = PC;

        // Show current PC position if the OP code isn't 9
        if(OP != 9)
            printf("%2d ", PC);
         
        // Increment by 3 so we can read information in chunks of 3
        PC+=3;

        // Switch OP code operations from values 1 tp 9
        switch (OP)
        {

            // Push a constant value (literal) M onto the stack
            case 1:
                SP++;
                pas[SP] = M; 
                printf("LIT");
                break;
            
            // Operation to be performed on the data at the top of the stack
            case 2:
                switch(M)
                {
                    case 0:
                        SP = BP - 1;
                        BP = pas[SP + 2];
                        PC = pas[SP + 3];
                        printf("RTN");
                        break;

                    // Negation
                    case 1:
                        pas[SP] = -pas[SP];
                        printf("NEG");
                        break;

                    // Add
                    case 2:
                        SP --;

                        pas[SP] = pas[SP] + pas[SP + 1];
                        printf("ADD");
                        break;

                    // Subtract
                    case 3:
                        SP --;
                        pas[SP] = pas[SP] - pas[SP + 1];
                        printf("SUB");
                        break;

                    // Multiply
                    case 4:
                        SP --;
                        pas[SP] = pas[SP] * pas[SP + 1];
                        printf("MUL");
                        break;

                    // Divide
                    case 5:
                        SP --;
                        pas[SP] = pas[SP] / pas[SP + 1];
                        printf("DIV");
                        break;

                    // Set to odd state
                    case 6:
                        pas[SP] = pas[SP] % 2;
                        printf("ODD");
                        break;
                    
                    // Modulo
                    case 7:
                        SP --;
                        pas[SP] = pas[SP] % pas[SP + 1];
                        printf("MOD");
                        break;

                    // Set to equivilance state
                    case 8:
                        SP --;
                        pas[SP] = !(pas[SP] == pas[SP + 1]);
                        printf("EQL");
                        break;

                    // Set to non-equivilance state
                    case 9:
                        SP --;
                        pas[SP] = !(pas[SP] != pas[SP + 1]);
                        printf("NEQ");
                        break;

                    // Set to less than state
                    case 10:
                        SP --;
                        pas[SP] = !(pas[SP] < pas[SP + 1]);
                        printf("LSS");
                        break;

                    // Set to less than or equal state
                    case 11:
                        SP --;
                        pas[SP] = !(pas[SP] <= pas[SP + 1]);
                        printf("LEQ");
                        break;

                    // Set to greater than state
                    case 12:
                        SP --;
                        pas[SP] = !(pas[SP] > pas[SP + 1]);
                        printf("GTR");
                        break;
                    
                    // Set to greater than or equal state
                    case 13:
                        SP --;
                        pas[SP] = !(pas[SP] >= pas[SP + 1]);
                        printf("GEQ");
                        break;
                }
                break;

            // Load value to top of stack from the stack location at offset M from L lexicographical levels down
            case 3:
                SP++;
                pas[SP] = pas[base(L) + M];
                printf("LOD");
                break;

            // Store value at top of stack in the stack location at offset M from L lexicographical levels down
            case 4:
                pas[base(L) + M] = pas[SP];
                SP--;
                printf("STO");
                break;

            // Call procedure at code index M (generates new Activation Record and PC = M)
            case 5:
                pas[SP + 1] = base(L);
                pas[SP + 2] = BP;
                pas[SP + 3] = PC;
                BP = SP + 1;
                PC = M;
                printf("CAL");
                break;

            // Allocate M memory words (increment SP by M). First four are reserved to Static Link (SL), Dynamic Link (DL), Return Address (RA), and Parameter (P)
            case 6:
                SP += M;
                printf("INC");
                break;
            
            // Jump to instruction M (PC = M)
            case 7:
                PC = M;
                printf("JMP");
                break;

            // Jump to instruction M if top stack element is 1
            case 8:
                if(pas[SP] == 1) PC = M;
                SP--;
                printf("JPC");
                break;

            // Switch by M scenario
            case 9:

                // Write the top stack element to the screen
                if(M == 1)
                {
                    printf("Top of Stack Value: %d\n", pas[SP]);
                    SP--;
                }

                // Read in input from the user and store it on top of the stack
                if(M == 2)
                {
                    SP++;
                    printf("Please Enter an Integer: ");
                    scanf("%d", &pas[SP]);

                }

                // End of program (Set Halt flag to zero)
                if(M == 3) halt = 0;

                printf("%2.d SYS", temp_PC);
                break;
        }

        // Print values after every iteration
        printf(" %2d %2d\t %2d %2d %2d   ", L, M, PC, BP, SP);
        
        // Print out stack in instructed format
        for(int i = length; i <= SP; i++)
        {
            // Print out activation records
            int temp_BP = BP;
            while(temp_BP != length)
            {
                if(i == temp_BP && temp_BP != length) printf(" |");
                temp_BP = pas[temp_BP + 1];
            }
            
            // Print out stack elements
            printf(" %2d", pas[i]);
        }

        printf("\n");
    }

    return 0;
} 