/*
	Functions for object creation and manipulation
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
#include "swap/object.h"
#include "swap/tokenizer.h"

static int creation = 0;
static int destruction = 0;

/* TODO: implement memory error */
jep_obj memory_error = {
	NULL,
	"memory error",
	JEP_STRING,
	NULL, NULL, NULL, NULL,
	0,
	2,
	-1,
	0,
	NULL,
	NULL
};

static void jep_print_array(jep_obj *array)
{
	printf("{ ");
	if (array != NULL && array->size > 0)
	{
		jep_obj *elem = array->head;
		while (elem != NULL)
		{
			if (elem->type == JEP_BYTE)
			{
				printf("[byte] %d", *((int *)(elem->val)));
			}
			else if (elem->type == JEP_INT)
			{
				printf("[int] %d", *((int *)(elem->val)));
			}
			else if (elem->type == JEP_LONG)
			{
				printf("[long] %ld", *((long *)(elem->val)));
			}
			else if (elem->type == JEP_DOUBLE)
			{
				printf("[double] %.2lf", *((double *)(elem->val)));
			}
			else if (elem->type == JEP_CHARACTER)
			{
				printf("[character] %c", *((char *)(elem->val)));
			}
			else if (elem->type == JEP_STRING)
			{
				printf("[string] %s", (char *)(elem->val));
			}
			else if (elem->type == JEP_ARRAY)
			{
				printf("[array] ");
				jep_print_array((jep_obj *)(elem->val));
			}
			else if (elem->type == JEP_REFERENCE)
			{
				printf("[reference] ");
			}
			else if (elem->type == JEP_FILE)
			{
				printf("[file] ");
			}
			else if (elem->type == JEP_ARGUMENT)
			{
				printf("[arg]");
			}
			else if (elem->type == JEP_STRUCT)
			{
				printf("[struct]");
			}
			else if (elem->type == JEP_STRUCTDEF)
			{
				printf("[structdef]");
			}
			else if (elem->type == JEP_NULL)
			{
				printf("[null]");
			}
			else if (elem->type == JEP_THREAD)
			{
				printf("[thread]");
			}
			if (elem->next != NULL)
			{
				printf(", ");
			}
			elem = elem->next;
		}
	}
	printf(" }");
}

/* frees the memory used by an array */
static void jep_free_array(jep_obj *array)
{
	if (array != NULL && array->size > 0)
	{
		jep_obj *elem = array->tail;
		jep_obj *prev = NULL;
		while (elem != NULL)
		{
			prev = elem->prev;
			jep_destroy_object(elem);
			elem = prev;
		}
	}
}

/* creates a string representation of an object */
char *jep_to_string(jep_obj *o)
{
	char *str = NULL;

	if (o == NULL)
	{
		return str;
	}

	if (o->type == JEP_STRING)
	{
		char *s = (char *)(o->val);
		int len = strlen(s);
		str = malloc(len + 1);
		strcpy(str, s);
		str[len] = '\0';
	}
	else if (o->type == JEP_CHARACTER)
	{
		char *c = (char *)(o->val);
		str = malloc(2);
		str[0] = *c;
		str[1] = '\0';
	}
	else if (o->type == JEP_BYTE)
	{
		unsigned char b = *(unsigned char *)(o->val);
		char s[100];

		int n = sprintf(s, "%d", b);

		if (n > 0)
		{
			if (n >= 100)
			{
				s[99] = '\0';
			}
			else
			{
				s[n] = '\0';
			}
			int len = strlen(s);
			str = malloc(len + 1);
			strcpy(str, s);
			str[len] = '\0';
		}
	}
	else if (o->type == JEP_INT)
	{
		int i = *(int *)(o->val);
		char s[100];

		int n = sprintf(s, "%d", i);

		if (n > 0)
		{
			if (n >= 100)
			{
				s[99] = '\0';
			}
			else
			{
				s[n] = '\0';
			}
			int len = strlen(s);
			str = malloc(len + 1);
			strcpy(str, s);
			str[len] = '\0';
		}
	}
	else if (o->type == JEP_DOUBLE)
	{
		double d = *(double *)(o->val);
		char s[100];

		int n = sprintf(s, "%.4f", d);

		if (n > 0)
		{
			if (n >= 100)
			{
				s[99] = '\0';
			}
			else
			{
				s[n] = '\0';
			}
			int len = strlen(s);
			str = malloc(len + 1);
			strcpy(str, s);
			str[len] = '\0';
		}
	}
	else if (o->type == JEP_ARRAY)
	{
		char *array = "[array]";
		str = malloc(strlen(array) + 1);
		strcpy(str, array);
	}
	else if (o->type == JEP_FUNCTION)
	{
		str = malloc(strlen(o->ident) + 18);
		strcpy(str, "function: ");
		strcat(str, o->ident);
		strcat(str, "(...)");
	}
	else if (o->type == JEP_REFERENCE)
	{
		str = malloc(12);
		strcpy(str, "[reference]");
	}
	else if (o->type == JEP_FILE)
	{
		str = malloc(7);
		strcpy(str, "[file]");
	}
	else if (o->type == JEP_ARGUMENT)
	{
		if (o->val == NULL)
		{
			str = malloc(6);
			strcpy(str, "[arg]");
		}
	}
	else if (o->type == JEP_NULL)
	{
		if (o->val == NULL)
		{
			str = malloc(7);
			strcpy(str, "[null]");
		}
	}
	else if (o->type == JEP_STRUCT)
	{
		str = malloc(9);
		strcpy(str, "[struct]");
	}
	else if (o->type == JEP_STRUCTDEF)
	{
		str = malloc(12);
		strcpy(str, "[structdef]");
	}
	else if (o->type == JEP_THREAD)
	{
		str = malloc(9);
		strcpy(str, "[thread]");
	}

	return str;
}

/* converts a character or string of characters into bytes */
jep_obj *jep_get_bytes(jep_obj *o)
{
	jep_obj *bytes = NULL;
	jep_obj *byte_array = NULL;
	if (o == NULL || o->val == NULL)
	{
		return NULL;
	}

	if (o->type != JEP_CHARACTER && o->type != JEP_STRING)
	{
		/* only converting strings and chars for now */
		return NULL;
	}

	byte_array = jep_create_object();
	byte_array->type = JEP_ARRAY;

	bytes = jep_create_object();
	bytes->type = JEP_LIST;

	if (o->type == JEP_CHARACTER)
	{
		jep_obj *byte = jep_create_object();
		byte->type = JEP_BYTE;
		unsigned char *c = malloc(1);
		*c = *((unsigned char *)(o->val));
		byte->val = c;
		jep_add_object(bytes, byte);
	}
	else if (o->type == JEP_STRING)
	{
		char *str = (char *)(o->val);
		size_t len = strlen(str);
		unsigned int i;
		for (i = 0; i < len; i++)
		{
			jep_obj *byte = jep_create_object();
			byte->type = JEP_BYTE;
			unsigned char *c = malloc(1);
			*c = str[i];
			byte->val = c;
			jep_add_object(bytes, byte);
		}
	}

	byte_array->val = bytes;
	byte_array->size = bytes->size;

	return byte_array;
}

/* compares the values of two objects */
int jep_compare_object(jep_obj *a, jep_obj *b)
{
	int equal = 0;

	if (a == NULL || b == NULL)
	{
		return 0;
	}

	if (a->type != b->type)
	{
		return equal;
	}

	switch (a->type)
	{
	case JEP_STRING:
	{
		if (!strcmp((char*)a->val, (char*)b->val))
		{
			equal = 1;
		}
	}
	break;

	case JEP_INT:
	{
		if (*(int*)a->val == *(int*)b->val)
		{
			equal = 1;
		}
	}
	break;

	case JEP_LONG:
	{
		if (*(long int*)a->val == *(long int*)b->val)
		{
			equal = 1;
		}
	}
	break;

	case JEP_DOUBLE:
	{
		if (*(double*)a->val == *(double*)b->val)
		{
			equal = 1;
		}
	}
	break;

	case JEP_CHARACTER:
	{
		if (*(char*)a->val == *(char*)b->val)
		{
			equal = 1;
		}
	}
	break;

	case JEP_BYTE:
	{
		if (*(unsigned char*)a->val == *(unsigned char*)b->val)
		{
			equal = 1;
		}
	}
	break;

	case JEP_STRUCTDEF:
	case JEP_FUNCTION:
	case JEP_FILE:
	{
		if ((long int)a->self == (long int)b->self)
		{
			equal = 1;
		}
	}
	break;

	case JEP_ARRAY:
	case JEP_STRUCT:
	{
		if (a->size != b->size)
		{
			equal = 0;
		}
		else if (a->size == 0)
		{
			equal = 1;
		}
		else
		{
			jep_obj* a_data = ((jep_obj*)a->val)->head;
			jep_obj* b_data = ((jep_obj*)b->val)->head;

			int inner_equal = 1;
			while (a_data != NULL && inner_equal)
			{
				inner_equal = jep_compare_object(a_data, b_data);
				a_data = a_data->next;
				b_data = b_data->next;
			}
			if (inner_equal)
			{
				equal = 1;
			}
		}
	}
	break;

	case JEP_REFERENCE:
	{
		jep_obj* a_ref = (jep_obj*)a->val;
		jep_obj* b_ref = (jep_obj*)b->val;
		equal = jep_compare_object(a_ref, b_ref);
	}
	break;

	case JEP_NULL:
	{
		equal = 1;
	}
	break;

	default:
		break;
	}


	return equal;
}

/* allocates memory for a new object */
jep_obj *jep_create_object()
{
	creation++;
	jep_obj *o = malloc(sizeof(jep_obj));

	o->val = NULL;
	o->ident = NULL;
	o->type = 0;
	o->prev = NULL;
	o->next = NULL;
	o->head = NULL;
	o->tail = NULL;
	o->size = 0;
	o->ret = 0;
	o->index = -1;
	o->mod = 0;
	o->array_ident = NULL;
	o->self = o;

	return o;
}

/* adds an object to a list */
void jep_add_object(jep_obj *list, jep_obj *o)
{
	if (list->head == NULL && list->tail == NULL)
	{
		list->head = o;
		list->tail = o;
		list->size++;
	}
	else if (list->tail->type == JEP_LIST)
	{
		jep_add_object(list->tail, o);
	}
	else
	{
		list->tail->next = o;
		o->prev = list->tail;
		list->tail = o;
		list->size++;
	}
}

/* removes the last object from a list */
void jep_pop_object(jep_obj *list)
{
	if (list->size == 1)
	{
		list->head = NULL;
		list->tail = NULL;
	}
	else if (list->size > 1)
	{
		list->tail = list->tail->prev;
		list->tail->next = NULL;
	}

	list->size--;
}

/* retreives an object from a list */
jep_obj *jep_get_object(const char *ident, jep_obj *list)
{
	if (list == NULL || ident == NULL)
	{
		return NULL;
	}

	jep_obj *o = NULL;
	jep_obj *obj = list->head;

	while (obj != NULL)
	{
		if (obj->ident != NULL && !strcmp(ident, obj->ident))
		{
			o = obj;
		}
		if (obj->type == JEP_LIST)
		{
			jep_obj *temp = NULL;
			if (o != NULL)
			{
				temp = o;
			}
			o = jep_get_object(ident, obj);
			if (o == NULL && temp != NULL)
			{
				o = temp;
			}
		}
		obj = obj->next;
	}

	return o;
}

/* copies the value of one obect into another */
void jep_copy_object(jep_obj *dest, jep_obj *src)
{
	if (dest == NULL || src == NULL)
	{
		return;
	}

	if (dest->val != NULL)
	{
		if (dest->type == JEP_ARRAY)
		{
			/* frees the memory used by an array */
			jep_free_array((jep_obj *)(dest->val));
			dest->size = 0;
			free(dest->val);
		}
		else if (dest->type == JEP_FUNCTION)
		{
			jep_obj *args = dest->head;

			/* destroy the body of non-native functions */
			if (dest->size == 2)
			{
				jep_obj *body = args->next;
				jep_destroy_object(body);
			}

			jep_destroy_object(args);
		}
		else if (dest->type == JEP_REFERENCE)
		{
		}
		else if (dest->type == JEP_FILE)
		{
			jep_file *file_obj = (jep_file *)(dest->val);
			(file_obj->refs)--;
			if (file_obj->refs <= 0)
			{
				if (file_obj->open)
				{
					/* call the appropriate close function */
					if (file_obj->type == 0)
					{
						fclose(file_obj->file);
					}
					else if (file_obj->type == 1)
					{
						jep_socket_close(file_obj->socket);
						jep_free_addrinf(file_obj->info);
					}
				}
				free(dest->val);
			}
		}
		else if (dest->type == JEP_STRUCT || dest->type == JEP_STRUCTDEF)
		{
			jep_destroy_list((jep_obj *)(dest->val));
			free(dest->val);
		}
		else if (dest->type == JEP_THREAD)
		{
			free(dest->val);
		}
		else
		{
			free(dest->val);
		}
	}

	dest->type = src->type;
	dest->ret = src->ret;

	if (src->type == JEP_BYTE)
	{
		unsigned char *b = malloc(sizeof(int));
		*b = *(unsigned char *)(src->val);
		dest->val = (void *)b;
	}
	else if (src->type == JEP_INT)
	{
		int *i = malloc(sizeof(int));
		*i = *(int *)(src->val);
		dest->val = (void *)i;
	}
	else if (src->type == JEP_LONG)
	{
		long *l = malloc(sizeof(long));
		*l = *(long *)(src->val);
		dest->val = (void *)l;
	}
	else if (src->type == JEP_DOUBLE)
	{
		double *d = malloc(sizeof(double));
		*d = *(double *)(src->val);
		dest->val = (void *)d;
	}
	else if (src->type == JEP_CHARACTER)
	{
		char *c = malloc(sizeof(char));
		*c = *(char *)(src->val);
		dest->val = (void *)c;
	}
	else if (src->type == JEP_STRING)
	{
		int len = strlen((char *)(src->val));
		dest->val = malloc(len + 1);
		strcpy(dest->val, (char *)(src->val));
	}
	else if (src->type == JEP_LIST)
	{
		dest->size = 0;
		jep_obj* src_val = src->head;
		while (src_val != NULL)
		{
			jep_obj* dest_val = jep_create_object();
			jep_copy_object(dest_val, src_val);
			jep_add_object(dest, dest_val);
			src_val = src_val->next;
		}
	}
	else if (src->type == JEP_ARRAY)
	{
		jep_obj *array = (jep_obj *)(src->val);
		jep_obj *dest_array = jep_create_object();
		dest_array->type = JEP_LIST;
		if (array != NULL && array->size > 0)
		{
			jep_obj *orig = array->head; /* original */
			while (orig != NULL)
			{
				jep_obj *copy = jep_create_object();
				copy->index = orig->index;
				copy->array_ident = orig->array_ident;
				jep_copy_object(copy, orig);
				jep_add_object(dest_array, copy);
				orig = orig->next;
			}
		}
		dest->size = src->size;
		dest->val = dest_array;
	}
	else if (src->type == JEP_FUNCTION)
	{
		jep_ast_node *n = jep_create_ast_node();
		jep_obj *args = jep_create_object();
		jep_obj *body = NULL;

		jep_obj *src_args = src->head;
		if (src_args != NULL)
		{
			jep_obj *a = src_args->head;
			while (a != NULL)
			{
				jep_obj *arg = jep_create_object();
				arg->type = JEP_ARGUMENT;
				arg->ident = a->ident;
				jep_add_object(args, arg);
				a = a->next;
			}
		}

		if (src->size == 2)
		{
			body = jep_create_object();
			body->type = JEP_FUNCTION_BODY;
			*n = *((jep_ast_node *)(src_args->next->val));
			body->val = n;
		}

		jep_add_object(dest, args);

		if (src->size == 2)
		{
			jep_add_object(dest, body);
		}

		dest->head->type = JEP_LIST;
	}
	else if (src->type == JEP_FILE)
	{
		dest->val = src->val;
		((jep_file *)(dest->val))->refs++;
	}
	else if (src->type == JEP_REFERENCE)
	{
		/* changed if condition from dest->type to src->type */
		dest->val = src->val;
	}
	else if (src->type == JEP_STRUCT || src->type == JEP_STRUCTDEF)
	{
		jep_obj *members = jep_create_object();
		members->type = JEP_LIST;
		jep_obj *src_mem = ((jep_obj *)(src->val))->head;
		while (src_mem != NULL)
		{
			jep_obj *mem = jep_create_object();
			mem->ident = src_mem->ident;
			mem->index = src_mem->index;
			jep_copy_object(mem, src_mem);
			jep_add_object(members, mem);
			src_mem = src_mem->next;
		}
		dest->val = members;
	}
	else if (src->type == JEP_THREAD)
	{
		jep_thread* src_thread = (jep_thread*)(src->val);
		jep_thread_args* src_args = (jep_thread_args*)(src_thread->args);

		jep_thread* dest_thread = malloc(sizeof(jep_thread));
		jep_thread_args* dest_args = malloc(sizeof(jep_thread_args));

		dest_args->proc = src_args->proc;
		dest_args->args = src_args->args;
		dest_args->list = src_args->list;

		dest_thread->thread_ptr = src_thread->thread_ptr;
		dest_thread->proc = src_thread->proc;
		dest_thread->args = dest_args;
		dest_thread->started = src_thread->started;

		dest->val = dest_thread;
	}
	else if (src->type == JEP_LIBRARY)
	{
		dest->val = src->val;
	}
	else if (src->type == JEP_NULL)
	{
		dest->val = NULL;
	}
}

void jep_copy_main_list(jep_obj *dest, jep_obj *src)
{
	if (dest == NULL || src == NULL)
	{
		return;
	}

	if (dest->val != NULL)
	{
		if (dest->type == JEP_ARRAY)
		{
			/* frees the memory used by an array */
			jep_free_array((jep_obj *)(dest->val));
			dest->size = 0;
			free(dest->val);
		}
		else if (dest->type == JEP_FUNCTION)
		{
			jep_obj *args = dest->head;

			/* destroy the body of non-native functions */
			if (dest->size == 2)
			{
				jep_obj *body = args->next;
				jep_destroy_object(body);
			}

			jep_destroy_object(args);
		}
		else if (dest->type == JEP_REFERENCE)
		{
		}
		else if (dest->type == JEP_FILE)
		{
			jep_file *file_obj = (jep_file *)(dest->val);
			(file_obj->refs)--;
			if (file_obj->refs <= 0)
			{
				if (file_obj->open)
				{
					/* call the appropriate close function */
					if (file_obj->type == 0)
					{
						fclose(file_obj->file);
					}
					else if (file_obj->type == 1)
					{
						jep_socket_close(file_obj->socket);
						jep_free_addrinf(file_obj->info);
					}
				}
				free(dest->val);
			}
		}
		else if (dest->type == JEP_STRUCT || dest->type == JEP_STRUCTDEF)
		{
			jep_destroy_list((jep_obj *)(dest->val));
			free(dest->val);
		}
		else if (dest->type == JEP_THREAD)
		{
			free(dest->val);
		}
		else
		{
			free(dest->val);
		}
	}

	dest->type = src->type;
	dest->ret = src->ret;

	if (src->type == JEP_LIST)
	{
		dest->size = 0;
		jep_obj* src_val = src->head;
		while (src_val != NULL)
		{
			jep_obj* dest_val = jep_create_object();
			jep_copy_object(dest_val, src_val);
			dest_val->ident = src_val->ident;
			jep_add_object(dest, dest_val);
			src_val = src_val->next;
		}
	}
}

/* copies the value of the self pointer of an object */
void jep_copy_self(jep_obj *dest, jep_obj *src)
{
	if (dest == NULL || src == NULL)
	{
		return;
	}

	if (dest->type != src->type)
	{
		return;
	}

	dest->self = src->self;

	if (dest->type == JEP_ARRAY || dest->type == JEP_STRUCT)
	{
		jep_obj *src_array = (jep_obj *)(src->val);
		jep_obj *dest_array = (jep_obj *)(dest->val);
		if (src_array != NULL && src_array->size > 0)
		{
			jep_obj *src_e = src_array->head;   /* source */
			jep_obj *dest_e = dest_array->head; /* destination */
			while (src_e != NULL && dest_e != NULL)
			{
				jep_copy_self(dest_e, src_e);
				dest_e = dest_e->next;
				src_e = src_e->next;
			}
		}
	}
	else if (dest->type == JEP_REFERENCE)
	{
		jep_obj *src_ref = (jep_obj *)(src->val);
		jep_obj *dest_ref = (jep_obj *)(dest->val);
		jep_copy_self(dest_ref, src_ref);
	}
}

/* frees the memory used by an object */
void jep_destroy_object(jep_obj *obj)
{
	destruction++;

	if (obj != NULL)
	{
		if (obj->type == JEP_INT && obj->val != NULL)
		{
			free(obj->val);
		}
		else if (obj->type == JEP_LONG && obj->val != NULL)
		{
			free(obj->val);
		}
		else if (obj->type == JEP_DOUBLE && obj->val != NULL)
		{
			free(obj->val);
		}
		else if (obj->type == JEP_CHARACTER && obj->val != NULL)
		{
			free(obj->val);
		}
		else if (obj->type == JEP_BYTE && obj->val != NULL)
		{
			free(obj->val);
		}
		else if (obj->type == JEP_STRING && obj->val != NULL)
		{
			free(obj->val);
		}
		else if (obj->type == JEP_ARRAY && obj->val != NULL)
		{
			jep_obj *array = (jep_obj *)(obj->val);
			jep_destroy_object(array);
		}
		else if (obj->type == JEP_FUNCTION)
		{
			jep_obj *args = obj->head;

			/* destroy the body of non-native functions */
			if (obj->size == 2)
			{
				jep_obj *body = args->next;
				jep_destroy_object(body);
			}

			if (args != NULL)
			{
				jep_destroy_object(args);
			}
		}
		else if (obj->type == JEP_FUNCTION_BODY)
		{
			free(obj->val);
		}
		else if (obj->type == JEP_ARGUMENT)
		{
			/* arguments have no value */
		}
		else if (obj->type == JEP_REFERENCE)
		{
			/* the values of references should be freed elsewhere */
		}
		else if (obj->type == JEP_NULL)
		{
			/* null objects have no memory allocated for a value */
		}
		else if (obj->type == JEP_FILE)
		{
			jep_file *file_obj = (jep_file *)(obj->val);
			(file_obj->refs)--;
			/* destroy the file object if nothing is using it */
			if (file_obj->refs <= 0)
			{
				if (file_obj->open)
				{
					/* call the appropriate close function */
					if (file_obj->type == 0)
					{
						fclose(file_obj->file);
					}
					else if (file_obj->type == 1)
					{
						jep_socket_close(file_obj->socket);
						jep_free_addrinf(file_obj->info);
					}
				}
				free(file_obj);
			}
		}
		else if (obj->type == JEP_STRUCT || obj->type == JEP_STRUCTDEF)
		{
			jep_destroy_object((jep_obj *)(obj->val));
		}
		else if (obj->type == JEP_THREAD && obj->val != NULL)
		{
			/*
			thread cleanup can probably be done in the base proc
			duh_thread_args* obj_args = (duh_thread_args*)(((duh_thread*)(obj->val))->args);

			jep_destroy_object(obj_args->proc);
			jep_destroy_object(obj_args->args);

			free(((duh_thread*)obj->val)->args);
			*/

			free(obj->val);
		}
		else if (obj->type == JEP_LIST)
		{
			jep_destroy_list(obj);
		}
		else if (obj->type == JEP_LIBRARY)
		{
			/*
			 * currently the only object of this type
			 * is the SwapNative shared library. The memory
			 * containing its value is freed by the functionality
			 * provided by the OS
			 */
		}
		else
		{
			printf("unrecognized type while destroying object %d\n", obj->type);
		}

		obj->val = NULL;
		free(obj);
	}
}

/* frees the memory in a list of objects */
void jep_destroy_list(jep_obj *list)
{
	if (list == NULL)
	{
		return;
	}

	jep_obj *obj = list->head;
	jep_obj *next = NULL;

	if (obj == NULL)
	{
		return;
	}

	do
	{
		next = obj->next;
		jep_destroy_object(obj);
		obj = next;
	} while (obj != NULL);
}

jep_obj *jep_number(const char *s)
{
	char *endptr;
	long int l;
	double d;
	void *val;
	jep_obj *obj;

	errno = 0;
	endptr = NULL;
	obj = NULL;

	/* check for bytes */
	const size_t len = strlen(s);
	if (s[len - 1] == 'b')
	{
		char *tmp = malloc(len + 1);
		strcpy(tmp, s);
		tmp[len - 1] = '\0';
		l = strtol(tmp, &endptr, 10);
		free(tmp);
		if (errno == ERANGE)
		{
			printf("byte out of range\n");
			return NULL;
		}
		else if (l > UCHAR_MAX || l < 0)
		{
			printf("byte out of range\n");
			return NULL;
		}
		unsigned char *b = malloc(1);
		*b = (unsigned char)(l & UCHAR_MAX);
		val = (void *)b;
		obj = jep_create_object();
		obj->val = (void *)val;
		obj->type = JEP_BYTE;

		return obj;
	}

	/* attempt to convert the string to a long int */
	l = strtol(s, &endptr, 10);

	if (errno == ERANGE)
	{
		printf("number out of range\n");
		return NULL;
	}
	else if (endptr != s && *endptr != '\0')
	{
		endptr = NULL;
		errno = 0;

		/* attempt to convert the string to a double */
		d = strtod(s, &endptr);

		if (errno == ERANGE)
		{
			printf("number out of range\n");
		}
		else if (endptr != s && *endptr != '\0')
		{
			printf("invalid number format\n");
		}
		else
		{
			double *d_ptr = malloc(sizeof(double));
			*d_ptr = d;
			val = (void *)d_ptr;
			obj = jep_create_object();
			obj->val = (void *)val;
			obj->type = JEP_DOUBLE;
		}
	}
	else if (l >= INT_MIN && l <= INT_MAX)
	{
		/* cast the value as an int if it will fit */
		int *l_ptr = malloc(sizeof(int));
		*l_ptr = (int)l;
		val = (void *)l_ptr;
		obj = jep_create_object();
		obj->val = (void *)val;
		obj->type = JEP_INT;
	}
	else
	{
		long int *l_ptr = malloc(sizeof(long int));
		*l_ptr = l;
		val = (void *)l_ptr;
		obj = jep_create_object();
		obj->val = (void *)val;
		obj->type = JEP_LONG;
	}

	return obj;
}

/* converts a token into a character object */
jep_obj *jep_character(const char *s)
{
	jep_obj *obj = NULL;

	obj = jep_create_object();
	obj->type = JEP_CHARACTER;

	/*
	 * NOTE: this function used to expect
	 * that a string of characters have more
	 * than just the '\0' character.
	 * It was modified to allow for the use of
	 * the '\0' character.
	 */

	if (s == NULL) /* || s[0] == '\0') */
	{
		obj->val = NULL;
	}
	/*
	else if (strlen(s) != 1)
	{
		printf("invalid character length\n");
		free(obj);
		obj = NULL;
	}
	*/
	else
	{
		char *c = malloc(1);
		*c = s[0];
		obj->val = (void *)c;
	}

	return obj;
}

/* converts a token into a string object */
jep_obj *jep_string(const char *s)
{
	jep_obj *obj = NULL;
	int len = strlen(s);
	obj = jep_create_object();
	obj->type = JEP_STRING;
	obj->val = malloc(len + 1);
	strcpy(obj->val, s);

	return obj;
}

/* convertes an ast into an array */
jep_obj *jep_array(jep_ast_node *ast)
{
	jep_obj *obj = jep_create_object();
	obj->type = JEP_ARRAY;
	return obj;
}

void jep_print_object(jep_obj *obj)
{
	static int inner_list = 0;
	if (obj != NULL)
	{
		if (obj->type == JEP_BYTE)
		{
			printf("[byte] %s: %d\n", obj->ident, *((int *)(obj->val)));
		}
		else if (obj->type == JEP_INT)
		{
			printf("[int] %s: %d\n", obj->ident, *((int *)(obj->val)));
		}
		else if (obj->type == JEP_LONG)
		{
			printf("[long] %s: %ld\n", obj->ident, *((long *)(obj->val)));
		}
		else if (obj->type == JEP_DOUBLE)
		{
			printf("[double] %s: %.2lf\n", obj->ident, *((double *)(obj->val)));
		}
		else if (obj->type == JEP_CHARACTER)
		{
			printf("[character] %s: %c\n", obj->ident, *((char *)(obj->val)));
		}
		else if (obj->type == JEP_STRING)
		{
			printf("[string] %s: %s\n", obj->ident, (char *)(obj->val));
		}
		else if (obj->type == JEP_ARRAY)
		{
			printf("[array] %s: ", obj->ident);
			jep_obj *array = (jep_obj *)(obj->val);
			jep_print_array(array);
			printf("\n");
		}
		else if (obj->type == JEP_FUNCTION)
		{
			printf("[function] %s (", obj->ident);
			jep_obj *args = obj->head;
			jep_obj *arg = NULL;
			if (args != NULL)
			{
				arg = args->head;
			}
			while (arg != NULL)
			{
				printf("%s", arg->ident);
				if (arg->next != NULL)
				{
					printf(", ");
				}
				arg = arg->next;
			}
			printf(")\n");
		}
		else if (obj->type == JEP_LIST)
		{
			inner_list++;
			printf("inner list %d\n", inner_list);
			jep_print_list(obj);
			inner_list--;
		}
		else if (obj->type == JEP_REFERENCE)
		{
			printf("[reference] %s\n", obj->ident);
		}
		else if (obj->type == JEP_FILE)
		{
			printf("[file] %s\n", obj->ident);
		}
		else if (obj->type == JEP_NULL)
		{
			printf("[null] %s\n", obj->ident);
		}
		else if (obj->type == JEP_STRUCT)
		{
			printf("[struct] %s\n", obj->ident);
		}
		else if (obj->type == JEP_STRUCTDEF)
		{
			printf("[structdef] %s\n", obj->ident);
		}
		else if (obj->type == JEP_LIBRARY)
		{
			printf("[library] %s\n", obj->ident);
		}
		else
		{
			printf("unrecognized type while printing object %d\n", obj->type);
		}
	}
	else
	{
		printf("[null]\n");
	}
}

/* prints a list of objects to stdout */
void jep_print_list(jep_obj *list)
{
	if (list == NULL)
	{
		return;
	}

	jep_obj *obj = NULL;

	obj = list->head;

	if (obj == NULL)
	{
		return;
	}

	do
	{
		jep_print_object(obj);
		obj = obj->next;
	} while (obj != NULL);
}

/* removes the scope list off the tail of a list */
void jep_remove_scope(jep_obj *list)
{
	if (list == NULL)
	{
		return;
	}
	else if (list->tail == NULL)
	{
		return;
	}
	else if (list->tail->type != JEP_LIST)
	{
		return;
	}

	jep_obj *tail = list->tail;

	if (tail->tail != NULL && tail->tail->type == JEP_LIST)
	{
		jep_remove_scope(tail);
	}
	else if (list->tail != NULL)
	{
		list->tail = list->tail->prev;
		if (list->tail != NULL)
		{
			list->tail->next = NULL;
		}
		list->size--;
		if (list->size == 0)
		{
			list->head = NULL;
		}
	}
}

void print_call_counts()
{
	printf("objects created: %d\n", creation);
	printf("objects destroyed: %d\n", destruction);
}
