#ifndef Data_Structure_h
#define Data_Structure_h
#include <iomanip>
#include <cmath>
#include <string>
#include <iostream>
using std::string;
using namespace std;


template <class T> class FibNode;
class Waiting_Queue_Node;
class Appointment_Queue_Node;

class Person
{
public:
    int id;
    string name;
    int contact;
    int profession; //1~8
    int age_group; //1~7
    int risk;  //0~3
    int letter;  //priority letter. if none, letter=0. letter>counter, indicating this registration should be taken care of (turn into appointment before date "letter")   

public:
    void print();
};

void Person::print()
{
    cout << "id: " <<id<<"\nname: "<<name<<"\ncontact: "<<contact<<"\nprofession: "<<profession<<"\nage_group: "<<age_group<<"\nrisk: "<<risk<<"\nletter: "<<letter<<"\n";
}

class Registration
{
public:
    Person *person=NULL;
    int time;  //registration time, = current counter
    int reg_id; //The unique identifier for each registration, starting from 0
    int registry;  //1~3
    string priority;
    int status; // 0~6. 0 indicates finished, 1~6 indicate the current location of the registration
    // handles. The number at the end indicate the corresponding status
    FibNode<string> *loc_or_central_node=NULL; //1
    FibNode<int> *letter_heap_node=NULL; //2
    Waiting_Queue_Node *mid_risk_node=NULL; //3
    Waiting_Queue_Node *high_risk_node=NULL; //4
    Waiting_Queue_Node *withdraw_then_register_node=NULL; //5
    Appointment_Queue_Node *appointment_queue_node=NULL; //0

public:
    void set_priority(void);
    void print(void);
};

void Registration::set_priority(void)
{
    priority.clear();
    priority += to_string(person->profession);
    priority += to_string(person->age_group);
    priority += to_string(time);
}
void Registration::print(void)
{
    cout << "reg_id: " << reg_id << "   "
         << "reg_time: " << time << "   "
         << "status: " << status << "   "
         << "letter: " << person->letter << "   "
         << "personal_id: " << person->id << "   "
         << "profession: " << person->profession << "   "
         << "age_group: " << person->age_group << "   "
         << "risk: " << person->risk << "   "
         << "name: " << person->name << "\n";
}


class Appointment
{
public:
    Person *person;
    int reg_id; //the registration id
    int loc; //1~3, appointment location
    int time; //The appointment time
    int waiting_time; //The time the person has waited from register to appointment finished

public:
    void print();
};

void Appointment::print()
{
    cout << "reg_id: " << reg_id << "   "
         << "appointed_time: " << time << "   "
         << "loc: " << loc << "   "
         << "waiting_time: " << waiting_time << "   "
         << "personal_id: " << person->id << "   "
         << "profession: " << person->profession << "   "
         << "age_group: " << person->age_group << "   "
         << "risk: " << person->risk << "   "
         << "name: " << person->name << "\n";
}


#endif //Data_Structure_h
