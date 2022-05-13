#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "../code/Data_Structure.h"
#include "BPlusTree.cpp"

// 随机建立一棵树
void Test1(BPlusTree *pTree, int count)
{
    pTree->ClearTree();

    // srand( (unsigned)time( NULL ) );//这是一个种子，如果不要随机功能，请把此句话注释掉
    for (int i = 0; i < count; i++)
    {
        int x = rand() % 999 + 1;
        (void)pTree->Insert(x, NULL); //
    }

    printf("successed!\n");
}

// 在树中查找某数据
void Test2(BPlusTree *pTree, int data)
{
    // char sPath[255] = {
    //     0,
    // };

    Registration *r = pTree->Search(data);
    if (r == NULL)
    {
        cout << "Search failed!" << endl;
        return;
    }

    cout << "key=" << data << " Reg_id=" << r->reg_id << endl;
    
}

// 在树中插入某数据
void Test3(BPlusTree *pTree, int data, Registration *Reg)
{
    bool success = pTree->Insert(data, Reg);
    if (true == success)
    {
        printf("\nsuccessed!\n");
    }
    else
    {
        printf("\nfailed!\n");
    }
}

// 在树中删除某数据
void Test4(BPlusTree *pTree, int data)
{
    bool success = pTree->Delete(data);
    if (true == success)
    {
        printf("\nsuccessed!\n");
    }
    else
    {
        printf("\nfailed!\n");
    }
}

// // 对树进行旋转
// BPlusTree *Test5(BPlusTree *pTree)
// {
//     BPlusTree *pNewTree = pTree->RotateTree();
//     delete pTree;
//     printf("\nsuccessed!\n");
//     return pNewTree;
// }

// 打印
void Test6(BPlusTree *pTree)
{
    pTree->PrintTree();
}

// 对树进行检查
void Test7(BPlusTree *pTree)
{
    bool success = pTree->CheckTree();
    if (true == success)
    {
        printf("\nsuccessed!\n");
    }
    else
    {
        printf("\nfailed!\n");
    }
}


int main(int argc, char *argv[])
{
    BPlusTree *pTree = new BPlusTree;

    int x = 1;
    int y = 0;
    Registration *n = NULL;
    
    while (0 != x)
    {
        printf("\n\n");
        printf("    *******************************************************************\n");
        printf("    *           欢迎进入B+树演示程序，请选择相应功能。                *\n");
        printf("    *           1。随机建立一棵B+树；                                 *\n");
        printf("    *           2。在B+树中查找一个数；                               *\n");
        printf("    *           3。在B+树中插入一个数(Reg=NULL)；                     *\n");
        printf("    *           4。在B+树中删除一个数；                               *\n");
        // printf("    *           5。对B+树旋转，进行重新平衡；                         *\n");
        printf("    *           6。显示整个B+树；                                     *\n");
        printf("    *           7。检查整个B+树；                                     *\n");
        printf("    *           0。退出程序；                                         *\n");
        printf("    *           11.Test(Insert 16 Reg information);                  *\n");
        printf("    *           12.Clear Tree;                                        *\n");
        printf("    *******************************************************************\n");
        printf("\n    您的选择是：");

        scanf("%d", &x);
        switch (x)
        {
        case 1:
            printf("请输入数据个数(10-150)：");
            scanf("%d", &y);
            Test1(pTree, y);
            break;

        case 2:
            printf("请输入要查找的数值：");
            scanf("%d", &y);
            Test2(pTree, y);
            break;

        case 3:
            printf("请输入要插入的数值与注册信息：");
            scanf("%d", &y);
            Test3(pTree, y, n);
            break;

        case 4:
            printf("请输入要删除的数值：");
            scanf("%d", &y);
            Test4(pTree, y);
            break;

        // case 5:
        //     pTree = Test5(pTree);
        //     break;

        case 6:
            Test6(pTree);
            break;

        case 7:
            Test7(pTree);
            break;

        case 0:
            delete pTree;
            return 0;
            break;

        case 11:
        {
            Registration *r[16];
            int key[16] = {8, 14, 2, 15, 3, 1, 16, 6, 5, 27, 37, 18, 25, 7, 13, 20};
            for (int i = 0; i < 16; i++)
            {
                r[i] = new Registration;
                r[i]->reg_id = i;
                pTree->Insert(key[i], r[i]);
                cout << "Insert " << key[i] << endl;
            }
            for (int i = 0; i < 16; i++)
            {
                cout << r[i]->reg_id << endl;
            }
            
            break;
        }
        case 12:
        {
            pTree->ClearTree();
            break;
        }
        // case 17: (in test_Vaccination_System.cpp)
        // {
        //     int d[21] = {0,11,21,31,41,51,61,71,81,91,100,4,14,24,75,95,34,74,72,3,8};
        //     for (int i = 0; i < 21; i++)
        //     {
        //         system.Reg_List.Delete(d[i]);
        //         system.Reg_List.PrintTree();
        //     }
        //     break;
        // }
        default:
            break;
        }
    }

    delete pTree;
    return 0;
}