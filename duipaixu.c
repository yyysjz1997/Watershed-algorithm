// #include <stdio.h>


// // #define MAX_LEN 17

// //��0��ʼ����
// // int arr[MAX_LEN] = {
// //     50,40,70,80,60,30,20,100,
// //     90,10,120,130,150,180,190,160,
// //     200,
// // };


// //�������1�� ���ڵ�
// //�������2�� �����ܳ���
// //����������������ڵ㣬�õ�������ӽڵ��±�
// //���������Χ���򷵻�length
// int max_child(int *arr,int father,int length)
// {
//     int left;
//     int right;
//     int temp;

//     //�����ڵ������һ�����ݣ��򷵻���ڵ�
//     if(father*2+2 == length){
//         return father*2+1;
//     }

//     //�����ڵ�û�����ݣ��򷵻�length
//     if((father*2+1)>=length){
//         return length;
//     }

//     //�õ������ڵ������
//     temp = father*2+1;  //��ڵ��±�
//     left = arr[temp];
//     right = arr[temp+1];

//     //�Ƚϲ����ؽϴ����ݵ��±�
//     if(left>right){
//         return temp;
//     }else{
//         return temp+1;
//     }
// }

// //�����������ݣ��������Ϊ�±�
// void swap(int p1,int p2)
// {
//     int temp = arr[p1];
//     arr[p1] = arr[p2];
//     arr[p2] = temp;
// }

// //�������1�� ��֪�б仯�����ܲ��������Ҫ��Ľڵ�
// //�������2�� �����ܳ���
// //��������Ϊ��ȫ����
// void reshape(int node,int length)
// {
//     //��ֹ����..
//     while(node<=length)
//     {
//         //�õ��ӽڵ�ϴ����ݵ��±�
//         int child = max_child(node,length);
//         if(child >= length){
//             //����ӽڵ�û�����ݣ��򷵻�
//             break;
//         }

//         //����ӽڵ�ȸ��ڵ��
//         if(arr[node]<arr[child]){
//             //�򽻻����ڵ����ӽڵ�
//             swap(node,child);
//             //����Խ�����ȥ�ĸ��ڵ��ٽ����ж�
//             node = child;
//         }else{
//             //���ڵ���ӽڵ��нϴ�����ݴ��򷵻�
//             break;
//         }
//     }
// }

// //��ʾ
// // void Display(int comp_count)
// // {
// //     int i;
// //     for(i=0;i<comp_count;i++){
// //         if(i%4 == 0){
// //             //���ƻ���
// //             printf("\n");
// //         }
// //         printf("%4d\t",arr[i]);
// //     }
// // }

// //��������߼�ʵ��
// void heap_sort(int comp_count)
// {
//     int i;

//     //���ԭʼ����
//     // printf("\nBefore:\n");
//     // Display(comp_count);


//     for(i=MAX_LEN/2-1;i>=0;i--){
//         reshape(i,comp_count);
//     };
//     //����������һ����֮�������
//     // printf("\nAfter:Build First\n");
//     // Display(comp_count);

//     //��ȡ��������ѭ������
//     for(i=comp_count-1;i>0;i--){
//         swap(0,i);
//         reshape(0,i);
//     }
//     // printf("\nAfter:Sort\n");
//     // Display(comp_count);
// }


//�������ʵ�֣��Լ����ʹ��...��һЩ͵��

#include <stdio.h>


#define MAX_LEN 17

//��0��ʼ����
int arr[MAX_LEN] = {
    50,40,70,80, 60,30,20,100,
    90,10,120,130, 150,180,190,160,
    200,
};



//�������1�� ���ڵ�
//�������2�� �����ܳ���
//����������������ڵ㣬�õ�������ӽڵ��±�
//���������Χ���򷵻�length
int max_child(int father,int length)
{
    int left;
    int right;
    int temp;

    //�����ڵ������һ�����ݣ��򷵻���ڵ�
    if(father*2+2 == length){
        return father*2+1;
    }

    //�����ڵ�û�����ݣ��򷵻�length
    if((father*2+1)>=length){
        return length;
    }

    //�õ������ڵ������
    temp = father*2+1;  //��ڵ��±�
    left = arr[temp];
    right = arr[temp+1];

    //�Ƚϲ����ؽϴ����ݵ��±�
    if(left>right){
        return temp;
    }else{
        return temp+1;
    }
}

//�����������ݣ��������Ϊ�±�
void swap(int p1,int p2)
{
    int temp = arr[p1];
    arr[p1] = arr[p2];
    arr[p2] = temp;
}

//�������1�� ��֪�б仯�����ܲ��������Ҫ��Ľڵ�
//�������2�� �����ܳ���
//��������Ϊ��ȫ����
void reshape(int node,int length)
{
    //��ֹ����..
    while(node<=length)
    {
        //�õ��ӽڵ�ϴ����ݵ��±�
        int child = max_child(node,length);
        if(child >= length){
            //����ӽڵ�û�����ݣ��򷵻�
            break;
        }

        //����ӽڵ�ȸ��ڵ��
        if(arr[node]<arr[child]){
            //�򽻻����ڵ����ӽڵ�
            swap(node,child);
            //����Խ�����ȥ�ĸ��ڵ��ٽ����ж�
            node = child;
        }else{
            //���ڵ���ӽڵ��нϴ�����ݴ��򷵻�
            break;
        }
    }
}

//��ʾ
void Display(void)
{
    int i;
    for(i=0;i<MAX_LEN;i++){
        if(i%4 == 0){
            //���ƻ���
            printf("\n");
        }
        printf("%4d\t",arr[i]);
    }
}

//��������߼�ʵ��
void heap_sort(void)
{
    int i;

    //���ԭʼ����
    printf("\nBefore:\n");
    Display();


    for(i=MAX_LEN/2-1;i>=0;i--){
        reshape(i,MAX_LEN);
    };
    //����������һ����֮�������
    printf("\nAfter:Build First\n");
    Display();

    //��ȡ��������ѭ������
    for(i=MAX_LEN-1;i>0;i--){
        swap(0,i);
        reshape(0,i);
    }
    printf("\nAfter:Sort\n");
    Display();
}


int main(void)
{
    heap_sort();
//  system("pause");
    return 0;
}



