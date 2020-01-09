#include "header\BTree.h"
#include <stdio.h>
#include <stdlib.h>
/*


*/
int search(BTree t,KeyType k){
    int i = 1;
    while(i<=t->keynum&&t->key[i]<k)i++;
    return i;
}






resultptr searchBTree(BTree t,KeyType k){
    BTree p = t,q = NULL;
    int found = 0,i = 0;
    while (p != NULL && found == 0){
        i = search(p,k);
        if(i<=p->keynum&&p->key[i]==k)
            found = 1;
        else{
            q = p;
            p = p->ptr[i-1];
        }
    }
    if(found)
        return getResult(p,i,1);
    else
        return getResult(q,i,0);
}





void split(BTree t,int s,BTree *ap,void *rec){
    int i,j,n = t->keynum;
    (*ap) = getBTree();
    (*ap)->ptr[0] = t->ptr[s];
    for(i = s+1,j = 1;i <= n;i++,j++){
        (*ap)->key[j] = t->key[i];
        (*ap)->record[j] = t->record[i];
        (*ap)->ptr[j] = t->ptr[i];
    }
    (*ap)->parent = t->parent;
    (*ap)->keynum = n-s;
    for(i = 0;i<=n-s;i++){
        if((*ap)->ptr[i])
            (*ap)->ptr[i]->parent = *ap;
    }
    t->keynum = s-1;
}

void newRoot(BTree *p,BTree left,KeyType x,BTree right,void *rec){
    (*p) = getBTree();
    (*p)->keynum = 1;
    (*p)->key[1] = x;
    (*p)->record[1] = rec;
    (*p)->ptr[0] = left,(*p)->ptr[1] = right;
    if(left)
        left->parent = *p;
    if(right)
        right->parent = *p;
    (*p)->parent = NULL;
}

void insert(BTree t,int i,KeyType x,BTree p,void *rec){
    int j,n = t->keynum;
    for(j = n;j >= i;j--){
        t->ptr[j+1] = t->ptr[j];
        t->key[j+1] = t->key[j];
        t->record[j+1] = t->record[j];
    }
    t->ptr[i] = p;
    t->key[i] = x;
    t->record[i] = rec;
    if(p)
        p->parent = t;
    t->keynum++;
}

Status insertBTree(BTree *t,KeyType k,BTree q,int i,void *rec){
    int s,finished = 0,need = 0;
    KeyType x;
    BTree ap;
    void *recd;
    if(q==NULL)
        newRoot(t,NULL,k,NULL,rec);
    else{
        x = k;ap = NULL;
        recd = rec;
        while(need==0&&finished==0){
            insert(q,i,x,ap,recd);
            if(q->keynum<M)
                finished = 1;
            else{
                s = (M+1)/2;
                split(q,s,&ap,recd);
                x = q->key[s];
                recd = q->record[s];
                if(q->parent){
                    q = q->parent;
                    i = search(q,x);
                }else{
                    need = 1;
                }
            }
        }
        if(need)
            newRoot(t,q,x,ap,recd);
    }
    return SUCCESS;
}
void print(BTree t,int depth,array last){
    int i,j;
    if(t==NULL)
        return;
   /* if(t->ptr[1]==NULL&&t->key[1]==-1){
        return;
    }*/
    printf("|\n");
    for(i = 0;i<=t->keynum;i++){

        if(i!=0&&(t->key[i]!=-1)){
             for(j = 0;j<depth-1;j++){
                // if(i!=1){
                    if(!(last->thelast[j]))
                        printf("|");
                    else
                        printf(" ");
                    printf("\t");
                // }
            }
           printf("|->");
           printf("%d\n",t->key[i]);
        }
        if((t->ptr[i])!=NULL){
                for(j = 0;j<depth-1;j++){
                    //if(i!=1){
                        if(!(last->thelast[j]))
                            printf("|");
                        else
                            printf(" ");
                        printf("\t");
                    //}
                }
           printf("|------>");
           if(i==t->keynum)
                append(last,1);
           else
                append(last,0);
           print(t->ptr[i],depth+1,last);
           pop(last);
        }
    }
}


void successor(BTree *t,int i){
    BTree temp = (*t)->ptr[i];
    while(temp->ptr[0]){
        temp = temp->ptr[0];
    }
    (*t)->key[i] = temp->key[1];//printf("%d\n",(*t)->key[i]);
    (*t)->record[i] = temp->record[i];
    (*t) = temp;
   // printBTree(temp);
}

void removeKey(BTree t,int i){
    while(i<t->keynum){
        t->ptr[i] = t->ptr[i+1];
        t->key[i] = t->key[i+1];
        t->record[i] = t->record[i+1];
        i++;
    }
    free(t->ptr[i]);
    t->keynum--;
}

void restore(BTree *root,BTree *t,int i){
    BTree par = (*t)->parent;
    if(par==NULL){
        if((*t)->keynum==0){
            free(*t);
            (*root) = (*t)->ptr[0];
            (*root)->parent = NULL;
        }
        return;
    }
    int j = 0;
    while(par->ptr[j]!=(*t))j++;
    if(j!=0&&par->ptr[j-1]->keynum>(M-1)/2){
        BTree sib = par->ptr[j-1];
        (*t)->keynum++;
        int n = (*t)->keynum;
        int k;
        for(k = n;k>1;k--){
            (*t)->key[k] = (*t)->key[k-1];
            (*t)->record[k] = (*t)->record[k-1];
            (*t)->ptr[k] = (*t)->ptr[k-1];
        }
        (*t)->ptr[1] = (*t)->ptr[0];
        (*t)->ptr[0] = sib->ptr[sib->keynum];
        (*t)->key[1] = par->key[j];
        (*t)->record[1] = par->record[j];
        par->key[j] = sib->key[sib->keynum];
        par->record[j] = sib->record[sib->keynum];
        removeKey(sib,sib->keynum);
    }else if(j!=M&&par->ptr[j+1]&&par->ptr[j+1]->keynum>(M-1)/2){
        BTree sib = par->ptr[j+1];
        (*t)->keynum++;
        (*t)->key[(*t)->keynum] = par->key[j+1];
        (*t)->record[(*t)->keynum] = par->record[j+1];
        (*t)->ptr[(*t)->keynum] = sib->ptr[0];
        par->key[j+1] = sib->key[0];
        par->record[j+1] = sib->record[0];
        removeKey(sib,0);
    }else{
        if(j!=0){
            BTree sib = par->ptr[j-1];
            ++sib->keynum;
            sib->key[sib->keynum] = par->key[j];
            sib->record[sib->keynum] = par->record[j];
            sib->ptr[sib->keynum] = (*t)->ptr[0];
            par->keynum--;
            int k;
            for(k = j;k<=par->keynum;k++){
                par->key[k] = par->key[k+1];
                par->record[k] = par->record[k+1];
                par->ptr[k] = par->ptr[k+1];
            }
            for(k = 1;k<=(*t)->keynum;k++){
                sib->keynum++;
                sib->key[sib->keynum] = (*t)->key[k];
                sib->record[sib->keynum] = (*t)->record[k];
                sib->ptr[sib->keynum] = (*t)->ptr[k];//5 1 5 78 15 21 02 32 65 98 4 45 1 54 87 65 15 54 87 65 1  65 145 15 54 8 6515 154
            }
            free(*t);
            (*t) = NULL;
        }else if(j!=M){
            BTree sib = par->ptr[j+1];
            int count = (*t)->keynum+1;
            par->keynum--;
            sib->keynum += count;
            int k;
            for(k = sib->keynum;k>count;k--){
                sib->key[k] = sib->key[k-count];
                sib->record[k] = sib->record[k-count];
                sib->ptr[k] = sib->ptr[k-count];
            }
            sib->ptr[count] = sib->ptr[0];
            sib->key[count] = par->key[j+1];
            sib->record[count] = par->record[j+1];
            par->ptr[j] = par->ptr[j+1];
            for(k = j+1;k<=par->keynum;k++){
                par->key[k] = par->key[k+1];
                par->record[k] = par->record[k+1];
                par->ptr[k] = par->ptr[k+1];
            }
            for(k = 1;k<=(*t)->keynum;k++){
                sib->key[k] = (*t)->key[k];
                sib->record[k] = (*t)->record[k];
                sib->ptr[k] = (*t)->ptr[k];
            }
            sib->ptr[0] = (*t)->ptr[0];
            free(*t);
            (*t) = NULL;
        }
        if(par->keynum<(M-1)/2)
            restore(root,&par,j);
    }
}

void deleteBTree(BTree *root,BTree *t,int i){
    if((*t)->ptr[i]){
        successor(t,i);
        deleteBTree(root,t,1);
    } else {
        removeKey((*t),i);
        if((*t)->keynum<(M-1)/2)
            restore(root,t,i);
    }
}

void printBTree(BTree t){
    if (t==NULL)
        return;
    array last = getArray();
    print(t,1,last);
}

Status destroyBTree(BTree t){
    if(t==NULL)
        return ERROR;
    int i = 0;
    for(i = 0;i<=t->keynum;i++){
        if(t->ptr[i]){
            destroyBTree(t->ptr[i]);
        }
    }
    free(t);
    return SUCCESS;
}




BTree getBTree(){
    BTree t = (BTree)malloc(sizeof(BTNode));
    t->parent = NULL;
    t->keynum = 0;
    for(int i = 0;i<=M;i++)
        t->ptr[i] = NULL,t->key[i] = -1;
    return t;
}



resultptr getResult(BTree t,int index,int tag){
    resultptr res = (result*)malloc(sizeof(result));
    res->pt = t;
    res->i = index;
    res->tag = tag;
    return res;
}





array append(array a,int last){
    if(a->n==a->max)
        a->thelast = (int *)realloc(a->thelast,sizeof(int)*((a->max)+10));
    a->max += 10;
    a->thelast[(a->n)++] = last;
    return a;
}




array pop(array a){
    if(a==NULL||a->n==0)
        return a;
    a->n -= 1;
    return a;
}





array getArray(){
    array a = (array)malloc(sizeof(struct arr));
    a->max = 10;
    a->n = 0;
    a->thelast = (int *)malloc(sizeof(int)*(a->max));
    return a;
}
