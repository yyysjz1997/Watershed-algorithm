/**********************************
������ ������ɫ ������ ����������
**********************************/
#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <math.h>

//��ʼ����ʼ���庯��
CvFont font;   

//���� �� �� ��
#define PI 3.1415926
//���弫��ֵ   
#define MAX 2000         

//����opencv����Ҫ�õ�һϵ������
int NearMat[MAX][MAX];      
int ColorMat[MAX];          
int SeqMat[MAX];            

IplImage* marker_mask = 0;
IplImage* markers = 0;
IplImage* img0 = 0, *img = 0, *img_gray = 0, *wshed = 0,*tre=0;
CvPoint prev_pt = {-1,-1};

typedef struct Huffman Huffman;  

//�������ɹ��������Ľṹ��
struct Huffman     
{
    int area;          //����Ӧ��Ȩֵ
    int depth;       //���
    int code;           //���
    char codeall[20];      //�����ɵĽ��
    Huffman *left;          //����
    Huffman *right;         //�Һ���
};

int max_child(int *arr,int father,int length);   //�ҵ��������ݣ�Ȩֵ�����ֵ��
void swap(int p1,int p2,int *arr,int *AreaNum);  //�����������ݣ��������Ϊ�±�
void reshape(int node,int length,int *arr,int *AreaNum);  //�Ƚ�������½��л��ơ��ѡ�
void heap_sort(int comp_count,int *AreaSize,int *AreaNum);  //���еĶ������������
Huffman* BuildHuffmanTree(int *list,int length);   //��������������庯��
void DrawTree(Huffman *tree,int i,int yyy,int xx,int yy);  //���������Ŀ��ӻ�
void bianma(char *aa,Huffman *tree,int i);  //����������ѭ��
void bian_ma(Huffman *tree);  //������
void SetRand(int Drop);           //���������Ĵ�����Ϊ��ֹ���ɵĵ㲻�����
int GetRand(int min,int max);      //��������� 
double GetDistance(CvPoint Point1,CvPoint Point2);   //����֮�����
int JudgePoint(CvPoint* PointList,int Numberlist,CvPoint OnCheckPoint,int Distance,int BorderX,int BorderY);  //�ж��ҵĵ���������ľ���
int GetBiggestItem(int* List,int ListLength) ;    //Ѱ���ڽ���������������
int FindItem(int* List,int ListLength,int Item);  //Ѱ���ض���Ԫ��
int GetCorlorAround(int* AreaColor,int NumAreaAll,int** AdjacencyList,int CurrentArea);  //���ĳ������Χ����ɫ
int GetNextArea(int* AreaColor,int NumAreaAll,int NumPaintedArea,int **AdjacencyList);   //��ú�ĳһ������󣬽�����һ���ٽ�������ж�
CheckPaint(int* AreaColor,int NumArea,int** AdjacencyList,int AreaID);  //����
int GetAreaColor(int* AreaColor,int NumArea,int** AdjacencyList) ; //��������ɫ
int GetAdjacencyList(int **AdjacencyList,int NumArea,int Width,int Height) ;    //�õ��ڽӾ���
void CheckNear(int (*p)[MAX],int n,IplImage *img);                   //����ڽ�����
int DrawMark(int comp_count,int **AreaOrder) ;   //���������С��д����
int GetAreaSize(int *AreaSize,int num) ;  //�������
void CaArea(int (*p)[MAX],int *q,int n);   //ȡ����ɫ����    
void PaintArea(int *q,IplImage *img1,IplImage *img2,int n) ;              //��������ɫ    

int main( int argc, char** argv )
{
    char* filename = argc >= 2 ? argv[1] : (char*)"vilia.jpg";
    CvRNG rng = cvRNG(-1);    //��ʼ�������������״̬//
    cvInitFont(&font,CV_FONT_HERSHEY_PLAIN,1.0,1.0,0.0,2,8);    //��ʼ������Ľṹ

    if( (img0 = cvLoadImage(filename,1)) == 0 )   //img0Ϊvilia.jpg���ļ�������ͼ���ļ�
        return 0;

    printf( "Hot keys: \n"
            "\tESC - quit the program\n"
            "\tr - restore the original image\n"
            "\tw or ENTER - run watershed algorithm\n"
            "\t\t(before running it, roughly mark the areas on the image)\n"
            "\t  (before that, roughly outline several markers on the image)\n" );

    cvNamedWindow( "image", 1 );   //�������ڱ��򿪣��������ڣ�����ͼ��
    cvNamedWindow( "watershed transform", 1 );
    cvNamedWindow( "mask", 1 );
    cvNamedWindow( "treee", 1 );

    img = cvCloneImage( img0 );   //��img0ͼ���img��
    img_gray = cvCloneImage( img0 );
    tre=cvCreateImage(cvSize(1600,800),IPL_DEPTH_8U,3);
    wshed = cvCloneImage( img0 );
    marker_mask = cvCreateImage( cvGetSize(img), 8, 1 );  //��img0ͼ���img��
    markers = cvCreateImage( cvGetSize(img), IPL_DEPTH_32S, 1 );
    cvCvtColor( img, marker_mask, CV_BGR2GRAY );  //��ɫ�ռ�ת��,����ʵ��BGR��ɫ��HSV��HSI����ɫ�ռ��ת����Ҳ����ת��Ϊ�Ҷ�
    cvCvtColor( marker_mask, img_gray, CV_GRAY2BGR );  //������ֻ�ý�RGBת��3ͨ���ĻҶ�ͼ��R=G=B,������ʾ��

    cvZero( marker_mask );    //�������е�����ͨ����Ԫ�ص�ֵ������Ϊ0
    cvZero( wshed );
    cvShowImage( "image", img );   //������ָ����������ʾͼ��
    cvShowImage( "watershed transform", wshed );

    printf("width=%d, height=%d\n",markers->width,markers->height);

    int Width=markers->width,Height=markers->height;
    long StartTime=0,EndTime=0;
    int k=0;
    printf("please input k\n ");
    scanf("%d",&k);
    int Inner=sqrt(Width*Height/k),Out=int(1.2*Inner);
    CvPoint TempPoint;
    CvPoint* PointList=NULL;    //������x��y������Ľṹ��
    int PointNumber=0,Numberlist=0,NowPoint=0;

    int MaxNum=2000;

    SetRand(5);  //ȡ5�������
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

        int c = cvWaitKey(0);   //����ˢ��ͼ��
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
            CvMemStorage* storage = cvCreateMemStorage(0);  //����һ���ڴ�洢��������ֵ���´������ڴ�洢����ָ��
            CvSeq* contours = 0;  //������һ������
            CvMat* color_tab;    //����仯����
            int i, j, comp_count = 0;    //comp_countΪ���ӵ����&�������&�߽����&�ڽӱ�ľ���߳�
            cvSaveImage( "wshed_mask.png", marker_mask );
            marker_mask = cvLoadImage( "wshed_mask.png", 0 );
            cvFindContours( marker_mask, storage, &contours, sizeof(CvContour),
                            CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );   //�Ӷ�ֵͼ���м��������������ؼ�⵽�������ĸ���
            cvZero( markers );
            for( ; contours != 0; contours = contours->h_next, comp_count++ )
            {
                cvDrawContours( markers, contours, cvScalarAll(comp_count+1),
                                cvScalarAll(comp_count+1), -1, -1, 8, cvPoint(0,0) );  //��ͼ���ϻ����ⲿ���ڲ�����
            }

            color_tab = cvCreateMat( 1, comp_count, CV_8UC3 );  //���������ɫ�б�   //ת����˾���   //RGB3ͨ������CV_8UC3
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
                    int idx = CV_IMAGE_ELEM( markers, int, i, j );  //BGR����ͨ��������һ���,��Ҫj*3
                    uchar* dst = &CV_IMAGE_ELEM( wshed, uchar, i, j*3 );
                    if( idx == -1 )   //���ʱ��Ϊ-1����ʾ�������ֵı߽�
                        dst[0] = dst[1] = dst[2] = (uchar)255;
                    else if( idx <= 0 || idx > comp_count )    //�쳣���
                        dst[0] = dst[1] = dst[2] = (uchar)0; // should not get here
                    else
                    {
                        uchar* ptr = color_tab->data.ptr + (idx-1)*3;
                        dst[0] = ptr[0]; dst[1] = ptr[1]; dst[2] = ptr[2];
                    }
                }
            CheckNear(NearMat,comp_count,markers);
            CaArea(NearMat,ColorMat,comp_count);

            cvAddWeighted( wshed, 0.5, img_gray, 0.5, 0, wshed );                           /*�̡�??��??��?����? ??��??-��???��?��3?�� image_gray?��???��?����??-????��?img0��?D??��*/
            cvShowImage( "watershed transform", wshed );

            PaintArea(ColorMat,markers,wshed,comp_count);                   /**/
            cvAddWeighted( wshed, 0.5, img_gray, 0.5, 0, wshed );
            cvShowImage( "watershed transform", wshed );

            int *AreaSize;               //�����С
            AreaSize=(int *) malloc(sizeof(int)*comp_count);

            GetAreaSize(AreaSize,comp_count);   //�õ������С

            int **AreaOrder;                  //�����С�����

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
            int xx=600,yy=40;  //���ʵĳ�ʼ����������������
            DrawTree(tree,20,depth,xx,yy);
            cvShowImage( "treee", tre);  //�½�ͼ�񴰿�
            //getchar();
            /*
            char hcode[20];
            printf("������������:\n");

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

 //������
int max_child(int *arr,int father,int length)   //�ҵ��������ݣ�Ȩֵ�����ֵ��
{
    int left;
    int right;
    int temp;

    //�����ڵ������һ�����ݣ��򷵻���ڵ�
    if(father*2+2 == length)
    {
        return father*2+1;
    }

    //�����ڵ�û�����ݣ��򷵻�length
    if((father*2+1)>=length)
    {
        return length;
    }

    //�õ������ڵ������
    temp = father*2+1;  //��ڵ��±�
    left = arr[temp];
    right = arr[temp+1];

    //�Ƚϲ����ؽϴ����ݵ��±�
    if(left>right)
    {
        return temp;
    }
    else
    {
        return temp+1;
    }
}

//������
void swap(int p1,int p2,int *arr,int *AreaNum)  //�����������ݣ��������Ϊ�±�
{
    //����Ȩֵ
    int temp = arr[p1];  
    arr[p1] = arr[p2];
    arr[p2] = temp;

    //�����������
    int temp2 = AreaNum[p1];
    AreaNum[p1] = AreaNum[p2];
    AreaNum[p2] = temp2;
}

//������
void reshape(int node,int length,int *arr,int *AreaNum)  //�Ƚ�������½��л��ơ��ѡ�
{

    while(node<=length)
    {
        //�õ��ӽڵ�ϴ����ݵ��±�
        int child = max_child(arr,node,length);
        if(child >= length)
        {
            //����ӽڵ�û�����ݣ��򷵻�
            break;
        }

        //����ӽڵ�ȸ��ڵ��
        if(arr[node]<arr[child])
        {
           //�򽻻����ڵ����ӽڵ�
            swap(node,child,arr,AreaNum);
           //����Խ�����ȥ�ĸ��ڵ��ٽ����ж�
            node = child;
        }
        else
        {
            //���ڵ���ӽڵ��нϴ�����ݴ��򷵻�
            break;
        }
    }
}

//������
//void Display(int comp_count)  //��ʾ��������
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

//������
void heap_sort(int comp_count,int *AreaSize,int *AreaNum)  //���еĶ������������
{
    int i;

    for(i=comp_count/2-1;i>=0;i--)
    {
        reshape(i,comp_count,AreaSize,AreaNum);
    }

    //��ȡ��������ѭ������
    for(i=comp_count-1;i>0;i--)
    {
        swap(0,i,AreaSize,AreaNum);
        reshape(0,i,AreaSize,AreaNum);
    }
}


//����������
Huffman* BuildHuffmanTree(int *list,int length)   //��������������庯��
{
    //һϵ�еĳ�ʼ��
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

    //�����������㷨
    while(TreeNum>1)   //ɭ��������
    {
        Tree2=(Huffman *) malloc(sizeof(Huffman));
        if(forest[0]->depth>forest[1]->depth)
        {
            temp=forest[0];
            forest[0]=forest[1];
            forest[1]=temp;
        }
        Tree2->left=forest[0];      //ԭ�����Ϊ����
        Tree2->right=forest[1];     //ԭ�����Ϊ�Һ���
        Tree2->area=forest[0]->area+forest[1]->area;  //�����ɽ���ֵ
        Tree2->depth=forest[1]->depth+1;   //��ȼ�һ
        for(j=2;j<TreeNum&&(forest[j]->area)<=(Tree2->area)/*&&(forest[j]->depth)<=(Tree2->depth)*/;j++)
        {
            forest[j-2]=forest[j];  //����
        }
        forest[j-2]=Tree2;
        for(k=j-1;k<TreeNum-1;k++)
        {
            forest[k]=forest[k+1];  //������һ���ж�
        }
        TreeNum--;      //���һ��
    }
    free(forest);

    return Tree2;  //���ؼ���õĹ�������
}

//����������
void DrawTree(BinaryTree *tree,int i,int yyy,int xx,int yy)  //���������Ŀ��ӻ�
{
    if(i>20)
    {
        printf("wrong!");   //Ҷ�ӽڵ����20����Ϊ��ʾ���󣡣�20��λ�õ�ֵ�������ã���Ҫ���ǵ���20������Ļ���ɲ��ˣ�
        return;
    }
    yyy++;
    CvFont font;
    cvInitFont(&font,CV_FONT_HERSHEY_DUPLEX,0.5,0.5,0,2,8);  //��ʼ��ͼ��
    if(yyy<=i)
    {
        cvRectangle(tre,cvPoint(xx-10,yy+10),cvPoint(xx+60,yy-20),cvScalarAll(255),CV_FILLED,8,0);   //����ʾ���Ȩֵ�ľ���
        char a[10];
        itoa(tree->area,a,10);   //char int����
        cvPutText(tre,a,cvPoint(xx,yy),&font,CV_RGB(0,0,0));   //дȨֵ
        if(tree->left!=NULL)  //����������
        {
            if(yyy==i)
                return;
            if(yyy==1)  //���
            {
                cvLine(tre,cvPoint(xx-10,yy+10),cvPoint(xx-200,yy+40),cvScalarAll(255),1,8,0);  //�����������εĿ�
                cvPutText(tre,"0",cvPoint(xx-195,yy+30),&font,cvScalarAll(255));   //��������Ϊ0
                DrawTree(tree->left,i,yyy,xx-220,yy+60);
            }
            if(yyy==2)
            {
                cvLine(tre,cvPoint(xx-10,yy+10),cvPoint(xx-100,yy+40),cvScalarAll(255),1,8,0);   //�����������εĿ�
                cvPutText(tre,"0",cvPoint(xx-95,yy+30),&font,cvScalarAll(255)); 
                DrawTree(tree->left,i,yyy,xx-180,yy+60);
            }
            if(yyy==3)
            {
                cvLine(tre,cvPoint(xx-10,yy+10),cvPoint(xx-55,yy+40),cvScalarAll(255),1,8,0);  //�����������εĿ�
                cvPutText(tre,"0",cvPoint(xx-35,yy+30),&font,cvScalarAll(255));
                DrawTree(tree->left,i,yyy,xx-80,yy+60);
            }
            if(yyy>=4)
            {
                cvLine(tre,cvPoint(xx-10,yy+10),cvPoint(xx-15,yy+40),cvScalarAll(255),1,8,0);  //�����������εĿ�
                cvPutText(tre,"0",cvPoint(xx-15,yy+30),&font,cvScalarAll(255));
                DrawTree(tree->left,i,yyy,xx-40,yy+60);
            }
        }
        if(tree->right!=NULL)   //����������
        {
            if(yyy==i)
                return;
            if(yyy==1)
            {
                cvLine(tre,cvPoint(xx+60,yy+10),cvPoint(xx+220,yy+40),cvScalarAll(255),1,8,0);   //�����������εĿ�
                cvPutText(tre,"1",cvPoint(xx+200,yy+30),&font,cvScalarAll(255));        //��������Ϊ0
                DrawTree(tree->right,i,yyy,xx+200,yy+60);  
            }
            if(yyy==2)
            {
                cvLine(tre,cvPoint(xx+60,yy+10),cvPoint(xx+150,yy+40),cvScalarAll(255),1,8,0);   //�����������εĿ�
                cvPutText(tre,"1",cvPoint(xx+120,yy+30),&font,cvScalarAll(255));
                DrawTree(tree->right,i,yyy,xx+140,yy+60);
            }
            if(yyy==3)
            {
                cvLine(tre,cvPoint(xx+60,yy+10),cvPoint(xx+90,yy+40),cvScalarAll(255),1,8,0);   //�����������εĿ�
                cvPutText(tre,"1",cvPoint(xx+60,yy+30),&font,cvScalarAll(255));
                DrawTree(tree->right,i,yyy,xx+85,yy+60);
            }
            if(yyy>=4)
            {
                cvLine(tre,cvPoint(xx+60,yy+10),cvPoint(xx+65,yy+40),cvScalarAll(255),1,8,0);    //�����������εĿ�
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

//����������
void bianma(char *aa,Huffman *tree,int i)  //����������ѭ��
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

//����������
void bian_ma(Huffman *tree)  //������
{
    tree->code=0;
    tree->codeall[0]='\0';
    char a[20]="\0";
    int i=0;
    bianma(a,tree,i);
}

//����������
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
}

//����������
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

//����������
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

//������
void SetRand(int Drop)           //���������Ĵ�����Ϊ��ֹ���ɵĵ㲻�����
{
    int i=0;
    srand(time(NULL));
    for(i=0;i<Drop;i++)
    {
       rand();
    }
}

//������
int GetRand(int min,int max)      //���������      
{
    if(max>30000||min<0)      //����
    {
        return -1;
    }          
    else
    {
        return (int) (((double) rand()/32767.0)*(max-min)+min);  //����һ���������Сֵ֮�����������
    }
}

//������
double GetDistance(CvPoint Point1,CvPoint Point2)   //����֮�����
{
    return sqrt((double) ( (Point1.x - Point2.x)*(Point1.x - Point2.x)+(Point1.y - Point2.y)*(Point1.y - Point2.y) ) );
}

//������
CvPoint GetPoint(CvPoint Center,int Inner,int Out,int PointNumber,int MaxNum)  //�ҵ㺯��
{
    double PartWidth=360.0/((double) MaxNum);
    double r=(double) GetRand(Inner,Out);
    int RandNumber=rand();
    double RandInPart=RandNumber/(32767.0);   //����һ��0-1������
    double Deg=RandInPart*PartWidth+(PointNumber-1)*PartWidth;   //��
    CvPoint Point;
    Point.x=Center.x+(int) r*cos(Deg*PI/180);
    Point.y=Center.y+(int) r*sin(Deg*PI/180);
    return Point;
}

//������
int JudgePoint(CvPoint* PointList,int Numberlist,CvPoint OnCheckPoint,int Distance,int BorderX,int BorderY)  //�ж��ҵĵ���������ľ���
{
    int i=0;
    if(OnCheckPoint.x>BorderX||OnCheckPoint.y>BorderY||OnCheckPoint.x<0||OnCheckPoint.y<0)  //����ͼ��Χ��
    {
        return 0;
    }
    for(i=0;i<=Numberlist;i++)
    {

        if(GetDistance(PointList[i],OnCheckPoint)<Distance)   //�������r������
        {
            return 0;
        }
    }
    return 1;
}

//������ɫ
int GetBiggestItem(int* List,int ListLength)     //Ѱ���ڽ���������������
{
    int MaxValue=List[0];
    int MaxID=0;
    int i=0;
    for(i=0;i<ListLength;i++)   //����
    {
        if(List[i]>MaxValue)
        {
            MaxValue=List[i];   //����󣬾ͽ���
            MaxID=i;
        }
    }
    return MaxID;  //���ֵ�ı��
}

//������ɫ
int FindItem(int* List,int ListLength,int Item)  //Ѱ���ض���Ԫ��
{
    for(int i=0;i<ListLength;i++)
    {
        if(Item==List[i])     //Ԫ�ض�Ӧ����
            return i+1;
    }
    return 0;
}

//������ɫ
int GetCorlorAround(int* AreaColor,int NumAreaAll,int** AdjacencyList,int CurrentArea)  //���ĳ������Χ����ɫ
{
    int i=0;
    int ColorList[10];
    int ColorListID=0;
    int counter=0;
    int ColorAround;
    for(i=0;i<NumAreaAll;i++)    //����
    {
        if(AdjacencyList[CurrentArea][i]==1)    //�������
        {
            if(FindItem(ColorList,10,AreaColor[i])==0)  //��ɫ
            {
                ColorAround++;       
                ColorList[ColorListID]=AreaColor[i];
                ColorListID++;
            }
        }
    }
    return ColorAround;
}

//������ɫ
int GetNextArea(int* AreaColor,int NumAreaAll,int NumPaintedArea,int **AdjacencyList)   //��ú�ĳһ������󣬽�����һ���ٽ�������ж�
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
            ColorAround[i]=GetCorlorAround(AreaColor,NumAreaAll,AdjacencyList,i);  //��ɫ�ļ�¼
        }
    }
    BestArea=GetBiggestItem(ColorAround,NumAreaAll);   //�ڽ�����������Ϊ���ѡ������
    if(ColorAround[BestArea]==0)
    {
        BestArea=keep;
    }
    free(ColorAround);
    return BestArea;
}


//������ɫ
CheckPaint(int* AreaColor,int NumArea,int** AdjacencyList,int AreaID)  //����
{
    int i=0;
    for(i=0;i<NumArea;i++)
    {
        if(i==AreaID) 
        {
           continue; 
        }
        if(AdjacencyList[AreaID][i]==1&&AreaColor[i]==AreaColor[AreaID])  //��ȷ
        {
           return 0;
        }
    }
    return 1;
}

//������ɫ
int GetAreaColor(int* AreaColor,int NumArea,int** AdjacencyList)  //��������ɫ
{
    int i=0,j=0;  //��ʼ��
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
        while(AreaColor[NextAreaID]<=4)    //��ɫ��
        {
            if(CheckPaint(AreaColor,NumArea,AdjacencyList,NextAreaID)==0)
            {
                AreaColor[NextAreaID]++;   //�ڽӻ���ɫ
            }
            else
            {
                Order[i]=NextAreaID;
                i++;
                NextAreaID=GetNextArea(AreaColor,NumArea,i,AdjacencyList);
                break;
            }
        }
        if(AreaColor[NextAreaID]>4)  //�˻أ�������
        {
            AreaColor[NextAreaID]=0;
            i--;
            NextAreaID=Order[i];
        }
    }
    free(Order);
    return 0;
}

//������ɫ
int GetAdjacencyList(int **AdjacencyList,int NumArea,int Width,int Height)     //�õ��ڽӾ���
{
    int i=0,j=0;
    for(i=0;i<NumArea;i++)   //��ʼ���ڽӾ���
        for(j=0;j<NumArea;j++)
            AdjacencyList[i][j]=0;

    for(i=1;i<markers->height-1;i++)
    {
        for(j=1;j<markers->width-1;j++)
        {
            int idx=CV_IMAGE_ELEM(markers,int ,i,j);
            if(idx==-1)          //-1Ϊ�߽磬����0������Ϊ�߽�
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
                            idxAround[count]=CV_IMAGE_ELEM(markers,int,i-1+x,j-1+y);    //��ȡĳ���������Ϣ
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
                            idxList[idxListNum]=idxAround[k];   //��¼ÿһ������ڽӹ�ϵ
                            idxListNum++;
                        }
                    }
                }
                for(x=0;x<idxListNum;x++)
                {
                    for(int y=0;y<idxListNum;y++)
                    {
                        AdjacencyList[idxList[x]-1][idxList[y]-1]=1;   //�洢
                    }
                }
            }
        }
    }
    return 0;
}

//������ɫ
void CheckNear(int (*p)[MAX],int n,IplImage *img)                   //����ڽ�����
{
    int i,j,idx;
    int idx1,idx2;

    for(i=0;i<MAX;i++)              //��ʼ��
    {
        for(j=0;j<MAX;j++)
        {
            p[i][j]=0;
        }
    }

    for( i = 1; i < img->height-1; i++ ) //������ͼƬ�������ر���һ��
    {
        for( j = 1; j < img->width-1; j++ )
        {
            idx = CV_IMAGE_ELEM( img, int, i, j );    //���е�����ڰ˸���
            if( idx == -1 )    //����
            {
                idx1= CV_IMAGE_ELEM( img, int, i, j-1 );    
                idx2= CV_IMAGE_ELEM( img, int, i, j+1 );
                if(idx1!=idx2&&idx1!=-1&&idx2!=-1)
                {
                    p[idx1][idx2]=1;              //���ڣ���¼                         
                    p[idx2][idx1]=1;                                    
                }

            }
        }

    }
}

//������ɫ
int DrawMark(int comp_count,int **AreaOrder)    //���������С��д����
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
                if(idx==counters+1)    //�ڲ���
                {
                    record_y+=i;
                    record_x+=j;
                    num_same_point++;   //������
                }
            }
         }
         sprintf(string,"%d",FindItem(AreaOrder[0],comp_count,counters));   //����
         cvPutText(wshed,string,cvPoint(record_x/num_same_point,record_y/num_same_point),&font,cvScalar(0));   //����
         for(i=0;i<10;i++) 
        {
            string[i]=0;  //���㡤
        }
    }

    return 0;
}

//������ɫ
int GetAreaSize(int *AreaSize,int num)   //�������
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
            elem=CV_IMAGE_ELEM(markers,int ,i,j);   //һ��һ�����ص����
            if(elem<=num&&elem>0)
            {
                AreaSize[elem-1]++;    //��Ӧ�����һ
            }
        }
    }
    return 0;
}

//������ɫ
void CaArea(int (*p)[MAX],int *q,int n)   //ȡ����ɫ����       
{
    int i;
    int idx=0;
    int t;
    for(i=0;i<MAX;i++)     
    {
        q[i]=0;
    }
    for(;idx<n&&idx>=0;)  //����ջԭ��
    {
        q[idx]+=1;                                  
        for(;q[idx]<5;)             //����ɫ��ѭ��                
        {
            t=q[idx];
            for(i=0;i<n;i++)
            {
                if(i==idx)
                    continue;
                if(q[idx]==q[i]&&p[idx][i]==1&&q[idx]!=0)  //ȡ������ȷ����ɫ
                {
                    q[idx]++;
                    break;
                }
            }
            if(t==q[idx])  //��¼���˳�ѭ��
                break;
        }
        if(q[idx]>4)   //����4ɫ��������
        {
            q[idx]=0;                               
            idx--;
        }
        else if(q[idx]<=4&&q[idx]>0) //�����������
            idx++;
        else
        {
            printf("wrong!");
            exit(1);
        }
    }
}

//������ɫ
void PaintArea(int *q,IplImage *img1,IplImage *img2,int n)               //��������ɫ                                                                                   
{
    CvMat *color_tab;
    int i,j;

    color_tab = cvCreateMat( 1, 4, CV_8UC3 );                                           //�����µľ���

        uchar* ptr = color_tab->data.ptr + 0*3;                                         //��¼
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
    cvReleaseMat( &color_tab );                                                         //�ͷ�
}
