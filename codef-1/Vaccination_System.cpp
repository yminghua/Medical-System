#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "System.h"
using namespace std;
#include <string>

using std::string;

void System::Register(int id, string name, int contact, int profession, int age_group, int risk, int letter, int registry)
{
    //for monthly report::waiting people in total:
    Waiting_adder++;
    tt_time -= counter;
    /* First generate a registration file based on the input data */
    Person *per;
    Registration *reg;
    per = new Person;
    reg = new Registration;
    per->age_group = age_group;
    per->contact = contact;
    per->id = id;
    per->letter = letter;
    per->name = name;
    per->profession = profession;
    per->risk = risk;

    reg->person = per;
    reg->registry = registry;
    reg->time = counter;
    reg->reg_id = global_reg_id;
    global_reg_id++;
    Month_global_reg_id_number++;
    reg->set_priority();

    Reg_List.insert(reg);

    /* Now check for special case: Priority letter, High/Midium risk, if has withdrawn */
    //First check whether the person has withdrawn before. If has, delete him from the withdrawn queue
    bool withdraw=false; //True if this person is in the withdrawn list
    Waiting_Queue_Node *i = withdrawn.head;
    while( i!= NULL){
        if(i->head_pointer->person->id == id){
            withdraw = true;
            withdrawn.deletenode(i);
            break;
        }
        i = i->next;
    }

    //Next check whether the person has priority letter
    if (letter!=0) 
    {
        reg->status = 2;
        FibNode<int> *handle = letter_heap.insert(reg, letter);
        reg->letter_heap_node = handle;
        /*if (registry == 1)
        {
            loc1.insert(reg, reg->priority);
        }
        else if (registry == 2)
        {
            loc2.insert(reg, reg->priority);
        }
        else
        {
            loc3.insert(reg, reg->priority);
        }*/
    } 
    else //no priority letter
    {
        //test risk status
        if (risk==3) {
            reg->status = 4;
            Waiting_Queue_Node *handle = high_risk.insert(reg);
            reg->high_risk_node = handle;
        } else if (risk==2) {
            reg->status = 3;
            Waiting_Queue_Node *handle = mid_risk.insert(reg);
            reg->mid_risk_node = handle;
        } else {
            //no risk, check for withdrawn
            if (withdraw) {
                reg->status = 5;
                Waiting_Queue_Node *handle = withdraw_then_register.insert(reg);
                reg->withdraw_then_register_node = handle;
            } else {
                reg->status = 1;
                FibNode<string> *handle;
                if (registry == 1)
                {
                    handle = loc1.insert(reg, reg->priority);
                }
                else if (registry == 2)
                {
                    handle = loc2.insert(reg, reg->priority);
                }
                else
                {
                    handle = loc3.insert(reg, reg->priority);
                }
                reg->loc_or_central_node = handle;                
            }
        }
    }
}


void System::Transfer() 
{
    central.combine(&loc1);
    central.combine(&loc2);
    central.combine(&loc3);
    //debug
}

void System::Gen_Appointment()
{
    int max = 100; // maximum daily capacity for the three local station. 50 for morning and afternoon each
    int load1 = max, load2 = max, load3 = max;                         
    int date1 = counter + 1, date2 = counter + 1, date3 = counter + 1; // initialize the appointment date for the three station
    Appointment *app;
    Appointment_Queue_Node *new_handle;

    while (load1 + load2 + load3 > 0) // while there's still capacity left for appointment to make
    {
        Registration *reg = NULL; // the registration to be dealt with
        int loc;                  // the set location for the appointment
        if (load1 == max/2)
            date1 = counter + 2;
        if (load2 == max/2)
            date2 = counter + 2;
        if (load3 == max/2)
            date3 = counter + 2; // if morning is full, change appointment date to afternoon

        while (true) // find the registration with the highest priority
        {
            if ((letter_heap.min != NULL)) // check for letter_heap
            {
                if ((letter_heap.min)->registration->person->letter <= counter + 2)
                {
                    FibNode<int> *fibnode = letter_heap.extractMin();
                    reg = fibnode->registration;
                    break;
                }
            }
            if ((mid_risk.head != NULL)) // check for mid risk waiting queue
            {
                if ((counter - mid_risk.show()) >= 2 * 30)
                {
                    reg = mid_risk.pop();
                    break;
                }
            }
            if ((withdraw_then_register.head != NULL)) // check for withdraw_then_register waiting queue
            {
                if (counter - withdraw_then_register.show() >= 2 * 14)
                {
                    reg = withdraw_then_register.pop();
                    break;
                }
            }
            if (central.min == NULL) // central heap is empty (no other registration is waiting), check for high_risk
            {
                if (high_risk.head != NULL)
                {
                    reg = high_risk.pop();
                    break;
                }
            }
            else // check for central heap
            {
                FibNode<string> *fibnode = central.extractMin();
                reg = fibnode->registration;
                break;
            }
            break;
        }

        if (reg == NULL)
            return;
        // reg isn't NULL, start to build the appointment for that registration
        //debug
        cout << "we've obtained a reg. it's detailed information is:\n" ;
        reg->print();
        app = new Appointment;
        //mohhthly
        App_adder++;
        cout << "the allocation for app succeed\n";
        app->person = reg->person;
        app->reg_id = reg->reg_id;
        cout << "the generation for app data person and reg_id succeed\n";
        // now set the appointment time and loc based on origin registry and current capacity
        if (reg->registry == 1)
        {
            if (load1 > 0)
                loc = 1;
            else if (load2 >= load3)
                loc = 2;
            else
                loc = 3;
        }
        else if (reg->registry == 2)
        {
            if (load2 > 0)
                loc = 2;
            else if (load1 >= load3)
                loc = 1;
            else
                loc = 3;
        }
        else
        {
            if (load3 > 0)
                loc = 3;
            else if (load1 >= load2)
                loc = 1;
            else
                loc = 2;
        }
        app->loc = loc;
        cout << "the generation for app data loc succeed\n";
        // loc is set, now set the time
        if (loc == 1)
        {
            load1--;
            app->time = date1;
        }
        else if (loc == 2)
        {
            load2--;
            app->time = date2;
        }
        else
        {
            load3--;
            app->time = date3;
        }
        cout << "the generation for app data time succeed\n";
        app->waiting_time = app->time - reg->time;
        cout << "the generation for app data waiting_time succeed\n";
        new_handle = appointment_queue.insert(app);
        cout << "new handle generated succeed\n";
        // debug
        cout << "an appointment has been generated: \n";
        app->print();
        reg->status = 0;
        reg->appointment_queue_node = new_handle;
    }
}

void System::Withdraw(int reg_id)
{
    Registration *reg = Reg_List.getitem(reg_id);
    switch (reg->status) 
    {
        case 0: 
        {
            Appointment_Queue_Node *old_handle = reg->appointment_queue_node;
            if (old_handle->head_pointer->time <= counter)
                return; //if this appointment has already been finished, withdraw request is invalid
            Waiting_adder-=1;//monthly
            Withdraw_adder+=1;
            tt_time += counter;
            tt_time_count++;
            appointment_queue.deletenode(old_handle);
            break;
        }
        case 1:
        {   
            Waiting_adder-=1;//monthly
            Withdraw_adder+=1;
            tt_time += counter;
            tt_time_count++;
            FibNode<string> *old_handle = reg->loc_or_central_node;
            if (counter==reg->time) // the registration has not yet transfered to the central heap
            {
                switch (reg->registry) 
                {
                    case 1: 
                    {
                        loc1.decrease(old_handle, " ");
                        loc1.removeMin();
                        break;
                    }
                    case 2:
                    {
                        loc2.decrease(old_handle, " ");
                        loc2.removeMin();
                        break;
                    }
                    case 3:
                    {
                        loc3.decrease(old_handle, " ");
                        loc3.removeMin();
                        break;
                    }
                }
            } 
            else // the registration is in the central heap
            {
                central.decrease(old_handle, " ");
                central.removeMin();
            }
            break;
        }
        case 2:
        {
            Waiting_adder-=1;//monthly
            Withdraw_adder+=1;
            tt_time += counter;
            tt_time_count++;
            FibNode<int> *old_handle = reg->letter_heap_node;
            letter_heap.decrease(old_handle, 0);
            letter_heap.removeMin();
            break;
        }
        case 3:
        {
            Waiting_adder-=1;//monthly
            Withdraw_adder+=1;
            tt_time += counter;
            tt_time_count++;
            Waiting_Queue_Node *old_handle = reg->mid_risk_node;
            mid_risk.deletenode(old_handle);
            break;
        }
        case 4:
        {
            Waiting_adder-=1;//monthly
            Withdraw_adder+=1;
            tt_time += counter;
            tt_time_count++;
            Waiting_Queue_Node *old_handle = reg->high_risk_node;
            high_risk.deletenode(old_handle);
            break;
        }
        case 5:
        {
            Waiting_adder-=1;//monthly
            Withdraw_adder+=1;
            tt_time += counter;
            tt_time_count++;
            Waiting_Queue_Node *old_handle = reg->withdraw_then_register_node;
            withdraw_then_register.deletenode(old_handle);
            break;
        }

    }
    reg->status = 6;
    withdrawn.insert(reg);
}

void System::Change_Status(int reg_id, int new_profession, int new_risk)
{
    Registration *reg = Reg_List.getitem(reg_id);
    reg->person->profession = new_profession;
    reg->person->risk = new_risk;
    reg->set_priority();
    switch (reg->status) 
    {
        case 0:
        case 2:
        {
            break;
        }
        case 1:
        {
            FibNode<string> *old_handle = reg->loc_or_central_node;
            //first delete it from the current heap
            if (counter == reg->time) // the registration has not yet transfered to the central heap
            {
                switch (reg->registry)
                {
                case 1:
                {
                    loc1.decrease(old_handle, " ");
                    loc1.removeMin();
                    break;
                }
                case 2:
                {
                    loc2.decrease(old_handle, " ");
                    loc2.removeMin();
                    break;
                }
                case 3:
                {
                    loc3.decrease(old_handle, " ");
                    loc3.removeMin();
                    break;
                }
                }
            }
            else // the registration is in the central heap
            {
                central.decrease(old_handle, " ");
                central.removeMin();
            }

            // now check for new risk status
            switch (new_risk) 
            {
                case 0:
                case 1: // insert the changed registration back in the heap
                {                   
                    FibNode<string> *new_handle;
                    if (counter == reg->time) // the registration has not yet transfered to the central heap
                    {
                        switch (reg->registry)
                        {
                        case 1:
                        {
                            new_handle = loc1.insert(reg, reg->priority);
                            break;
                        }
                        case 2:
                        {
                            new_handle = loc2.insert(reg, reg->priority);
                            break;
                        }
                        case 3:
                        {
                            new_handle = loc3.insert(reg, reg->priority);
                            break;
                        }
                        }
                    }
                    else // the registration is in the central heap
                    {
                        new_handle = central.insert(reg, reg->priority);
                    }
                    reg->status = 1;
                    reg->loc_or_central_node = new_handle;
                    break;
                }
                case 2: // insert into mid_risk
                {
                    Waiting_Queue_Node *new_handle;
                    new_handle = mid_risk.insert(reg);
                    reg->status = 3;
                    reg->mid_risk_node = new_handle;
                    break;
                }
                case 3: // insert into high_risk
                {
                    Waiting_Queue_Node *new_handle;
                    new_handle = high_risk.insert(reg);
                    reg->status = 4;
                    reg->high_risk_node = new_handle;
                    break;
                }
            }
            break;
        }
        case 3:
        {
            Waiting_Queue_Node *old_handle = reg->mid_risk_node;
            switch (new_risk) 
            {
                case 0:
                case 1: // insert into heap
                {
                    FibNode<string> *new_handle;
                    mid_risk.deletenode(old_handle);
                    if (counter == reg->time) // the registration has not yet transfered to the central heap
                    {
                        switch (reg->registry)
                        {
                        case 1:
                        {
                            new_handle = loc1.insert(reg, reg->priority);
                            break;
                        }
                        case 2:
                        {
                            new_handle = loc2.insert(reg, reg->priority);
                            break;
                        }
                        case 3:
                        {
                            new_handle = loc3.insert(reg, reg->priority);
                            break;
                        }
                        }
                    }
                    else // the registration is in the central heap
                    {
                        new_handle = central.insert(reg, reg->priority);
                    }
                    reg->status = 1;
                    reg->loc_or_central_node = new_handle;
                    break;
                }
                case 2: // no change, stay in mid_risk
                {
                    break;
                }
                case 3: // transfer to high_risk
                {
                    Waiting_Queue_Node *new_handle;
                    mid_risk.deletenode(old_handle);
                    new_handle = high_risk.insert(reg);
                    reg->status = 4;
                    reg->high_risk_node = new_handle;
                    break;
                }
            }
            break;
        }
        case 4:
        {
            Waiting_Queue_Node *old_handle = reg->high_risk_node;
            switch (new_risk) 
            {
            case 0:
            case 1: // insert into heap
            {
                FibNode<string> *new_handle;
                high_risk.deletenode(old_handle);
                if (counter == reg->time) // the registration has not yet transfered to the central heap
                {
                    switch (reg->registry)
                    {
                    case 1:
                    {
                        new_handle = loc1.insert(reg, reg->priority);
                        break;
                    }
                    case 2:
                    {
                        new_handle = loc2.insert(reg, reg->priority);
                        break;
                    }
                    case 3:
                    {
                        new_handle = loc3.insert(reg, reg->priority);
                        break;
                    }
                    }
                }
                else // the registration is in the central heap
                {
                    new_handle = central.insert(reg, reg->priority);
                }
                reg->status = 1;
                reg->loc_or_central_node = new_handle;
                break;
            }
            case 2: // transfer into mid_risk
            {
                Waiting_Queue_Node *new_handle;
                high_risk.deletenode(old_handle);
                new_handle = mid_risk.insert(reg);
                reg->status = 3;
                reg->mid_risk_node = new_handle;
                break;
            }
            case 3: // nochange, stay in high_risk
            {
                break;
            }
            }
            break;
        }
        case 5:
        {
            Waiting_Queue_Node *old_handle = reg->withdraw_then_register_node;
            switch (new_risk) 
            {
                case 0:
                case 1:
                {
                    break;
                }
                case 2:
                {
                    Waiting_Queue_Node *new_handle;
                    withdraw_then_register.deletenode(old_handle);
                    new_handle = mid_risk.insert(reg);
                    reg->status = 3;
                    reg->mid_risk_node = new_handle;
                    break;
                }
                case 3:
                {
                    Waiting_Queue_Node *new_handle;
                    withdraw_then_register.deletenode(old_handle);
                    new_handle = high_risk.insert(reg);
                    reg->status = 4;
                    reg->high_risk_node = new_handle;
                    break;
                }

            }
            break;
        }
    }
}

void System::Half_Day_Pass()
{
    //for the weekly report::
    int num = appointment_queue.updateR(counter);
    Waiting_adder -= num;
    tt_time = num * counter;
    tt_time_count += num;
    if(counter%14 == 0) {
        //automatically form weekly report::
        Weekly_Report();
        week_reg_index = global_reg_id;
        appointment_queue.resetNPA();
        //appointment_queue.resetR();
    }
    //for the monthly report::
    if(counter%30 == 0) {
        Monthly_Report();
        Month_global_reg_id_number = 0; //reset 0. add with global_reg_id.
        Waiting_adder=0;   //registration,+1, finish or withdraw, -1.
        App_adder=0; Withdraw_adder=0;
        tt_time = 0;
        tt_time_count =0;
    }

    counter++;
    Transfer(); // transfer the data in the local heap to the central heap. (per half day)
    if (counter%2 == 0) Gen_Appointment();
}

void System::n_Day_Pass(int n)
{
    for (int i = 1; i <= 2 * n; i++)
        Half_Day_Pass();
}

void System::Weekly_Report()    //this one only can sort by name
{
    // Part for appointmenting::
    AppWR_Queue appoint;
    Appointment_Queue_Node * node;
    node = appointment_queue.Rpointer;
    while(node!=NULL){
        Appointment_Queue_Node* p = appoint.head;
        while(1){
            if (p == NULL) {appoint.iinsert(node->head_pointer,NULL);break;}
            if(node->head_pointer->person->name>p->head_pointer->person->name){
                if(p->next == NULL){appoint.iinsert(node->head_pointer,p);break;}
                p = p->next;       
            }
            else {appoint.iinsert(node->head_pointer,p->prev);break;}
        }
        node = node->next;
    }
    Appointment_Queue_Node* queueingp = appoint.head;
    cout<<"weekly:appointmentpeople"<<endl;
    while(queueingp !=NULL){
        Person* member = queueingp->head_pointer->person;
        cout<<queueingp->head_pointer->reg_id<<member->profession  //continue
        <<member->age_group<<member->risk<<counter-Reg_List.reprarray[queueingp->head_pointer->reg_id]->time<<endl;
        queueingp = queueingp->next;    //the time have to count such way.
    }//finish the appointming report!!
    appoint.deleteall();

    //this part: about treated people

    Appointment_Queue_Node* treatedp = appointment_queue.Nhead;
    cout<<"weekly:treatedpeople"<<endl;
    while(treatedp !=NULL && treatedp!=appointment_queue.Rpointer){   //add1
        Person* member = treatedp->head_pointer->person;
        cout<<treatedp->head_pointer->reg_id<<member->profession  //continue
        <<member->age_group<<member->risk<<treatedp->head_pointer->waiting_time<<endl;
        treatedp = treatedp->Nnext;
    }//finish the treated report!!     

    //final part: hardest part: queueing people
    RegWR_Queue instack;
    for (int i = week_reg_index;i<global_reg_id;i++){
        int judge = Reg_List.reprarray[i]->status;
        if(judge>=1&&judge<=5){  //IMPORTANT: here need to fix, i don't know exact status that proper
            Waiting_Queue_Node* p1 = instack.head;
            while (1){                  
                if (p1 == NULL) {instack.iinsert(Reg_List.reprarray[i],NULL);break;}
                if(Reg_List.reprarray[i]->person->name>p1->head_pointer->person->name){
                    if(p1->next == NULL){instack.iinsert(Reg_List.reprarray[i],p1);break;}
                    p1 = p1->next;      
                }
                else {instack.iinsert(Reg_List.reprarray[i],p1->prev);break;}   
            }         
        }
    }// finish the sort

    Waiting_Queue_Node* instackp = instack.head;
    cout<<"weekly:queueingpeople"<<endl;
    while(instackp !=NULL){
        Person* member = instackp->head_pointer->person;
        cout<<instackp->head_pointer->reg_id<<member->profession  //continue
        <<member->age_group<<member->risk<<counter-Reg_List.reprarray[instackp->head_pointer->reg_id]->time<<endl;
        instackp = instackp->next;    //the time have to count such way.
    }//finish the appointming report!!
    instack.deleteall();  
    //now, over!!
}

void System::Monthly_Report()
{
    cout<<"Registation number in recent Month:"<<Month_global_reg_id_number<<endl;
    cout<<"Total waiting number:"<<Waiting_adder<<endl;
    cout<<"appointment made number:"<<App_adder<<endl;
    cout<<"number withdraw:"<<Withdraw_adder<<endl;
    cout<<"everage time:"<<(tt_time+(Month_global_reg_id_number-tt_time_count)*counter) / Month_global_reg_id_number<<endl;
}

void System::print()
{
    cout<<"The Registration Files in loc1 are:\n";
    loc1.print_reg();
    cout<<"\n";

    cout << "The Registration Files in loc2 are:\n";
    loc2.print_reg();
    cout<<"\n";

    cout << "The Registration Files in loc3 are:\n";
    loc3.print_reg();
    cout<<"\n";

    cout << "The Registration Files in central are:\n";
    central.print_reg();
    cout<<"\n";

    cout << "The Registration Files in letter heap are:\n";
    letter_heap.print_reg();
    cout<<"\n";

    cout << "The Registration Files in mid_risk waiting queue are:\n";
    mid_risk.print();
    cout << "\n";

    cout << "The Registration Files in high_risk waiting queue are:\n";
    high_risk.print();
    cout << "\n";

    cout << "The Registration Files in withdraw_then_register waiting queue are:\n";
    withdraw_then_register.print();
    cout << "\n";

    cout << "The Registration Files that have been withdrawn are:\n";
    withdrawn.print();
    cout << "\n";

    cout << "The Appointment Files in appointment_queue are:\n";
    appointment_queue.print();
    cout << "\n";
}
