#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Header.h"
bool noLogs = false;
// функція отримує лексеми з вхідного файлу F і записує їх у таблицю лексем TokenTable 
// результат функції - кількість лексем
unsigned int GetTokens(FILE* F, Token TokenTable[], FILE* logFile)
{
	States state = States::Start;
	Token TempToken;
	// кількість лексем
	unsigned int NumberOfTokens = 0;
	char ch;
	char buf[16];
	int line = 1;
	// читання першого символу з файлу 
	ch = getc(F);
	// пошук лексем
	while (1)
	{
		switch (state)
		{
			// стан Start - початок виділення чергової лексеми 
			// якщо поточний символ маленька літера, то переходимо до стану Letter
			// якщо поточний символ цифра, то переходимо до стану Digit
			// якщо поточний символ пробіл, символ табуляції або переходу на новий рядок, то переходимо до стану Separators
			// якщо поточний символ / то є ймовірність, що це коментар, переходимо до стану SComment
			// якщо поточний символ EOF (ознака кінця файлу), то переходимо до стану EndOfFile
			// якщо поточний символ відмінний від попередніх, то переходимо до стану Another
		case States::Start:
		{
			if (ch == EOF)
				state = States::EndOfFile;
			else if ((ch <= 'z' && ch >= 'a') || (ch <= 'Z' && ch >= 'A'))
				state = States::Letter;
			else if (ch <= '9' && ch >= '0')
				state = States::Digit;
			else if (ch == ' ' || ch == '\t' || ch == '\n')
				state = States::Separators;
			else if (ch == '@')
			{
				ch = getc(F);
				if (ch == '@')
				{
					state = States::SComment;
				}
			}
			else if (ch == '#')
				state = States::ProgName;
			else
				state = States::Another;
			break;
		}
		// стан Finish - кінець виділення чергової лексеми і запис лексеми у таблицю лексем
		case States::Finish:
		{

			if (NumberOfTokens < MAX_TOKENS)
			{
				TokenTable[NumberOfTokens++] = TempToken;
				if (ch != EOF)
					state = States::Start;
				else
					state = States::EndOfFile;
			}
			else
			{
				printf("\n\t\t\ttoo many tokens !!!\n");				
				fprintf(logFile, "\n\t\t\ttoo many tokens !!!\n"); noLogs = true;
				return NumberOfTokens - 1;
			}
			break;
		}
		// стан EndOfFile - кінець файлу, можна завершувати пошук лексем
		case States::EndOfFile:
		{
			if (!noLogs)
			fprintf(logFile, "Program has no lexical mistakes.\n");
			return NumberOfTokens;
		}
		case States::ProgName:
		{
			if (ch == '#')  // Если начинается с #
			{
				buf[0] = ch;
				int j = 1;
				ch = getc(F);
				// Считываем до конца возможного ключевого слова
				while (((ch <= 'z' && ch >= 'a') || (ch <= 'Z' && ch >= 'A')) && j < 15)
				{
					buf[j++] = ch;
					ch = getc(F);
				}
				buf[j] = '\0'; // Завершаем строку
				// Сравниваем с "#Program"
				if (strcmp(buf, "#Program") == 0)
				{
					TempToken.type = ProgramName;  // Помечаем как старт программы
					strcpy_s(TempToken.name, buf); // Сохраняем имя
					TempToken.value = 0;
					TempToken.line = line;
					state = States::Finish;
				}
				else
				{
					// Если это не "#Program", помечаем как неизвестный токен
					TempToken.type = Unknown;
					strcpy_s(TempToken.name, buf);
					TempToken.value = 0;
					TempToken.line = line;
					state = States::Finish;
				}
			}
			break;
		}
		// стан Letter - поточний символ - маленька літера, поточна лексема - ключове слово або ідентифікатор
		case States::Letter:
		{
			buf[0] = ch;
			int j = 1;
			ch = getc(F);
			// Считываем возможное ключевое слово, идентификатор или комбинацию слов
			while (((ch <= 'z' && ch >= 'a') || (ch <= '9' && ch >= '0') || (ch <= 'Z' && ch >= 'A') || ch == '_') && j < 15)
			{
				buf[j++] = ch;
				ch = getc(F);
			}
			buf[j] = '\0';
			TypeOfTokens temp_type = Unknown;
			// Сравниваем с ключевыми словами и определяем тип токена
			if (!strcmp(buf, "Start"))
				temp_type = StartProgram;
			else if (!strcmp(buf, "Variable"))
				temp_type = Variable;
			else if (!strcmp(buf, "Stop"))
				temp_type = StopProgram;
			else if (!_stricmp(buf, "Integer32_t"))
				temp_type = Type;
			else if (!strcmp(buf, "Put"))
				temp_type = Input;
			else if (!strcmp(buf, "Get"))
				temp_type = Output;
			else if (!strcmp(buf, "If"))
				temp_type = If;
			else if (strcmp(buf, "Then") == 0)
			{
				// Пропускаем пробелы
				while (ch == ' ')
				{
					ch = getc(F);
				}
				// Проверяем, начинается ли следующее слово с "Goto"
				if (ch == 'G')
				{
					buf[j++] = ch;
					ch = getc(F);
					while (((ch <= 'z' && ch >= 'a')) && j < 15)
					{
						buf[j++] = ch;
						ch = getc(F);
					}
					buf[j] = '\0';
					// Если это "Goto", объединяем токен "Then Goto"
					if (strcmp(buf, "ThenGoto") == 0)
					{
						strcpy_s(buf, "Then Goto");
					}
				}
				temp_type = Then;
			}
			else if (!strcmp(buf, "Goto"))
				temp_type = Goto;
			else if (!strcmp(buf, "Mod"))
				temp_type = Mod;
			else if (!strcmp(buf, "Div"))
				temp_type = Div;
			else if (!strcmp(buf, "Mul"))
				temp_type = Mul;
			else if (!strcmp(buf, "And"))
				temp_type = And;
			else if (!strcmp(buf, "Or"))
				temp_type = Or;
			else if (!strcmp(buf, "For"))
				temp_type = For;
			else if (!strcmp(buf, "To"))
				temp_type = To;
			else if (!strcmp(buf, "Next"))
				temp_type = Next;
			else if (!strcmp(buf, "Step"))
				temp_type = Step;
			else if ((buf[0] >= 'A' && buf[0] <= 'Z') && strlen(buf) <= 8)
				temp_type = Identifier;
			else
			{
				temp_type = Unknown;
				fprintf(logFile, "Lexical Log: Mistake in %d line, current token %s is Unknown\n", line, TempToken.name);
				noLogs = true;
			}

			strcpy_s(TempToken.name, buf);
			TempToken.type = temp_type;
			TempToken.value = 0;
			TempToken.line = line;
			state = States::Finish;
			break;
		}
		// стан Digit - поточний символ - цифра, поточна лексема - число
		case States::Digit:
		{
			buf[0] = ch;
			int j = 1;

			ch = getc(F);

			while ((ch <= '9' && ch >= '0') && j < 15)
			{
				buf[j++] = ch;
				ch = getc(F);
			}
			buf[j] = '\0';

			strcpy_s(TempToken.name, buf);
			TempToken.type = Number;
			TempToken.value = atoi(buf);
			TempToken.line = line;
			state = States::Finish;
			break;
		}
		// стан Separators - поточний символ пробіл, символ табуляції або переходу на новий рядок - видаляємо їх
		case States::Separators:
		{
			if (ch == '\n')
				line++;

			ch = getc(F);

			state = States::Start;
			break;
		}
		// стан SComment - поточний символ /,  можливо це коментар
		case States::SComment:
		{
			ch = getc(F);
			if (ch == '@')
			{
				ch = getc(F); // Считываем следующий символ
				if (ch == '@')
					state = States::Comment;
				else
				{
					strcpy_s(TempToken.name, "@");
					TempToken.type = Unknown;
					TempToken.value = 0;
					TempToken.line = line;
					state = States::Finish;
				}
			}
		}
		// стан Comment  - поточний символ /,  отже це коментар, видаляємо усі символи до кінця рядка
		case States::Comment:
		{
			while (ch != '\n' && ch != EOF)
			{
				ch = getc(F);
			}
			if (ch == EOF)
			{
				state = States::EndOfFile;
				break;
			}
			state = States::Start;
			break;
		}
		// стан Another - поточний символ - символ, відмінний від попередніх
		case States::Another:
		{
			switch (ch)
			{
			case '(':
			{
				strcpy_s(TempToken.name, "(");
				TempToken.type = LBraket;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = States::Finish;
				break;
			}
			case ')':
			{
				strcpy_s(TempToken.name, ")");
				TempToken.type = RBraket;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = States::Finish;
				break;
			}
			case ';':
			{
				strcpy_s(TempToken.name, ";");
				TempToken.type = Semicolon;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = States::Finish;
				break;
			}
			case ',':
			{
				strcpy_s(TempToken.name, ",");
				TempToken.type = Comma;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = States::Finish;
				break;
			}
			case '+':
			{
				strcpy_s(TempToken.name, "+");
				TempToken.type = Add;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = States::Finish;
				break;
			}
			case '-':
			{
				strcpy_s(TempToken.name, "-");
				TempToken.type = Sub;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = States::Finish;
				break;
			}
			case '!':
			{
				ch = getc(F);
				if (ch == ' ')
				{
					strcpy_s(TempToken.name, "!");
					TempToken.type = Not;
					TempToken.value = 0;
					TempToken.line = line;
					state = States::Finish;
				}
				else if (ch == '>')
				{
					strcpy_s(TempToken.name, "!>");
					TempToken.type = NotGreater;
					TempToken.value = 0;
					TempToken.line = line;
					ch = getc(F);
					state = States::Finish;
				}
				else if (ch == '<')
				{
					strcpy_s(TempToken.name, "!<");
					TempToken.type = NotLess;
					TempToken.value = 0;
					TempToken.line = line;
					ch = getc(F); // Считываем следующий символ после '<'
					state = States::Finish;
				}
				else
				{
					// Если нет пары с '>' или '<', токен '!' сам по себе
					strcpy_s(TempToken.name, "!");
					TempToken.type = Unknown;
					TempToken.value = 0;
					TempToken.line = line;
					fprintf(logFile, "Lexical Log: Mistake in %d line, current token %s is Unknown\n", line, TempToken.name);
					noLogs = true;
					state = States::Finish;
				}
				break;
			}
			case '<':
			{
				ch = getc(F); // Считываем следующий символ
				if (ch == '>') // Если следующий символ '>'
				{
					strcpy_s(TempToken.name, "<>");
					TempToken.type = NotEquality;
					TempToken.value = 0;
					TempToken.line = line;
					ch = getc(F); // Считываем символ после '>'
					state = States::Finish;
				}
				else
				{
					// Если нет пары с '>', токен '<' сам по себе
					strcpy_s(TempToken.name, "<");
					TempToken.type = Unknown;
					TempToken.value = 0;
					TempToken.line = line;
					fprintf(logFile, "Lexical Log: Mistake in %d line, current token %s is Unknown\n", line, TempToken.name);
					noLogs = true;
					state = States::Finish;
				}
				break;
			}
			case '>':
			{
				ch = getc(F); // Считываем следующий символ
				if (ch == '>') // Если следующий символ тоже '>'
				{
					strcpy_s(TempToken.name, ">>");
					TempToken.type = Assign;
					TempToken.value = 0;
					TempToken.line = line;
					ch = getc(F); // Считываем символ после '>>'
					state = States::Finish;
				}
				else
				{
					// Если нет пары с '>', токен '>' сам по себе
					strcpy_s(TempToken.name, ">");
					TempToken.type = Unknown;
					TempToken.value = 0;
					TempToken.line = line;
					fprintf(logFile, "Lexical Log: Mistake in %d line, current token %s is Unknown\n", line, TempToken.name);
					noLogs = true;
					state = States::Finish;
				}
				break;
			}
			case '=':
			{
				strcpy_s(TempToken.name, "=");
				TempToken.type = Equality;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = States::Finish;
				break;
			}
			case ':':
			{
				strcpy_s(TempToken.name, ":");
				TempToken.type = Unknown;
				TempToken.value = 0;
				TempToken.line = line;
				fprintf(logFile, "Lexical Log: Mistake in %d line, current token %s is Unknown\n", line, TempToken.name);
				noLogs = true;
				ch = getc(F);
				state = States::Finish;
				break;
			}
			default:
			{
				TempToken.name[0] = ch;
				TempToken.name[1] = '\0';
				TempToken.type = Unknown;
				TempToken.value = 0;
				TempToken.line = line;
				fprintf(logFile, "Lexical Log: Mistake in %d line, current token %s is Unknown\n", line, TempToken.name);
				noLogs = true;
				ch = getc(F);
				state = States::Finish;
				break;
			}
			}
		}
		}
	}
}
// функція друкує таблицю лексем на екран 
void PrintTokens(Token TokenTable[], unsigned int TokensNum)
{
	char type_tokens[16];
	printf("\n\n---------------------------------------------------------------------------\n");
	printf("| TOKEN TABLE                                                             |\n");
	printf("---------------------------------------------------------------------------\n");
	printf("| line number | token           | value      | token code | type of token |\n");
	printf("---------------------------------------------------------------------------");
	for (unsigned int i = 0; i < TokensNum; i++)
	{
		switch (TokenTable[i].type)
		{
		case ProgramName: strcpy_s(type_tokens, "ProgramName");
			break;
		case StartProgram: strcpy_s(type_tokens, "StartProgram");
			break;
		case Variable: strcpy_s(type_tokens, "Variable");
			break;
		case Type: strcpy_s(type_tokens, "Type");
			break;
		case Identifier: strcpy_s(type_tokens, "Identifier");
			break;
		case StopProgram: strcpy_s(type_tokens, "StopProgram");
			break;
		case Input: strcpy_s(type_tokens, "Input");
			break;
		case Output: strcpy_s(type_tokens, "Output");
			break;
		case If: strcpy_s(type_tokens, "If");
			break;
		case Then: strcpy_s(type_tokens, "Then");
			break;
		case Goto: strcpy_s(type_tokens, "Goto");
			break;
		case Assign: strcpy_s(type_tokens, "Assign");
			break;
		case Add: strcpy_s(type_tokens, "Add");
			break;
		case Sub: strcpy_s(type_tokens, "Sub");
			break;
		case Mul: strcpy_s(type_tokens, "Mul");
			break;
		case Div: strcpy_s(type_tokens, "Div");
			break;
		case Equality: strcpy_s(type_tokens, "Equality");
			break;
		case NotEquality: strcpy_s(type_tokens, "NotEquality");
			break;
		case NotGreater: strcpy_s(type_tokens, "NotGreater");
			break;
		case NotLess: strcpy_s(type_tokens, "NotLess");
			break;
		case Not: strcpy_s(type_tokens, "Not");
			break;
		case And: strcpy_s(type_tokens, "And");
			break;
		case Or:strcpy_s(type_tokens, "Or");
			break;
		case LBraket: strcpy_s(type_tokens, "LBraket");
			break;
		case RBraket: strcpy_s(type_tokens, "RBraket");
			break;
		case Number: strcpy_s(type_tokens, "Number");
			break;
		case Semicolon: strcpy_s(type_tokens, "Semicolon");
			break;
		case Comma: strcpy_s(type_tokens, "Comma");
			break;
		case For: strcpy_s(type_tokens, "For");
			break;
		case To: strcpy_s(type_tokens, "To");
			break;
		case Next: strcpy_s(type_tokens, "Next");
			break;
		case Step: strcpy_s(type_tokens, "Step");
			break;
		case Mod: strcpy_s(type_tokens, "Mod");
			break;
		case Unknown: strcpy_s(type_tokens, "Unknown");
			break;
		}
		printf("\n|%12d |%16s |%11d |%11d | %-13s |\n", TokenTable[i].line, TokenTable[i].name, TokenTable[i].value, TokenTable[i].type, type_tokens);
		printf("---------------------------------------------------------------------------");
	}
}
// функція друкує таблицю лексем у файл
void PrintTokensToFile(char* FileName, Token TokenTable[], unsigned int TokensNum)
{
	FILE* F;
	if ((fopen_s(&F, FileName, "wt")) != 0)
	{
		printf("Error: Can not create file: %s\n", FileName);
		return;
	}
	char type_tokens[16];
	fprintf(F, "---------------------------------------------------------------------------\n");
	fprintf(F, "| TOKEN TABLE                                                             |\n");
	fprintf(F, "---------------------------------------------------------------------------\n");
	fprintf(F, "| line number |      token      |    value   | token code | type of token |\n");
	fprintf(F, "---------------------------------------------------------------------------");
	for (unsigned int i = 0; i < TokensNum; i++)
	{
		switch (TokenTable[i].type)
		{
		case ProgramName: strcpy_s(type_tokens, "ProgramName");
			break;
		case StartProgram: strcpy_s(type_tokens, "StartProgram");
			break;
		case Variable: strcpy_s(type_tokens, "Variable");
			break;
		case Type: strcpy_s(type_tokens, "Type");
			break;
		case Identifier: strcpy_s(type_tokens, "Identifier");
			break;
		case StopProgram: strcpy_s(type_tokens, "StopProgram");
			break;
		case Input: strcpy_s(type_tokens, "Input");
			break;
		case Output: strcpy_s(type_tokens, "Output");
			break;
		case If: strcpy_s(type_tokens, "If");
			break;
		case Then: strcpy_s(type_tokens, "Then");
			break;
		case Goto: strcpy_s(type_tokens, "Goto");
			break;
		case Assign: strcpy_s(type_tokens, "Assign");
			break;
		case Add: strcpy_s(type_tokens, "Add");
			break;
		case Sub: strcpy_s(type_tokens, "Sub");
			break;
		case Mul: strcpy_s(type_tokens, "Mul");
			break;
		case Div: strcpy_s(type_tokens, "Div");
			break;
		case Equality: strcpy_s(type_tokens, "Equality");
			break;
		case NotEquality: strcpy_s(type_tokens, "NotEquality");
			break;
		case NotGreater: strcpy_s(type_tokens, "NotGreater");
			break;
		case NotLess: strcpy_s(type_tokens, "NotLess");
			break;
		case Not: strcpy_s(type_tokens, "Not");
			break;
		case And: strcpy_s(type_tokens, "And");
			break;
		case Or:strcpy_s(type_tokens, "Or");
			break;
		case LBraket: strcpy_s(type_tokens, "LBraket");
			break;
		case RBraket: strcpy_s(type_tokens, "RBraket");
			break;
		case Number: strcpy_s(type_tokens, "Number");
			break;
		case Semicolon: strcpy_s(type_tokens, "Semicolon");
			break;
		case Comma: strcpy_s(type_tokens, "Comma");
			break;
		case For: strcpy_s(type_tokens, "For");
			break;
		case To: strcpy_s(type_tokens, "To");
			break;
		case Next: strcpy_s(type_tokens, "Next");
			break;
		case Mod: strcpy_s(type_tokens, "Mod");
			break;
		case Step: strcpy_s(type_tokens, "Step");
			break;
		case Unknown: strcpy_s(type_tokens, "Unknown");
			break;
		}
		fprintf(F, "\n|%12d |%16s |%11d |%11d | %-13s |\n", TokenTable[i].line, TokenTable[i].name, TokenTable[i].value, TokenTable[i].type, type_tokens);
		fprintf(F, "---------------------------------------------------------------------------");
	}
	fclose(F);
}
