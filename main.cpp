/**********************************
随机打点 重新填色 堆排序 哈夫曼编码
**********************************/
#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <math.h>

//初始化初始字体函数
CvFont font;   

//定义 “ π ”
#define PI 3.1415926
//定义极限值   
#define MAX 2000         

//定义opencv函数要用的一系列数组
int NearMat[MAX][MAX];      
int ColorMat[MAX];          
int SeqMat[MAX];            

IplImage* marker_mask = 0;
IplImage* markers = 0;
IplImage* img0 = 0, *img = 0, *img_gray = 0, *wshed = 0,*tre=0;
CvPoint prev_pt = {-1,-1};

typedef struct Huffman Huffman;  

//定义生成哈夫曼树的结构体
struct Huffman     
{
    int area;          //结点对应的权值
    int depth;       //深度
    int code;           //结点
    char codeall[20];      //新生成的结点
    Huffman *left;          //左孩子
    Huffman *right;         //右孩子
};

int max_child(int *arr,int father,int length);   //找到整个数据（权值的最大值）
void swap(int p1,int p2,int *arr,int *AreaNum);  //交换两个数据，输入变量为下标
void reshape(int node,int length,int *arr,int *AreaNum);  //比较完后，重新进行绘制“堆”
void heap_sort(int comp_count,int *AreaSize,int *AreaNum);  //进行的堆排序的主函数
Huffman* BuildHuffmanTree(int *list,int length);   //哈弗曼编码的主体函数
void DrawTree(Huffman *tree,int i,int yyy,int xx,int yy);  //哈夫曼树的可视化
void bianma(char *aa,Huffman *tree,int i);  //哈夫曼编码循环
void bian_ma(Huffman *tree);  //结点编码
void SetRand(int Drop);           //生成随机点的次数（为防止生成的点不随机）
int GetRand(int min,int max);      //生成随机点 
double GetDistance(CvPoint Point1,CvPoint Point2);   //两点之间距离
int JudgePoint(CvPoint* PointList,int Numberlist,CvPoint OnCheckPoint,int Distance,int BorderX,int BorderY);  //判断找的点与其他点的距离
int GetBiggestItem(int* List,int ListLength) ;    //寻找邻接区域数最多的区域
int FindItem(int* List,int ListLength,int Item);  //寻找特定的元素
int GetCorlorAround(int* AreaColor,int NumAreaAll,int** AdjacencyList,int CurrentArea);  //填充某区域周围的颜色
int GetNextArea(int* AreaColor,int NumAreaAll,int NumPaintedArea,int **AdjacencyList);   //填好后某一个区域后，进行下一个临界区域的判断
CheckPaint(int* AreaColor,int NumArea,int** AdjacencyList,int AreaID);  //检查点
int GetAreaColor(int* AreaColor,int NumArea,int** AdjacencyList) ; //给区域颜色
int GetAdjacencyList(int **AdjacencyList,int NumArea,int Width,int Height) ;    //得到邻接矩阵
void CheckNear(int (*p)[MAX],int n,IplImage *img);                   //检查邻接区域
int DrawMark(int comp_count,int **AreaOrder) ;   //根据区域大小，写数字
int GetAreaSize(int *AreaSize,int num) ;  //区域面积
void CaArea(int (*p)[MAX],int *q,int n);   //取得颜色矩阵    
void PaintArea(int *q,IplImage *img1,IplImage *img2,int n) ;              //区域填颜色    

int main( int argc, char** argv )
{
    char* filename = argc >= 2 ? argv[1] : (char*)"vilia.jpg";
    CvRNG rng = cvRNG(-1);    //初始化随机数生成器状态//
    cvInitFont(&font,CV_FONT_HERSHEY_PLAIN,1.0,1.0,0.0,2,8);    //初始化字体的结构

    if( (img0 = cvLoadImage(filename,1)) == 0 )   //img0为vilia.jpg的文件，保存图像到文件
        return 0;

    printf( "Hot keys: \n"
            "\tESC - quit the program\n"
            "\tr - restore the original image\n"
            "\tw or ENTER - run watershed algorithm\n"
            "\t\t(before running it, roughly mark the areas on the image)\n"
            "\t  (before that, roughly outline several markers on the image)\n" );

    cvNamedWindow( "image", 1 );   //三个窗口被打开，创建窗口，建立图像
    cvNamedWindow( "watershed transform", 1 );
    cvNamedWindow( "mask", 1 );
    cvNamedWindow( "treee", 1 );

    img = cvCloneImage( img0 );   //把img0图像给img等
    img_gray = cvCloneImage( img0 );
    tre=cvCreateImage(cvSize(1600,800),IPL_DEPTH_8U,3);
    wshed = cvCloneImage( img0 );
    marker_mask = cvCreateImage( cvGetSize(img), 8, 1 );  //把img0图像给img等
    markers = cvCreateImage( cvGetSize(img), IPL_DEPTH_32S, 1 );
    cvCvtColor( img, marker_mask, CV_BGR2GRAY );  //颜色空间转换,可以实现BGR颜色向HSV，HSI等颜色空间的转换，也可以转换为灰度
    cvCvtColor( marker_mask, img_gray, CV_GRAY2BGR );  //这两句只用将RGB转成3通道的灰度图即R=G=B,用来显示用

    cvZero( marker_mask );    //将数组中的所有通道的元素的值都设置为0
    cvZero( wshed );
    cvShowImage( "image", img );   //用来在指定窗口中显示图像
    cvShowImage( "watershed transform", wshed );

    printf("width=%d, height=%d\n",markers->width,markers->height);

    int Width=markers->width,Height=markers->height;
    long StartTime=0,EndTime=0;
    int k=0;
    printf("please input k\n ");
    scanf("%d",&k);
    int Inner=sqrt(Width*Height/k),Out=int(1.2*Inner);
    CvPoint TempPoint;
    CvPoint* PointList=NULL;    //定义点的x，y的坐标的结构体
    int PointNumber=0,Numberlist=0,NowPoint=0;

    int MaxNum=2000;

    SetRand(5);  //取5遍随机数
    PointList=(CvPoint*) malloc(sizeof(CvPoint)*20000);
    if(PointList==NULL)
    {
       // printf("Malloc Memory For ListPoint Error!\nSize Required %d\n",sizeof(CvPoint)*MaxNumPoint);
        exit(1);
    }

    StartTime=GetTickCount();

    PointList[Numberlist].x=(int)Width/2.0;
    PointList[Numberlist].y=(int)Height/2.0;

    Numberlist++;

    while(NowPoint!=Numberlist)
    {

        TempPoint=GetPoint(PointList[NowPoint],Inner,Out,PointNumber,MaxNum);

        if(JudgePoint(PointList,Numberlist,TempPoint,Inner,Width,Height)==1)
        {
            Numberlist++;
            PointList[Numberlist]=TempPoint;
            PointNumber=0;
        }
        else
        {

            PointNumber++;
        }
        if(PointNumber>=MaxNum)
        {
            NowPoint++;
            PointNumber=0;


        }
    }

    EndTime=GetTickCount();

    printf("Target: %d  in Fact: %d\n",k,Numberlist);

    printf("Processed in %ld ms\n",EndTime-StartTime);

    for(int i=0;i<Numberlist;i++)
    {
        cvRectangle(marker_mask,cvPoint(PointList[i].x-1,PointList[i].y-1),cvPoint(PointList[i].x+1,PointList[i].y+1),cvScalarAll(255),CV_FILLED,8,0);
        cvRectangle(img,cvPoint(PointList[i].x-1,PointList[i].y-1),cvPoint(PointList[i].x+1,PointList[i].y+1),cvScalarAll(255),CV_FILLED,8,0);
    }

    cvShowImage( "image", img );
    cvShowImage( "mask", marker_mask );
    free(PointList);
    for(;;)
    {

        int c = cvWaitKey(0);   //不断刷新图像
        //int c='w';
        if( c == 27 )
            break;

        if( c == 'r' )
        {
            cvZero( marker_mask );
            cvCopy( img0, img );
            cvShowImage( "image", img );
        }

        if( c == 'w' || c == '\r' )
        {
            CvMemStorage* storage = cvCreateMemStorage(0);  //创建一个内存存储器，返回值：新创建的内存存储器的指针
            CvSeq* contours = 0;  //创建了一个序列
            CvMat* color_tab;    //矩阵变化函数
            int i, j, comp_count = 0;    //comp_count为种子点个数&区域个数&边界个数&邻接表的矩阵边长
            cvSaveImage( "wshed_mask.png", marker_mask );
            marker_mask = cvLoadImage( "wshed_mask.png", 0 );
            cvFindContours( marker_mask, storage, &contours, sizeof(CvContour),
                            CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );   //从二值图像中检索轮廓，并返回检测到的轮廓的个数
            cvZero( markers );
            for( ; contours != 0; contours = contours->h_next, comp_count++ )
            {
                cvDrawContours( markers, contours, cvScalarAll(comp_count+1),
                                cvScalarAll(comp_count+1), -1, -1, 8, cvPoint(0,0) );  //在图像上绘制外部和内部轮廓
            }

            color_tab = cvCreateMat( 1, comp_count, CV_8UC3 );  //创建随机颜色列表   //转变成了矩阵   //RGB3通道就用CV_8UC3
            for( i = 0; i < comp_count; i++ )
            {
                uchar* ptr = color_tab->data.ptr + i*3;
                ptr[0] = (uchar)(cvRandInt(&rng)%180 + 50);
                ptr[1] = (uchar)(cvRandInt(&rng)%180 + 50);
                ptr[2] = (uchar)(cvRandInt(&rng)%180 + 50);
            }

            {
            double t = (double)cvGetTickCount();
            cvWatershed( img0, markers );
            t = (double)cvGetTickCount() - t;
            printf( "exec time = %gms\n", t/(cvGetTickFrequency()*1000.) );
            }

            // paint the watershed image
            for( i = 0; i < markers->height; i++ )
                for( j = 0; j < markers->width; j++ )
                {
                    int idx = CV_IMAGE_ELEM( markers, int, i, j );  //BGR三个通道的数是一起的,故要j*3
                    uchar* dst = &CV_IMAGE_ELEM( wshed, uchar, i, j*3 );
                    if( idx == -1 )   //输出时若为-1，表示各个部分的边界
                        dst[0] = dst[1] = dst[2] = (uchar)255;
                    else if( idx <= 0 || idx > comp_count )    //异常情况
                        dst[0] = dst[1] = dst[2] = (uchar)0; // should not get here
                    else
                    {
                        uchar* ptr = color_tab->data.ptr + (idx-1)*3;
                        dst[0] = ptr[0]; dst[1] = ptr[1]; dst[2] = ptr[2];
                    }
                }
            CheckNear(NearMat,comp_count,markers);
            CaArea(NearMat,ColorMat,comp_count);

            cvAddWeighted( wshed, 0.5, img_gray, 0.5, 0, wshed );                           /*μ÷??í??÷?èμ? ??ê??-í???μ?±3?° image_gray?ú???°?íò??-????á?img0μ?D??￠*/
            cvShowImage( "watershed transform", wshed );

            PaintArea(ColorMat,markers,wshed,comp_count);                   /**/
            cvAddWeighted( wshed, 0.5, img_gray, 0.5, 0, wshed );
            cvShowImage( "watershed transform", wshed );

            int *AreaSize;               //区域大小
            AreaSize=(int *) malloc(sizeof(int)*comp_count);

            GetAreaSize(AreaSize,comp_count);   //得到区域大小

            int **AreaOrder;                  //区域大小和序号

            AreaOrder=(int **) malloc(sizeof(int)*2);
            for(i=0;i<2;i++)
            {
                AreaOrder[i]=(int *) malloc(sizeof(int)*comp_count);
            }                          
            for(i=0;i<comp_count;i++)
            {
                AreaOrder[0][i]=i;
                AreaOrder[1][i]=AreaSize[i];
            }

            heap_sort(comp_count,AreaSize,AreaOrder[0]);
            DrawMark(comp_count,AreaOrder);
            cvShowImage( "watershed transform", wshed );
            cvReleaseMemStorage( &storage );
            cvReleaseMat( &color_tab );
            Huffman *tree;
            tree=BuildHuffmanTree(AreaOrder[1],20);
            bian_ma(tree);
           
            int depth=0;
            int xx=600,yy=40;  //合适的初始画哈夫曼树的坐标
            DrawTree(tree,20,depth,xx,yy);
            cvShowImage( "treee", tre);  //新建图像窗口
            //getchar();
            /*
            char hcode[20];
            printf("请输入求解编码:\n");

            scanf("%s",hcode);
            GetCode(tree,hcode);
            */

            for(i=0;i<2;i++)  //free molloc 
            {
                free(AreaOrder[i]);
            }
            free(AreaOrder);
            free(AreaSize);
        }
    }

    return 1;
 }

 //堆排序
int max_child(int *arr,int father,int length)   //找到整个数据（权值的最大值）
{
    int left;
    int right;
    int temp;

    //如果左节点是最后一个数据，则返回左节点
    if(father*2+2 == length)
    {
        return father*2+1;
    }

    //如果左节点没有数据，则返回length
    if((father*2+1)>=length)
    {
        return length;
    }

    //得到两个节点的数据
    temp = father*2+1;  //左节点下标
    left = arr[temp];
    right = arr[temp+1];

    //比较并返回较大数据的下标
    if(left>right)
    {
        return temp;
    }
    else
    {
        return temp+1;
    }
}

//堆排序
void swap(int p1,int p2,int *arr,int *AreaNum)  //交换两个数据，输入变量为下标
{
    //交换权值
    int temp = arr[p1];  
    arr[p1] = arr[p2];
    arr[p2] = temp;

    //交换区域编码
    int temp2 = AreaNum[p1];
    AreaNum[p1] = AreaNum[p2];
    AreaNum[p2] = temp2;
}

//堆排序
void reshape(int node,int length,int *arr,int *AreaNum)  //比较完后，重新进行绘制“堆”
{

    while(node<=length)
    {
        //得到子节点较大数据的下标
        int child = max_child(arr,node,length);
        if(child >= length)
        {
            //如果子节点没有数据，则返回
            break;
        }

        //如果子节点比父节点大
        if(arr[node]<arr[child])
        {
           //则交换父节点与子节点
            swap(node,child,arr,AreaNum);
           //并针对交换下去的父节点再进行判断
            node = child;
        }
        else
        {
            //父节点比子节点中较大的数据大，则返回
            break;
        }
    }
}

//堆排序
//void Display(int comp_count)  //显示堆排序结果
//{
//     int i;
//     for(i=0;i<comp_count;i++)
//     {
//         if(i%4 == 0)
//         {
//             printf("\n");
 //        }
//         printf("%4d\t",arr[i]);
//     }
// }

//堆排序
void heap_sort(int comp_count,int *AreaSize,int *AreaNum)  //进行的堆排序的主函数
{
    int i;

    for(i=comp_count/2-1;i>=0;i--)
    {
        reshape(i,comp_count,AreaSize,AreaNum);
    }

    //提取大数，并循环整理
    for(i=comp_count-1;i>0;i--)
    {
        swap(0,i,AreaSize,AreaNum);
        reshape(0,i,AreaSize,AreaNum);
    }
}


//哈夫曼编码
Huffman* BuildHuffmanTree(int *list,int length)   //哈弗曼编码的主体函数
{
    //一系列的初始化
    int i=0,j=0,k=0;        
    Huffman **forest=NULL;
    Huffman *Tree2=NULL;
    int TreeNum=length;
    Huffman *left_child;
    Huffman *right_child;
    Huffman *temp;
    forest=(Huffman **) malloc(sizeof(Huffman*)*length);
    for(i=0;i<length;i++)
    {
        forest[i]=(Huffman *) malloc(sizeof(Huffman));
    }
    for(i=0;i<length;i++)
    {
        forest[i]->area=list[i];
        forest[i]->depth=1;
        forest[i]->left=NULL;
        forest[i]->right=NULL;
    }

    //哈夫曼编码算法
    while(TreeNum>1)   //森林中有树
    {
        Tree2=(Huffman *) malloc(sizeof(Huffman));
        if(forest[0]->depth>forest[1]->depth)
        {
            temp=forest[0];
            forest[0]=forest[1];
            forest[1]=temp;
        }
        Tree2->left=forest[0];      //原根结点为左孩子
        Tree2->right=forest[1];     //原根结点为右孩子
        Tree2->area=forest[0]->area+forest[1]->area;  //新生成结点的值
        Tree2->depth=forest[1]->depth+1;   //深度加一
        for(j=2;j<TreeNum&&(forest[j]->area)<=(Tree2->area)/*&&(forest[j]->depth)<=(Tree2->depth)*/;j++)
        {
            forest[j-2]=forest[j];  //遍历
        }
        forest[j-2]=Tree2;
        for(k=j-1;k<TreeNum-1;k++)
        {
            forest[k]=forest[k+1];  //进行下一个判断
        }
        TreeNum--;      //完成一轮
    }
    free(forest);

    return Tree2;  //返回计算好的哈夫曼树
}

//哈夫曼编码
void DrawTree(BinaryTree *tree,int i,int yyy,int xx,int yy)  //哈夫曼树的可视化
{
    if(i>20)
    {
        printf("wrong!");   //叶子节点大于20，则为显示错误！（20的位置的值可以设置，主要考虑到，20以上屏幕容纳不了）
        return;
    }
    yyy++;
    CvFont font;
    cvInitFont(&font,CV_FONT_HERSHEY_DUPLEX,0.5,0.5,0,2,8);  //初始化图像
    if(yyy<=i)
    {
        cvRectangle(tre,cvPoint(xx-10,yy+10),cvPoint(xx+60,yy-20),cvScalarAll(255),CV_FILLED,8,0);   //画显示结点权值的矩形
        char a[10];
        itoa(tree->area,a,10);   //char int交换
        cvPutText(tre,a,cvPoint(xx,yy),&font,CV_RGB(0,0,0));   //写权值
        if(tree->left!=NULL)  //考虑左子树
        {
            if(yyy==i)
                return;
            if(yyy==1)  //深度
            {
                cvLine(tre,cvPoint(xx-10,yy+10),cvPoint(xx-200,yy+40),cvScalarAll(255),1,8,0);  //画连接两矩形的框
                cvPutText(tre,"0",cvPoint(xx-195,yy+30),&font,cvScalarAll(255));   //左子树表为0
                DrawTree(tree->left,i,yyy,xx-220,yy+60);
            }
            if(yyy==2)
            {
                cvLine(tre,cvPoint(xx-10,yy+10),cvPoint(xx-100,yy+40),cvScalarAll(255),1,8,0);   //画连接两矩形的框
                cvPutText(tre,"0",cvPoint(xx-95,yy+30),&font,cvScalarAll(255)); 
                DrawTree(tree->left,i,yyy,xx-180,yy+60);
            }
            if(yyy==3)
            {
                cvLine(tre,cvPoint(xx-10,yy+10),cvPoint(xx-55,yy+40),cvScalarAll(255),1,8,0);  //画连接两矩形的框
                cvPutText(tre,"0",cvPoint(xx-35,yy+30),&font,cvScalarAll(255));
                DrawTree(tree->left,i,yyy,xx-80,yy+60);
            }
            if(yyy>=4)
            {
                cvLine(tre,cvPoint(xx-10,yy+10),cvPoint(xx-15,yy+40),cvScalarAll(255),1,8,0);  //画连接两矩形的框
                cvPutText(tre,"0",cvPoint(xx-15,yy+30),&font,cvScalarAll(255));
                DrawTree(tree->left,i,yyy,xx-40,yy+60);
            }
        }
        if(tree->right!=NULL)   //考虑右子树
        {
            if(yyy==i)
                return;
            if(yyy==1)
            {
                cvLine(tre,cvPoint(xx+60,yy+10),cvPoint(xx+220,yy+40),cvScalarAll(255),1,8,0);   //画连接两矩形的框
                cvPutText(tre,"1",cvPoint(xx+200,yy+30),&font,cvScalarAll(255));        //右子树表为0
                DrawTree(tree->right,i,yyy,xx+200,yy+60);  
            }
            if(yyy==2)
            {
                cvLine(tre,cvPoint(xx+60,yy+10),cvPoint(xx+150,yy+40),cvScalarAll(255),1,8,0);   //画连接两矩形的框
                cvPutText(tre,"1",cvPoint(xx+120,yy+30),&font,cvScalarAll(255));
                DrawTree(tree->right,i,yyy,xx+140,yy+60);
            }
            if(yyy==3)
            {
                cvLine(tre,cvPoint(xx+60,yy+10),cvPoint(xx+90,yy+40),cvScalarAll(255),1,8,0);   //画连接两矩形的框
                cvPutText(tre,"1",cvPoint(xx+60,yy+30),&font,cvScalarAll(255));
                DrawTree(tree->right,i,yyy,xx+85,yy+60);
            }
            if(yyy>=4)
            {
                cvLine(tre,cvPoint(xx+60,yy+10),cvPoint(xx+65,yy+40),cvScalarAll(255),1,8,0);    //画连接两矩形的框
                cvPutText(tre,"1",cvPoint(xx+40,yy+30),&font,cvScalarAll(255));
                DrawTree(tree->right,i,yyy,xx+40,yy+60);
            }
        }
    }
    else
    {
        return;
    }

}

//哈夫曼编码
void bianma(char *aa,Huffman *tree,int i)  //哈夫曼编码循环
{
    char bb[20];
    strcpy(bb,aa);
    int m=strlen(bb);
    if(m>=20)
    {
        return;
    }
    bb[m]=tree->code;
    bb[m+1]='\0';
    strcpy(tree->codeall,bb);
    i++;
    if(i>=50)
        return;
    if(tree->left!=NULL)
        bianma(bb,tree->left,i);
    if(tree->right!=NULL)
        bianma(bb,tree->right,i);
    return;
}

//哈夫曼编码
void bian_ma(Huffman *tree)  //结点编码
{
    tree->code=0;
    tree->codeall[0]='\0';
    char a[20]="\0";
    int i=0;
    bianma(a,tree,i);
}

//哈夫曼编码
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
}

//哈夫曼编码
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

//哈夫曼编码
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

//随机打点
void SetRand(int Drop)           //生成随机点的次数（为防止生成的点不随机）
{
    int i=0;
    srand(time(NULL));
    for(i=0;i<Drop;i++)
    {
       rand();
    }
}

//随机打点
int GetRand(int min,int max)      //生成随机点      
{
    if(max>30000||min<0)      //错误
    {
        return -1;
    }          
    else
    {
        return (int) (((double) rand()/32767.0)*(max-min)+min);  //生成一个在最大最小值之间的任意数字
    }
}

//随机打点
double GetDistance(CvPoint Point1,CvPoint Point2)   //两点之间距离
{
    return sqrt((double) ( (Point1.x - Point2.x)*(Point1.x - Point2.x)+(Point1.y - Point2.y)*(Point1.y - Point2.y) ) );
}

//随机打点
CvPoint GetPoint(CvPoint Center,int Inner,int Out,int PointNumber,int MaxNum)  //找点函数
{
    double PartWidth=360.0/((double) MaxNum);
    double r=(double) GetRand(Inner,Out);
    int RandNumber=rand();
    double RandInPart=RandNumber/(32767.0);   //生成一个0-1的数字
    double Deg=RandInPart*PartWidth+(PointNumber-1)*PartWidth;   //度
    CvPoint Point;
    Point.x=Center.x+(int) r*cos(Deg*PI/180);
    Point.y=Center.y+(int) r*sin(Deg*PI/180);
    return Point;
}

//随机打点
int JudgePoint(CvPoint* PointList,int Numberlist,CvPoint OnCheckPoint,int Distance,int BorderX,int BorderY)  //判断找的点与其他点的距离
{
    int i=0;
    if(OnCheckPoint.x>BorderX||OnCheckPoint.y>BorderY||OnCheckPoint.x<0||OnCheckPoint.y<0)  //点在图像范围里
    {
        return 0;
    }
    for(i=0;i<=Numberlist;i++)
    {

        if(GetDistance(PointList[i],OnCheckPoint)<Distance)   //满足大于r的条件
        {
            return 0;
        }
    }
    return 1;
}

//重新填色
int GetBiggestItem(int* List,int ListLength)     //寻找邻接区域数最多的区域
{
    int MaxValue=List[0];
    int MaxID=0;
    int i=0;
    for(i=0;i<ListLength;i++)   //遍历
    {
        if(List[i]>MaxValue)
        {
            MaxValue=List[i];   //如果大，就交换
            MaxID=i;
        }
    }
    return MaxID;  //最大值的编号
}

//重新填色
int FindItem(int* List,int ListLength,int Item)  //寻找特定的元素
{
    for(int i=0;i<ListLength;i++)
    {
        if(Item==List[i])     //元素对应上了
            return i+1;
    }
    return 0;
}

//重新填色
int GetCorlorAround(int* AreaColor,int NumAreaAll,int** AdjacencyList,int CurrentArea)  //填充某区域周围的颜色
{
    int i=0;
    int ColorList[10];
    int ColorListID=0;
    int counter=0;
    int ColorAround;
    for(i=0;i<NumAreaAll;i++)    //遍历
    {
        if(AdjacencyList[CurrentArea][i]==1)    //区域存在
        {
            if(FindItem(ColorList,10,AreaColor[i])==0)  //填色
            {
                ColorAround++;       
                ColorList[ColorListID]=AreaColor[i];
                ColorListID++;
            }
        }
    }
    return ColorAround;
}

//重新填色
int GetNextArea(int* AreaColor,int NumAreaAll,int NumPaintedArea,int **AdjacencyList)   //填好后某一个区域后，进行下一个临界区域的判断
{
    int* ColorAround=NULL;
    int i=0;
    int BestArea=0;
    int keep=0,flag=0;
    ColorAround=(int*) malloc(sizeof(int)*NumAreaAll);
    for(i=0;i<NumAreaAll;i++)
    {
        ColorAround[i]=0;
    } 
    for(i=0;i<NumAreaAll;i++)
    {
        if(AreaColor[i]==0)
        {
            if(flag==0)
            {
                keep=i;
                flag=1;
            }
            ColorAround[i]=GetCorlorAround(AreaColor,NumAreaAll,AdjacencyList,i);  //颜色的记录
        }
    }
    BestArea=GetBiggestItem(ColorAround,NumAreaAll);   //邻接数最大的区域为最佳选择区域
    if(ColorAround[BestArea]==0)
    {
        BestArea=keep;
    }
    free(ColorAround);
    return BestArea;
}


//重新填色
CheckPaint(int* AreaColor,int NumArea,int** AdjacencyList,int AreaID)  //检查点
{
    int i=0;
    for(i=0;i<NumArea;i++)
    {
        if(i==AreaID) 
        {
           continue; 
        }
        if(AdjacencyList[AreaID][i]==1&&AreaColor[i]==AreaColor[AreaID])  //正确
        {
           return 0;
        }
    }
    return 1;
}

//重新填色
int GetAreaColor(int* AreaColor,int NumArea,int** AdjacencyList)  //给区域颜色
{
    int i=0,j=0;  //初始化
    int* Order;
    Order=(int*) malloc(sizeof(int)*NumArea);
    for(i=0;i<NumArea;i++) 
    {
        Order[i]=0;
    }
    for(i=0;i<NumArea;i++) 
    {
        AreaColor[i]=0;
    }
    int NextAreaID=0;

    i=0;
    AreaColor[0]=1;
    Order[i]=0;
    i++;
    NextAreaID=GetNextArea(AreaColor,NumArea,i,AdjacencyList);
    while(i<NumArea)
    {
        AreaColor[NextAreaID]++;
        while(AreaColor[NextAreaID]<=4)    //四色法
        {
            if(CheckPaint(AreaColor,NumArea,AdjacencyList,NextAreaID)==0)
            {
                AreaColor[NextAreaID]++;   //邻接换颜色
            }
            else
            {
                Order[i]=NextAreaID;
                i++;
                NextAreaID=GetNextArea(AreaColor,NumArea,i,AdjacencyList);
                break;
            }
        }
        if(AreaColor[NextAreaID]>4)  //退回，重新来
        {
            AreaColor[NextAreaID]=0;
            i--;
            NextAreaID=Order[i];
        }
    }
    free(Order);
    return 0;
}

//重新填色
int GetAdjacencyList(int **AdjacencyList,int NumArea,int Width,int Height)     //得到邻接矩阵
{
    int i=0,j=0;
    for(i=0;i<NumArea;i++)   //初始化邻接矩阵
        for(j=0;j<NumArea;j++)
            AdjacencyList[i][j]=0;

    for(i=1;i<markers->height-1;i++)
    {
        for(j=1;j<markers->width-1;j++)
        {
            int idx=CV_IMAGE_ELEM(markers,int ,i,j);
            if(idx==-1)          //-1为边界，大于0的整数为边界
            {
                int idxAround[8];
                int idxList[8];
                int count=0;
                int idxListNum=0;
                for(int x=0;x<3;x++)
                {
                    for(int y=0;y<3;y++)
                    {
                        if(x!=1||y!=1)
                        {
                            idxAround[count]=CV_IMAGE_ELEM(markers,int,i-1+x,j-1+y);    //读取某点的像素信息
                            count++;
                        }
                    }
                }
                for(int k=0;k<8;k++)
                {
                    if(idxAround[k]!=-1)
                    {
                        if(FindItem(idxList,idxListNum,idxAround[k])==0)
                        {
                            idxList[idxListNum]=idxAround[k];   //记录每一个点的邻接关系
                            idxListNum++;
                        }
                    }
                }
                for(x=0;x<idxListNum;x++)
                {
                    for(int y=0;y<idxListNum;y++)
                    {
                        AdjacencyList[idxList[x]-1][idxList[y]-1]=1;   //存储
                    }
                }
            }
        }
    }
    return 0;
}

//重新填色
void CheckNear(int (*p)[MAX],int n,IplImage *img)                   //检查邻接区域
{
    int i,j,idx;
    int idx1,idx2;

    for(i=0;i<MAX;i++)              //初始化
    {
        for(j=0;j<MAX;j++)
        {
            p[i][j]=0;
        }
    }

    for( i = 1; i < img->height-1; i++ ) //将整个图片所有像素遍历一遍
    {
        for( j = 1; j < img->width-1; j++ )
        {
            idx = CV_IMAGE_ELEM( img, int, i, j );    //所有点的相邻八个点
            if( idx == -1 )    //相邻
            {
                idx1= CV_IMAGE_ELEM( img, int, i, j-1 );    
                idx2= CV_IMAGE_ELEM( img, int, i, j+1 );
                if(idx1!=idx2&&idx1!=-1&&idx2!=-1)
                {
                    p[idx1][idx2]=1;              //相邻，记录                         
                    p[idx2][idx1]=1;                                    
                }

            }
        }

    }
}

//重新填色
int DrawMark(int comp_count,int **AreaOrder)    //根据区域大小，写数字
{
    char string[10];
    int i=0,j=0;
    for(i=0;i<10;i++) 
    {
        string[i]=0;
    }
    for(int counters=0;counters<comp_count;counters++)
    {
        int record_y=0,record_x=0,num_same_point=0;
         for( i = 0; i < markers->height; i++ )
         {
            for( j = 0; j < markers->width; j++ )
            {
                int idx = CV_IMAGE_ELEM( markers, int, i, j );
                if(idx==counters+1)    //内部点
                {
                    record_y+=i;
                    record_x+=j;
                    num_same_point++;   //计数的
                }
            }
         }
         sprintf(string,"%d",FindItem(AreaOrder[0],comp_count,counters));   //复制
         cvPutText(wshed,string,cvPoint(record_x/num_same_point,record_y/num_same_point),&font,cvScalar(0));   //打字
         for(i=0;i<10;i++) 
        {
            string[i]=0;  //归零·
        }
    }

    return 0;
}

//重新填色
int GetAreaSize(int *AreaSize,int num)   //区域面积
{
    int max=0;
    int min=0;
    int i=0,j=0;
    int elem=0;
    for(i=0;i<num;i++) 
    {
        AreaSize[i]=0;
    }
    for(i=0;i<markers->height-1;i++)
    {
        for(j=0;j<markers->width-1;j++)
        {
            elem=CV_IMAGE_ELEM(markers,int ,i,j);   //一个一个像素点计数
            if(elem<=num&&elem>0)
            {
                AreaSize[elem-1]++;    //对应区域加一
            }
        }
    }
    return 0;
}

//重新填色
void CaArea(int (*p)[MAX],int *q,int n)   //取得颜色矩阵       
{
    int i;
    int idx=0;
    int t;
    for(i=0;i<MAX;i++)     
    {
        q[i]=0;
    }
    for(;idx<n&&idx>=0;)  //进出栈原理
    {
        q[idx]+=1;                                  
        for(;q[idx]<5;)             //在四色中循环                
        {
            t=q[idx];
            for(i=0;i<n;i++)
            {
                if(i==idx)
                    continue;
                if(q[idx]==q[i]&&p[idx][i]==1&&q[idx]!=0)  //取得了正确的颜色
                {
                    q[idx]++;
                    break;
                }
            }
            if(t==q[idx])  //记录，退出循环
                break;
        }
        if(q[idx]>4)   //大于4色，重新来
        {
            q[idx]=0;                               
            idx--;
        }
        else if(q[idx]<=4&&q[idx]>0) //其他特殊情况
            idx++;
        else
        {
            printf("wrong!");
            exit(1);
        }
    }
}

//重新填色
void PaintArea(int *q,IplImage *img1,IplImage *img2,int n)               //区域填颜色                                                                                   
{
    CvMat *color_tab;
    int i,j;

    color_tab = cvCreateMat( 1, 4, CV_8UC3 );                                           //创建新的矩阵

        uchar* ptr = color_tab->data.ptr + 0*3;                                         //记录
        ptr[0] = (uchar)(0);
        ptr[1] = (uchar)(0);
        ptr[2] = (uchar)(255);

        ptr=color_tab->data.ptr+1*3;
        ptr[0] = (uchar)(255);
        ptr[1] = (uchar)(0);
        ptr[2] = (uchar)(0);

        ptr=color_tab->data.ptr+2*3;
        ptr[0] = (uchar)(0);
        ptr[1] = (uchar)(255);
        ptr[2] = (uchar)(0);

        ptr=color_tab->data.ptr+3*3;
        ptr[0] = (uchar)(0);
        ptr[1] = (uchar)(255);
        ptr[2] = (uchar)(255);

    for( i = 0; i < img1->height; i++ )
    {
        for( j = 0; j < img1->width; j++ )
            {
                int idx = CV_IMAGE_ELEM(img1, int, i, j );
                uchar* dst = &CV_IMAGE_ELEM( img2, uchar, i, j*3 );
                if( idx == -1 )
                    dst[0] = dst[1] = dst[2] = (uchar)255;
                else if( idx <= 0 || idx > n)
                    dst[0] = dst[1] = dst[2] = (uchar)0; // should not get here
                else
                {
                    uchar* ptr = color_tab->data.ptr + (ColorMat[idx]-1)*3;
                    dst[0] = ptr[0]; dst[1] = ptr[1]; dst[2] = ptr[2];                 
                }
            }
    }
    cvReleaseMat( &color_tab );                                                         //释放
}
