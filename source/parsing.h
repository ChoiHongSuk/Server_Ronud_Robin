#ifndef _PARSING_H_
#define	_PARSING_H_

#include "parsing_list.h"
#include "mem_string.h"

#define SERVER_MAX				2048
#define IP_MAX					255
#define PORT_MAX				65535
#define HEADER_PATH_MAX			64
#define IP_PREFIX				4
#define EQUAL					'='
#define AMP						'&'
#define SLIDE					'/'
#define Q_MARK					'?'
#define SHAP					'#'
#define COLON					": "
#define COLON_SIZE				2
#define RULE_COLON				':'
#define SP						' '
#define DOT						'.'
#define COM						','
#define PORT_MAX				65535
#define IP_MAX					255
#define EXIT					"q"
#define LF						'\n'
#define CRLF					"\r\n"
#define CRLF_SIZE				2
#define CRLFCRLF				"\r\n\r\n"
#define CRLFCRLF_SIZE			4
#define SINGLE					1
#define CONTENT_LENGTH			"Content-Length"
#define CONTENT_TYPE			"Content-Type"
#define CONTENT_TYPE_CON		"application/x-www-form-urlencoded"
#define NONE					"none"
#define NONE_SIZE				4
#define RULE_PATH				"PATH"
#define RULE_HOST				"HOST"
#define RULE_TYPE_SIZE_MAX		4
#define RULE_MATCH_SIZE_MAX		5
#define HOST					"Host"
#define START					"Start"
#define ANY						"Any"
#define END						"End"
#define DEC						10

/* http parsing */
typedef struct uri
{
	char* path;
	int path_size;
	char* query;
	char* frag;
}_uri;

typedef struct buffer
{
	char* buffer;
	char* req_line;
	char* head_line;
}_buffer;

typedef struct http_header
{
	_buffer buff;
	char* method;
	char* version;
	_uri uri;
	char* body;
	char* garbage;
}_http_header;

/* http parsing enum */
typedef enum
{
	BUFFER,
	REQ,
	HEAD,
	METHOD,
	PATH,
	QUERY,
	FRAG,
	VERSION,
	BODY,
	GARBAGE
}_type;

typedef enum
{
	OP_QUERY,
	OP_PAYLOAD,
	OP_BODY,
	OP_BODY_PAYLOAD
}_query_option;

/* rule parsing enum */
typedef enum
{
	TYPE_PATH,
	TYPE_HOST,
	TYPE_GARB
}_rule_type;

typedef enum
{
	MATCH_START,
	MATCH_ANY,
	MATCH_END,
	MATCH_GARB
}_match;

/* http parsing */
int start_http_header_parsing(char** argv);
int file_read(char* path, _http_header* http, _node_t* head, _qp_t* qp_head);
int buffer_full_size(char* path, int* full_size);
int buffer_devide(char* buffer, int full_size, _http_header* http, _node_t* head, _qp_t* qp_head);
int request_extractor(char* req_line, int req_size, _http_header* http, _qp_t* qp_head);
int uri_extractor(char* uri_line, int uri_size, _http_header* http, _qp_t* qp_head);
int query_payload_extractor(char* line, int size, int qp_option, int option, _http_header* http, _qp_t* qp_head);
int head_extractor(char* head_line, int head_size, _node_t* head);
int garb_extractor(char* garb_line, int garb_size, _http_header* http);
int content_length_get(_node_t* head, int* c_length);
int content_type_get(_node_t* head);
int content_length_convert(char* value, int value_size, int* c_length);

int http_buffer_gen(_http_header* http, char* buffer, int size, int type);
int http_print_buffer(_http_header* http, _node_t* head, _qp_t* qp_head);
void http_buffer_free(_http_header* http);

/* rule parsing */
int config_file_read(_rule_node_t* rule_head, char* rule_path);
int config_parsing(_rule_node_t* rule_head, _addr_node_t* addr_head, char* buffer, int line_size);
int ip_port_convert(char* ip, char* port, int ip_size, int port_size, long* ip_t, int* port_t);
int addr_parsing(_addr_node_t* addr_head, char* ip_addr, int ip_addr_size);
int number_max_get(int number, int size);

/* parsing compare */
int parsing_compare(_rule_node_t* rule_head, _node_t* head, _http_header* http);
int path_compare(_rule_node* rule_head, _http_header* http);
int host_compare(_rule_node* rule_head, char* value, unsigned int value_size);
int string_find(char* buffer, char* string, int buffer_size, int match);
int round_robin(_addr_node_t* addr_head);

#endif
