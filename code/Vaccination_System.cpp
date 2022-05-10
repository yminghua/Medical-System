#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "System.h"
using namespace std;
#include <string>

using std::string;

void System::Register(int id, string name, int contact, int profession, int age_group, int risk, int letter, int registry)
{
    //cout<<"register"<<endl;
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
    Reg_List.Insert(reg->reg_id, reg); //modified for BP
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
//         cout<<"aaa"<<endl;
//         reg->print();
//         cout<<(letter_heap.min ==NULL)<<endl;
//         cout<<letter_heap.total_n<<endl;
         FibNode<int> *handle = letter_heap.insert(reg, letter);
//         cout<<"bbb"<<endl;
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
//    cout<<"aabb"<<endl;
//    cout<<counter<<endl;
//    print();
//    cout<<central.min<<endl;
//    cout<<loc1.cons<<" "<<loc2.cons<<" "<<loc3.cons<<endl;
    central.combine(&loc1);
    central.combine(&loc2);
    central.combine(&loc3);
//    cout<<"bbaa"<<endl;
    //debug
}

void System::Gen_Appointment()
{
    int max = 4; // maximum daily capacity for the three local station. max/2 for morning and afternoon each
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
                    reg = letter_heap.min->registration;
                    letter_heap.removeMin();
                    //FibNode<int> *fibnode = letter_heap.extractMin();
                    //reg = fibnode->registration;
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
            if (central.min == NULL) // central heap is empty (no other registration is waiting), check for waiting queues
            {
                if (withdraw_then_register.head != NULL)
                {
                    reg = withdraw_then_register.pop();
                    break;
                }
                else if (mid_risk.head != NULL)
                {
                    reg = mid_risk.pop();
                    break;
                }
                if (high_risk.head != NULL)
                {
                    reg = high_risk.pop();
                    break;
                }
            }
            else // check for central heap
            {
                reg = central.min->registration;
                central.removeMin();
            //    FibNode<string> *fibnode = central.extractMin();
            //    reg = fibnode->registration;
               
                break;
            }
            break;
        }

        if (reg == NULL)
            return;
        // reg isn't NULL, start to build the appointment for that registration
        //debug
        //cout << "we've obtained a reg. it's detailed information is:\n" ;
        //reg->print();
        app = new Appointment;

        //monthly
        App_adder++;
        //cout << "the allocation for app succeed\n";
        app->person = reg->person;
        app->reg_id = reg->reg_id;
        //cout << "the generation for app data person and reg_id succeed\n";
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
        //cout << "the generation for app data loc succeed\n";
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
        //cout << "the generation for app data time succeed\n";
        app->waiting_time = app->time - reg->time;
        //cout << "the generation for app data waiting_time succeed\n";
        new_handle = appointment_queue.insert(app);
        //cout << "new handle generated succeed\n";
        // debug
        //cout << "an appointment has been generated: \n";
        //app->print();
        reg->status = 0;
        reg->appointment_queue_node = new_handle;
    }
}

void System::Withdraw(int reg_id)
{
    Registration *reg = Reg_List.Search(reg_id);
    if (reg == NULL)
    {
        cout << "This reg_id does not exist!\n";
        return;
    }
    //cout<< reg->status<<endl;
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
//    cout<<"ewdf"<<endl;
    withdrawn.insert(reg);
//    cout<<"hey"<<endl;
}

void System::Change_Status(int reg_id, int new_profession, int new_risk)
{
    Registration *reg = Reg_List.Search(reg_id);
    if (reg==NULL) {
        cout<<"This reg_id does not exist!\n";
        return;
    }
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
/*//    cout<<"finish"<<endl;
    //for the weekly report::
    int num = appointment_queue.updateR(counter);
//   cout<<"finish"<<endl;
    Waiting_adder -= num;
    tt_time = num * counter;
    tt_time_count += num;
    //Weekly_Report();
    if(counter%14 == 0 && counter!=0) {
        //automatically form weekly report::
        //cout<<"hello"<<endl;
        Weekly_Report();
        week_reg_index = global_reg_id;
        appointment_queue.resetNPA();
        //appointment_queue.resetR();
    }
    //for the monthly report::
    if (counter % 30 == 0 && counter != 0)
    {
        Monthly_Report();
        Month_global_reg_id_number = 0; //reset 0. add with global_reg_id.
        //Waiting_adder=0;   //registration,+1, finish or withdraw, -1.
        App_adder=0; Withdraw_adder=0;
        tt_time = 0;
        tt_time_count =0;
    }*/

    counter++;
 //  cout<<"finish"<<endl;
    Transfer(); // transfer the data in the local heap to the central heap. (per half day)
 //  cout<<"finish1"<<endl;
    if (counter%2 == 0) Gen_Appointment();
 //  cout<<"finish2"<<endl;

    /* ==== test data ==== */
    switch (counter)
    {
    case 1:
    {
        Register(594855,"Devin_Gosse",10295,6,1,1,0,3);
        Register(897139,"Faithe_Herbert",19254,5,1,0,0,2);
        Register(585898,"Abel_Pansy",14891,3,1,2,0,1);
        Register(665107,"Salome_Jefferson",12530,2,1,0,0,3);
        Register(156497,"Leopold_Back",16419,3,1,1,4,3);
        Register(773826,"Robin_Ezekiel",11764,3,2,1,0,1);
        Register(285016,"Valentina_Quiller",12069,5,2,2,3,3);
        Register(742095,"Rodney_Dobbin",17689,5,3,0,0,2);
        Register(658781,"Nathaniel_Brewster",15178,2,3,0,0,3);
        Register(262627,"Calvin_Christiana",16957,3,5,3,5,3);


        Register(962263,"Bishop_Adam",17728,2,2,1,1,1);
        Register(408620,"Sabina_Frederick",12462,8,1,1,0,2);
        Register(867720,"Kenneth_Camp",11100,6,7,1,0,3);
        Register(967331,"Amos_Vaughan",18198,4,4,2,0,2);
        Register(765764,"Kyle_Dunbar",16745,6,7,2,0,3);
        Register(720947,"Payne_Frank",18219,3,3,0,0,1);
        Register(236769,"Theodore_Robin",12826,3,4,1,4,3);
        Register(554857,"Ivan_Shelley",18456,4,3,0,6,1);
        Register(387554,"Cheryl_Lucas",15001,1,2,0,0,2);
        Register(658045,"Benson_Philip",18263,2,6,0,0,3);

        break;
    }
    case 2:
    {

        cout << "a\n";
        Register(587429,"Norton_Sailsbury",11964,7,4,3,0,2);
        Register(892282,"Ian_Kipling",16888,4,6,1,0,1);
        Register(379365,"Abigail_Noel",10506,7,7,1,5,3);
        Register(934848,"Valerie_Walkley",13716,2,7,0,0,2);
        Register(734597,"Norman_Catharine",19208,8,2,1,0,3);
        Register(836431,"Kelly",14225,5,5,0,0,2);
        Register(627165,"Ursula_Abraham",15844,4,1,0,0,1);
        Register(614764,"Baron_Yerkes",18172,4,1,1,0,2);
        Register(845238,"Phyllis_Lattimore",15197,1,3,3,0,3);
        Register(659971,"Mona_Bloor",11828,8,2,0,0,2);
        cout << "b\n";

        
        /*Withdraw(2);  // register in 1, withdraw in 2. withdraw before appointment
        Withdraw(7);  // register in 1, withdraw in 2. medium risk and letter
        Withdraw(11); // register in 1, withdraw in 2. with letter
        /*
        Change_Status(16, 3, 2); // register in 1, change in 2. increase risk from 0 to 2
        Change_Status(17, 3, 3); // register in 1, change in 2. increase risk from 1 to 3, with letter
        */
        break;
    }
    case 3:
    {
        Register(897139,"Faithe_Herbert",19254,5,1,0,0,2);//reg_id 2, re-register in 3
        Register(285016,"Valentina_Quiller",12069,5,2,2,3,3);//reg_id 7, re-register in 3 with letter 3
        Register(666897,"Anastasia_Simpson",12108,6,3,1,0,3);
        Register(130976,"Roy_Jeremiah",14263,3,6,1,6,1);
        Register(958361,"Frederic_Dutt",16205,2,4,0,0,3);
        Register(362402,"Ben_Adolph",19182,3,3,1,0,2);
        Register(280356,"Tabitha_Zephaniah",12961,8,2,1,0,2);
        Register(447696,"Fay_Hornby",13144,1,3,2,7,1);
        Register(309638,"Odelia_Evans",11710,2,4,0,0,3);
        Register(875964,"Morgan_Wyclif(fe)",17556,7,3,2,7,1);

        Withdraw(18); // register in 1, withdraw in 3. withdraw after appointment
        Withdraw(21); // register in 2, withdraw in 3. high risk

        Change_Status(24, 6, 0); // register in 2, change in 3.change profession to 6
        Change_Status(27, 5, 2); // register in 2, change in 3.change profession to 5,increase risk from 0 to 2
        break;
    }
    case 4:
    {
        Register(931398,"Arvin_Davy",16790,7,7,0,0,3);
        Register(389995,"Aaron_Kennedy",13458,2,2,0,0,3);
        Register(891929,"Erica_Masefield",11744,1,1,3,0,2);
        Register(720357,"Evan_Saroyan",10017,7,5,0,0,1);
        Register(860644,"Gerald_Partridge",16110,7,1,1,0,3);
        Register(710110,"Oswald_Beerbohm",15491,4,2,0,0,3);
        Register(957454,"Cecil_Micah",14770,4,4,0,6,2);
        Register(271086,"Howar_Edmund",17241,1,1,0,0,3);
        Register(741347,"Lucy_Raman",19078,1,4,1,0,2);
        Register(646230,"Edith_Bertram",15424,8,5,0,0,2);
        // cout<<"ccc"<<endl;
        Withdraw(40); // register in 3, withdraw in 4. medium risk.

        Change_Status(34, 7, 1); // register in 3, change in 4.change profession to 7, with letter
        break;
    }
    case 5:
    {
        Register(799424,"Carl_Wodehous",11210,8,7,1,0,3);
        Register(924631,"Irma_Clara",11162,7,4,0,5,2);
        Register(587429,"Norton_Sailsbury",11964,7,4,3,0,2);//reg_id 21, re-register in 5,high risk
        Register(987272,"Rory_Yale",16857,8,2,1,0,3);
        Register(857266,"Christ_Betty",14540,1,1,1,5,2);
        Register(291140,"Kirk_Eve",17962,2,6,3,0,1);
        Register(705873,"Vito_Melville",19432,4,1,1,0,3);
        Register(501131,"Trista_Malachi",17254,5,6,1,0,2);
        Register(606442,"Eudora_Joyce",13755,6,7,1,0,2);
        Register(449761,"Ruth_Emerson",15718,5,5,1,0,2);

        Withdraw(47); // register in 4, withdraw in 5. with letter

        Change_Status(43, 1, 2); // register in 4, change in 5.decrease risk from 3 to 2
        break;
    }
    case 6:
    {
        Register(281736,"Francis_Bessemer",19725,6,4,1,0,2);
        Register(308013,"Jo_Buck",11648,2,5,1,0,1);
        Register(310934,"Sheila_Graham",10335,6,7,0,7,3);
        Register(358839,"Boris_Titus",19541,4,2,0,0,3);
        Register(875964,"Morgan_Wyclif(fe)",17556,7,3,2,7,1);//reg_id 40, continuously re-register in 6, medium risk
        Register(957454,"Cecil_Micah",14770,4,4,0,6,2);//reg_id 47, continuously re-register in 6. with letter
        Register(308953,"Maurice_Ben",11500,7,4,0,0,1);
        Register(497233,"Hardy_Grey",19708,5,4,2,0,2);
        Register(184518,"Beverly_Moulton",17353,6,4,1,0,3);
        Register(530925,"Gary_Doherty",18423,7,5,1,0,1);

        Withdraw(54); // register in 5, withdraw in 6. continuously withdraw, before appointment
        Withdraw(55); // register in 5, withdraw in 6. continuously withdraw, with letter
        Withdraw(56); // register in 5, withdraw in 6. continuously withdraw, high risk

        Change_Status(52, 7, 0); // register in 5, change in 6.with letter, increase risk from 0 to 3, with letter
        break;
    }
    case 7:
    {
        Register(202897,"Samantha_Beaufort",17301,3,1,0,0,2);
        Register(399566,"Sharon_Trollpoe",18648,2,1,1,0,1);
        Register(437676,"Ogden_Evan",13062,3,6,1,0,3);
        Register(491089,"Amy_Faraday",12814,8,2,1,8,3);
        Register(949977,"Elizabeth_Gabriel",15486,7,6,1,0,2);
        Register(486387,"Regina_Lloyd",16325,2,1,1,0,3);
        Register(675371,"Ryan_Malan",14690,7,4,1,0,2);
        Register(879157,"Milo_Hood",16822,7,7,1,0,2);
        Register(223911,"Upton_Lindberg(h)",19923,1,3,2,0,1);
        Register(711111,"Miriam_Judd",12449,5,1,0,0,1);

        Change_Status(63, 4, 3); // register in 6, change in 7. change profession to 4,
        Change_Status(68, 4, 1); // register in 6, change in 7.decrease risk from 2 to 1,change profession to 4

        break;
    }
    case 8:
    {
        Register(591148,"Yehudi_Lawrence",11995,6,1,0,0,1);
        Register(981843,"Nina_Wheatley",10074,7,4,0,0,3);
        Register(594856,"Darlene_Fred",12701,2,2,1,9,2);
        Register(328879,"Wright_Washington",13801,6,4,2,0,2);
        Register(940838,"Douglas_Kingsley",19441,7,1,0,0,2);
        Register(487774,"Bess_Grant",16838,4,4,1,0,2);
        Register(819642,"Jessica_Rhys",15572,4,5,1,0,1);
        Register(194479,"Judith_Dunlop",16456,3,2,3,0,2);
        Register(299908,"Mandel_Sharp",15430,7,3,2,0,1);
        Register(262344,"Noah_Daniell",10926,1,6,0,0,1);

        Withdraw(82);            // register in 8, withdraw in 8. continuously withdraw, before appointment
        Withdraw(83);            // register in 8, withdraw in 8. continuously withdraw, medium risk
        Change_Status(74, 5, 1); // register in 7, continuously change in 8. with letter
        Change_Status(75, 6, 2); // register in 7, continuously change in 8,increase risk from 1 to 2,
        Change_Status(76, 7, 3); // register in 7, continuously change in 8,increase risk from 1 to 3,
        break;
    }
    case 9:
    {
        Withdraw(29); // register in 2, withdraw in 9. high risk

        Register(742680,"Olivia_Stevenson",11616,3,7,2,0,1);
        Register(138806,"Polly_Halifax",16592,3,6,2,3,2);
        Register(647472,"Thera_Joshua",15292,4,1,0,0,3);
        Register(747895,"Ferdinand_Galsworthy",19343,7,2,0,2,1);
        Register(289978,"Archer_Bird",14897,7,6,1,0,1);
        Register(889087,"Una_Harrington",11288,7,5,1,0,1);
        Register(216961,"Nat_Hume",17719,8,5,2,0,2);
        Register(649728,"Beacher_Dickens",14766,4,6,0,0,1);
        Register(652981,"Xavier_Bethune",14048,6,6,1,0,3);
        Register(351062,"Dale_Eipstein",13024,2,1,2,0,1);

        Change_Status(88, 3, 1); // register in 8, change in 9.decrease risk from 3 to 1
        break;
    }
    }
    /* ==== test data end ====*/
}

void System::n_Day_Pass(int n)
{
    for (int i = 1; i <= 2 * n; i++)
        Half_Day_Pass();
}

/*void System::Weekly_Report()    //this one only can sort by name
{
    cout << "==Weekly Report==" << endl;
    cout<<"\nsorted by name"<<endl;
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
        //appoint.print();
        node = node->next;
    }
//    Appointment_Queue_Node* queueingp = appoint.head;
    cout<<"\nweekly:appointmentpeople\n"<<endl;
    appoint.print();
//    while(queueingp !=NULL){
//        Person* member = queueingp->head_pointer->person;        
//        cout<<queueingp->head_pointer->reg_id<<" "<<member->name<<" "<<member->profession  //continue
//        <<member->age_group<<member->risk<<counter-Reg_List.reprarray[queueingp->head_pointer->reg_id]->time<<endl;
//        queueingp = queueingp->next;    //the time have to count such way.
//    }//finish the appointming report!!
    appoint.deleteall();

    //this part: about treated people

    Appointment_Queue_Node* treatedp = appointment_queue.Nhead;
    cout<<"\nweekly:treatedpeople\n"<<endl;
    while(treatedp !=NULL && treatedp!=appointment_queue.Rpointer){   //add1
//        Person* member = treatedp->head_pointer->person;
        treatedp->head_pointer->print();
//        cout<<treatedp->head_pointer->reg_id<<" "<<member->name<<member->profession  //continue
//        <<member->age_group<<member->risk<<treatedp->head_pointer->waiting_time<<endl;
        treatedp = treatedp->Nnext;
    }//finish the treated report!!     

    //final part: hardest part: queueing people
    RegWR_Queue instack;
    for (int i = 0;i<global_reg_id;i++){
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

//    Waiting_Queue_Node* instackp = instack.head;
    cout<<"\nweekly:queueingpeople\n"<<endl;
    instack.print();
//    while(instackp !=NULL){
//        Person* member = instackp->head_pointer->person;
//        cout<<instackp->head_pointer->reg_id<<" "<<member->name<<" "<<member->profession  //continue
//        <<member->age_group<<member->risk<<counter-Reg_List.reprarray[instackp->head_pointer->reg_id]->time<<endl;
//        instackp = instackp->next;    //the time have to count such way.
//    }//finish the appointming report!!
    instack.deleteall();  
    //now, over!!
}*/

void System::Monthly_Report()
{
    cout << "====Monthly Report====" << endl;
    cout<<"Registation number in recent Month:"<<Month_global_reg_id_number<<endl;
    cout<<"Total waiting number:"<<Waiting_adder<<endl;
    cout<<"Appointment made number:"<<App_adder<<endl;
    cout<<"number withdraw:"<<Withdraw_adder<<endl;
    cout<<"Average waiting time:"<<(tt_time+(Month_global_reg_id_number-tt_time_count)*counter) / global_reg_id << " half days"<<endl;
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

    cout << "Reg_List is:\n";
    Reg_List.PrintTree();
    cout << "\n";
}

/*void System::Weekly_ReportA()    //this one only can sort by name
{
    cout << "==Weekly Report==" << endl;
    cout<<"\nsorted by age group"<<endl;
    // Part for appointmenting::
    AppWR_Queue appoint;
    Appointment_Queue_Node * node;
    node = appointment_queue.Rpointer;
    while(node!=NULL){
        Appointment_Queue_Node* p = appoint.head;
        while(1){
            if (p == NULL) {appoint.iinsert(node->head_pointer,NULL);break;}
            if(node->head_pointer->person->age_group>p->head_pointer->person->age_group){
                if(p->next == NULL){appoint.iinsert(node->head_pointer,p);break;}
                p = p->next;       
            }
            else {appoint.iinsert(node->head_pointer,p->prev);break;}
        }
        //appoint.print();
        node = node->next;
    }
//    Appointment_Queue_Node* queueingp = appoint.head;
    cout<<"\nweekly:appointmentpeople\n"<<endl;
    appoint.print();
//    while(queueingp !=NULL){
//        Person* member = queueingp->head_pointer->person;        
//        cout<<queueingp->head_pointer->reg_id<<" "<<member->name<<" "<<member->profession  //continue
//        <<member->age_group<<member->risk<<counter-Reg_List.reprarray[queueingp->head_pointer->reg_id]->time<<endl;
//        queueingp = queueingp->next;    //the time have to count such way.
//    }//finish the appointming report!!
    appoint.deleteall();

    //this part: about treated people

    Appointment_Queue_Node* treatedp = appointment_queue.Ahead;
    cout<<"\nweekly:treatedpeople\n"<<endl;
    while(treatedp !=NULL && treatedp!=appointment_queue.Rpointer){   //add1
//        Person* member = treatedp->head_pointer->person;
        treatedp->head_pointer->print();
//        cout<<treatedp->head_pointer->reg_id<<" "<<member->name<<member->profession  //continue
//        <<member->age_group<<member->risk<<treatedp->head_pointer->waiting_time<<endl;
        treatedp = treatedp->Nnext;
    }//finish the treated report!!     

    //final part: hardest part: queueing people
    RegWR_Queue instack;
    for (int i = 0;i<global_reg_id;i++){
        int judge = Reg_List.reprarray[i]->status;
        if(judge>=1&&judge<=5){  //IMPORTANT: here need to fix, i don't know exact status that proper
            Waiting_Queue_Node* p1 = instack.head;
            while (1){                  
                if (p1 == NULL) {instack.iinsert(Reg_List.reprarray[i],NULL);break;}
                if(Reg_List.reprarray[i]->person->age_group>p1->head_pointer->person->age_group){
                    if(p1->next == NULL){instack.iinsert(Reg_List.reprarray[i],p1);break;}
                    p1 = p1->next;      
                }
                else {instack.iinsert(Reg_List.reprarray[i],p1->prev);break;}   
            }         
        }
    }// finish the sort

//    Waiting_Queue_Node* instackp = instack.head;
    cout<<"\nweekly:queueingpeople\n"<<endl;
    instack.print();
//    while(instackp !=NULL){
//        Person* member = instackp->head_pointer->person;
//        cout<<instackp->head_pointer->reg_id<<" "<<member->name<<" "<<member->profession  //continue
//        <<member->age_group<<member->risk<<counter-Reg_List.reprarray[instackp->head_pointer->reg_id]->time<<endl;
//        instackp = instackp->next;    //the time have to count such way.
//    }//finish the appointming report!!
    instack.deleteall();  
    //now, over!!
}*/

/*void System::Weekly_ReportP()    //this one only can sort by Profession
{
    cout << "==Weekly Report==" << endl;
    cout<<"\nsorted by profession"<<endl;
    // Part for appointmenting::
    AppWR_Queue appoint;
    Appointment_Queue_Node * node;
    node = appointment_queue.Rpointer;
    while(node!=NULL){
        Appointment_Queue_Node* p = appoint.head;
        while(1){
            if (p == NULL) {appoint.iinsert(node->head_pointer,NULL);break;}
            if(node->head_pointer->person->profession > p->head_pointer->person->profession){
                if(p->next == NULL){appoint.iinsert(node->head_pointer,p);break;}
                p = p->next;       
            }
            else {appoint.iinsert(node->head_pointer,p->prev);break;}
        }
        //appoint.print();
        node = node->next;
    }
//    Appointment_Queue_Node* queueingp = appoint.head;
    cout<<"\nweekly:appointmentpeople\n"<<endl;
    appoint.print();
//    while(queueingp !=NULL){
//        Person* member = queueingp->head_pointer->person;        
//        cout<<queueingp->head_pointer->reg_id<<" "<<member->name<<" "<<member->profession  //continue
//        <<member->age_group<<member->risk<<counter-Reg_List.reprarray[queueingp->head_pointer->reg_id]->time<<endl;
//        queueingp = queueingp->next;    //the time have to count such way.
//    }//finish the appointming report!!
    appoint.deleteall();

    //this part: about treated people

    Appointment_Queue_Node* treatedp = appointment_queue.Phead;
    cout<<"\nweekly:treatedpeople\n"<<endl;
    while(treatedp !=NULL && treatedp!=appointment_queue.Rpointer){   //add1
//        Person* member = treatedp->head_pointer->person;
        treatedp->head_pointer->print();
//        cout<<treatedp->head_pointer->reg_id<<" "<<member->name<<member->profession  //continue
//        <<member->age_group<<member->risk<<treatedp->head_pointer->waiting_time<<endl;
        treatedp = treatedp->Nnext;
    }//finish the treated report!!     

    //final part: hardest part: queueing people
    RegWR_Queue instack;
    for (int i = 0;i<global_reg_id;i++){
        int judge = Reg_List.reprarray[i]->status;
        if(judge>=1&&judge<=5){  //IMPORTANT: here need to fix, i don't know exact status that proper
            Waiting_Queue_Node* p1 = instack.head;
            while (1){                  
                if (p1 == NULL) {instack.iinsert(Reg_List.reprarray[i],NULL);break;}
                if(Reg_List.reprarray[i]->person->profession>p1->head_pointer->person->profession){
                    if(p1->next == NULL){instack.iinsert(Reg_List.reprarray[i],p1);break;}
                    p1 = p1->next;      
                }
                else {instack.iinsert(Reg_List.reprarray[i],p1->prev);break;}   
            }         
        }
    }// finish the sort

//    Waiting_Queue_Node* instackp = instack.head;
    cout<<"\nweekly:queueingpeople\n"<<endl;
    instack.print();
//    while(instackp !=NULL){
//        Person* member = instackp->head_pointer->person;
//        cout<<instackp->head_pointer->reg_id<<" "<<member->name<<" "<<member->profession  //continue
//        <<member->age_group<<member->risk<<counter-Reg_List.reprarray[instackp->head_pointer->reg_id]->time<<endl;
//        instackp = instackp->next;    //the time have to count such way.
//    }//finish the appointming report!!
    instack.deleteall();  
    //now, over!!
}*/
