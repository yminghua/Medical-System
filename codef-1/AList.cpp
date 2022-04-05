#include "Data_Structure.h"
#include <string>

class AList
{
public:
    AList(int size = 20);
//    virtual ~AList();
//    int getlength(void);
//    int setitem(int index, Registration* value);
    Registration* getitem(int index);
    void insert(Registration* value);
//    bool member(Registration* value);


    int numitems;
    Registration** reprarray;
    int maxsize, minsize;
    void allocate(void);
//    void deallocate(void);
};

AList::AList(int size)
{
    maxsize = size;
    if (size < 20)
        minsize = size;
    else
        minsize = 20;
    numitems = 0;
    reprarray = new Registration* [maxsize];
}

void AList::insert(Registration* value)
{
    if (numitems == maxsize)
        allocate();
    reprarray[numitems] = value;
    ++numitems;
    return;
}

Registration* AList::getitem(int index)
{
    if ((index >= 0) && (index < numitems))
    {
        return reprarray[index];
    }
    else
    {
        cout << "ALIST: Index error: index out or range\n";
        return NULL;
    }
}

void AList::allocate(void)
{
    int newsize = 2 * maxsize;
    Registration** newarray = new Registration*[newsize];
    for (int i = 0; i < numitems; ++i)
    {
        newarray[i] = reprarray[i];
    }
    delete[] reprarray;
    reprarray = newarray;
    maxsize = newsize;
    return;
}