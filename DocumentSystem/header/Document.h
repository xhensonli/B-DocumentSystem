#ifndef DOCUMENT_H_INCLUDED
#define DOCUMENT_H_INCLUDED



#endif // DOCUMENT_H_INCLUDED
#include <time.h>
 typedef struct{
    int readerId;
    char name[30];
 } reader,*readerptr;

 typedef struct rcd{
    readerptr reader;
    char borrowTime[25];
    char returnTime[25];
 }rcd,*rcdptr;

 typedef struct queueNode{
    rcdptr data;
    struct queueNode *next;
 }qNode;


 typedef struct{
    int count;
    qNode *front;
    qNode *rear;
 }que,*queue;

 typedef struct{
    int docId;
    char name[30];
    char writer[30];
    int total;
    int count;
    queue borrower;
 }document,*docptr;


typedef struct docNode{
    docptr data;
    struct docNode *next;
}docNode,*docNodeptr;




typedef struct{
    docNodeptr *rcd;
    int size;
    int count;
}HashTable,*Hashptr;

int getId();

docptr getDoc(char *name,char *writer,int total);


void initQueue(queue *q);

int enQueue(queue q,rcdptr rdr);

void deQueue(queue q);

rcdptr getFront(queue q);

int removeElem(queue q,int rdid);

int borrowDoc(docptr doc,readerptr rdr);

int returnDoc(docptr doc,int readerid);

int addHash(Hashptr table,docptr doc);

void checkAndInsert(BTree *t,Hashptr table,KeyType k,void *rec);

void insertReader(BTree *t,void *rec);

int printDocByWriter(Hashptr table,char *name);

void printDocDetail(docptr doc);

int removeHash(Hashptr table,docptr doc);

void writeLog(char *msg);

int updateDoc(Hashptr tb,docptr doc,char *nm,char *wrtr,int tot);

int hashfun(char *data);

char *catchString(int n,...);

void save(BTree t,BTree rt);

int load(BTree *t,BTree *rt,Hashptr tb);

void saveReader(BTree rt,FILE *fp_reader);

int loadReader(BTree *rt);

void printAllDoc(BTree t);

void printAllReader(BTree rt);

void flush();

void menu();
