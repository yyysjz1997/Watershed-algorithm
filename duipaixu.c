// #include <stdio.h>


// // #define MAX_LEN 17

// //从0开始索引
// // int arr[MAX_LEN] = {
// //     50,40,70,80,60,30,20,100,
// //     90,10,120,130,150,180,190,160,
// //     200,
// // };


// //输入变量1： 父节点
// //输入变量2： 数据总长度
// //根据输入变量，父节点，得到其最大子节点下标
// //如果超出范围，则返回length
// int max_child(int *arr,int father,int length)
// {
//     int left;
//     int right;
//     int temp;

//     //如果左节点是最后一个数据，则返回左节点
//     if(father*2+2 == length){
//         return father*2+1;
//     }

//     //如果左节点没有数据，则返回length
//     if((father*2+1)>=length){
//         return length;
//     }

//     //得到两个节点的数据
//     temp = father*2+1;  //左节点下标
//     left = arr[temp];
//     right = arr[temp+1];

//     //比较并返回较大数据的下标
//     if(left>right){
//         return temp;
//     }else{
//         return temp+1;
//     }
// }

// //交换两个数据，输入变量为下标
// void swap(int p1,int p2)
// {
//     int temp = arr[p1];
//     arr[p1] = arr[p2];
//     arr[p2] = temp;
// }

// //输入变量1： 已知有变化，可能不满足大树要求的节点
// //输入变量2： 数据总长度
// //重新整理为完全大树
// void reshape(int node,int length)
// {
//     //防止意外..
//     while(node<=length)
//     {
//         //得到子节点较大数据的下标
//         int child = max_child(node,length);
//         if(child >= length){
//             //如果子节点没有数据，则返回
//             break;
//         }

//         //如果子节点比父节点大
//         if(arr[node]<arr[child]){
//             //则交换父节点与子节点
//             swap(node,child);
//             //并针对交换下去的父节点再进行判断
//             node = child;
//         }else{
//             //父节点比子节点中较大的数据大，则返回
//             break;
//         }
//     }
// }

// //显示
// // void Display(int comp_count)
// // {
// //     int i;
// //     for(i=0;i<comp_count;i++){
// //         if(i%4 == 0){
// //             //控制换行
// //             printf("\n");
// //         }
// //         printf("%4d\t",arr[i]);
// //     }
// // }

// //堆排序的逻辑实现
// void heap_sort(int comp_count)
// {
//     int i;

//     //输出原始数据
//     // printf("\nBefore:\n");
//     // Display(comp_count);


//     for(i=MAX_LEN/2-1;i>=0;i--){
//         reshape(i,comp_count);
//     };
//     //输出构建完第一个树之后的数据
//     // printf("\nAfter:Build First\n");
//     // Display(comp_count);

//     //提取大数，并循环整理
//     for(i=comp_count-1;i>0;i--){
//         swap(0,i);
//         reshape(0,i);
//     }
//     // printf("\nAfter:Sort\n");
//     // Display(comp_count);
// }


//堆排序的实现，自己理解使用...有一些偷懒

#include <stdio.h>


#define MAX_LEN 17

//从0开始索引
int arr[MAX_LEN] = {
    50,40,70,80, 60,30,20,100,
    90,10,120,130, 150,180,190,160,
    200,
};



//输入变量1： 父节点
//输入变量2： 数据总长度
//根据输入变量，父节点，得到其最大子节点下标
//如果超出范围，则返回length
int max_child(int father,int length)
{
    int left;
    int right;
    int temp;

    //如果左节点是最后一个数据，则返回左节点
    if(father*2+2 == length){
        return father*2+1;
    }

    //如果左节点没有数据，则返回length
    if((father*2+1)>=length){
        return length;
    }

    //得到两个节点的数据
    temp = father*2+1;  //左节点下标
    left = arr[temp];
    right = arr[temp+1];

    //比较并返回较大数据的下标
    if(left>right){
        return temp;
    }else{
        return temp+1;
    }
}

//交换两个数据，输入变量为下标
void swap(int p1,int p2)
{
    int temp = arr[p1];
    arr[p1] = arr[p2];
    arr[p2] = temp;
}

//输入变量1： 已知有变化，可能不满足大树要求的节点
//输入变量2： 数据总长度
//重新整理为完全大树
void reshape(int node,int length)
{
    //防止意外..
    while(node<=length)
    {
        //得到子节点较大数据的下标
        int child = max_child(node,length);
        if(child >= length){
            //如果子节点没有数据，则返回
            break;
        }

        //如果子节点比父节点大
        if(arr[node]<arr[child]){
            //则交换父节点与子节点
            swap(node,child);
            //并针对交换下去的父节点再进行判断
            node = child;
        }else{
            //父节点比子节点中较大的数据大，则返回
            break;
        }
    }
}

//显示
void Display(void)
{
    int i;
    for(i=0;i<MAX_LEN;i++){
        if(i%4 == 0){
            //控制换行
            printf("\n");
        }
        printf("%4d\t",arr[i]);
    }
}

//堆排序的逻辑实现
void heap_sort(void)
{
    int i;

    //输出原始数据
    printf("\nBefore:\n");
    Display();


    for(i=MAX_LEN/2-1;i>=0;i--){
        reshape(i,MAX_LEN);
    };
    //输出构建完第一个树之后的数据
    printf("\nAfter:Build First\n");
    Display();

    //提取大数，并循环整理
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



