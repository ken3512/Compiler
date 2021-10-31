/**
 * @file lex.c
 * @author   Kenny Subrahmanyam
 * @author-2 Isaac Subrahmanyam
 * @date 2021-06-24
 * @copyright Copyright (c) 2021
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"

// Array of reserved symbols
char reservedSymbols[][12] = {"odd", "==", "<>", "<", "<=", ">", ">=", "%", "*", "/", "+", "-", "(", ")", ",", ".", ";", ":=", "begin", "end", "if", "then", "else", "while", "do", "call", "write", "read", "const", "var", "procedure"};

lexeme *list;
int lex_index;

void printerror(int type);
void printtokens();
int find(char * sym);
int type(char a);

lexeme *lexanalyzer(char *input)
{
	
	list = malloc(500 * sizeof(lexeme));
	lex_index = 0;

	// Variables 
	int length = strlen(input), i = 0, next = 0, curType = -1, preType = -1, 
	Sym, comment = 0, errorType, error = 0, loop = 1;

	char temp[13];
	
	// Initialize temp array
	for(i = 0; i < 12; i++) temp[i] = '\0';

	i = 0;

	// Loops until all characters in input array are processed
	while(loop == 1)
	{
		// Tests if current symble is a comment
		if(comment == 0 && input[i] == '/' && input[i + 1] == '*')
			comment = 1;

		// If comment is true execute appropriate code 
		if(comment == 1) 
		{	
			i++;
		
			// Check if comment has ended
			if(input[i - 1] == '*' && input[i] == '/')
			{
				comment = 0;
				if(preType != -1)
					preType = 4;
				i++;
			}
		}	

		// Variables that store information about current symol trying to be added
		int tempType = type(temp[0]);
		int tempLen = strlen(temp);

		// If null terminator is reached set loop to 0
		if(input[i] == '\0') loop = 0;

		// If the type is an identifier and its length is 12 then break and give error flag
		if(tempType == 1 && tempLen == 12) 
		{
			error = 1;
			errorType = 4;
			break;
		}

		// If the type is a number and its length is 6 then break and give error flag
		if(tempType == 2 && tempLen == 6) 
		{
			error = 1;
			errorType = 3;
			break;
		}
	

		// If comment is false then execute the following code
		if(comment == 0)
		{

			// Keeps track of current 
			if(preType != -1)
				curType = type(input[i]);

			// Code executed at the begining of the while loop
			if(preType == -1 && type(input[i]) != 4)
			{
				// Initialize curType and preType 
				curType = preType = type(input[i]);
				
				// Put first value in temp array
				temp[next] = input[i];
				next++;
				i++;

				// Skip current loop iteration
				continue;

			}
			else if(preType == -1 && type(input[i]) == 4)
			{
				i++;
				continue;
			}

			Sym = 0;

			// Test if current symbol is only one character long and put it into list if so
			for(int j = 7; j <= 16; j++)
				if(temp[0] == reservedSymbols[j][0]) Sym = 1;
	
			// Account for special cases with < and >
			if((temp[0] == '>' && input[i] != '=') || (temp[0] == '<' && (input[i] != '>' && input[i] != '='))) Sym = 1;

			// Once two symbols have been read in, put symbol into list 
			if(tempLen == 2 && type(temp[0]) == 3) Sym = 1;	


			// If curType is equal to preType or specific type switch has accured read next value into temp
			if((curType == preType || (curType == 2 && preType == 1) || ((curType == 1 && preType == 2) && type(temp[0]) == 1)) && Sym == 0 && curType != 4)
			{
				temp[next] = input[i];
				next++;
				i++;
			}
			else if(curType != 4 && !(curType == 1 && preType == 2))
			{
				// Check if temp is a valid symbol
				if(find(temp) == 0)
				{
					printf("%s", temp);
					error = 1;
					errorType = 1;
					break;
				}

				// Put symbol stored in temp into list
				list[lex_index].type = find(temp);
				if(list[lex_index].type == 32) strcpy(list[lex_index].name, temp);
				if(list[lex_index].type == 33) list[lex_index].value = atoi(temp);
				lex_index++;

				// Reset temp variable
				next = 0;
				for(int j = 0; j < 12; j++) temp[j] = '\0';
			} 
			else if(curType == 1 && preType == 2) // Else if that catches invalid identifier error and executes appropriate code
			{
				error = 1;
				errorType = 2;
				break;
			}
			else i++; // Else statement that increaments i if current charactor is a space

			// Registers last read in symbol
			if(loop == 0 && comment == 0)
			{
				// Check if temp is a valid symbol
				if(find(temp) == 0)
				{
					error = 1;
					errorType = 1;
					break;
				}

				// Put symbol stored in temp into list
				list[lex_index].type = find(temp);
				if(list[lex_index].type == 32) strcpy(list[lex_index].name, temp);
				if(list[lex_index].type == 33) list[lex_index].value = atoi(temp);
				lex_index++;
			} 

			// Update preType 
			preType = curType;
		}
		
	}
	
	// Check if we have a never ending comment error, if so execute appropriate code
	if(comment == 1)
	{
		errorType = 5;
		error = 1;
	}
	
	// if not error is found print out token list
	if(error == 0)
	{
		printtokens();
		return list;
	}

	// If error is found print error and return null	
	printerror(errorType);
	return NULL;
}

// Given a symbol returns its correct type if it is valid
int find(char * sym)
{
	// Check if current symbol is from reservedSymbols array
	for(int i = 0; i < 33; i++)
		if(strcmp(sym, reservedSymbols[i]) == 0) return i + 1;

	// Check if current symbol is a indentifier
	if(isalpha(sym[0]) != 0) return 32;

	// Check if current symbol is a number
	if(isdigit(sym[0]) != 0) return 33;

	// Return zero if invalid symbol error has accured
	return 0;
}

// Given a charactor determine if it is a letter, number, symbol or space, and return appropriate number
int type(char a)
{
	if(iscntrl(a) != 0 || a == ' ') return 4;
	if(isalpha(a) != 0)	return 1;
	if(isdigit(a) != 0)	return 2;
	return 3;
}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case oddsym:
				printf("%11s\t%d", "odd", oddsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "==", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "<>", neqsym);
				break;
			case lessym:
				printf("%11s\t%d", "<", lessym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case slashsym:
				printf("%11s\t%d", "/", slashsym);
				break;
			case plussym:
				printf("%11s\t%d", "+", plussym);
				break;
			case minussym:
				printf("%11s\t%d", "-", minussym);
				break;
			case lparentsym:
				printf("%11s\t%d", "(", lparentsym);
				break;
			case rparentsym:
				printf("%11s\t%d", ")", rparentsym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case becomessym:
				printf("%11s\t%d", ":=", becomessym);
				break;
			case beginsym:
				printf("%11s\t%d", "begin", beginsym);
				break;
			case endsym:
				printf("%11s\t%d", "end", endsym);
				break;
			case ifsym:
				printf("%11s\t%d", "if", ifsym);
				break;
			case thensym:
				printf("%11s\t%d", "then", thensym);
				break;
			case elsesym:
				printf("%11s\t%d", "else", elsesym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
	list[lex_index++].type = -1;
}

void printerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else if (type == 5)
		printf("Lexical Analyzer Error: Neverending Comment\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");

	free(list);

	return;
}