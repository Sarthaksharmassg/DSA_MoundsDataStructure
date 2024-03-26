#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>

// Set Threshold as per requirement
#define Threshold 10
#define TMAX __INT_MAX__

typedef struct ListNode* LNode;
typedef struct ListNode{
    int value;
    LNode next;
} ListNode;

typedef struct MoundNode{
    LNode list;
    bool dirty;
    int c;
} MoundNode;
typedef MoundNode* MNode;

// Global variables
int tree_size;
int tree_level;
FILE *fp;

// Global Dynamic Array
MNode tree;

// Calculates level of an index
int calculateLevel(int index){
    double res = (double)index + 2;
    double ans = ceil(log2(res));
    return (int)ans ;
}

// Calculates parentIndex of an index
int parentIndex(int index){
    return (index-1)/2 ;
}

// Returns TMAX if list is NULL, else head value
int val(MoundNode node){
    if(node.list==NULL){
        return TMAX;
    }
    else{
        return node.list->value;
    }
}

// Inserts ListNode at head of a MoundNode
void insertAtHead(MNode node, LNode ln){
    if(node->list==NULL){
        node->list=ln;
    }
    else{
        ln->next = node->list;
        node->list = ln;
    }
    (node->c)++;
    return ;
}

// Calculates index of ancestor at count level above
int ancestorIndex(int index, int count){
    int new_index = index;
    while(count--){
        new_index = parentIndex(new_index) ;
    }
    return new_index;
}

// Adds a level to the tree when called
void addLevel(){
    tree = realloc(tree, sizeof(MoundNode)*(tree_size*2 + 1));
    for(int i=0;i<tree_size+1;i++){
        tree[tree_size+i].c = 0;
        tree[tree_size+i].dirty = false;
        tree[tree_size+i].list = NULL;
    }
    tree_level++;
    tree_size = tree_size * 2 + 1;
    return;
}

// Finds suitable ancestor index to insert the value
// Uses Binary search b/w levels of low_index and index
int findAncestor(int v, int low_index, int index){
    if(low_index == index){
        return index;
    }
    int parent_index = parentIndex(index) ;
    if(v > val(tree[parent_index])){
        return index;
    }

    // Calculating level of indices to apply Binary search on
    int low_level = calculateLevel(low_index);
    int high_level = calculateLevel(index);

    int mid_level = (low_level + high_level)/2 ;
    int mid_index = ancestorIndex(index, high_level-mid_level);
    // Finds ancestor index at level above an index

    if(v <= val(tree[mid_index])){
        return findAncestor(v,0,mid_index);
    }
    else{
        return findAncestor(v,mid_index+1,index);
    } 
}

void insert(int v){
    int threshcount = 0;

    // randomly select index for insert
    int random = rand();
    // index will be a random no b/w 0 and tree.size-1
    int index = random % tree_size ;

    while(true){
        // check if index i is suitable or not
        if(v <= val(tree[index])){
            index = findAncestor(v,0,index);
            LNode ln = malloc(sizeof(ListNode));
            ln->value = v;
            ln->next = NULL;
            insertAtHead(tree+index, ln);
            break;
        }
        else{
            threshcount++;
            // Check threshcount with Threshold
            if(threshcount>=Threshold){
                // threshold crossed, add a Level
                addLevel();
                break;
            }
            else{
                // try another random index
                index = rand() % tree_size ;
            }
        }
    }
    if(threshcount>=Threshold){
        // Threshold was crossed and new level added, so try insert now
        insert(v);
    }
    else return ;
}

// returns index of left child
int leftindex(int index){
    return (2*index+1);
}

// returns index of right child
int rightindex(int index){
    return (2*index + 2);
}

// restores Mound property
void moundify(int index){
    // Checking if leftchild is dirty
    if (leftindex(index) < tree_size && tree[leftindex(index)].dirty)
    {
        moundify(leftindex(index));
    }
    // Checking if rightchild is dirty
    if (rightindex(index) < tree_size && tree[rightindex(index)].dirty)
    {
        moundify(rightindex(index));
    }

    int min_index = index;
    //checking if the leftindex exists and then comparing the value with value at min_index
    if(leftindex(index)<tree_size && val(tree[min_index])>val(tree[leftindex(index)])){
        min_index = leftindex(index);
    }
    //checking if the rightindex exists and then comparing the value with values at min_index
    if(rightindex(index)<tree_size && val(tree[min_index])>val(tree[rightindex(index)])){
        min_index = rightindex(index);
    }
    // if Mound property still holds, set dirty to false
    if(min_index == index){
        tree[index].dirty = false;
        return ;
    }
    else{
        // Swapping of MoundNodes
        MoundNode temp = (tree[index]);
        tree[index] = tree[min_index];
        tree[min_index] = temp;

        // Set dirty to false for index, true for min_child
        tree[index].dirty = false;
        tree[min_index].dirty = true;
        // Moundify child index that was swapped
        moundify(min_index);
    }
    return;
}

//Returns the smallest value of the MoundTree
int extractmin(int index){
    if(tree[index].list==NULL){
        return TMAX;
    }
    // Removing the head node at tree[index]
    LNode temp = tree[index].list;
    tree[index].list = temp->next;
    int t = temp->value;

    // Free the node
    free(temp);
    // Set dirty bit to true
    tree[index].dirty = true;
    // Call Moundify at index
    moundify(index);
    return t; 
}

// prints MoundNodes each in a line in order of array index
// prints Each MoundNode by traversing the ListNodes
void printMounds(){
    for (int i = 0; i < tree_size; i++)
    {
        if (val(tree[i]) == TMAX)
        {
            printf("Node is Empty => TMAX\n");
        }
        else
        {
            LNode node = tree[i].list;
            printf("Node values => ");
            while (node->next != NULL)
            {
                printf("%d -> ", node->value);
                node = node->next;
            }
            if (node->next == NULL)
            {
                printf("%d ", node->value);
                node = node->next;
            }
            printf("\n");
        }
    }
    return ;
}

// Calls extractMin() repeatedly until mound is empty, prints each value
void removeMin()
{
    while (true)
    {
        int emin = extractmin(0);
        if (emin == TMAX)
        {
            break;
        }
        printf("%d\n", emin);
    }
    return ;
}

void main(int argc, char* argv[]){

    // initialise first level of Tree
    tree = (MNode)malloc(sizeof(MoundNode));
    tree->c = 0;
    tree->dirty = false;
    tree->list = NULL;
    tree_level++;
    tree_size++;

    // Read file and insert values
    fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        printf("File could not be opened\n");
        return;
    }

    // needed to generate different random no in each execution
    srand(time(NULL));

    int num;
    while (fscanf(fp, "%d", &num) != EOF)
    {
        // fscanf reads and stores the number into num
        insert(num);
    }

    // Close the file
    fclose(fp);

    // Print values to check correctness, uncomment below four lines

    // printf("%d is treesize\n", tree_size);
    // printf("Printing values\n\n");
    // printMounds();
    // printf("\nAll values printed succesfully\n");

    printf("\nExtracting values :\n");

    // removeMin calls extractMin() repeatedly and prints each value
    removeMin();

    printf("\nAll values extracted succesfully\n");

    // Freeing the memory associated with tree
    free(tree);

    return;
}
