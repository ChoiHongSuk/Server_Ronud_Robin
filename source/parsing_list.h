#ifndef _PARSING_LIST_H_
#define _PARSING_LIST_H_
#include "parsing.h"
#define IP_PREFIX	4

/* http current */
typedef struct node_t
{
	struct node* current;
	struct node* next;
}_node_t;

typedef struct query_payload_t
{
	struct query_payload* current;
	struct query_payload* next;
}_qp_t;

/* http */
typedef struct node
{
	char* name;
	char* value;
	unsigned int value_size;

	struct node* next;
	struct node* prev;
}_node;

typedef struct query_payload
{
	char* name;
	char* value;
	unsigned int value_size;
	int option;

	struct query_payload* next;
	struct query_payload* prev;
}_qp;

/* addr, rule current */
typedef struct addr_node_t
{
	unsigned int count;
	struct addr_node* current;
	struct addr_node* next;
}_addr_node_t;

typedef struct rule_node_t
{
	struct rule_node* current;
	struct rule_node* next;
}_rule_node_t;

/* ip addr node */
typedef struct addr_node
{
	unsigned char ip[IP_PREFIX];
	unsigned short port;

	struct addr_node* next;
	struct addr_node* prev;
}_addr_node;

/* rule node */
typedef struct rule_node
{
	int type;
	int match;
	char* string;
	unsigned int string_size;
	struct addr_node_t* addr_node;

	struct rule_node* next;
	struct rule_node* prev;
}_rule_node;

int add_list(_node_t* head, char* name, char* value, int name_size, int value_size);
int query_payload_add_list(_qp_t* qp_head, char* name, char* value, int name_size, int value_size, int option);
int rule_add_list(_rule_node_t* rule_head, _addr_node_t* addr_head, int type, int match, char* string, int string_size);
int addr_add_list(_addr_node_t* addr_head, int* ip_t, int port_t);
void print_list_query_payload(_qp_t* qp_head, int option);
void print_list_header(_node_t* head);
void delete_list(_node_t* head);
void delete_list_query_payload(_qp_t* qp_head);
void delete_list_rule(_rule_node_t* rule_head);
void delete_list_addr(_addr_node_t* addr_head);

#endif
