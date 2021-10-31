/**
 * @file codegen.c
 * @author   Kenny Subrahmanyam
 * @date 2021-07-27
 * @copyright Copyright (c) 2021
 */

// Libraries
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

// Variables
instruction *code;
int code_index;
lexeme *list;
int lex_level;
int token_index = 0;
symbol *table;
int sym_index;
int var_num = 0;
int main_PC = 0;

// Function prototypes
void command(int OP, int L, int M);
int find_sym(char name[12], int kind, int kind2);

void printcode();
void mark();

void accept();
int nextToken();

void program();
void block();
void conDeclar();
void varDeclar();
void proDeclar();
void statement();
void condition();
void expression();
void term();
void factor();

// Function that generates code and prints result
instruction *generate_code(lexeme *tokens, symbol *symbols)
{
	code = malloc(500 * sizeof(instruction));
	code_index = 0;

	list = tokens;
	table = symbols;
	sym_index = 0;

	// Function that parses through code and generates code
	program();

	// Function that prints generated code
	printcode();
	return code;
}

// Non-Terminal grammar function
void program()
{
	sym_index++;
	command(7, 0, 0); // Jmp command

	block();
	accept();
	code[0].m = main_PC;

	command(9, 0, 3); // Halt command
}

// Non-Terminal grammar function
void block()
{
	if(nextToken() == 29) conDeclar();
	if(nextToken() == 30) varDeclar();

	int temp = var_num;

	if(nextToken() == 31) proDeclar();
	
	main_PC = code_index * 3;
	command(6, 0, 3 + temp); // Inc command

	statement();	
}

// Non-Terminal grammar function
void conDeclar()
{
	accept();
	accept();
	accept();
	accept();

	sym_index++;

	while(1)
	{
		if(nextToken() == 17)
		{
			accept();
			break;
		}
		else
		{
			accept();
			accept();
			accept();
			accept();	
			sym_index++;
		}
	}
}

// Non-Terminal grammar function
void varDeclar()
{
	accept();
	accept();
	var_num++;
	sym_index++;

	while(1)
	{
		if(nextToken() == 17)
		{
			accept();
			break;
		}
		else
		{
			accept();
			accept();
			var_num++;	
			sym_index++;
		}
	}
}

// Non-Terminal grammar function
void proDeclar()
{
	while(nextToken() == 31)
	{
		accept();
		
		lex_level++;
		
		accept();
		accept();

		table[sym_index].val = (code_index * 3);
		sym_index++;
		
		var_num = 0;
		block();
		accept();
		lex_level--;
		
		command(2, 0, 0); // Rtn command
	}
}

// Non-Terminal grammar function
void statement()
{
	char name[12];
	int i, Jpc_index, Jmp_index;
	

	switch(nextToken())
	{
		case 32: // Ident

			strcpy(name, list[token_index].name);
			accept();
			accept();
			expression();

			i = find_sym(name, 2, 2);

			command(4, lex_level - table[i].level, table[i].addr); // Sto command
			break;

		case 26: // call

			accept();

			i = find_sym(list[token_index].name, 3, 3);

			int PC = table[i].val;
			command(5, lex_level - table[i].level, PC); // Cal command
			accept();
			break;

		case 19: // begin
			accept();

			statement();

			while(1)
			{
				if(nextToken() == 20)
				{
					accept();
					break;
				}
				else
				{
					accept();
					statement();
				}	
			}
		
			break;

		case 21: // if
			accept();
			condition();
			Jpc_index = code_index;
			command(8, 0, 0); // Jpc command
			accept();
			statement();

			if(nextToken() == 23)
			{
				accept();
				Jmp_index = code_index;
				command(7, 0, 0); // Jmp command
				code[Jpc_index].m = code_index * 3;
				statement();
				code[Jmp_index].m = code_index * 3;
			}
			else
				code[Jpc_index].m = code_index * 3;

				
			break;

		case 24: // while
			accept();
			Jmp_index = 3 * code_index;
			condition();
			accept();

			Jpc_index = code_index;
			command(8, 0, 0); // Jpc command
			statement();
			command(7, 0, Jmp_index); // Jmp command
			code[Jpc_index].m = code_index * 3;
			break;

		case 28: // read
			accept();

			command(9, 0, 2); // Read command

			strcpy(name, list[token_index].name);
			i = find_sym(name, 2, 2);
			command(4, lex_level - table[i].level, table[i].addr); // Sto command

			accept();
			break;

		case 27: // write
			accept();
			expression();
			command(9, 0, 1); // Write command
			break;
	}
}

// Non-Terminal grammar function
void condition()
{
	if(nextToken() == 1)
	{
		accept();
		expression();
		command(2, 0, 6); // Odd command
	}
	else
	{
		expression();
		int t = nextToken();
		accept();
		expression();

		switch(t)
		{
			case 2: command(2, 0, 8); break; // Eql command
			case 3: command(2, 0, 9); break; // Neq command
			case 4: command(2, 0, 10); break; // Lss command
			case 5: command(2, 0, 11); break; // Leq command
			case 6: command(2, 0, 12); break; // Gtr command
			case 7: command(2, 0, 13); break; // Geq command
		}
	}
}

// Non-Terminal grammar function
void expression()
{
	int neg = 0;

	
	if(nextToken() == 11) accept();
	if(nextToken() == 12) 
	{
		accept();
		neg = 1;
	}

	term();

	if(neg == 1)
		command(2, 0, 1); // Neg command

	while(nextToken() == 11 || nextToken() == 12)
	{
		if(nextToken() == 12) neg = 1;
		else neg = 0;

		accept();
		term();

		if(neg == 1) command(2, 0, 3); // Sub command
		else command(2, 0, 2); // Add command
	}
}

// Non-Terminal grammar function
void term()
{
	factor();

	while(nextToken() == 8 || nextToken() == 9 || nextToken() == 10)
	{
		int t = nextToken();
		accept();
		factor();

		switch(t)
		{
			case 8: command(2, 0, 7); break; // Mod command
			case 9: command(2, 0, 4); break; // Mul command
			case 10: command(2, 0, 5); break; // Div command
		}
	}
}

// Non-Terminal grammar function
void factor()
{
	if(nextToken() == 33) 
	{
		command(1, 0, list[token_index].value); // Lit command
		accept();
	}
	else if(nextToken() == 32)
	{
		int i = find_sym(list[token_index].name, 1, 2);

		if(table[i].kind == 1) command(1, 0, table[i].val); // Lit command
		else if(table[i].kind == 2) command(3, lex_level - table[i].level, table[i].addr); // Lod command

		accept();
	}
	else
	{
		accept();
		expression();
		accept();
	}
}

// Function that accepts current token
void accept()
{
	mark();
	token_index++;
}

// Function that returns current token
int nextToken()
{
	return list[token_index].type;
}

// Function that adds a command to the code
void command(int OP, int L, int M)
{
	code[code_index].opcode = OP; 
	code[code_index].m = M;
	code[code_index].l = L;
	code_index++;
}

// Function that finds correct symbol based on name
int find_sym(char name[12], int kind, int kind2)
{
	for(int i = sym_index; i > 0; i--)
		if(strcmp(table[i].name, name) == 0 && table[i].mark == 0 && (table[i].kind == kind || table[i].kind == kind2)) return i;
	return -1;
}

// Function that marks symbols in table based on current lexigraphical level
void mark()
{
	int temp_level = lex_level;
	for(int i = sym_index - 1; i >= 0; i--)
	{
		if(temp_level > table[i].level) temp_level = table[i].level;
		if(temp_level < table[i].level) table[i].mark = 1;
		else table[i].mark = 0;
	}
}

// Function that prints all generated code
void printcode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < code_index; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("ODD\t");
						break;
					case 7:
						printf("MOD\t");
						break;
					case 8:
						printf("EQL\t");
						break;
					case 9:
						printf("NEQ\t");
						break;
					case 10:
						printf("LSS\t");
						break;
					case 11:
						printf("LEQ\t");
						break;
					case 12:
						printf("GTR\t");
						break;
					case 13:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
}