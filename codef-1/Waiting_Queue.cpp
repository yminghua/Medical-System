#include <string>
#include "Data_Structure.h"
//#include              //many head function
// class KMPlist
// KMPlist<T>::KMPlist(int size)    //module
//Registration
 class Waiting_Queue_Node
{
public:
    Registration* head_pointer;
    Waiting_Queue_Node* prev, *next=NULL;

//    Waiting_Queue_Node(Registration* input){
//        head_pointer = input;
//        prev = NULL;
//        next = NULL;
//    }
    void set_prev(Waiting_Queue_Node* input){prev = input;}
    void set_next(Waiting_Queue_Node* input){next = input;}
};

 class Waiting_Queue
{
public:
    Waiting_Queue_Node *head=NULL;
    Waiting_Queue_Node *tail=NULL;
    Waiting_Queue(){head = NULL;tail = NULL;}
    Waiting_Queue_Node* insert(Registration *node);
    int show();
    Registration* pop();
    void deletenode(Waiting_Queue_Node* node);
    void print();
};

Waiting_Queue_Node* Waiting_Queue::insert(Registration *node)
{
    Waiting_Queue_Node *one = new Waiting_Queue_Node;
    one->head_pointer = node;
    one->next =NULL;
    one->prev =NULL;
    if (head == NULL)
    {
        head = one;
        tail = one;
    }
    else
    {
        tail->next = one;
        one->prev = tail;
        tail = one;
    }
    return one;
}

 int Waiting_Queue::show(){
    return (*head).head_pointer->time;
}

Registration *Waiting_Queue::pop()
{
    Waiting_Queue_Node* drop = head;
    head = head->next;
    Registration *out = drop->head_pointer;
    delete drop;
    return out;
}

 void Waiting_Queue::deletenode(Waiting_Queue_Node* node){

    if(head == node) pop();
    if(tail == node) {(node->prev)->next=NULL;delete(node);}
    else{
        (node->prev)->next = node->next;
        (node->next)->prev = node->prev;
        delete(node);
    }
}

void Waiting_Queue::print(){
    Waiting_Queue_Node* pointer=head;
    if (pointer == NULL)
    {
        cout << "The queue is emptied\n";
        return;
    }
    cout << "== Registration Information: ==" << endl;
    while(pointer!=NULL){
        pointer->head_pointer->print();
        pointer = pointer->next;
    }
}

class RegWR_Queue
{
public:
    Waiting_Queue_Node* head=NULL;
    RegWR_Queue(){head = NULL;}
//    void insert(Registration* node);
    void iinsert(Registration* node, Waiting_Queue_Node* loc);        //new added
//    int show();
    void pop();
//    void deletenode(Waiting_Queue_Node* node);
    void deleteall();
    void print();
};


void RegWR_Queue::pop(){
    Waiting_Queue_Node* drop = head;
    head = head->next;
    delete(drop);
}
void RegWR_Queue::deleteall(){
    while(head!=NULL) pop();
}

void RegWR_Queue::iinsert(Registration* node, Waiting_Queue_Node* loc){
    Waiting_Queue_Node* one = new Waiting_Queue_Node;
    one->head_pointer = node;
    one->next = NULL;
    one->prev = NULL;
    if (loc==NULL){one->next = head;head=one;return;}
    one->prev = loc;one->next = loc->next;
    if(loc->next!=NULL)loc->next->prev = one;
    loc->next = one;
}// new added!!!!!!

void RegWR_Queue::print(){
    Waiting_Queue_Node* pointer=head;
    cout<<"regWR queue print test::"<<endl;
    while(pointer!=NULL){
        pointer->head_pointer->print();
        pointer = pointer->next;
    }
}