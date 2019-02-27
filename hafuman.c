// #include <stdio.h>

// #define MAXBIT      100
// #define MAXVALUE  10000
// #define MAXLEAF     30
// #define MAXNODE    MAXLEAF*2 -1

// typedef struct
// {
//     int bit[MAXBIT];
//     int start;
// } HCodeType;        /* ����ṹ�� */

// typedef struct
// {
//     int weight;
//     int parent;
//     int lchild;
//     int rchild;
// } HNodeType;        /* ���ṹ�� */

// /* ����һ�Ź������� */
// void HuffmanTree (HNodeType HuffNode[MAXNODE],  int n)
// {
//     /* i��j�� ѭ��������m1��m2���������������ͬ������������СȨֵ����Ȩֵ��
//         x1��x2���������������ͬ������������СȨֵ����������е���š�*/
//     int i, j, m1, m2, x1, x2;
//     /* ��ʼ����Ź����������� HuffNode[] �еĽ�� */
//     for (i=0; i<2*n-1; i++)
//     {
//         HuffNode[i].weight = 0;
//         HuffNode[i].parent =-1;
//         HuffNode[i].lchild =-1;
//         HuffNode[i].lchild =-1;
//     } /* end for */

//     /* ���� n ��Ҷ�ӽ���Ȩֵ */
//     for (i=0; i<n; i++)
//     {
//         printf ("Please input weight of leaf node %d: \n", i);
//         scanf ("%d", &HuffNode[i].weight);
//     } /* end for */

//     /* ѭ������ Huffman �� */
//     for (i=0; i<n-1; i++)
//     {
//         m1=m2=MAXVALUE;     /* m1��m2�д�������޸�����ҽ��Ȩֵ��С��������� */
//         x1=x2=0;
//         /* �ҳ����н����Ȩֵ��С���޸�����������㣬���ϲ�֮Ϊһ�Ŷ����� */
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
//             /* �����ҵ��������ӽ�� x1��x2 �ĸ������Ϣ */
//         HuffNode[x1].parent  = n+i;
//         HuffNode[x2].parent  = n+i;
//         HuffNode[n+i].weight = HuffNode[x1].weight + HuffNode[x2].weight;
//         HuffNode[n+i].lchild = x1;
//         HuffNode[n+i].rchild = x2;

//         printf ("x1.weight and x2.weight in round %d: %d, %d\n", i+1, HuffNode[x1].weight, HuffNode[x2].weight);  /* ���ڲ��� */
//         printf ("\n");
//     } /* end for */
// } /* end HuffmanTree */

// int main(void)
// {
//     HNodeType HuffNode[MAXNODE];            /* ����һ�����ṹ������ */
//     HCodeType HuffCode[MAXLEAF],  cd;       /* ����һ������ṹ�����飬 ͬʱ����һ����ʱ���������������ʱ����Ϣ */
//     int i, j, c, p, n;
//     printf ("Please input n:\n");
//     scanf ("%d", &n);
//     HuffmanTree (HuffNode, n);

//     for (i=0; i < n; i++)
//     {
//         cd.start = n-1;
//         c = i;
//         p = HuffNode[c].parent;
//         while (p != -1)   /* �������� */
//         {
//             if (HuffNode[p].lchild == c)
//                 cd.bit[cd.start] = 0;
//             else
//                 cd.bit[cd.start] = 1;
//             cd.start--;        /* �����ĵ�һλ */
//             c=p;
//             p=HuffNode[c].parent;    /* ������һѭ������ */
//         } /* end while */

//         /* ���������ÿ��Ҷ���Ĺ���������ͱ������ʼλ */
//         for (j=cd.start+1; j<n; j++)
//         { HuffCode[i].bit[j] = cd.bit[j];}
//         HuffCode[i].start = cd.start;
//     } /* end for */

//     /* ����ѱ���õ����д��ڱ���Ĺ��������� */
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
#define n 5  //Ҷ����Ŀ
#define m (2*n-1)    //�������
#define maxval 10000.0
#define maxsize 100   //��������������λ��
typedef struct
{
 char ch;
 float weight;
 int lchild,rchild,parent;
}hufmtree;

typedef struct
{
 char bits[n];   //λ��
 int start;      //������λ���е���ʼλ��
 char ch;        //�ַ�
}codetype;
void huffman(hufmtree tree[]);//������������
void huffmancode(codetype code[],hufmtree tree[]);//���ݹ��������������������
void decode(hufmtree tree[]);//���ζ�����ģ����ݹ�����������
void main()
{
  hufmtree tree[m];
 codetype code[n];
 int i,j;//ѭ������
 //printf("                            �������������롪��\n");
 printf("all numbers is\n",n);


 huffman(tree);//������������
 huffmancode(code,tree);//���ݹ��������������������
 printf("output all numbers huff number\n");
 for(i=0;i<n;i++)
 {
  printf("%c: ",code[i].ch);
  for(j=code[i].start;j<n;j++)
   printf("%c ",code[i].bits[j]);
  printf("\n");
 }
 printf("read it and finishthe number that you want is :\n");
 decode(tree);//���ζ�����ģ����ݹ�����������
}
void huffman(hufmtree tree[])//������������
{
 int i,j,p1,p2;//p1,p2�ֱ��סÿ�κϲ�ʱȨֵ��С�ʹ�С�������������±�
 float small1,small2,f;
 char c;
 for(i=0;i<m;i++)    //��ʼ��
 {
  tree[i].parent=0;
  tree[i].lchild=-1;
  tree[i].rchild=-1;
  tree[i].weight=0.0;
 }
 //printf("�����ζ���ǰ%d�������ַ���Ȩֵ(�м��ÿո����)��\n",n);
 for(i=0;i<n;i++)  //����ǰn�������ַ���Ȩֵ
 {
  printf("input %d's number and it quanzhi",i+1);
  scanf("%c %f",&c,&f);
  getchar();
  tree[i].ch=c;
  tree[i].weight=f;
 }
 for(i=n;i<m;i++)      //����n-1�κϲ�������n-1���½��
 {
  p1=0;p2=0;
  small1=maxval;small2=maxval;   //maxval��float���͵����ֵ
  for(j=0;j<i;j++)    //ѡ������Ȩֵ��С�ĸ����
   if(tree[j].parent==0)
    if(tree[j].weight<small1)
    {
     small2=small1;  //�ı���СȨ����СȨ����Ӧ��λ��
     small1=tree[j].weight;
     p2=p1;
     p1=j;
    }
    else
     if(tree[j].weight<small2)
     {
      small2=tree[j].weight;  //�ı��СȨ��λ��
      p2=j;
     }
  tree[p1].parent=i;
  tree[p2].parent=i;
  tree[i].lchild=p1;  //��СȨ��������½�������
  tree[i].rchild=p2;  //��СȨ��������½����Һ���
  tree[i].weight=tree[p1].weight+tree[p2].weight;
 }
}//huffman
void huffmancode(codetype code[],hufmtree tree[])//���ݹ��������������������
//codetype code[]Ϊ����Ĺ���������
//hufmtree tree[]Ϊ��֪�Ĺ�������
{
 int i,c,p;
 codetype cd;   //�������
 for(i=0;i<n;i++)
 {
  cd.start=n;
  cd.ch=tree[i].ch;
  c=i;       //��Ҷ���������ϻ���
  p=tree[i].parent;   //tree[p]��tree[i]��˫��
  while(p!=0)
  {
   cd.start--;
   if(tree[p].lchild==c)
    cd.bits[cd.start]='0';   //tree[i]�������������ɴ���'0'
   else
    cd.bits[cd.start]='1';   //tree[i]�������������ɴ���'1'
   c=p;
   p=tree[p].parent;
  }
  code[i]=cd;    //��i+1���ַ��ı������code[i]
 }
}//huffmancode


void decode(hufmtree tree[])//���ζ�����ģ����ݹ�����������
{
 int i,j=0;
 char b[maxsize];
 char endflag='2';    //���Ľ�����־ȡ2
 i=m-1;             //�Ӹ���㿪ʼ��������
 printf("input the Huff number above that you want to find (the last number Please input 2 which is endword):");
 gets(b);
 printf("the number that you want is :");
 while(b[j]!='2')
 {
  if(b[j]=='0')
   i=tree[i].lchild;   //��������
  else
   i=tree[i].rchild;   //�����Һ���
  if(tree[i].lchild==-1)     //tree[i]��Ҷ���
  {
   printf("%c",tree[i].ch);
   i=m-1;      //�ص������
  }
  j++;
 }
 printf("\n");
 if(tree[i].lchild!=-1&&b[j]!='2')   //���Ķ��꣬����δ��Ҷ�ӽ��
  printf("\nERROR\n");  //��������д�
}//decode