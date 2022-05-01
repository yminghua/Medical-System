#include "Data_Structure.h"
#include "Fibonacci_heap.h"
#include "Waiting_Queue.cpp"
#include "Appointment_Queue.cpp"
#include "AList.cpp"
#include "..\BP_Tree\BPlusTree.cpp"



class System
{
public:
    FibHeap<string> loc1; //1
    FibHeap<string> loc2; //1
    FibHeap<string> loc3; //1
    FibHeap<string> central; //1
    FibHeap<int> letter_heap; //2
    Waiting_Queue mid_risk; //3               // mid_risk people wait in the queue for 30 days
    Waiting_Queue high_risk; //4              // high_risk people wait indefinite
    Waiting_Queue withdraw_then_register; //5 // re-register people wait in the queue for 14 days
    Waiting_Queue withdrawn; //6              // Record people who have withdrawn their registration
    Appointment_Queue appointment_queue; // 0
    BPlusTree Reg_List;                  // Store all the registration that's been made so far, track them by their reg_id
    int counter = 0;        // Record the time the system has been running. If counter=k, it means the system has run for k/2 day. i.e. an increment in the counter means half day passed
    int global_reg_id = 0;

    //for report:
    int week_reg_index = 0;  //every 7 days, reset to the first one who registrate in a new week.
    int Month_global_reg_id_number =0; // adder with 60 period.just like globalregid, but need to clean every60.
    int Waiting_adder=0; //registration need +1,withdraw -1, finish-1, and reset 60 periods.
    int App_adder=0,Withdraw_adder=0; //appointment&withdraw adder.Nothing else. reset60.
    double tt_time = 0; //reset60. every registration, add -counter, every withdraw or finish add counter.
    int tt_time_count = 0;
public:
    void Register(int id, string name, int contact, int profession, int age_group, int risk, int letter, int registry); // Add in a new registration
    void Transfer();                                                                                                    // Transfer the data from the three local heap to the central heap
    void Gen_Appointment();                                                                                             // Generate appoinments for the next day based on registrations stored in the central heap
    void Withdraw(int reg_id);                                                                                          // a withdraw request
    void Change_Status(int reg_id, int new_profession, int new_risk);                                                   // Change the status of a person in the waiting list (i.e. in local or central heap)
    void Half_Day_Pass();                                                                                               // Indicate that half day has passed in the system
    void n_Day_Pass(int n);
    void Weekly_Report();
    void Weekly_ReportP();
    void Weekly_ReportA();
    void Monthly_Report();
    void print();
};
