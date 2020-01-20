#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FatalError( Str )   fprintf( stderr, "%s\n", Str ), exit( 1 )
#define MaxTrees (12)
#define Error(Str) FatalError(Str)
#define Infinity (6000000000L)
#define Capacity (4095)

/*
The purpose of this code is 
to examine the effect of quanta on awt using binomial heap.
*/

typedef double ElementType;

typedef struct BinNode{
    ElementType Item;
    int left;
    struct BinNode *LeftChild;
    struct BinNode *NextSibling;
}BinNode;

typedef struct Collection{
    int CurrentSize;
    struct BinNode *TheTrees[ MaxTrees ];
}Collection;

typedef struct list{
    char which[10];
    int time;
    int much;
    struct list *next;
}list;


struct Collection *Initialize();
struct Collection *Insert(ElementType Item,int much, struct Collection *H);
ElementType DeleteMin(struct Collection *H);
struct Collection *Merge( struct Collection *H1, struct Collection *H2 );
int FindMin( struct Collection *H );
int IsEmpty( struct Collection *H );
struct list* insertLast(struct list *head,struct list *add);


int main(){
    char *filename;
    filename = "input.txt";
    FILE *fp=fopen(filename, "r");
    struct list *head=NULL;
    int line = 0, emax = 0, total = 0;
    int num=0;

    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    }
    //This loop reads the input file and adds the contents to the linked list.
    char str[5];
    while(fscanf(fp, "%s", str) != EOF){
        struct list *newNode=(list*)malloc(sizeof(list));
        newNode->next=NULL;
        int control;
        for(control = 0; control<3; control++){
            if(control==0){
                strcpy(newNode->which,str);
            }else if(control==1){
                if(fscanf(fp, "%s", str) != EOF){
                    newNode->much=atoi(str);
                    total+=newNode->much;
                    if(emax<newNode->much){
                        emax=newNode->much;
                    }
                }

            }else if(control==2){
                if(fscanf(fp, "%s", str) != EOF){
                    newNode->time=atoi(str);
                }

            }
        }
        num++;
        head=insertLast(head,newNode);
        line++;
    }
    
    int i,q,j;
    printf("%-7s%-7s\n","q","AWT");
    //q is quanta
    for(q=1;q<=10;q++){
        struct Collection *H1;
        H1 = Initialize( );
        struct Collection *H2;
        H2 = Initialize( );
        int waitingTime=0;
        list *iter=head;
        int count=0;
		//total time of code implementation
        for(i=0;i<total;){
            while(iter->time<=i && count<num){
                count++;
                Insert(iter->much,(int)iter->much,H1);
                if(iter->next!=NULL){
                    iter=iter->next;
                }
            }
            //deleting from H2 and adding to H1
            while(!IsEmpty(H2)){
                int num=DeleteMin(H2);
                Insert(num,num,H1);
            }
            //the min size node of heap shoul decrease as quanta value
            for(j=1;j<=q;j++){
                if(!IsEmpty(H1)){
                    int remain=FindMin(H1)-1;
                    double item =remain;
                    DeleteMin(H1);
                    waitingTime+=(H1->CurrentSize+H2->CurrentSize);
                    i++;
                    //adding nodes to h2 heap whichs are waiting
                    while(iter->time<=i && count<num){
                        count++;
                        Insert(iter->much,(int)iter->much,H2);
                        if(iter->next!=NULL){
                            iter=iter->next;
                        }
                    }
                    //if it is not finished yet i have to add it again with new value
                    if(item!=0){
                        item = (1.0/(exp((-1.0)*pow((item*2)/(3.0*emax),3))))*item ;
                        Insert(item,remain,H1);
                    }else{
                    	break;
					}
                    
                    
                }else{
                    break;
                }
            }
        }
        printf("%-7d%-7.2f\n",q,1.0*waitingTime/num);
    }
    return 0;
}

//enqueue implementation
struct list* insertLast(struct list *head,struct list *add){
    if(head==NULL){
        head=add;
    }else{
        struct list* iter=head;
        while(iter->next!=NULL){
            iter=iter->next;
        }
        iter->next=add;
    }
    return head;
}

struct Collection *Initialize(){
    struct Collection *H=(Collection*)malloc(sizeof(struct Collection));
    int i;

    if(H == NULL)
        FatalError("Out of space!!!");

    H->CurrentSize = 0;
    for( i = 0; i<MaxTrees; i++)
        H->TheTrees[i] = NULL;

    return H;
}

static void DestroyTree(BinNode *T){
    if(T!=NULL){
        DestroyTree(T->LeftChild);
        DestroyTree(T->NextSibling);
        free( T );
    }
}

struct Collection *Insert(ElementType Item,int much, struct Collection *H){
    struct BinNode *NewNode;
    struct Collection *OneItem;

    NewNode = (BinNode *)malloc(sizeof(struct BinNode));
    if( NewNode == NULL )
        FatalError( "Out of space!!!" );
    NewNode->LeftChild = NewNode->NextSibling = NULL;
    NewNode->Item = Item;
    NewNode->left=much;
    OneItem = Initialize( );
    OneItem->CurrentSize = 1;
    OneItem->TheTrees[ 0 ] = NewNode;

    return Merge(H,OneItem);
}

ElementType DeleteMin(struct Collection *H){
    int i, j;
    int MinTree;
    struct Collection *DeletedQueue;
    struct BinNode *DeletedTree;
    struct BinNode *OldRoot;
    ElementType MinItem;


    MinItem = Infinity;
    for( i = 0; i < MaxTrees; i++){
        if( H->TheTrees[i] && H->TheTrees[ i ]->Item<MinItem){
            MinItem = H->TheTrees[ i ]->Item;
            MinTree = i;
        }
    }

    DeletedTree = H->TheTrees[ MinTree ];
    OldRoot = DeletedTree;
    DeletedTree = DeletedTree->LeftChild;
    free( OldRoot );

    DeletedQueue = Initialize( );
    DeletedQueue->CurrentSize = ( 1 << MinTree ) - 1;

    for( j = MinTree - 1; j >= 0; j-- ){
        DeletedQueue->TheTrees[ j ] = DeletedTree;
        DeletedTree = DeletedTree->NextSibling;
        DeletedQueue->TheTrees[ j ]->NextSibling = NULL;
    }

    H->TheTrees[ MinTree ] = NULL;
    H->CurrentSize -= DeletedQueue->CurrentSize + 1;

    Merge( H, DeletedQueue );
    return MinItem;
}

int FindMin(struct Collection *H){
    int i;
    ElementType MinItem;

    if(IsEmpty(H)){
        Error("Empty binomial queue");
    }

    MinItem = Infinity;
    ElementType minValue = Infinity;

    for(i=0; i<MaxTrees; i++){
        if( H->TheTrees[ i ] && H->TheTrees[ i ]->Item < MinItem ){
            MinItem = H->TheTrees[ i ]->Item;
            minValue=H->TheTrees[ i ]->left;
        }
    }

    return (int)minValue;
}

int IsEmpty(struct Collection *H){
    return H->CurrentSize == 0;
}

struct BinNode *CombineTrees(struct BinNode *T1, struct BinNode *T2){
    if( T1->Item > T2->Item )
        return CombineTrees( T2, T1 );
    T2->NextSibling = T1->LeftChild;
    T1->LeftChild = T2;
    return T1;
}

struct Collection *Merge(struct Collection *H1, struct Collection *H2){
    struct BinNode *T1;
    struct BinNode *T2;
    struct BinNode *Carry = NULL;
    int i, j;

    if( H1->CurrentSize + H2->CurrentSize > Capacity )
        Error( "Merge would exceed capacity" );

    H1->CurrentSize += H2->CurrentSize;
    for( i = 0, j = 1; j <= H1->CurrentSize; i++, j *= 2 ){
        T1 = H1->TheTrees[ i ]; T2 = H2->TheTrees[ i ];

        switch( !!T1 + 2 * !!T2 + 4 * !!Carry ){
            case 0:
            case 1:
                break;
            case 2:
                H1->TheTrees[ i ] = T2;
                H2->TheTrees[ i ] = NULL;
                break;
            case 4:
                H1->TheTrees[ i ] = Carry;
                Carry = NULL;
                break;
            case 3: /* H1 and H2 */
                Carry = CombineTrees( T1, T2 );
                H1->TheTrees[ i ] = H2->TheTrees[ i ] = NULL;
                break;
            case 5: /* H1 and Carry */
                Carry = CombineTrees( T1, Carry );
                H1->TheTrees[ i ] = NULL;
                break;
            case 6: /* H2 and Carry */
                Carry = CombineTrees( T2, Carry );
                H2->TheTrees[ i ] = NULL;
                break;
            case 7: /* All three */
                H1->TheTrees[ i ] = Carry;
                Carry = CombineTrees( T1, T2 );
                H2->TheTrees[ i ] = NULL;
                break;
        }
    }
    return H1;
}
