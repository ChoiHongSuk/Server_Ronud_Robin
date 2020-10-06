#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parsing.h"
#include "parsing_list.h"

int add_list(_node_t* head, char* name, char* value, int name_size, int value_size)
{
	if ((head == NULL) || (name == NULL) || (name_size < 0) || (value_size < 0)) {
		printf("list param error(header_node)\n");
		return -1;
	}

	_node* new_node = (_node*)calloc(1, sizeof(_node));
	if (new_node == NULL) {
		printf("new_node is NULL\n");
		return -1;
	}
	new_node->next = head->next;

	/* data */
	new_node->name = (char*)calloc(name_size + 1, 1);
	if (new_node->name == NULL) {
		printf("node name is NULL\n");
		return -1;
	}
	new_node->value = (char*)calloc(value_size + 1, 1);		
	if (new_node->value == NULL) {
		printf("node value is NULL\n");
		return -1;
	}
	memcpy(new_node->name, name, name_size);
	memcpy(new_node->value, value, value_size);
	new_node->value_size = value_size;

	head->next = new_node;

	if (head->current == NULL) {
		new_node->next = NULL;
		head->current = new_node;
		new_node->prev = NULL;
	}
	else {
		head->next->next->prev = head->next;
	}

	return 0;
}

/* query payload list add */
int query_payload_add_list(_qp_t* qp_head, char* name, char* value, int name_size, int value_size, int option)
{
	if ((qp_head == NULL) || (name == NULL) || (name_size < 0) || (value_size < 0) || (option < 0)) {
		printf("list param error(query payload)\n");
		return -1;
	}

	_qp* new_node = (_qp*)calloc(1, sizeof(_qp));
	if (new_node == NULL) {
		printf("new_node is NULL\n");
		return -1;
	}
	new_node->next = qp_head->next;

	/* data */
	new_node->name = (char*)calloc(name_size + 1, 1);
	if (new_node->name == NULL) {
		printf("node name is NULL\n");
		return -1;
	}
	new_node->value = (char*)calloc(value_size + 1, 1);		
	if (new_node->value == NULL) {
		printf("node value is NULL\n");
		return -1;
	}
	memcpy(new_node->name, name, name_size);
	memcpy(new_node->value, value, value_size);
	new_node->value_size = value_size;
	new_node->option = option;

	qp_head->next = new_node;

	if (qp_head->current == NULL) {
		new_node->next = NULL;
		qp_head->current = new_node;
		new_node->prev = NULL;
	}
	else {
		qp_head->next->next->prev = qp_head->next;
	}

	return 0;
}

/* rule list add */
int rule_add_list(_rule_node_t* rule_head, _addr_node_t* addr_head, int type, int match, char* string, int string_size)
{
	if ((rule_head == NULL) || (addr_head == NULL) || (type < 0) || (match < 0) || (string == NULL) || (string_size < 0)) {
		printf("list param error(rule_node)\n");
		return -1;
	}

	_rule_node* new_node = (_rule_node*)calloc(1, sizeof(_rule_node));
	if (new_node == NULL) {
		printf("new_node is NULL\n");
		return -1;
	}
	new_node->next = rule_head->next;

	/* Data */
	new_node->type = type;
	new_node->match = match;
	/* stirng */
	new_node->string = (char*)calloc(string_size + 1, 1);
	if (new_node->string == NULL) {
		printf("node name is NULL\n");
		return -1;
	}
	memcpy(new_node->string, string, string_size);
	new_node->string_size = string_size;

	/* addr */
	new_node->addr_node = (_addr_node_t*)calloc(1, sizeof(_addr_node_t));
	if (new_node->addr_node == NULL) {
		printf("node addr_node is NULL\n");
		return -1;
	}
	new_node->addr_node = addr_head;

	rule_head->next = new_node;

	if (rule_head->current == NULL) {
		new_node->next = NULL;
		rule_head->current = new_node; 
		new_node->prev = NULL;
	}
	else {
		rule_head->next->next->prev = rule_head->next;
	}

	return 0;
}

/* addr list add */
int addr_add_list(_addr_node_t* addr_head, long* ip_t, int port_t)
{
	if ((addr_head == NULL) || (ip_t == NULL) || (port_t < 0)) {
		printf("list param error(addr_node)\n");
		return -1;
	}
	if ((ip_t[0] < 0) || (ip_t[1] < 0) || (ip_t[2] < 0) || (ip_t[3] < 0)) {
		printf("ip_t size error\n");
		return -1;
	}

	if (addr_head->count > SERVER_MAX) {
		printf("server is FULL\n");
		return 0;
	}
	
	_addr_node* new_node = (_addr_node*)calloc(1, sizeof(_addr_node));
	if (new_node == NULL) {
		printf("new_node is NULL\n");
		return -1;
	}
	new_node->next = addr_head->next;

	/* DATA */
	new_node->ip[0] = ip_t[0];
	new_node->ip[1] = ip_t[1];
	new_node->ip[2] = ip_t[2];
	new_node->ip[3] = ip_t[3];
	new_node->port = port_t;

	addr_head->next = new_node;

	if (addr_head->current == NULL) {
		new_node->next = new_node;
		addr_head->current = new_node;
		new_node->prev = new_node;
	}
	else {
		addr_head->next->prev = addr_head->current;
		addr_head->current->next = addr_head->next; /* current->next 원점 */
		addr_head->next->next->prev = addr_head->next;
	}
	addr_head->count++;

	return 0;
}

void print_list_query_payload(_qp_t* qp_head, int option)
{
	_qp* curr = NULL;
	int count = 0;

	if ((qp_head == NULL) || (qp_head->current == NULL)) {
		printf("\tnone\n");
		return;
	}

	curr = qp_head->current;
	while (curr != NULL) {
		if (curr->option == option) {
			if (curr->name != NULL) {
				if (curr->value == NULL) {
					printf("\tname: %s\n", curr->name);
					count++;
				}
				else {
					printf("\tname: %s, value: %s\n", curr->name, curr->value);
					count++;
				}
			}
		}
		curr = curr->prev;
	}
	if (count == 0) printf("\tnone\n");
}

void print_list_header(_node_t* head)
{
	_node* curr = NULL;
	
	if ((head == NULL) || (head->current == NULL)) {
		printf("\tnone\n");
	}

	curr = head->current;
	while (curr != NULL) {
		if (curr->name != NULL) {
			if (curr->value == NULL) {
				printf("\t%s\n", curr->name);
			}
			else {
				printf("\t%s : %s\n", curr->name, curr->value);
			}
		}
		curr = curr->prev;
	}
}

void delete_list(_node_t* head)
{
	_node* curr = NULL;
	_node* tmp = NULL;

	if ((head == NULL) || (head->current == NULL)) return;

	curr = head->current;
	while (curr != NULL) {
		if (curr->name != NULL) {
			free(curr->name);
		}
		if (curr->value != NULL) {
			free(curr->value);
		}

		tmp = curr->prev;
		if (curr != NULL) {
			free(curr);
		}
		curr = tmp;
	}
}


void delete_list_query_payload(_qp_t* qp_head)
{
	_qp* curr = NULL;
	_qp* tmp = NULL;

	if ((qp_head == NULL) || (qp_head->current == NULL)) return;

	curr = qp_head->current;
	while (curr != NULL) {
		if (curr->name != NULL) {
			free(curr->name);
		}
		if (curr->value != NULL) {
			free(curr->value);
		}

		tmp = curr->prev;
		if (curr != NULL) {
			free(curr);
		}
		curr = tmp;
	}
}

void delete_list_rule(_rule_node_t* rule_head)
{
	_rule_node* r_curr = NULL;
	_rule_node* r_tmp = NULL;
	
	if ((rule_head == NULL) || (rule_head->current == NULL)) return;
	
	r_curr = rule_head->current;
	while (r_curr != NULL) {
		if (r_curr->string != NULL) {
			free(r_curr->string);
		}

		delete_list_addr(r_curr->addr_node);

		r_tmp = r_curr->prev;
		if (r_curr != NULL) {
			free(r_curr);
		}
		r_curr = r_tmp;
	}
}

void delete_list_addr(_addr_node_t* addr_head)
{
	_addr_node* a_curr = NULL;
	_addr_node* a_tmp = NULL;

	if ((addr_head == NULL) || (addr_head->current == NULL)) return;

	a_curr = addr_head->current;
	a_tmp = a_curr->prev;

	if (a_curr != NULL) {
		free(a_curr);
	}
	a_curr = a_tmp;
	while (a_curr != addr_head->current) {
		a_tmp = a_curr->prev;
		if (a_curr != NULL) {
			free(a_curr);
		}
		a_curr = a_tmp;
	}
}
