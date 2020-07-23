#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parsing.h"
#include "mem_string.h"
#include "parsing_list.h"

int parsing_compare(_rule_node_t* rule_head, _node_t* head, _http_header* http)
{
	_node* curr = NULL;
	_rule_node* r_curr = NULL;
	char* value = NULL;
	int rv = 0;
	unsigned int value_size = 0;

	if ((rule_head == NULL) || (http == NULL) || (head == NULL)) {
		printf("parsing compare param error\n");
		return -1;
	}

	/* header node search */
	curr = head->current;
	while (curr != NULL) {
		if (strcmp(curr->name, HOST) == 0) {
			if ((curr->value == NULL) || (curr->value_size < 0)) {
				printf("http헤더에서 Host부분을 가져올 수 없습니다.\n");
				return -1;
			}
			value = curr->value;
			value_size = curr->value_size;
		}
		curr = curr->prev;
	}

	r_curr = rule_head->current;
	while (r_curr != NULL) {
		if (r_curr->type == TYPE_PATH) {
			rv = path_compare(r_curr,  http);
			if (rv < 0) {
				return -1;
			}
			if (rv == 0) {
				break;
			}
		}
		if (r_curr->type == TYPE_HOST) {
			rv = host_compare(r_curr, value, value_size);
			if (rv < 0) {
				return -1;
			}
			if (rv == 0) {
				break;
			}
		}
		r_curr = r_curr->prev;
	}
	if (r_curr == NULL) {
		printf("=================================================\n");
		printf("config파일에서 매치되는 rule을 찾을 수 없습니다.\n");
		printf("=================================================\n");
		return 1;	
	}


	return 0;
}

int path_compare(_rule_node* rule_head, _http_header* http)
{
	unsigned int string_size = 0;
	int rv = 0;

	if ((http->uri.path == NULL) || (rule_head->string == NULL) || (http->uri.path_size < 0) || (rule_head->addr_node == NULL) || (rule_head->string_size < 0) || (rule_head->match < 0)) {
		printf("config파일에서 Path를 가져올 수 없습니다.\n");
		return -1;
	}
	if ((rule_head->match == MATCH_START) || (rule_head->match == MATCH_END)) {
		string_size = rule_head->string_size;
	}	
	else {
		string_size = http->uri.path_size;
	}

	rv = string_find(http->uri.path, rule_head->string, string_size, rule_head->match);
	if (rv == 0) {
		/* ip, port round robin */
		rv = round_robin(rule_head->addr_node);
		if (rv < 0) return -1;
	}
	if (rv < 0) {
		return -1;
	}
	if (rv > 0) {
		return 1;
	}

	return 0;
}

int host_compare(_rule_node* rule_head, char* value, unsigned int value_size)
{
	unsigned int string_size = 0;
	int rv = 0;

	if ((rule_head->string == NULL) || (rule_head->match < 0) || (rule_head->addr_node == NULL)) {
		printf("config파일에서 Host를 가져올 수 없습니다.\n");
		return -1;
	}
	if ((rule_head->match == MATCH_START) || (rule_head->match == MATCH_END)) {
		string_size = rule_head->string_size;
	}
	else {
		string_size = value_size;
	}

	rv = string_find(value, rule_head->string, string_size, rule_head->match);
	if (rv == 0) {
		/* ip, port round robin */	
		rv = round_robin(rule_head->addr_node);		
		if (rv < 0) return -1;
	}
	if (rv < 0) {
		return -1;
	}
	if (rv > 0) {
		return 1;
	}

	return 0;
}

int string_find(char* buffer, char* string, int buffer_size, int match)
{
	char* ptr = NULL;

	if ((buffer == NULL) || (string == NULL) || (buffer_size < 0) || (match < 0)) {
		printf("string find param error\n");
		return -1;
	}

	if (match == MATCH_START) {
		ptr = memstr(buffer, string, buffer_size);
		if (ptr == NULL) {
			return 1;
		}
	}
	else if (match == MATCH_ANY) {
		ptr = memstr(buffer, string, buffer_size);
		if (ptr == NULL) {
			return 1;
		}
	}
	else if (match == MATCH_END) {
		ptr = memstrr(buffer, string, buffer_size);
		if (ptr == NULL) {
			return 1;
		}
	}
	else {
		printf("string find error\n");
		return -1;
	}

	return 0;
}

int round_robin(_addr_node_t* addr_head)
{
	if (addr_head == NULL) {
		printf("round robin error\n");
		return -1;
	}

	printf("=================================================\n");
	printf("매칭서버->  %u.%u.%u.%u:%u\n", addr_head->current->ip[0], addr_head->current->ip[1], addr_head->current->ip[2], addr_head->current->ip[3], addr_head->current->port);
	printf("=================================================\n");
	addr_head->current = addr_head->current->prev;

	return 0;
}
