#include <stdio.h>
#include <string.h>
#include "main.h"
#include "process.h"
#include "list.h"
//#include "fork.h"

//inclde/linux/pid.h
enum pid_type
{
        PIDTYPE_PID, //プロセスのPID
        //PIDTYPE_TGID, //スレッドグループのりぃだぁ
        //PIDTYPE_PGID,
        //PIDTYPE_SID,
        PIDTYPE_MAX
};

//inclde/linux/pid.c
//珍しく変数をstaticで確保してる
static struct list_head *pid_hash[PIDTYPE_MAX];

struct tmp_struct
{
	int value;
	char message[100];
	struct list_head list;
};

typedef struct tmp_struct t_t;

void addValue(t_t *list,int value){
	//やりたいことはこういうこと
	list->value = value;
}

void addMessage(t_t *list,char* msg){
	strcpy(list->message,msg);
}

//[詳解]#3プロセスの機能を作ってみる
int main(void)
{

	t_t tmp_1,tmp_2;

	addValue(&tmp_1,999);
	addMessage(&tmp_1,"hello!");
	addMessage(&tmp_2,"I'm tmp2");
	list_add(&tmp_1.list,&tmp_2.list);

	printf("tmp_1:%d\n", tmp_1.value);
	printf("tmp_2:%d\n", tmp_2.value);

	printf("tmp_1:%s\n", tmp_1.message);
	printf("tmp_2:%s\n", tmp_2.message);

	//tmp_1からどうやっってtmp_2のmessageを呼ぶの?
	printf("tmp_1->next(=tmp_2):%s\n", tmp_1.list.next);

	/* code */
	return 0;
}