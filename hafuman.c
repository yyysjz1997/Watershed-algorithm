// #include <stdio.h>

// #define MAXBIT      100
// #define MAXVALUE  10000
// #define MAXLEAF     30
// #define MAXNODE    MAXLEAF*2 -1

// typedef struct
// {
//     int bit[MAXBIT];
//     int start;
// } HCodeType;        /* 编码结构体 */

// typedef struct
// {
//     int weight;
//     int parent;
//     int lchild;
//     int rchild;
// } HNodeType;        /* 结点结构体 */

// /* 构造一颗哈夫曼树 */
// void HuffmanTree (HNodeType HuffNode[MAXNODE],  int n)
// {
//     /* i、j： 循环变量，m1、m2：构造哈夫曼树不同过程中两个最小权值结点的权值，
//         x1、x2：构造哈夫曼树不同过程中两个最小权值结点在数组中的序号。*/
//     int i, j, m1, m2, x1, x2;
//     /* 初始化存放哈夫曼树数组 HuffNode[] 中的结点 */
//     for (i=0; i<2*n-1; i++)
//     {
//         HuffNode[i].weight = 0;
//         HuffNode[i].parent =-1;
//         HuffNode[i].lchild =-1;
//         HuffNode[i].lchild =-1;
//     } /* end for */

//     /* 输入 n 个叶子结点的权值 */
//     for (i=0; i<n; i++)
//     {
//         printf ("Please input weight of leaf node %d: \n", i);
//         scanf ("%d", &HuffNode[i].weight);
//     } /* end for */

//     /* 循环构造 Huffman 树 */
//     for (i=0; i<n-1; i++)
//     {
//         m1=m2=MAXVALUE;     /* m1、m2中存放两个无父结点且结点权值最小的两个结点 */
//         x1=x2=0;
//         /* 找出所有结点中权值最小、无父结点的两个结点，并合并之为一颗二叉树 */
//         for (j=0; j<n+i; j++)
//         {
//             if (HuffNode[j].weight < m1 && HuffNode[j].parent==-1)
//             {
//                 m2=m1;
//                 x2=x1;
//                 m1=HuffNode[j].weight;
//                 x1=j;
//             }
//             else if (HuffNode[j].weight < m2 && HuffNode[j].parent==-1)
//             {
//                 m2=HuffNode[j].weight;
//                 x2=j;
//             }
//         } /* end for */
//             /* 设置找到的两个子结点 x1、x2 的父结点信息 */
//         HuffNode[x1].parent  = n+i;
//         HuffNode[x2].parent  = n+i;
//         HuffNode[n+i].weight = HuffNode[x1].weight + HuffNode[x2].weight;
//         HuffNode[n+i].lchild = x1;
//         HuffNode[n+i].rchild = x2;

//         printf ("x1.weight and x2.weight in round %d: %d, %d\n", i+1, HuffNode[x1].weight, HuffNode[x2].weight);  /* 用于测试 */
//         printf ("\n");
//     } /* end for */
// } /* end HuffmanTree */

// int main(void)
// {
//     HNodeType HuffNode[MAXNODE];            /* 定义一个结点结构体数组 */
//     HCodeType HuffCode[MAXLEAF],  cd;       /* 定义一个编码结构体数组， 同时定义一个临时变量来存放求解编码时的信息 */
//     int i, j, c, p, n;
//     printf ("Please input n:\n");
//     scanf ("%d", &n);
//     HuffmanTree (HuffNode, n);

//     for (i=0; i < n; i++)
//     {
//         cd.start = n-1;
//         c = i;
//         p = HuffNode[c].parent;
//         while (p != -1)   /* 父结点存在 */
//         {
//             if (HuffNode[p].lchild == c)
//                 cd.bit[cd.start] = 0;
//             else
//                 cd.bit[cd.start] = 1;
//             cd.start--;        /* 求编码的低一位 */
//             c=p;
//             p=HuffNode[c].parent;    /* 设置下一循环条件 */
//         } /* end while */

//         /* 保存求出的每个叶结点的哈夫曼编码和编码的起始位 */
//         for (j=cd.start+1; j<n; j++)
//         { HuffCode[i].bit[j] = cd.bit[j];}
//         HuffCode[i].start = cd.start;
//     } /* end for */

//     /* 输出已保存好的所有存在编码的哈夫曼编码 */
//     for (i=0; i<n; i++)
//     {
//         printf ("%d 's Huffman code is: ", i);
//         for (j=HuffCode[i].start+1; j < n; j++)
//         {
//             printf ("%d", HuffCode[i].bit[j]);
//         }
//         printf ("\n");
//     }
//     getch();
//     return 0;
// }

#include<stdio.h>
#define n 5  //叶子数目
#define m (2*n-1)    //结点总数
#define maxval 10000.0
#define maxsize 100   //哈夫曼编码的最大位数
typedef struct
{
 char ch;
 float weight;
 int lchild,rchild,parent;
}hufmtree;

typedef struct
{
 char bits[n];   //位串
 int start;      //编码在位串中的起始位置
 char ch;        //字符
}codetype;
void huffman(hufmtree tree[]);//建立哈夫曼树
void huffmancode(codetype code[],hufmtree tree[]);//根据哈夫曼树求出哈夫曼编码
void decode(hufmtree tree[]);//依次读入电文，根据哈夫曼树译码
void main()
{
  hufmtree tree[m];
 codetype code[n];
 int i,j;//循环变量
 //printf("                            ——哈夫曼编码——\n");
 printf("all numbers is\n",n);


 huffman(tree);//建立哈夫曼树
 huffmancode(code,tree);//根据哈夫曼树求出哈夫曼编码
 printf("output all numbers huff number\n");
 for(i=0;i<n;i++)
 {
  printf("%c: ",code[i].ch);
  for(j=code[i].start;j<n;j++)
   printf("%c ",code[i].bits[j]);
  printf("\n");
 }
 printf("read it and finishthe number that you want is :\n");
 decode(tree);//依次读入电文，根据哈夫曼树译码
}
void huffman(hufmtree tree[])//建立哈夫曼树
{
 int i,j,p1,p2;//p1,p2分别记住每次合并时权值最小和次小的两个根结点的下标
 float small1,small2,f;
 char c;
 for(i=0;i<m;i++)    //初始化
 {
  tree[i].parent=0;
  tree[i].lchild=-1;
  tree[i].rchild=-1;
  tree[i].weight=0.0;
 }
 //printf("【依次读入前%d个结点的字符及权值(中间用空格隔开)】\n",n);
 for(i=0;i<n;i++)  //读入前n个结点的字符及权值
 {
  printf("input %d's number and it quanzhi",i+1);
  scanf("%c %f",&c,&f);
  getchar();
  tree[i].ch=c;
  tree[i].weight=f;
 }
 for(i=n;i<m;i++)      //进行n-1次合并，产生n-1个新结点
 {
  p1=0;p2=0;
  small1=maxval;small2=maxval;   //maxval是float类型的最大值
  for(j=0;j<i;j++)    //选出两个权值最小的根结点
   if(tree[j].parent==0)
    if(tree[j].weight<small1)
    {
     small2=small1;  //改变最小权、次小权及对应的位置
     small1=tree[j].weight;
     p2=p1;
     p1=j;
    }
    else
     if(tree[j].weight<small2)
     {
      small2=tree[j].weight;  //改变次小权及位置
      p2=j;
     }
  tree[p1].parent=i;
  tree[p2].parent=i;
  tree[i].lchild=p1;  //最小权根结点是新结点的左孩子
  tree[i].rchild=p2;  //次小权根结点是新结点的右孩子
  tree[i].weight=tree[p1].weight+tree[p2].weight;
 }
}//huffman
void huffmancode(codetype code[],hufmtree tree[])//根据哈夫曼树求出哈夫曼编码
//codetype code[]为求出的哈夫曼编码
//hufmtree tree[]为已知的哈夫曼树
{
 int i,c,p;
 codetype cd;   //缓冲变量
 for(i=0;i<n;i++)
 {
  cd.start=n;
  cd.ch=tree[i].ch;
  c=i;       //从叶结点出发向上回溯
  p=tree[i].parent;   //tree[p]是tree[i]的双亲
  while(p!=0)
  {
   cd.start--;
   if(tree[p].lchild==c)
    cd.bits[cd.start]='0';   //tree[i]是左子树，生成代码'0'
   else
    cd.bits[cd.start]='1';   //tree[i]是右子树，生成代码'1'
   c=p;
   p=tree[p].parent;
  }
  code[i]=cd;    //第i+1个字符的编码存入code[i]
 }
}//huffmancode


void decode(hufmtree tree[])//依次读入电文，根据哈夫曼树译码
{
 int i,j=0;
 char b[maxsize];
 char endflag='2';    //电文结束标志取2
 i=m-1;             //从根结点开始往下搜索
 printf("input the Huff number above that you want to find (the last number Please input 2 which is endword):");
 gets(b);
 printf("the number that you want is :");
 while(b[j]!='2')
 {
  if(b[j]=='0')
   i=tree[i].lchild;   //走向左孩子
  else
   i=tree[i].rchild;   //走向右孩子
  if(tree[i].lchild==-1)     //tree[i]是叶结点
  {
   printf("%c",tree[i].ch);
   i=m-1;      //回到根结点
  }
  j++;
 }
 printf("\n");
 if(tree[i].lchild!=-1&&b[j]!='2')   //电文读完，但尚未到叶子结点
  printf("\nERROR\n");  //输入电文有错
}//decode