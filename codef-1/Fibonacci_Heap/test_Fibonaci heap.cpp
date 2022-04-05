/**
 * C 语言: 斐波那契堆
 *
 * @author skywang
 * @date 2014/04/06
 */

#include <iostream>
#include "Fibonaci heap.h"
using namespace std;

#define DEBUG 0

// 共8个
// int a[] = {12,  7, 25, 15, 28,
//            33, 41,  1};
// 共14个
// int b[] = {18, 35, 20, 42,  9,
//            31, 23,  6, 48, 11,
//            24, 52, 13,  2};

// string a[] = {"12",  "7", "25", "15", "28",
//            "33", "41",  "1"};

// string b[] = {"18", "35", "20", "42",  "9",
//            "31", "23",  "6", "48", "11",
//            "24", "52", "13",  "2"};

string a[] = {"1234", "1235", "1334", "1245", "2345", "2134"};
string b[] = {"123", "456", "213", "134", "331", "611"};


// 验证"基本信息(斐波那契堆的结构)"
void testBasic()
{
    int i;
    int blen=sizeof(b)/sizeof(b[0]);
    FibHeap<string>* hb=new FibHeap<string>();

    // 斐波那契堆hb
    cout << "== Add successively into heap_hb: ";
    for(i=0; i<blen; i++)
    {
        cout << b[i] <<" ";
        hb->insert(b[i]);
    }
    cout << endl;
    cout << "== Delete minimum node (hb)" << endl;
    hb->removeMin();
    hb->print();
}

// 验证"插入操作"
void testInsert()
{
    int i;
    int alen=sizeof(a)/sizeof(a[0]);
    FibHeap<string>* ha=new FibHeap<string>();

    cout << "== Add successively into heap_ha: ";
    for(i=0; i<alen; i++)
    {
        cout << a[i] <<" ";
        ha->insert(a[i]);
    }
    cout << endl;
    cout << "== Delete minimum node (ha)" << endl;
    ha->removeMin();
    ha->print();

    // 斐波那契堆ha
    cout << "== Insert 50" << endl;
    ha->insert("50");
    ha->print();
}

// 验证"合并操作"
void testUnion()
{
    int i;
    int alen=sizeof(a)/sizeof(a[0]);
    int blen=sizeof(b)/sizeof(b[0]);
    FibHeap<string>* ha=new FibHeap<string>();
    FibHeap<string>* hb=new FibHeap<string>();

    cout << "== Add successively into heap_ha: ";
    for(i=0; i<alen; i++)
    {
        cout << a[i] <<" ";
        ha->insert(a[i]);
    }
    cout << endl;
    cout << "== Delete minimum node (ha)" << endl;
    ha->removeMin();
    ha->print();

    // 斐波那契堆hb
    cout << "== Add successively into heap_hb: ";
    for(i=0; i<blen; i++)
    {
        cout << b[i] <<" ";
        hb->insert(b[i]);
    }
    cout << endl;
    cout << "== Delete minimum node (hb)" << endl;
    hb->removeMin();
    hb->print();

    // 将"斐波那契堆hb"合并到"斐波那契堆ha"中。
    cout << "== Merge ha and hb" << endl;
    ha->combine(hb);
    ha->print();
}

// 验证"删除最小节点"
void testRemoveMin()
{
    int i;
    int alen=sizeof(a)/sizeof(a[0]);
    int blen=sizeof(b)/sizeof(b[0]);
    FibHeap<string>* ha=new FibHeap<string>();
    FibHeap<string>* hb=new FibHeap<string>();

    cout << "== Add successively into heap_ha: ";
    for(i=0; i<alen; i++)
    {
        cout << a[i] <<" ";
        ha->insert(a[i]);
    }
    cout << endl;
    cout << "== Delete minimum node (ha)" << endl;
    ha->removeMin();
    //ha->print();

    // 斐波那契堆hb
    cout << "== Add successively into heap_hb: ";
    for(i=0; i<blen; i++)
    {
        cout << b[i] <<" ";
        hb->insert(b[i]);
    }
    cout << endl;
    cout << "== Delete minimum node (hb)" << endl;
    hb->removeMin();
    //hb->print();

    // 将"斐波那契堆hb"合并到"斐波那契堆ha"中。
    cout << "== Merge ha and hb" << endl;
    ha->combine(hb);
    ha->print();


    cout << "== Delete minimum node (ha)" << endl;
    ha->removeMin();
    ha->print();
}

// 验证"减小节点"
void testDecrease()
{
    int i;
    int blen=sizeof(b)/sizeof(b[0]);
    FibHeap<string>* hb=new FibHeap<string>();

    // 斐波那契堆hb
    cout << "== Add successively into heap_hb: ";
    for(i=0; i<blen; i++)
    {
        cout << b[i] <<" ";
        hb->insert(b[i]);
    }
    cout << endl;
    cout << "== Delete minimum node (hb)" << endl;
    hb->removeMin();
    hb->print();

    cout << "== Decrease 213 to 1" << endl;
    hb->update("213", "1");
    hb->print();
}

// 验证"增大节点"
void testIncrease()
{
    int i;
    int blen=sizeof(b)/sizeof(b[0]);
    FibHeap<string>* hb=new FibHeap<string>();

    // 斐波那契堆hb
    cout << "== Add successively into heap_hb: ";
    for(i=0; i<blen; i++)
    {
        cout << b[i] <<" ";
        hb->insert(b[i]);
    }
    cout << endl;
    cout << "== Delete minimum node (hb)" << endl;
    hb->removeMin();
    hb->print();

    cout << "== Increase 213 to 631" << endl;
    hb->update("213", "631");
    hb->print();
}

// 验证"删除节点"
void testDelete()
{
    int i;
    int blen=sizeof(b)/sizeof(b[0]);
    FibHeap<string>* hb=new FibHeap<string>();

    // 斐波那契堆hb
    cout << "== Add successively into heap_hb: ";
    for(i=0; i<blen; i++)
    {
        cout << b[i] <<" ";
        hb->insert(b[i]);
    }
    cout << endl;
    cout << "== Delete minimum node (hb)" << endl;
    hb->removeMin();
    hb->print();

    cout << "== Delete node 213" << endl;
    hb->remove("213");
    hb->print();
}

int main()
{
    // 验证"基本信息(斐波那契堆的结构)"
    testBasic();
    // 验证"插入操作"
    //testInsert();
    // 验证"合并操作"
    //testUnion();
    // 验证"删除最小节点"
    //testRemoveMin();
    // 验证"减小节点"
    //testDecrease();
    // 验证"增大节点"
    //testIncrease();
    // 验证"删除节点"
    //testDelete();

    return 0;
}