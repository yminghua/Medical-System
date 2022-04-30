#include <string>
#include "Data_Structure.h"
//#include              //many head function
// class KMPlist
// KMPlist<T>::KMPlist(int size)    //module
//Appointment

 class Appointment_Queue_Node
{
public:
    Appointment *head_pointer = NULL;
    Appointment_Queue_Node *prev = NULL, *next = NULL;
    Appointment_Queue_Node *Nprev = NULL, *Nnext = NULL;
    Appointment_Queue_Node *Pprev = NULL, *Pnext = NULL;
    Appointment_Queue_Node *Aprev = NULL, *Anext = NULL;
    Appointment_Queue_Node(Appointment* input){
        head_pointer = input;
        prev = NULL; Nprev = NULL; Pprev = NULL; Aprev = NULL;
        next = NULL; Nnext = NULL; Pnext = NULL; Anext = NULL;

    }
    void set_prev(Appointment_Queue_Node* input){prev = input;}
    void set_next(Appointment_Queue_Node* input){next = input;}
};

 class Appointment_Queue
{
public:
    Appointment_Queue_Node *head = NULL,  *tail = NULL;
    Appointment_Queue_Node *Nhead = NULL;
    Appointment_Queue_Node *Phead = NULL;
    Appointment_Queue_Node *Ahead = NULL;
    Appointment_Queue_Node *Rpointer = NULL;     //line of the finish & appointment


    Appointment_Queue(){head = NULL;tail = NULL;Nhead = NULL;
    Phead = NULL;Ahead = NULL;Rpointer= NULL;}//ini

    Appointment_Queue_Node* insert(Appointment* node);
    int show();
    Appointment* pop();
    void deletenode(Appointment_Queue_Node* node);
    void resetNPA(){Nhead = NULL;Phead = NULL;Ahead = NULL;} //well define
    void resetR(){Rpointer= NULL;} //well define
    int updateR(int nowday);//new one::
    void insertNPA(Appointment_Queue_Node* node);
    void deleteNPA(Appointment_Queue_Node* node);

    //for debug::
    void print();


};


Appointment_Queue_Node* Appointment_Queue::insert(Appointment* node){  //
    Appointment_Queue_Node* one = new Appointment_Queue_Node(node);
    one->head_pointer = node;
    //if(Nhead == NULL) {Nhead = one;Phead = one;Ahead = one;}
    if(Rpointer == NULL) {Rpointer=one;}

    if(head == NULL) {head = one;tail = one;}
    else{
        Appointment_Queue_Node * p = tail->prev;
        if(node->time >=tail->head_pointer->time){        
            tail->set_next(one);
            one->set_prev(tail);
            tail = one;
            return one;}
        while(1){
            if(p == NULL){head = one;Rpointer = one ; one->next = tail;tail->prev = head;
            return one;}
            if(node->time < p->head_pointer->time){
                p=p->prev;
            }
            else{
                
                one->next = p->next;
                one->prev = p;
                p->next->prev = one;
                if(Rpointer == p->next) Rpointer = one;
                p->next = one;
                return one;
            }
        }


    }
    return one;
}

 int Appointment_Queue::show(){
    return (*head).head_pointer->time;
}

  Appointment* Appointment_Queue::pop(){
    Appointment_Queue_Node* drop = head;
    Appointment* out = drop->head_pointer;
    //if(head->Nnext!=NULL||head->Nprev!=NULL||Nhead == head)deleteNPA(head);
    if(head == NULL) return NULL;
    head = head->next;
    if(head == NULL) tail = NULL;
    delete drop;

    return out;
}

 void Appointment_Queue::deletenode(Appointment_Queue_Node* node){
     //cout<<"cc"<<endl;
    if(Rpointer == node) Rpointer = node->next;
    if(head == node) {pop();return;}
    if(tail == node) {//if(node->Nnext!=NULL||node->Nprev!=NULL||Nhead == node)deleteNPA(node);
    (node->prev)->next=NULL;tail = node->prev; delete node;return;}
    else{
        //if(node->Nnext!=NULL||node->Nprev!=NULL||Nhead == node)deleteNPA(node);
        (node->prev)->next = node->next;
        (node->next)->prev = node->prev;
        delete node;
    }
    //cout<<"cc"<<endl;
}

void Appointment_Queue::insertNPA(Appointment_Queue_Node* node){
    Appointment_Queue_Node* p = Nhead;
    while(1){
        if (node->head_pointer->person->name > p->head_pointer->person->name)
        {
            //cout<<"succ3"<<endl;  
            if(p->Nnext == NULL){
                p->Nnext = node;
                node->Nprev = p;
                break;
            }
            p = p->Nnext;
        }
        else{
        node->Nnext=p;node->Nprev=p->Nprev;
        if(p->Nprev!=NULL) p->Nprev->Nnext = node; 
        else Nhead = node;
        p->Nprev = node;
        break;
        }
    }
                                       //for the name queue: link list
    p = Phead;
    while(1){
        if (node->head_pointer->person->profession > p->head_pointer->person->profession)
        {
            if(p->Pnext == NULL){
                p->Pnext = node;
                node->Pprev = p;
                break;
            }
            p = p->Pnext;
        }
        else{node->Pnext=p;node->Pprev=p->Pprev;
        if(p->Pprev!=NULL) p->Pprev->Pnext = node; 
        else Phead = node;
        p->Pprev = node;
        break;
        }
    }
    p = Ahead;
    while(1){
        if (node->head_pointer->person->age_group > p->head_pointer->person->age_group)
        {
            if(p->Anext == NULL){
                p->Anext = node;
                node->Aprev = p;
                break;
            }
            p = p->Anext;
        }
        else{node->Anext=p;node->Aprev=p->Aprev;
        if(p->Aprev!=NULL) p->Aprev->Anext = node; 
        else Ahead = node;
        p->Aprev = node;
        break;
        }
    }



}

void Appointment_Queue::deleteNPA(Appointment_Queue_Node* node){
    if(node->Nprev == NULL)Nhead=node->Nnext;
    else{if(node->Nnext == NULL)node->Nprev->Nnext = NULL;
    else{
        node->Nprev->Nnext = node->Nnext;
        node->Nnext->Nprev = node->Nprev;
    }
    }//finish the N

    if(node->Pprev == NULL)Phead=node->Pnext;
    else{if(node->Pnext == NULL)node->Pprev->Pnext = NULL;
    else{
        node->Pprev->Pnext = node->Pnext;
        node->Pnext->Pprev = node->Pprev;
    }
    }//finish the P

    if(node->Aprev == NULL)Ahead=node->Anext;
    else{if(node->Anext == NULL)node->Aprev->Anext = NULL;
    else{
        node->Aprev->Anext = node->Anext;
        node->Anext->Aprev = node->Aprev;
    }
    }//finish the A

}

int Appointment_Queue::updateR(int nowday){
    int ccounter=0;
    if (Rpointer == NULL) {return ccounter;}
    //cout<<"yeah1"<<endl;
    //cout<<(Rpointer->head_pointer->reg_id )<<endl;
    while (Rpointer->head_pointer->time<=nowday){   
        if(Nhead ==NULL){Nhead = Rpointer; Phead = Rpointer; Ahead = Rpointer;}
        else {//cout<<"yeah"<<endl;
        insertNPA(Rpointer);
        //cout<<"yeah"<<endl;
        }
        Rpointer = Rpointer->next;
        ccounter++;
        if(Rpointer == NULL) return ccounter;
    }
    //        cout<<"?"<<endl;
    return ccounter;

}

void Appointment_Queue::print(){                  //debug printing
    Appointment_Queue_Node *pointer = head;
    if (pointer == NULL) {
        cout << "The queue is emptied\n";
        return;
    }
    cout << "== Registration Information: ==" << endl;
    while(pointer != NULL){
        pointer->head_pointer->print();
        pointer = pointer->next;
    }

}





//assistant queue::
 class AppWR_Queue
{
public:
    Appointment_Queue_Node* head;
    AppWR_Queue(){head = NULL;}
//    void insert(Appointment* node);
    void iinsert(Appointment* node, Appointment_Queue_Node* loc);        //new added
//    int show();
    void pop();
//    void deletenode(Appointment_Queue_Node* node);
    void deleteall();
    void print();
};

void AppWR_Queue::pop(){
    Appointment_Queue_Node* drop = head;
    head = head->next;
    delete(drop);
}
void AppWR_Queue::deleteall(){
    while(head!=NULL) pop();
}

void AppWR_Queue::iinsert(Appointment* node, Appointment_Queue_Node* loc){
    Appointment_Queue_Node* one = new Appointment_Queue_Node(node);
    one->prev = NULL;one->next = NULL;one->head_pointer =node;
    if (loc==NULL){one->next = head;
    if(head!=NULL){head->prev = one;}
    head=one;return;
    }
    one->prev = loc;one->next = loc->next;
    if(loc->next!=NULL)loc->next->prev = one;
    loc->next = one;
}

void AppWR_Queue::print(){
    Appointment_Queue_Node *pointer = head;
//    cout<<"appWR queue test print::"<<endl; 
    while(pointer != NULL){
        pointer->head_pointer->print();
        pointer = pointer->next;
    }   
}
