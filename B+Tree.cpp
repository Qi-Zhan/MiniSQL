#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define M 3
typedef struct treeNode* TreeNode;
struct treeNode {
    int keynum;       //索引/真实值数
    int array[M+1]; //索引/真实
    TreeNode child[M+2];
    int depth;//判断是不是叶子结点
};

int IsLeaf(TreeNode X) {
    for (int i = 0; i <= X->keynum; i++) {
        if (X->child[i] != NULL) return 0;
    }
    return 1;
}

int IsRoot(TreeNode X) {
    if(X->depth==0)//深度为0的是根
        return 1;
    return 0;
}

TreeNode CreateNode() {
    TreeNode X = (TreeNode)malloc(sizeof(struct treeNode));
    X->keynum = 0;
    for (int i = 0; i <= M; i++) {
        X->array[i] = 0;
        X->child[i] = NULL;
    }
    X->child[M+1] = NULL;
    X->depth = 0;
    return X;
}

void UpdateDepth(TreeNode X){
    for (int i = 0; i <= X->keynum; i++)
    {
        if(X->child[i]!=NULL){
            X->child[i]->depth = X->depth + 1;
            UpdateDepth(X->child[i]);
        }
    }
}

void UpdateKey(TreeNode X){
    for (int i = 0; i < X->keynum; i++)
    {
        if(X->child[i+1]!=NULL && IsLeaf(X->child[i+1]))X->array[i] = X->child[i + 1]->array[0];
    }
}

TreeNode SplitRoot(TreeNode X,int flag){
    //printf("SplitRoot\n");
    TreeNode NewRoot = CreateNode();
    if(flag==0){
        int M1 = ceil((M + 1) / 2.0);
        int M2 = floor((M + 1) / 2.0);
        TreeNode Y = CreateNode();
        Y->keynum = M2;
        for (int i = 0; i < M2; i++) {
            Y->array[i] = X->array[i + M1];
            X->array[i + M1] = 0;
        }
        X->keynum = M1;
        NewRoot->keynum = 1;
        NewRoot->child[0] = X;
        NewRoot->child[1] = Y;
        UpdateKey(NewRoot);
    }else{
        int middle = ceil((M+1) / 2);//2
        TreeNode Y = CreateNode();
        int M1 = ceil((M - 1) / 2.0);//因为要拿出来一个作为NewRoot
        int M2 = floor((M - 1) / 2.0);
        Y->keynum = M2;
        X->keynum = M1;
        for (int i = middle; i < M+1; i++)
        {
            Y->child[i - middle] = X->child[i];
        }
        for (int i = middle; i < M; i++)
        {
            Y->array[i - middle] = X->array[i];
        }
        NewRoot->keynum = 1;
        NewRoot->child[0] = X;
        NewRoot->child[1] = Y;
        NewRoot->array[0] = X->array[middle-1];
        
    }
    UpdateDepth(NewRoot);
    return NewRoot;
}

void SplitNode(TreeNode X, int order, int flag) {
    //printf("SplitNode\n");
    //（分裂X的第i个孩子）+更新键值
    //叶子分裂0，非叶子分裂1
    TreeNode Y = CreateNode();
    if (flag == 0) {
        int M1 = ceil((M + 1) / 2.0);
        int M2 = floor((M + 1) / 2.0);
        Y->keynum = M2;
        for (int i = 0; i < M2; i++) {
            Y->array[i] = X->child[order]->array[i + M1];
            X->child[order]->array[i + M1] = 0;
        }
        X->child[order]->keynum = M1;
        for (int i = X->keynum; i > order; i--) {
            X->child[i + 1] = X->child[i];
        }
        X->child[order + 1] = Y;
        X->keynum++;
        UpdateKey(X);
    } else {
        int middle = ceil((M+1) / 2);//2
        int M1 = ceil((M - 1) / 2.0);
        int M2 = floor((M - 1) / 2.0);
        Y->keynum = M2;
        for (int i = 0; i < M2; i++)
        {
            Y->array[i] = X->child[order]->array[i + M1 + 1];
            Y->child[i] = X->child[order]->child[i + M1 + 1];
        }
        Y->child[M2] = X->child[order]->child[M];
        X->child[order]->keynum = M1;
        for (int i = X->keynum; i >= order; i--)
        {
            X->child[i + 1] = X->child[i];
        }
        X->child[order + 1] = Y;
        for (int i = X->keynum-1; i >= order; i--)
        {
            X->array[i + 1] = X->array[i];
        }
        X->keynum++;
        X->array[order] = X->child[order]->array[middle - 1];
    }
}

void prtarray(TreeNode X) { 
    printf("[");
    printf("%d", X->array[0]);
    for (int i = 1; i < X->keynum; i++){
        printf(",%d", X->array[i]);
    }
    printf("]");
}

void print_line(TreeNode X) {
    //利用depth和队列层序输出
    TreeNode queue[20000];
    int front = 0, rear = 0;
    queue[0] = X;
    int nowdepth = X->depth;
    while (front<=rear)
    {
        for (int i = 0; i <= queue[front]->keynum; i++)
        {
            if(queue[front]->child[i]!=NULL){
                queue[++rear] = queue[front]->child[i];
            }
        }
        if(queue[front]->depth > nowdepth){
                printf("\n");
                nowdepth = queue[front]->depth;
        }
        prtarray(queue[front]);
        front++;
    }
}

TreeNode InsertNode(int x, TreeNode Root) {
    if (Root == NULL) {
        Root = CreateNode();
        Root->array[0] = x;
        Root->keynum++;
    } else if (!IsLeaf(Root)) { //不是叶子结点
        int i = 0;
        for (i = 0; i <= Root->keynum; i++) {
            if (i < Root->keynum && x < Root->array[i]){
                Root->child[i] = InsertNode(x, Root->child[i]);
                Root->child[i]->depth = Root->depth + 1;
                
                break;
            }
            if(i==Root->keynum){
                Root->child[i] = InsertNode(x, Root->child[i]);
                Root->child[i]->depth = Root->depth + 1;
            }
        }
        if(i==Root->keynum+1)
            i--;
        if (IsLeaf(Root->child[i]) &&
            Root->child[i]->keynum == M + 1) { //叶子分裂
            SplitNode(Root, i, 0);

        } else if (!IsLeaf(Root->child[i]) &&
            Root->child[i]->keynum == M) { //非叶子分裂
            SplitNode(Root, i, 1);
        } else {
            if(IsLeaf(Root->child[i]))
                UpdateKey(Root); //更新倒数第二排键值
        }
    } else { //是叶子结点
        int flag = 1;
        int i = 0;
        
        for (i = Root->keynum - 1; i >=0; i--)
        {
            if(Root->array[i]==x){
                printf("Key %d is duplicated\n", x);
                flag = 0;
                break;
            }
        }
        if(flag==1){
            for (i = Root->keynum - 1; i >= 0; i--) {
                if (Root->array[i] < x) { //找到插入位置
                    break;
                } else {
                    Root->array[i + 1] = Root->array[i];
                }
            }
            Root->keynum++;
            Root->array[i + 1] = x;
            
        }
    }
    if(IsRoot(Root)){//单独判断根
        if(IsLeaf(Root)&&Root->keynum==M+1)
            Root = SplitRoot(Root, 0);
        else if(!IsLeaf(Root)&&Root->keynum==M)
            Root = SplitRoot(Root, 1);
    }
    return Root;
}

int main() {
    int N, x;
    scanf("%d", &N);
    TreeNode root = NULL;
    for (int i = 0; i < N; i++) {
        scanf("%d", &x);
        root = InsertNode(x, root);
    }
    print_line(root);
    return 0;
}