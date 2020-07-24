## 1. path, host 라운드 로빈
>	* 파싱한 HTTP 헤더의 파싱 결과와 config파일에 정의된 룰을 비교해 서버 라운드로빈
***
## 2. 사용방법
>	* 프로그램과 함께 config 파일 경로를 인자로 넣음
>	* ``` #http_main config.txt ```
>	* 프로그램 내에서 HTTP헤더 파일 경로 입력 (라운드 로빈 후 다시 입력), 프로그램 종료 시 'q' 입력
~~~ C
C http헤더 경로를 입력하세여. (or Exit: q) > test.txt
192.168.216.173:443
http헤더 경로를 입력하세여. (or Exit: q) > test.txt
192.168.216.172:443
http헤더 경로를 입력하세여. (or Exit: q) > 
q
C 프로그램을 종료합니다.
~~~
***
## 3. 소스코드
> * 주요 구조체
>	* struct rule_node, struct addr_node
~~~ C
C typedef struct rule_node
{
 		int type;
 		int match;
		char* string;
 		struct addr_node* addr_node;
		 
		struct rule_node* tail;
		struct rule_node* next;
		struct rule_node* prev;
}_rule_node;

typedef struct addr_node
{
 		char* ip;
		char* port;
		
		struct rule_node* tail;
 		struct rule_node* next;
 		struct rule_node* prev;
}_addr_node;
~~~
> * 주요 파일
>	* rule_parsing
>		* 파일을 라인으로 읽어 SP 구분자로 쪼갠 후 각 rule_node의 항목 별로 파싱
>	* compare
>		* http 헤더 파싱 결과물과 config 파싱 결과물로 type별 string을 match방법으로 찾음
>	* memstring
>		* compare에서 string을 뒤로 찾을 때의 memstrr()함수를 사용함
