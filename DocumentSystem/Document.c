#include <stdio.h>
#include <stdlib.h>
#include "header/BTree.h"
#include "header/Document.h"
#include <time.h>
#include <string.h>
#include <stdarg.h>
int getId(){
    static int id = 0;
    return id++;
}
int last_id = 0;
void flush(){
    char c;
    while((c = getchar())!=10&&c!=EOF);
}

char *int2str(int num){
    char *res = (char *)malloc(sizeof(char)*50);
    itoa(num,res,10);
    return res;
}

docptr getDoc(char *pname,char *pwriter,int ptotal){
    docptr p = (docptr)malloc(sizeof(document));
    p->docId = ++last_id;
    p->count = p->total = ptotal;
    strcpy(p->name,pname);
    strcpy(p->writer,pwriter);
    initQueue(&(p->borrower));
    return p;
}

void printDoc(docptr p){
    printf("\t|  %-8d   |   %-15.15s     |   %-15.15s     |   %-8d   |   %-8d  |\n",p->docId,p->name,p->writer,p->count,p->total);
     printf("\t--------------------------------------------------------------------------------------------\n");
}

void printCompletDoc(docptr p){
    printf("\t|  %-8d   |   %-15s     |   %-15s     |   %-8d   |   %-8d  |\n",p->docId,p->name,p->writer,p->count,p->total);
     printf("\t--------------------------------------------------------------------------------------------\n");
}

void initQueue(queue *q){
    (*q) = (queue)malloc(sizeof(que));
    (*q)->count = 0;
    (*q)->rear = (*q)->front = NULL;
}

int enQueue(queue q,rcdptr rc){
    if(q==NULL||rc==NULL)
        return 0;
    qNode *node = (qNode *)malloc(sizeof(qNode));
    node->data = rc;
    node->next = NULL;


    if(q->rear)
        q->rear->next = node;
    q->rear = node;
    if(q->front==NULL)
        q->front = node;
        q->count++;
}

void deQueue(queue q){
    if(q==NULL||q->front==NULL)
        return;
    qNode *t = q->front;
    free(t);
    q->front = q->front->next;
    q->count--;
}

rcdptr getFront(queue q){
    if(q==NULL||q->front==NULL)
        return NULL;
    return q->front->data;
}

int removeElem(queue q,int rdid){
    if(q==NULL||q->front==NULL)
        return 0;
    qNode *p = q->front;
    int res = 0;
    if(q->front->data->reader->readerId==rdid){
        qNode *temp = q->front;
        q->front = q->front->next;
        free(temp);
        q->count--;
        res = 1;
    }else {
        while(p->next){
            if(p->next->data->reader->readerId==rdid){
                qNode *temp = p->next;
                p->next = p->next->next;
                free(temp);
                res = 1;
                q->count--;
                break;
            }
            p = p->next;
        }
    }
    return res;
}

int borrowDoc(docptr doc,readerptr rdr){
    if(doc==NULL||doc->count<=0)
        return 0;
    rcdptr rc = (rcdptr)malloc(sizeof(rcd));
    rc->reader = rdr;
    time_t t = time(NULL);
    strftime(rc->borrowTime, 25, "%Y-%m-%d %H:%M:%S", localtime(&t));
    time_t t2 = t+30*24*60*60;
    strftime(rc->returnTime, 25, "%Y-%m-%d %H:%M:%S", localtime(&t2));
    enQueue(doc->borrower,rc);
    doc->count--;
    return 1;
}

int returnDoc(docptr doc,int readerid){
    if(doc==NULL)
        return 0;
    if(removeElem(doc->borrower,readerid)){
        doc->count++;
        return 1;
    }
    return 0;
}

int hashfun(char *data){
    int t = 0,i;
    for(i = 0;data[i];i++){
        t += data[i];
    }
    return t^(t>>16);
}

int existHash(Hashptr table,docptr doc){
    int index = hashfun(doc->writer)%table->size;
    docNodeptr p = table->rcd[index];
    if(p==NULL)
        return 0;
    int exist = 0;
    while(p->next){
        if(p->next->data->docId==doc->docId){
            exist = 1;
        }
    }
    return exist;
}
int addHash(Hashptr table,docptr doc){
    if(table==NULL||doc==NULL)
        return 0;
    if(existHash(table,doc))
        return 0;
    int index = hashfun(doc->writer)%table->size;
    docNodeptr p = (docNodeptr)malloc(sizeof(docNode));
    p->data = doc;
    p->next = table->rcd[index];
    table->rcd[index] = p;
    table->count++;
    return 1;
}

int printDocByWriter(Hashptr table,char *name){
    if(table==NULL)
        return 0;
    int index = hashfun(name)%table->size;
    docNodeptr p = table->rcd[index];
    printf("\n\t--------------------------------------------------------------------------------------------\n");
    printf("\t|   docId     |         docName       |        docWriter      |   docCount   |   docTotal  |\n");
    printf("\t--------------------------------------------------------------------------------------------\n");
    while(p){
        printDoc(p->data);
        p = p->next;
    }
}

void printDocDetail(docptr doc){
    printf("\n\t--------------------------------------------------------------------------------------------\n");
    printf("\t|   docId     |         docName       |        docWriter      |   docCount   |   docTotal  |\n");
    printf("\t--------------------------------------------------------------------------------------------\n");
    printCompletDoc(doc);

    printf("\tThe borrower list:\n");
    qNode *qp = doc->borrower->front;
    printf("\t---------------------------------------------------------------------------------\n");
    printf("\t|     Id      |     Name     |        BorrowTime       |         ReturnTime     |\n");
    printf("\t---------------------------------------------------------------------------------\n");
    while(qp){
        readerptr rdr = qp->data->reader;
        printf("\t|  %-8d   |   %-8s   |   %s   |   %s  |\n",rdr->readerId,rdr->name,qp->data->borrowTime,qp->data->returnTime);
        printf("\t---------------------------------------------------------------------------------\n");
        qp = qp->next;
    }
}



int removeHash(Hashptr table,docptr doc){
    int index = hashfun(doc->writer)%table->size;
    docNodeptr p = table->rcd[index];
    if(p==NULL)
        return 0;
    if(p->data->docId==doc->docId){
        docNodeptr temp = p;
        table->rcd[index] = p->next;
        free(temp);
        return 1;
    }
    int result = 0;
    while(p->next){
        if(p->next->data->docId==doc->docId){
            docNodeptr temp = p->next;
            p->next = temp->next;
            free(temp);
            result = 1;
            break;
        }
    }
    return 0;
}

void writeLog(char *msg){
    FILE *p = fopen("Document.log","a");
    if(p)
        fputs(msg,p);
    fclose(p);
}

char *catchString(int n,...){
    va_list list;
    va_start(list,n);
    char *msg = (char *)malloc(sizeof(char)*70),*p;
    int i = 0;
    for(;i<70;i++){
        msg[i] = 0;
    }
    while(n--){
        p = va_arg(list,char*);
        strcat(msg,p);
    }
    va_end(list);
    return msg;
}

void save(BTree t,BTree rt){
    FILE *fp_index = fopen("index.dat","wb");
    FILE *fp_queue = fopen("queue.dat","wb");
    FILE *fp_reader = fopen("reader.dat","wb");
    saveBTree(t,fp_index,fp_queue);
    saveReader(rt,fp_reader);
    fclose(fp_reader);
    fclose(fp_index);
    fclose(fp_queue);
}

void saveReader(BTree rt,FILE *fp_reader){
    if(rt==NULL)
        return;
    //BTree p = t;
    readerptr rdr;
    BTree visit ;
    int i;
    for(i = 1;i<=rt->keynum;i++){
         rdr = (readerptr)(rt->record[i]);
         fwrite(rdr,sizeof(reader),1,fp_reader);
    }
    for(i = 0;i<=rt->keynum&&rt->ptr[i];i++){
        visit = rt->ptr[i];
        saveReader(visit,fp_reader);
    }
}

void saveBTree(BTree t,FILE *index,FILE *queue){
    if(t==NULL)
        return;
    //BTree p = t;
    docptr doc;
    BTree visit ;
    int i;
    for(i = 1;i<=t->keynum;i++){
         doc = (docptr)(t->record[i]);
         fwrite(doc,sizeof(document),1,index);
         int count = doc->borrower->count;
         fwrite(&count,sizeof(int),1,queue);
         qNode *p = doc->borrower->front;
         while(p){
             fwrite(p->data->reader,sizeof(reader),1,queue);
             fwrite(p->data->borrowTime,sizeof(char)*30,1,queue);
             fwrite(p->data->returnTime,sizeof(char)*30,1,queue);
             p = p->next;
         }
    }
    for(i = 0;i<=t->keynum&&t->ptr[i];i++){
        visit = t->ptr[i];
        saveBTree(visit,index,queue);
    }
}
int loadBTree(BTree *t,docptr doc){
    resultptr res = searchBTree(*t,doc->docId);
    int result = 1;
    if(!res->tag){
        result = (insertBTree(t,doc->docId,res->pt,res->i,doc)==SUCCESS);
    }
    return result;
}


int loadReader(BTree *rt){
    FILE *fp_reader = fopen("reader.dat","rb");
    if(!fp_reader)
        return 0;
    int result = 1;
    readerptr rdr = (readerptr)malloc(sizeof(reader));
    while(fread(rdr,sizeof(reader),1,fp_reader)){
        resultptr res = searchBTree(*rt,rdr->readerId);
        if(!res->tag){
            insertBTree(rt,rdr->readerId,res->pt,res->i,rdr);
            rdr = (readerptr)malloc(sizeof(reader));
        }
    }
    return result;
}

int load(BTree *t,BTree *rt,Hashptr tb){
    if(t==NULL||tb==NULL)
        return;
    FILE *fp_index = fopen("index.dat","rb");
    FILE *fp_queue = fopen("queue.dat","rb");
    if(!fp_index||!fp_queue)
        return;
    loadReader(rt);
    docptr doc = (docptr)malloc(sizeof(document));
    qNode *p;
    int lastid = 0;
    int result = 1;
    readerptr rdr = (readerptr)malloc(sizeof(reader));
    while(fread(doc,sizeof(document),1,fp_index)){
        lastid = doc->docId>lastid?doc->docId:lastid;
        result = loadBTree(t,doc);

        addHash(tb,doc);
        initQueue(&(doc->borrower));
        int count;
        fread(&count,sizeof(int),1,fp_queue);
        while(count--){
           rcdptr rc = (rcdptr)malloc(sizeof(rcd));
           fread(rdr,sizeof(reader),1,fp_queue);
           fread(rc->borrowTime,sizeof(char)*30,1,fp_queue);
           fread(rc->returnTime,sizeof(char)*30,1,fp_queue);
           resultptr res = searchBTree(*rt,rdr->readerId);
           if(res->tag){

                rc->reader = (readerptr)(res->pt->record[res->i]);
                enQueue(doc->borrower,rc);
           }else{
                insertReader(rt,rdr);
                rc->reader = rdr;
                enQueue(doc->borrower,rc);
                rdr = (readerptr)malloc(sizeof(reader));

           }
        }
        doc = (docptr)malloc(sizeof(document));
    }
    free(rdr);
    free(doc);
    fclose(fp_queue);
    fclose(fp_index);
    last_id = lastid;
    return result;
}
int updateDoc(Hashptr tb,docptr doc,char *nm,char *wrtr,int tot){
    if(tot<(doc->total-doc->count)||!nm||!wrtr)
        return 0;
    removeHash(tb,doc);
    strcpy(doc->name,nm);
    strcpy(doc->writer,wrtr);
    int inc = tot-doc->total;
    doc->count += inc;
    doc->total = tot;
    addHash(tb,doc);
    return 1;
}


void checkAndInsert(BTree *t,Hashptr table,KeyType k,void *rec){
    resultptr p = searchBTree(*t,k);
        if(p->tag){
            printf("%d exist\n",k);
        }else{
            insertBTree(t,k,p->pt,p->i,rec);
            addHash(table,(docptr)rec);
            printf("Insert %d successfully\n",k);
           time_t t = time(NULL);
           char time[25];
           strftime(time, 25, "%Y-%m-%d %H:%M:%S", localtime(&t));
           char *msg = catchString(4,time,"      Add new document ",((docptr)rec)->name,"\n");
           writeLog(msg);
           free(msg);
        }
        free(p);
}

void insertReader(BTree *t,void *rec){
    readerptr rdr = (readerptr)rec;
    resultptr res = searchBTree(*t,rdr->readerId);
    if(res->tag){
        printf("%d exist\n",rdr->readerId);
    }else{

        insertBTree(t,rdr->readerId,res->pt,res->i,rec);
        time_t t = time(NULL);
        char time[25];
        strftime(time, 25, "%Y-%m-%d %H:%M:%S", localtime(&t));
        char id[10];
        itoa(rdr->readerId,id,10);
        char *msg = catchString(4,time,"      Add new reader ",id,"\n");
        writeLog(msg);
        free(msg);

    }
    free(res);
}


void menu(){
    printf("\n");
    printf("\t\t\t==========================MENU==============================\n");
    printf("\t\t\t|                     1.新增文献                           |\n");
    printf("\t\t\t|                     2.新增读者信息                       |\n");
    printf("\t\t\t|                     3.查看文献列表                       |\n");
    printf("\t\t\t|                     4.查看读者列表                       |\n");
    printf("\t\t\t|                     5.文献详细信息                       |\n");
    printf("\t\t\t|                     6.打印索引结构                       |\n");
    printf("\t\t\t|                     7.查找著者著作                       |\n");
    printf("\t\t\t|                     8.借书                               |\n");
    printf("\t\t\t|                     9.还书                               |\n");
    printf("\t\t\t|                     10.更新文献信息                      |\n");
    printf("\t\t\t|                     11.删除文献                          |\n");
    printf("\t\t\t|                     12.销毁索引                          |\n");
    printf("\t\t\t|                     13.清屏                              |\n");
    printf("\t\t\t|                     0.退出                               |\n");
    printf("\t\t\t============================================================\n");
}

void printDocIndex(BTree t){
    if(t){
        printDocIndex(t->ptr[0]);
        int i = 1;
        for(;i<=t->keynum;i++){
            printDoc((docptr)(t->record[i]));
            printDocIndex(t->ptr[i]);
        }
    }
}

void printAllDoc(BTree t){
    printf("\n\t--------------------------------------------------------------------------------------------\n");
    printf("\t|   docId     |         docName       |        docWriter      |   docCount   |   docTotal  |\n");
    printf("\t--------------------------------------------------------------------------------------------\n");
    printDocIndex(t);
}


void printReaderIndex(BTree rt){
    if(rt){
        printReaderIndex(rt->ptr[0]);
        int i = 1;
        for(;i<=rt->keynum;i++){
            readerptr rdr = (readerptr)(rt->record[i]);
            printf("\t|  %-8d   |   %-8s   |\n",rdr->readerId,rdr->name);
            printf("\t------------------------------\n");
            printReaderIndex(rt->ptr[i]);
        }
    }
}

void printAllReader(BTree rt){
    printf("\t------------------------------\n");
    printf("\t|     Id      |     Name     |\n");
    printf("\t------------------------------\n");
    printReaderIndex(rt);
}
