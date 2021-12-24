/*
	Functions for tokenizing a string of characters
	Copyright (C) 2016 John Powell

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "swap/tokenizer.h"
#include "swap/import.h"

/**
 * one-character symbols
 */
const char *symbols[] =
{
	"+", "-", "/", "*", ";", ",", "(", ")", "[", "]", "{",
	"}", "<", ">", "=", ".", "&", "|", "^", "!", "%", ":" };

/**
 * two-character symbols
 */
const char *symbols2[] =
{
	"++", "--", "+=", "-=", "*=", "/=", "%=", "==", "<=",
	">=", ">>", "<<", "&=", "|=", "^=", "&&", "||", "!=",
	"::" };

/**
 * three-character symbols
 */
const char *symbols3[] =
{
	"<<=", ">>=" };

/**
 * keywords
 */
const char *keywords[] =
{
	"if", "else", "for", "while", "function", "return", "import", "null",
	"struct", "local", "const", "new", "try", "catch", "throw", "switch",
	"case", "break", "default" };

/**
 * escape characters
 */
const char escapes[] =
{
	'a', 'b', 'f', 'n', 'r', 't', 'v', '\\', '\'', '"', '?', '0' };

/**
 * checks for an escape character
 */
static int jep_is_escape(char c)
{
	int i;
	for (i = 0; i < 12; i++)
	{
		if (escapes[i] == c)
		{
			return i;
		}
	}
	return -1;
}

/**
 * checks for a symbol character
 */
static int jep_is_symbol_char(char c)
{
	switch (c)
	{
	case '+':
	case '-':
	case '*':
	case '/':
	case '<':
	case '>':
	case '!':
	case '=':
	case '.':
	case ',':
	case ';':
	case '(':
	case ')':
	case '{':
	case '}':
	case '[':
	case ']':
	case '&':
	case '|':
	case '^':
	case '%':
	case ':':
		return 1;
	default:
		return 0;
	}
}

/**
 * checks for a single-character symbol
 */
static int jep_is_symbol(const char *s)
{
	int i;
	for (i = 0; i < 22; i++)
	{
		if (!strcmp(symbols[i], s))
		{
			return i;
		}
	}
	return -1;
}

/**
 * checks for a double-character symbol
 */
static int jep_is_symbol2(const char *s)
{
	int i;
	for (i = 0; i < 19; i++)
	{
		if (!strcmp(symbols2[i], s))
		{
			return i;
		}
	}
	return -1;
}

/**
 * checks for a tripple-character symbol
 */
static int jep_is_symbol3(const char *s)
{
	int i;
	for (i = 0; i < 2; i++)
	{
		if (!strcmp(symbols3[i], s))
		{
			return i;
		}
	}
	return -1;
}

/**
 * checks for a keyword
 */
static int jep_is_keyword(const char *s)
{
	int i;
	for (i = 0; i < 19; i++)
	{
		if (!strcmp(s, keywords[i]))
		{
			return i;
		}
	}

	return -1;
}

/**
 * checks for the beginning of an identifier
 */
static int jep_is_ident_start(char c)
{
	if (isalpha(c) || c == '_' || c == '$')
	{
		return 1;
	}

	return 0;
}

/**
 * checks for identifier characters
 */
static int jep_is_ident(char c)
{
	if (isalnum(c) || c == '_' || c == '$')
	{
		return 1;
	}

	return 0;
}

/**
 * gives a token a token code. Only symbols and keywords get token codes
 */
static void jep_classify_token(jep_token *t)
{
	if (t->type == T_SYMBOL)
	{
		if (t->val->size == 1)
		{
			t->token_code = jep_is_symbol(t->val->buffer) + 1;
		}
		else if (t->val->size == 2)
		{
			t->token_code = jep_is_symbol2(t->val->buffer) + 23;
		}
		else if (t->val->size == 3)
		{
			t->token_code = jep_is_symbol3(t->val->buffer) + 42;
		}
	}
	else if (t->type == T_KEYWORD)
	{
		int code = jep_is_keyword(t->val->buffer);
		t->token_code = code + 44;

		if (code == 11)
		{
			/* detect new*/
			t->type = T_SYMBOL;
		}
		else if (code == 9 || code == 10)
		{
			/* detect modifiers */
			t->type = T_MODIFIER;
		}
	}
}

/**
 * determines the appropriate escaped character for an escape sequence
 */
static int jep_escape(char c, char *esc)
{
	int result = 1;
	switch (jep_is_escape(c))
	{
	case 0:
		*esc = '\a';
		break;

	case 1:
		*esc = '\b';
		break;

	case 2:
		*esc = '\f';
		break;

	case 3:
		*esc = '\n';
		break;

	case 4:
		*esc = '\r';
		break;

	case 5:
		*esc = '\t';
		break;

	case 6:
		*esc = '\v';
		break;

	case 7:
		*esc = '\\';
		break;

	case 8:
		*esc = '\'';
		break;

	case 9:
		*esc = '\"';
		break;

	case 10:
		*esc = '\?';
		break;

	case 11:
		*esc = '\0';
		break;

	default:
		printf("tokenizer error: invalid escape character %c\n", c);
		result = 0;
		break;
	}
	return result;
}

/**
 * allocates memory for a new token
 */
jep_token *jep_create_token()
{
	jep_token *t = malloc(sizeof(jep_token));
	t->val = jep_create_string_builder();
	t->type = 0;
	t->token_code = 0;
	t->row = 0;
	t->column = 0;
	t->unary = 0;
	t->postfix = 0;
	t->file = NULL;
	return t;
}

/**
 * frees the memory allocated for a token
 */
void jep_destroy_token(jep_token *t)
{
	jep_destroy_string_builder(t->val);
	free(t);
}

/**
 * creates a new token stream
 */
jep_token_stream *jep_create_token_stream()
{
	jep_token_stream *ts = malloc(sizeof(jep_token_stream));
	ts->size = 0;
	ts->cap = 50;
	ts->tok = malloc(50 * sizeof(jep_token));
	ts->error = 0;
	ts->dir_size = 0;
	ts->dir_cap = 50;
	ts->dir = malloc(50 * sizeof(jep_token));
	return ts;
}

/**
 * frees memory allocated for a token stream
 */
void jep_destroy_token_stream(jep_token_stream *ts)
{
	int i;
	for (i = 0; i < ts->size; i++)
	{
		jep_destroy_string_builder(ts->tok[i].val);
	}
	for (i = 0; i < ts->dir_size; i++)
	{
		jep_destroy_string_builder(ts->dir[i].val);
	}
	free(ts->tok);
	free(ts->dir);
	free(ts);
}

/**
 * adds a token to a token stream
 */
void jep_append_token(jep_token_stream *ts, jep_token t)
{
	if (ts->size >= ts->cap)
	{
		int new_cap = ts->cap + ts->cap / 2;
		jep_token *new_tok = NULL;
		new_tok = realloc(ts->tok, sizeof(jep_token) * new_cap);
		if (new_tok != NULL)
		{
			ts->tok = new_tok;
			ts->cap = new_cap;
		}
		else
		{
			if (!ts->error)
			{
				printf("tokenizer error: failed to reallocate memory for token stream\n");
				ts->error = 1;
			}
			return;
		}
	}

	/* check to see if we're tokenizing a file from an import statement */
	if (ts->size > 0 && ts->tok[ts->size - 1].token_code == T_EOF)
	{
		ts->tok[ts->size - 1] = t;
	}
	else
	{
		ts->tok[ts->size++] = t;
	}
}

/**
 * adds a directive token to a token stream
 */
void jep_append_directive(jep_token_stream *ts, jep_token d)
{
	if (ts->dir_size >= ts->dir_cap)
	{
		int new_cap = ts->dir_cap + ts->dir_cap / 2;
		jep_token *new_dir = NULL;
		new_dir = realloc(ts->dir, sizeof(jep_token) * new_cap);
		if (new_dir != NULL)
		{
			ts->dir = new_dir;
			ts->dir_cap = new_cap;
		}
		else
		{
			if (!ts->error)
			{
				printf("tokenizer error: failed to reallocate memory for token stream\n");
				ts->error = 1;
			}
			return;
		}
	}

	/* check to see if we're tokenizing a file from an import statement */
	if (ts->dir_size > 0 && ts->dir[ts->dir_size - 1].token_code == T_EOF)
	{
		ts->dir[ts->dir_size - 1] = d;
	}
	else
	{
		ts->dir[ts->dir_size++] = d;
	}
}

/**
 * checks if a token stream already contains a directive
 */
int jep_has_directive(jep_token_stream *ts, const char *dir)
{
	int i;
	for (i = 0; i < ts->dir_size; i++)
	{
		if (!strcmp(ts->dir[i].val->buffer, dir))
		{
			return 1;
		}
	}
	return 0;
}

/**
 * tokenizes the contents of a text file
 */
void jep_tokenize_file(jep_token_stream *ts, const char *file_name)
{
	if (ts->error)
	{
		return;
	}

	FILE *in_file;			/* the input file                        */
	jep_string_builder *sb; /* puts the character data into a string */
	char *s;				/* the string of character data          */
	int row;				/* the row of each token in the file     */
	int col;				/* the column of each token in the file  */
	int i;					/* loop index                            */

	in_file = fopen(file_name, "r");
	row = 1;
	col = 1;

	if (in_file == NULL)
	{
		/* failed to open the input file */
		if (!ts->error)
		{
			printf("tokenizer error: could not open file: %s\n", file_name);
			ts->error = 2;
		}
		return;
	}

	sb = jep_create_string_builder();
	jep_scan_file(in_file, sb);
	s = sb->buffer;
	i = 0;
	while (i < sb->size)
	{
		/* skip block comments */
		if (s[i] == '/' && s[i + 1] == '*')
		{
			i += 2;
			col += 2;
			while (!(s[i] == '*' && s[i + 1] == '/') && i < sb->size)
			{
				if (s[i] == '\n')
				{
					col = 1;
					row++;
				}
				else
				{
					col++;
				}

				i++;
			}
			i += 2;
			col += 2;
		}

		/* skip line comments */
		if ((s[i] == '/' && s[i + 1] == '/') || s[i] == '#')
		{
			if (s[i] == '#' && s[i + 1] == '{')
			{
				i += 2;
				col += 2;
				jep_string_builder *dir = jep_create_string_builder();
				while (s[i] != '\n' && s[i] != '}' && i < sb->size)
				{
					jep_append_char(dir, s[i]);
					i++;
					col++;
				}
				if (s[i] != '\n')
				{
					while (s[i] != '\n' && i < sb->size)
					{
						i++;
						col++;
					}
				}
				if (!jep_has_directive(ts, dir->buffer))
				{
					jep_token dir_tok =
					{
						dir, T_DIRECTIVE, 0, row, col, 0, 0, file_name };
					jep_append_directive(ts, dir_tok);
				}
				else
				{
					// stop tokenizing the file if the directive
					// already exists
					jep_destroy_string_builder(dir);
					return;
				}
			}
			else
			{
				while (s[i] != '\n' && i < sb->size)
				{
					i++;
					col++;
				}
			}
		}

		/* detect identifiers */
		if (jep_is_ident_start(s[i]))
		{
			jep_string_builder *val = jep_create_string_builder();
			jep_token ident =
			{
				val, T_IDENTIFIER, 0, row, col, 0, 0, file_name };
			do
			{
				jep_append_char(ident.val, s[i++]);
				col++;
			} while (jep_is_ident(s[i]));
			if (jep_is_keyword(ident.val->buffer) >= 0)
			{
				ident.type = T_KEYWORD;
			}
			jep_classify_token(&ident);
			jep_append_token(ts, ident);
		}

		/* detect characters */
		if (s[i] == '\'')
		{
			jep_string_builder *val = jep_create_string_builder();
			jep_token c =
			{
				val, T_CHARACTER, 0, row, col, 0, 0, file_name };
			i++;
			col++;
			do
			{
				/* check for escape sequences */
				if (s[i] == '\\')
				{
					char esc;
					if (jep_escape(s[i + 1], &esc))
					{
						i++;
						col++;
						jep_append_char(c.val, esc);
					}
					else
					{
						ts->error++;
					}
				}
				else
				{
					jep_append_char(c.val, s[i]);
				}
				i++;
				col++;
			} while (s[i] != '\'' && i < sb->size);
			jep_append_token(ts, c);
		}

		/* detect strings */
		if (s[i] == '"')
		{
			jep_string_builder *val = jep_create_string_builder();
			jep_token str =
			{
				val, T_STRING, 0, row, col, 0, 0, file_name };
			i++;
			col++;
			while (s[i] != '"' && i < sb->size)
			{
				/* check for escape sequences */
				if (s[i] == '\\')
				{
					char esc;
					if (jep_escape(s[i + 1], &esc))
					{
						i++;
						col++;
						jep_append_char(str.val, esc);
					}
					else
					{
						ts->error++;
					}
				}
				else
				{
					jep_append_char(str.val, s[i]);
				}
				i++;
				col++;
			}
			jep_append_token(ts, str);
		}

		/* detect symbols */
		if (jep_is_symbol_char(s[i]))
		{
			jep_string_builder *val = jep_create_string_builder();
			jep_token sym =
			{
				val, T_SYMBOL, 0, row, col, 0, 0, file_name };
			char symbol[] = { s[i], '\0', '\0', '\0' };
			if (i < sb->size - 1)
			{
				symbol[1] = s[i + 1];
				symbol[2] = s[i + 2];
			}

			if (jep_is_symbol3(symbol) >= 0)
			{
				/* handle 3 character symbols */
				jep_append_string(sym.val, symbol);
				i += 2;
				col += 2;
			}
			else if (symbol[2] = '\0', jep_is_symbol2(symbol) >= 0)
			{
				/* handle 2 character symbols */
				jep_append_string(sym.val, symbol);
				i++;
				col++;
			}
			else if (symbol[1] = '\0', jep_is_symbol(symbol) >= 0)
			{
				/* handle 1 character symbols */
				jep_append_string(sym.val, symbol);
			}

			jep_classify_token(&sym);
			jep_append_token(ts, sym);

			if (sym.token_code == T_SEMICOLON && ts->size > 2 && ts->tok[ts->size - 3].token_code == T_IMPORT)
			{
				/* handle imports */
				char *local_path = ts->tok[ts->size - 2].val->buffer;
				char *import_path = jep_get_import(local_path);

				FILE *import = NULL;
				if ((import = fopen(local_path, "r")) != NULL)
				{
					/* check for local imports */
					fclose(import);
					jep_tokenize_file(ts, ts->tok[ts->size - 2].val->buffer);
				}
				else
				{
					/* attempt system import */
					jep_tokenize_file(ts, import_path);
				}

				free(import_path);
			}
		}

		/* detect numbers */
		if (isdigit(s[i]))
		{
			jep_string_builder *val = jep_create_string_builder();
			jep_token num =
			{
				val, T_NUMBER, 0, row, col, 0, 0, file_name };
			int dec = 0;
			do
			{
				/* keep track of occurrences of '.' */
				if (s[i] == '.')
					dec++;

				if (dec < 2)
				{
					jep_append_char(num.val, s[i]);
					i++;
				}
				col++;
			} while ((isdigit(s[i]) || s[i] == '.') && dec < 2);
			if (s[i] == 'b')
			{
				jep_append_char(num.val, s[i]);
				i++;
				col++;
			}
			jep_append_token(ts, num);
			i--;
			col--;
		}

		if (s[i] == '\n')
		{
			/* increment the row when we find a '\n' */
			row++;
			col = 1;
		}
		else
		{
			col++;
		}
		i++;
	}

	/* append an END token */
	jep_token end_token =
	{
		jep_create_string_builder(), T_END, T_EOF, row, col, 0, 0, file_name };
	jep_append_string(end_token.val, "EOF");
	jep_append_token(ts, end_token);

	/* free memory */
	fclose(in_file);
	jep_destroy_string_builder(sb);
}

/**
 * prints the tokens in a token stream
 */
void jep_print_tokens(jep_token_stream *ts, FILE *f)
{
	int i;
	fprintf(f, "-----------------------------------------------\n");
	fprintf(f, "%-12s %-7s %-7s %-12s value\n", "type", "row", "column", "token code");
	fprintf(f, "-----------------------------------------------\n");
	for (i = 0; i < ts->size; i++)
	{
		switch (ts->tok[i].type)
		{
		case T_SYMBOL:
			fprintf(f, "%-12s %-7d %-7d %-12d %s\n", "[symbol]",
				ts->tok[i].row, ts->tok[i].column,
				ts->tok[i].token_code, ts->tok[i].val->buffer);
			break;
		case T_IDENTIFIER:
			fprintf(f, "%-12s %-7d %-7d %-12d %s\n", "[identifier]",
				ts->tok[i].row, ts->tok[i].column,
				ts->tok[i].token_code, ts->tok[i].val->buffer);
			break;
		case T_CHARACTER:
			fprintf(f, "%-12s %-7d %-7d %-12d %s\n", "[character]",
				ts->tok[i].row, ts->tok[i].column,
				ts->tok[i].token_code, ts->tok[i].val->buffer);
			break;
		case T_STRING:
			fprintf(f, "%-12s %-7d %-7d %-12d %s\n", "[string]",
				ts->tok[i].row, ts->tok[i].column,
				ts->tok[i].token_code, ts->tok[i].val->buffer);
			break;
		case T_NUMBER:
			fprintf(f, "%-12s %-7d %-7d %-12d %s\n", "[number]",
				ts->tok[i].row, ts->tok[i].column,
				ts->tok[i].token_code, ts->tok[i].val->buffer);
			break;
		case T_KEYWORD:
			fprintf(f, "%-12s %-7d %-7d %-12d %s\n", "[keyword]",
				ts->tok[i].row, ts->tok[i].column,
				ts->tok[i].token_code, ts->tok[i].val->buffer);
			break;
		case T_MODIFIER:
			fprintf(f, "%-12s %-7d %-7d %-12d %s\n", "[modifier]",
				ts->tok[i].row, ts->tok[i].column,
				ts->tok[i].token_code, ts->tok[i].val->buffer);
			break;
		case T_END:
			fprintf(f, "%-12s %-7d %-7d %-12d %s\n", "[end]",
				ts->tok[i].row, ts->tok[i].column,
				ts->tok[i].token_code, ts->tok[i].val->buffer);
			break;
		default:
			break;
		}
	}
	fprintf(f, "\n---------------\ndirectives\n---------------\n");
	for (i = 0; i < ts->dir_size; i++)
	{
		fprintf(f, "%s\n", ts->dir[i].val->buffer);
	}
}
