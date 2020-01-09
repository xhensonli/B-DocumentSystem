#include <stdio.h>
#include <stdlib.h>
#include "header\BTree.h"
#include "header/Document.h"
#include <conio.h>
#include <string.h>
#include <time.h>

int main()
{
    int order;
    BTree t = getBTree();
    BTree rdrt = getBTree();
    Hashptr tb = (Hashptr)malloc(sizeof(HashTable));
    if(load(&t,&rdrt,tb))
        printf("¼ÓÔØ³É¹¦\n");
    menu();
    printf("\nORDER>>>>> ");
    while(!scanf("%d",&order)){
        printf("Illegal command.\n");
        flush();
        printf("\nORDER>>>>> ");
    }
    while(order){
        fflush(stdin);
        switch(order){
            case 1:{
                char name[30],writer[30];int tot;
                printf("name?\n");
                scanf("%s",name);
                printf("writer?\n");
                scanf("%s",writer);
                printf("total?\n");
                scanf("%d",&tot);

                docptr doc = getDoc(name,writer,tot);
                checkAndInsert(&t,tb,doc->docId,doc);
                save(t,rdrt);
                printf("Insert Successfully.\n");
                break;
            }
            case 2:{
                int id;
                char name[20];
                printf("Input the readerId.\n");
                scanf("%d",&id);
                printf("Input the readerName.\n");
                scanf("%s",name);
                readerptr rdr = (readerptr)malloc(sizeof(reader));
                rdr->readerId = id;
                strcpy(rdr->name,name);
                insertReader(&rdrt,rdr);
                save(t,rdrt);
                printf("Insert Successfully.\n");
                break;
            }
            case 3:{
                printAllDoc(t);
                break;
            }
            case 4:{
                printAllReader(rdrt);
                break;
            }
            case 5:{
                printf("Please input the docId.\n");
                int id;
                scanf("%d",&id);
                resultptr res = searchBTree(t,id);
                if(!res->tag){
                    printf("Do not exist.\n");
                }else {
                    printDocDetail((docptr)(res->pt->record[res->i]));
                }
                break;
            }

            case 6:{
                printf("Choose document(D) or reader (R). \n");

                char ch;
                while((ch = getchar())!='D'&&ch!='d'&&ch!='R'&&ch!='r'){
                    flush();
                    printf("Please input correct letter.\n");
                }
                printf("\n");
                BTree temp = (ch=='D'||ch=='d')?t:rdrt;
                printBTree(temp);
                break;
            }
            case 7:{
                char wrtr[30];
                printf("Input the writer you wanna search.\n");
                scanf("%s",wrtr);
                printDocByWriter(tb,wrtr);
                break;
            }
            case 8:{
                int id,docid;char stname[30];
                printf("Input the studentId.\n");
                scanf("%d",&id);
                resultptr rres = searchBTree(rdrt,id);
                if(!rres->tag){
                    printf("Illegal Reader.\n");
                    break;
                }
                readerptr rdr = (readerptr)(rres->pt->record[rres->i]);
                free(rres);
                printf("Input the docId. Enter ctrl+Z to complete.\n");
                while(~scanf("%d",&docid)){
                    resultptr res = searchBTree(t,docid);
                    if(res->tag){
                        if(borrowDoc((docptr)(res->pt->record[res->i]),rdr)){
                            time_t tm = time(NULL);
                            char time[25];
                            strftime(time, 25, "%Y-%m-%d %H:%M:%S", localtime(&tm));
                            char *msg = catchString(6,time,"      Borrow ",((docptr)(res->pt->record[res->i]) )->name,"    studentId : ",int2str(id),"\n");
                            writeLog(msg);
                            free(msg);
                            printf("Borrow successfully\n");
                        }else{
                            printf("fail\n");
                        }
                    } else {
                        printf("Do not exist.\n");
                    }
                    free(res);
                    printf("Input the docId. Enter ctrl+Z to complete.\n");
                }
                save(t,rdrt);
                break;
            }
            case 9:{
                int id;
                printf("Input the studentId\n");
                scanf("%d",&id);
                printf("Input the docId. Enter ctrl+Z to complete.\n");
                int docid;
                int n;
                while(~(n = scanf("%d",&docid))){
                    if(!n){
                        flush();
                        printf("Illegal input.\n");
                        continue;
                    }
                    resultptr res = searchBTree(t,docid);
                    if(res->tag){
                        if(returnDoc((docptr)(res->pt->record[res->i]),id)){
                            printf("Return successfully\n");
                            time_t tm = time(NULL);
                            char time[25];
                            strftime(time, 25, "%Y-%m-%d %H:%M:%S", localtime(&tm));
                            char *msg = catchString(6,time,"      Return ",((docptr)(res->pt->record[res->i]) )->name,"    studentId : ",int2str(id),"\n");
                            writeLog(msg);
                            free(msg);
                        }else{
                            printf("fail\n");
                        }
                    } else {
                        printf("Do not exist.\n");
                    }
                    printf("Input the docId. Enter ctrl+Z to complete.\n");
                }
                save(t,rdrt);
                break;
            }
            case 10:{
                printf("Input the docId you wanna update.\n");
                int id,count;
                char name[50],writer[50];
                scanf("%d",&id);
                resultptr res = searchBTree(t,id);
                if(res->tag){
                    printf("name?\n");
                    scanf("%s",name);
                    printf("writer?\n");
                    scanf("%s",writer);
                    printf("total?\n");
                    scanf("%d",&count);
                    if(updateDoc(tb,(docptr)(res->pt->record[res->i]),name,writer,count)){

                        time_t tm = time(NULL);
                        char time[25],num[10];
                        itoa(id,num,10);
                        strftime(time, 25, "%Y-%m-%d %H:%M:%S", localtime(&tm));
                        char *msg = catchString(4,time,"      Update document ",num,"\n");
                        writeLog(msg);
                        free(msg);
                        printf("Update successfully.\n");
                    }
                    else
                        printf("Fail.\n");
                }else{
                    printf("Do not exist.\n");
                }
                save(t,rdrt);
                break;
            }
            case 11:{
                printf("Input the docId you wanna delete.\n");
                int id;
                scanf("%d",&id);
                resultptr res = searchBTree(t,id);
                if(res->tag){
                    char book_name[50];
                    strcpy(book_name,((docptr)(res->pt->record[res->i]) )->name );
                    removeHash(tb,(docptr)(res->pt->record[res->i]));
                    deleteBTree(&t,&(res->pt),res->i);
                    printf("Delete successfully.\n");
                    time_t tm = time(NULL);
                    char time[25];
                    strftime(time, 25, "%Y-%m-%d %H:%M:%S", localtime(&tm));
                    char *msg = catchString(4,time,"      delete Document ",book_name,"\n");
                    writeLog(msg);
                    free(msg);
                }else {
                    printf("Do not exist.\n");

                }
                save(t,rdrt);
                break;
            }
            case 12:{
                printf("Do you wanna destroy the index,which will remove all of the document?(Y/N)\n");
                char confirm;
                while((confirm = getchar())!='Y'&&confirm!='y'&&confirm!='N'&&confirm!='n'){
                    flush();
                    printf("Please enter correct letter.\n");
                }
                if(confirm=='y'||confirm=='Y'){
                    destroyBTree(t);
                    t = getBTree();
                    printf("Destroy successfully\n");
                    save(t,rdrt);
                    time_t tm = time(NULL);
                    char time[25];
                    strftime(time, 25, "%Y-%m-%d %H:%M:%S", localtime(&tm));
                    char *msg = catchString(2,time,"      Destroy all.\n");
                    writeLog(msg);
                    free(msg);
                    printf("Destroy Successfully.\n");
                    }
                break;
            }
            case 13:{
                system("cls");
                menu();
                break;
            }

        }
        fflush(stdin);
        printf("\nORDER>>>>> ");
        while(!scanf("%d",&order)){
            printf("Illegal command.\n");
            flush();
            printf("\nORDER>>>>> ");
        }
    }
}

