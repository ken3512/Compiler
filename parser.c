/**
 * @file parser.c
 * @author   Kenny Subrahmanyam
 * @date 2021-07-15
 * @copyright Copyright (c) 2021
 */

// Libraries
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

// Global variables
symbol *table;
int sym_index;
int error;
int lex_level;
lexeme *list;
int token_index = 0;
int * address;

// Function prototypes
void printtable();
void errorend(int x);

void accept();
int nextToken();
void symbol_insert(int kind, char * name, int value);
int declarError();
void mark();
int exists(char * name, int kind, int kind2);

// Function prototypes for parsing functions
void program();
void block();
void conDeclar();
void varDeclar();
void proDeclar();
void statement();
void condition();
void relOp();
void expression();
void term();
void factor();

// Function that returns finished table or prints error
symbol *parse(lexeme *input)
{
	table = malloc(1000 * sizeof(symbol));
	address = calloc(100, sizeof(int));
	sym_index = 0;
	error = 0;

	for(int i = 0; i < 100; i++) address[i] = 3;

	list = input;
	program();


	if(error)
	{
		free(table);
		return NULL;
	}
	else
	{
		printtable();
		return table;
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

// Non-Terminal grammar function
void program()
{
	if(error == 0)
	{
		// Insert main procedure
		symbol_insert(3, "main", 0);
		block();

		// Registers "." terminal at the end of program
		if(nextToken() == 16) accept(); // check if current token is correct
		else 
		{
			if(error == 0)
				errorend(3);
			error = 1;
		}
	}
	else return;

}

// Non-Terminal grammar function
void block()
{
	if(error == 0)
	{
		// Execute different components of block based on next token 
		if(nextToken() == 29) conDeclar();
		if(nextToken() == 30) varDeclar();
		if(nextToken() == 31) proDeclar();
		statement();	
	}
	else return;

}

// Non-Terminal grammar function
void conDeclar()
{
	char name[12];

	// If error is 0 execute code for constant declaration grammar
	if(error == 0)
	{
		// Accept current token
		accept();


		if(nextToken() == 32) // check if current token is correct
		{
			strcpy(name, list[token_index].name);

			// Accept current token
			accept();
		}
		else
		{
			if(error == 0)
				errorend(4);
			error = 1;
			return;
		
		}
		if(nextToken() == 18) accept(); // check if current token is correct
		else
		{
			if(error == 0)
				errorend(5);
			error = 1;
			return;
		
		}
		if(nextToken() == 33) // check if current token is correct
		{
			// Add symbol to symbol table
			symbol_insert(1, name, list[token_index].value);

			// Accept current token
			accept();
		}
		else
		{
			if(error == 0)
				errorend(5);
			error = 1;
			return;
		
		}

		while(1)
		{
			if(nextToken() == 17) // check if current token is correct
			{
				// Accept current token
				accept();
				break;
			}
			else if(nextToken() == 15) // check if current token is correct
			{
				// Accept current token
				accept();
				
				if(nextToken() == 32)  // check if current token is correct
				{
					strcpy(name, list[token_index].name);
					accept();
				}
				else
				{
					if(error == 0)
						errorend(4);
					error = 1;
					return;
				
				}
				if(nextToken() == 18) accept(); // check if current token is correct
				else
				{
					if(error == 0)
						errorend(5);
					error = 1;
					return;
				
				}
				if(nextToken() == 33) // check if current token is correct
				{
					symbol_insert(1, name, list[token_index].value);

					// Accept current token
					accept();
				}
				else
				{
					if(error == 0)
						errorend(5);
					error = 1;
					return;
				
				}	
			}
			else
			{
				if(error == 0)
					errorend(6);
				error = 1;
				return;
			
			}	
		}
	}
}

// Non-Terminal grammar function
void varDeclar()
{
	if(error == 0)
	{
		// Accept current token
		accept();

		if(nextToken() == 32) 
		{
			symbol_insert(2, list[token_index].name, 0);

			// Accept current token
			accept();
		}
		else
		{
			if(error == 0)
				errorend(4);
			error = 1;
			return;
		
		}
		

		while(1)
		{
			if(nextToken() == 17) // check if current token is correct
			{
				accept();
				break;
			}
			else if(nextToken() == 15) // check if current token is correct
			{
				accept();
				if(nextToken() == 32) 
				{
					symbol_insert(2, list[token_index].name, 0);
					accept(); 
				}
				else
				{
					if(error == 0)
						errorend(4);
					error = 1;
					return;
					
				
				}
				
			}
			else
			{
				if(error == 0)
					errorend(6);
				error = 1;
				return;
			}	
		}
	}
	else return;
}

// Non-Terminal grammar function
void proDeclar()
{
	if(error == 0)
	{
		while(nextToken() == 31) // check if current token is correct
		{
			// Accept current token
			accept();

			if(nextToken() == 32) 
			{
				symbol_insert(3, list[token_index].name, 0);
				lex_level++;

				// Execute function that marks variables based on current lex level
				mark();

				// Accept current token
				accept();
			}
			else
			{
				if(error == 0)
					errorend(4);
				error = 1;
				return;
			
			}
			if(nextToken() == 17) accept(); // check if current token is correct
			else
			{
				if(error == 0)
					errorend(6);
				error = 1;
				return;
			
			}

			block();

			if(nextToken() == 17) // check if current token is correct
			{
				// Accept current token
				accept();
				lex_level--;
				mark();
			}
			else
			{
				if(error == 0)
					errorend(2);
				error = 1;
				return;
			
			}
		}
	}
	else return;

}

// Non-Terminal grammar function
void statement()
{
	if(error == 0)
	{
		switch(nextToken()) // check if current token is correct
		{
			case 32: // Ident

				if(exists(list[token_index].name, 2, 2) == 0)
				{
					if(error == 0)
						errorend(7);
					error = 1;
					return;
				}

				accept();

				if(nextToken() == 18) accept(); // check if current token is correct
				else
				{
					if(error == 0)
						errorend(2);
					error = 1;
					return;
				
				}

				expression();

				break;

			case 26: // call

				// Accept current token
				accept();

				if(nextToken() == 32) // check if current token is correct
				{
					if(exists(list[token_index].name, 3, 3) == 0)
					{
						if(error == 0)
							errorend(7);
						error = 1;
						return;
					}

					accept();
				}
				else
				{
					if(error == 0)
						errorend(14);
					error = 1;
					return;
				
				}
			
				break;

			case 19: // begin

				// Accept current token
				accept();

				statement();

				while(1) // Might need to make error catch more specific
				{
					if(nextToken() == 20) // check if current token is correct
					{
						accept();
						break;
					}
					else if(nextToken() == 17) // check if current token is correct
					{
						accept();
						statement();
					}
					else
					{
						if(error == 0)
							errorend(2);
						error = 1;
						return;
					
					}	
				}
			
				break;

			case 21: // if

				// Accept current token
				accept();

				condition();

				if(nextToken() == 22) accept(); // check if current token is correct
				else
				{
					if(error == 0)
						errorend(9);
					error = 1;
					return;
				}

				statement();

				if(nextToken() == 23) // check if current token is correct
				{
					accept();
					statement();
				}
			
				break;

			case 24: // while

				// Accept current token
				accept();

				condition();

				if(nextToken() == 25) accept(); // check if current token is correct
				else
				{
					if(error == 0)
						errorend(8);
					error = 1;
					return;
				
				}

				statement();
			
				break;

			case 28: // read

				// Accept current token
				accept();

				if(nextToken() == 32) // check if current token is correct
				{
					if(exists(list[token_index].name, 2, 2) == 0)
					{
						if(error == 0)
							errorend(7);
						error = 1;
						return;
					}

					accept();
				}
				else
				{
					if(error == 0)
						errorend(14);
					error = 1;
					return;
				
				}
			
				break;

			case 27: // write

				accept();
				expression();
			
				break;

			case 20: break;
			case 17: break;
			case 16: break;
			case 23: break;

			default:
				if(error == 0)
					errorend(2);
				error = 1;
				return;
				break;
		}
	}
	else return;
}
	
// Non-Terminal grammar function
void condition()
{
	if(error == 0)
	{
		if(nextToken() == 1) // check if current token is correct
		{
			// Accept current token
			accept();
			expression();
		}
		else if(nextToken() == 32 || nextToken() == 33 || nextToken() == 11 || nextToken() == 12) // check if current token is correct
		{
			expression();
			relOp();
			expression();
		}
		else
		{
			if(error == 0)
				errorend(11);
			error = 1;
			return;
		
		}
	}
	else return;

}

// Non-Terminal grammar function
void relOp()
{
	if(error == 0)
	{
		// check if current token is correct
		if(nextToken() == 2 || nextToken() == 3 || nextToken() == 4 || nextToken() == 5 || nextToken() == 6 || nextToken() == 7) accept(); // Accept current token
		else
		{
			if(error == 0)
				errorend(12);
			error = 1;
			return;
		
		}
	}
	else return;

}

// Non-Terminal grammar function
void expression()
{
	if(error == 0)
	{
		
		if(nextToken() == 11 || nextToken() == 12) accept(); // check if current token is correct

		term();

		while(nextToken() == 11 || nextToken() == 12) // check if current token is correct
		{
			// Accept current token
			accept();
			term();
		}
	}
	else return;

}

// Non-Terminal grammar function
void term()
{
	if(error == 0)
	{
		factor();

		while(nextToken() == 8 || nextToken() == 9 || nextToken() == 10) // check if current token is correct
		{
			// Accept current token
			accept();
			factor();
		}
	}
	else return;

}

// Non-Terminal grammar function
void factor()
{
	if(error == 0)
	{
		if(nextToken() == 33) accept(); // check if current token is correct
		else if(nextToken() == 32)
		{
			if(exists(list[token_index].name, 1, 2) == 0)
			{
				if(error == 0)
					errorend(7);
				error = 1;
				return;
			}

			// Accept current token
			accept();
		}
		else if(nextToken() == 13) // check if current token is correct
		{
			// Accept current token
			accept();
			
			expression();
			if(nextToken() == 14) accept();
			else
			{
				if(error == 0)
					errorend(13);
				error = 1;
				return;
			}
		}
		else
		{
			if(error == 0) // Maybe
				errorend(2);
			error = 1;
			return;
		}
	}
	else return;

}

// Function that inserts symbols into symbol table
void symbol_insert(int kind, char * name, int value)
{	
	if(error == 0)
	{
		table[sym_index].kind = kind;
		strcpy(table[sym_index].name, name);
		table[sym_index].level = lex_level;

		if(kind == 1)
			table[sym_index].val = value;
		if(kind == 2)
		{
			table[sym_index].addr = address[lex_level];
			address[lex_level]++;
		}

		sym_index++;
	}
}

// Function that checks for competing symbol error
int declarError(char * name, int kind, int kind2)
{
	for(int i = sym_index - 1; i >= 0; i--)
	{
		if(strcmp(table[i].name, name) == 0 && table[i].level == lex_level && table[i].mark == 0)
		{
			if(error == 0)
				errorend(1);
			error = 1;
			return 1;
		}
	}

	return 0;
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

// Checks if indentifier is declared and unmarked
int exists(char * name, int kind, int kind2)
{
	for(int i = sym_index - 1; i >= 0; i--)
		if(table[i].mark == 0 && strcmp(name, table[i].name) == 0 && (table[i].kind == kind || table[i].kind == kind2)) return 1;	
	return 0;
}

// Function that prints error given a number for the error type
void errorend(int x)
{
	switch (x)
	{
		case 1:
			printf("Parser Error: Competing Symbol Declarations\n");
			break;
		case 2:
			printf("Parser Error: Unrecognized Statement Form\n");
			break;
		case 3:
			printf("Parser Error: Programs Must Close with a Period\n");
			break;
		case 4:
			printf("Parser Error: Symbols Must Be Declared with an Identifier\n");
			break;
		case 5:
			printf("Parser Error: Constants Must Be Assigned a Value at Declaration\n");
			break;
		case 6:
			printf("Parser Error: Symbol Declarations Must Be Followed By a Semicolon\n");
			break;
		case 7:
			printf("Parser Error: Undeclared Symbol\n");
			break;
		case 8:
			printf("Parser Error: while Must Be Followed By do\n");
			break;
		case 9:
			printf("Parser Error: if Must Be Followed By then\n");
			break;
		case 10:
			printf("Parser Error: begin Must Be Followed By end\n");
			break;
		case 11:
			printf("Parser Error: while and if Statements Must Contain Conditions\n");
			break;
		case 12:
			printf("Parser Error: Conditions Must Contain a Relational-Operator\n");
			break;
		case 13:
			printf("Parser Error: ( Must Be Followed By )\n");
			break;
		case 14:
			printf("Parser Error: call and read Must Be Followed By an Identifier\n");
			break;
		default:
			printf("Implementation Error: Unrecognized Error Code\n");
			break;
	}
	
}

// Function that prints symbol table
void printtable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address\n");
	printf("------------------------------------------------------\n");
	for (i = 0; i < sym_index; i++)
		printf("%4d | %11s | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr); 
}