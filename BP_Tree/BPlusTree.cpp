#include <stdio.h>
#include "stdlib.h"
#include <iostream>
using namespace std;
#include "BPlusTree.h"

CNode::CNode()
{
    m_Type = NODE_TYPE_LEAF;
    m_Count = 0;
    m_pFather = NULL;
}
CNode::~CNode()
{
    DeleteChildren();
}

//Get a nearby brother
CNode *CNode::GetBrother(int &flag)
{
    // CNode *pFather = GetFather();
    CInternalNode *pFather = (CInternalNode *)(GetFather()); //Get pointer of father node //Modified
    if (NULL == pFather)
    {
        printf("Father is NULL!!!\n");
        return NULL;
    }

    CNode *pBrother = NULL;
    cout << "father's first key is: " << pFather->GetElement(1) << endl;
    
    for (int i = 1; i <= pFather->GetCount() + 1; i++)
    {
        //Find the location of current node
        if (pFather->GetPointer(i) == this)
        {
            cout << "Find!" << endl;
            if (i == (pFather->GetCount() + 1)) //Indicate that current node is on the rightmost
            {
                pBrother = pFather->GetPointer(i - 1); //Get its previous pointer
                if (pBrother == NULL)
                {
                    cout << "Brother is NULL" << endl;
                }
                flag = FLAG_LEFT;
                cout << "Brother's first key: " << pBrother->GetElement(1) << endl;
            }
            else
            {
                pBrother = pFather->GetPointer(i + 1); //Find the latter pointer first
                if (pBrother == NULL)
                {
                    cout << "Brother is NULL" << endl;
                }
                flag = FLAG_RIGHT;
                cout << "Brother's first key: " << pBrother->GetElement(1) << endl;
            }
        }
    }

    return pBrother;
}

//Delete children recursively
void CNode::DeleteChildren()
{
    for (int i = 1; i <= GetCount() + 1; i++) // Modified
    {
        CNode *pNode = GetPointer(i);
        if (NULL != pNode)
        {
            pNode->DeleteChildren();
        }

        delete pNode;
    }
}

//Initialize the key and pointer of internal node to INVALID and NULL, respectively
CInternalNode::CInternalNode()
{
    m_Type = NODE_TYPE_INTERNAL;

    int i = 0;
    for (i = 0; i < MAXNUM_KEY; i++)
    {
        m_Keys[i] = INVALID;
    }

    for (i = 0; i < MAXNUM_POINTER; i++)
    {
        m_Pointers[i] = NULL;
    }
}
CInternalNode::~CInternalNode()
{
    for (int i = 0; i < MAXNUM_POINTER; i++)
    {
        m_Pointers[i] = NULL;
    }
}

//Insert key into internal node
bool CInternalNode::Insert(KEY_TYPE value, CNode *pNode)
{
    int i;
    if (GetCount() >= MAXNUM_KEY)
    {
        return false;
    }

    int j = 0;

    //Find the location to insert
    for (i = 0; (value > m_Keys[i]) && (i < m_Count); i++)
    {
    }

    //Move back and keep the current location empty
    for (j = m_Count; j > i; j--)
    {
        m_Keys[j] = m_Keys[j - 1];
    }

    for (j = m_Count + 1; j > i + 1; j--)
    {
        m_Pointers[j] = m_Pointers[j - 1];
    }

    //Put key and pointer into current location
    m_Keys[i] = value;
    m_Pointers[i + 1] = pNode;
    pNode->SetFather(this); //Set father pointer again

    m_Count++;

    return true;
}

//Delete key and pointer in internal node
bool CInternalNode::Delete(KEY_TYPE key)
{
    int i, j, k;
    for (i = 0; (key >= m_Keys[i]) && (i < m_Count); i++)
    {
    }

    for (j = i - 1; j < m_Count - 1; j++)
    {
        m_Keys[j] = m_Keys[j + 1];
    }
    m_Keys[j] = INVALID;

    for (k = i; k < m_Count; k++)
    {
        m_Pointers[k] = m_Pointers[k + 1];
    }
    m_Pointers[k] = NULL;

    m_Count--;

    return true;
}

//Split internal node
KEY_TYPE CInternalNode::Split(CInternalNode *pNode, KEY_TYPE key)
{
    int i = 0, j = 0;

    //Situation 1
    if ((key > this->GetElement(ORDER_V)) && (key < this->GetElement(ORDER_V + 1)))
    {
        // Move key(V+1 -- 2V)

        for (i = ORDER_V + 1; i <= MAXNUM_KEY; i++)
        {
            j++;
            pNode->SetElement(j, this->GetElement(i));
            this->SetElement(i, INVALID);
        }

        // Move pointer(V+2 -- 2V+1)
        j = 0;
        for (i = ORDER_V + 2; i <= MAXNUM_POINTER; i++)
        {
            j++;
            this->GetPointer(i)->SetFather(pNode);
            pNode->SetPointer(j+1, this->GetPointer(i)); //Modified
            this->SetPointer(i, INVALID);
        }

        //Set number of key
        this->SetCount(ORDER_V);
        pNode->SetCount(ORDER_V);

        return key;
    }

    //Situation 2

    int position = 0;
    if (key < this->GetElement(ORDER_V))
    {
        position = ORDER_V;
    }
    else
    {
        position = ORDER_V + 1;
    }

    //Get key
    KEY_TYPE RetKey = this->GetElement(position);

    //Move key(position+1 -- 2V)
    j = 0;
    for (i = position + 1; i <= MAXNUM_KEY; i++)
    {
        j++;
        pNode->SetElement(j, this->GetElement(i));
        this->SetElement(i, INVALID);
    }

    //Move pointer(position+1 -- 2V+1)
    j = 0;
    for (i = position + 1; i <= MAXNUM_POINTER; i++)
    {
        j++;
        this->GetPointer(i)->SetFather(pNode);
        pNode->SetPointer(j, this->GetPointer(i));
        this->SetPointer(i, INVALID);
    }

    //Clear the extracted position
    this->SetElement(position, INVALID);

    //Set number of key
    this->SetCount(position - 1);
    pNode->SetCount(MAXNUM_KEY - position);

    return RetKey;
}

bool CInternalNode::Combine(CNode *pNode)
{
    if (this->GetCount() + pNode->GetCount() + 1 > MAXNUM_DATA) //Reserve a place for a new key
    {
        return false;
    }

    //Get the new key
    CNode *k = pNode->GetPointer(1);
    while (k->GetType() != NODE_TYPE_LEAF)
    {
        k = k->GetPointer(1);
    }
    KEY_TYPE NewKey = k->GetElement(1);

    m_Keys[m_Count] = NewKey;
    m_Count++;
    m_Pointers[m_Count] = pNode->GetPointer(1);
    
    pNode->GetPointer(1)->SetFather(this); //Modified

    for (int i = 1; i <= pNode->GetCount(); i++)
    {
        m_Keys[m_Count] = pNode->GetElement(i);
        m_Count++;
        m_Pointers[m_Count] = pNode->GetPointer(i + 1);

        pNode->GetPointer(i+1)->SetFather(this); //Modified
    }

    return true;
}

bool CInternalNode::MoveOneElement(CNode *pNode)
{
    if (this->GetCount() >= MAXNUM_DATA)
    {
        return false;
    }

    int i, j;

    //Brother is on the left
    if (pNode->GetElement(1) < this->GetElement(1))
    {
        //Make a place first
        for (i = m_Count; i > 0; i--)
        {
            m_Keys[i] = m_Keys[i - 1];
        }
        for (j = m_Count + 1; j > 0; j--)
        {
            m_Pointers[j] = m_Pointers[j - 1];
        }

        //Get new key
        // Modified(new)
        CNode *k = this->GetPointer(1);
        while (k->GetType() != NODE_TYPE_LEAF)
        {
            k = k->GetPointer(1);
        }
        m_Keys[0] = k->GetElement(1);
        
        m_Pointers[0] = pNode->GetPointer(pNode->GetCount() + 1);
        pNode->GetPointer(pNode->GetCount() + 1)->SetFather(this); // Modified(new)

        pNode->SetElement(pNode->GetCount(), INVALID);
        pNode->SetPointer(pNode->GetCount() + 1, NULL);
    }
    else //Brother is on the right
    {
        //Get new key
        // Modified(new)
        CNode *k = pNode->GetPointer(1);
        while (k->GetType() != NODE_TYPE_LEAF)
        {
            k = k->GetPointer(1);
        }
        m_Keys[m_Count] = k->GetElement(1);

        m_Pointers[m_Count + 1] = pNode->GetPointer(1);
        pNode->GetPointer(1)->SetFather(this); //Modified(new)

        for (i = 1; i <= pNode->GetCount() - 1; i++) // Modified(new)
        {
            pNode->SetElement(i, pNode->GetElement(i + 1));
        }
        for (j = 1; j <= pNode->GetCount(); j++) // Modified(new)
        {
            pNode->SetPointer(j, pNode->GetPointer(j + 1));
        }

        // Modified(new)
        pNode->SetElement(pNode->GetCount(), INVALID);
        pNode->SetPointer(pNode->GetCount() + 1, NULL);

    }

    //Set number of key
    this->SetCount(this->GetCount() + 1);
    pNode->SetCount(pNode->GetCount() - 1);

    return true;
}

//Clear data in leaf node
CLeafNode::CLeafNode()
{
    m_Type = NODE_TYPE_LEAF;

    for (int i = 0; i < MAXNUM_DATA; i++)
    {
        m_Datas[i] = INVALID;
        Reg_Datas[i] = NULL; // Modified(new)
    }

    for (int i = 0; i < MAXNUM_BUFFER; i++)
    {
        Buffer_Value[i] = INVALID;
        Buffer_Block[i] = NULL; //necessary if buffer mode is open..   :drush8
    }    

    m_pPrevNode = NULL;
    m_pNextNode = NULL;
}
CLeafNode::~CLeafNode()
{
}

//Insert data in leaf node
bool CLeafNode::Insert(KEY_TYPE value, Registration *data)
{
    int i, j;
    if (GetCount() >= MAXNUM_DATA)
    {
        return false;
    }
    //new added :: for buffer mode!!!!!!!!!!!!!!!1 drush8
    if(this->if_buf ==1){
        if(buffer_Count == MAXNUM_BUFFER){
            Buffersort();
            Buffermerge();
            Buffer_Value[buffer_Count] = value;
            Buffer_Block[buffer_Count] = data;
            buffer_Count++;m_Count++;
            return true;
        }
        m_Count++;
        Buffer_Value[buffer_Count] = value;
        Buffer_Block[buffer_Count] = data;
        buffer_Count++;
//        Buffersort();
//        Buffermerge();        
        return true;
    }
    //buffer mode work is over!!!!!!!!!!!!!!!!!


    //Find the location to insert
    for (i = 0; (value > m_Datas[i]) && (i < m_Count); i++)
    {
    }

    //Move back and keep the current location empty
    for (j = m_Count; j > i; j--)
    {
        m_Datas[j] = m_Datas[j - 1];
        Reg_Datas[j] = Reg_Datas[j - 1]; // Modified
    }

    //Store data
    m_Datas[i] = value;
    Reg_Datas[i] = data; //modified

    m_Count++;

    return true;
}

bool CLeafNode::Delete(KEY_TYPE value)
{
    int i, j;
    bool found = false;
    for (i = 0; i < m_Count; i++)
    {
        if (value == m_Datas[i])
        {
            found = true;
            break;
        }
    }
    if (false == found)
    {
        //Buffer mode judging & work!  drush8
        if(this->if_buf ==1) return Bufferdelete(value); 
        return false;
    }

    //The following data moves forward
    for (j = i; j < m_Count - 1; j++)
    {
        m_Datas[j] = m_Datas[j + 1];
        Reg_Datas[j] = Reg_Datas[j + 1]; // Modified(new)
    }

    m_Datas[j] = INVALID;
    Reg_Datas[j] = NULL; // Modified(new)
    m_Count--;

    // ���سɹ�
    return true;
}

//Split leaf node, cut the last half of the data of this leaf node to the specified leaf node
KEY_TYPE CLeafNode::Split(CLeafNode *pNode)
{
    if(this->if_buf ==1) {
        Buffersort();
        Buffermerge();   
        pNode->Buffersort();
        pNode->Buffermerge();
           //drush8
    }

    //Move the last half of the leaf node to the specified node
    int j = 0;
    for (int i = ORDER_V + 1; i <= MAXNUM_DATA; i++)
    {
        j++;
        pNode->SetElement(j, this->GetElement(i));
        pNode->Reg_Datas[j-1] = this->Reg_Datas[i-1]; // Modified(new)
        this->SetElement(i, INVALID);
        this->Reg_Datas[i-1] = NULL; // Modified(new)
    }

    this->SetCount(this->GetCount() - j);
    pNode->SetCount(pNode->GetCount() + j);

    // return the first element as key
    return pNode->GetElement(1);
}

//Combine node, cut all data of specified leaf node to this leaf node
bool CLeafNode::Combine(CLeafNode *pNode)
{
    if (this->GetCount() + pNode->GetCount() > MAXNUM_DATA)
    {
        return false;
    }

    if(this->if_buf ==1) {
        Buffersort();
        Buffermerge();
    }
    if(pNode->if_buf ==1) {
        pNode->Buffersort();
        pNode->Buffermerge();
    }         //for buffer mode necessary..  drush8

    for (int i = 1; i <= pNode->GetCount(); i++)
    {
        this->Insert(pNode->GetElement(i), pNode->Reg_Datas[i-1]); // Modified(new)
    }

    return true;
}
BPlusTree::BPlusTree()
{
    m_Depth = 0;
    m_Root = NULL;
    m_pLeafHead = NULL;
    m_pLeafTail = NULL;
}
BPlusTree::~BPlusTree()
{
    ClearTree();
}

//Search data in tree
Registration* BPlusTree::Search(KEY_TYPE data)
{
    // printf("search begin\n");
    Registration *Reg_result = NULL;
    int i = 0;
    // int offset = 0;
    /*if (NULL != sPath)
    {
        (void)sprintf(sPath + offset, "The serach path is:");
        offset += 19;
    }*/

    //printf("1\n");
    CNode *pNode = GetRoot();

    //Use loop to find the corresponding leaf node
    //printf("2\n");
    while (NULL != pNode)
    {
        //printf("3\n");
        //The node is the leaf node, and the loop ends
        if (NODE_TYPE_LEAF == pNode->GetType())
        {
            break;
        }

        //Find the first location where the key value is greater than or equal to key
        for (i = 1; (data >= pNode->GetElement(i)) && (i <= pNode->GetCount()); i++)
        {
        }

        /*if (NULL != sPath)
        {
            (void)sprintf(sPath + offset, " %3d -->", pNode->GetElement(1));
            offset += 8;
        }*/

        pNode = pNode->GetPointer(i);
    }
    //printf("4\n");

    //Did not find
    if (NULL == pNode)
    {
        //printf("This Reg_id does not exist\n");
        return NULL; // Modified(new)
    }

    /*if (NULL != sPath)
    {
        (void)sprintf(sPath + offset, "%3d", pNode->GetElement(1));
        offset += 3;
    }*/

    //Keep finding in leaf nodes
   // printf("5\n");
    CLeafNode *pNode_leaf = (CLeafNode *)pNode;

    if(pNode_leaf->if_buf ==1) {          ///for buffer merge   drush8
        pNode_leaf->Buffersort();
        pNode_leaf->Buffermerge();
    }
   // printf("6\n");
    bool found = false;
    for (i = 1; (i <= pNode_leaf->GetCount()); i++)
    {
        //printf("7\n");
        if (data == pNode_leaf->GetElement(i))
        {
            //printf("8\n");
            found = true;
            Reg_result = pNode_leaf->Reg_Datas[i-1]; // Modified(new)
            if (Reg_result == NULL)
            {
                cout << "Error!" << endl;
            }
            
        }
        //printf("9\n");
    }
    
    // Modified(new)
    // if (found == false)
    // {
    //     cout << "This Reg_id does not exist!" << endl;
    // }
    //

    /*if (NULL != sPath)
    {
        if (true == found)
        {

            (void)sprintf(sPath + offset, " ,successed.");
        }
        else
        {
            (void)sprintf(sPath + offset, " ,failed.");
        }
    }*/
    //printf("10\n");
    return Reg_result;
}

//Insert data in tree
bool BPlusTree::Insert(KEY_TYPE data, Registration *Reg_data)
{
    //Check for repeated insertion
    // Modified(new)
    if (Search(data) != NULL)
    {
        return false;
    }
    //

    // for debug
    // if (289 == data)
    //{
    //    printf("\n%d,check failed!",data);
    //}

    //Find the corresponding leaf node
    CLeafNode *pOldNode = SearchLeafNode(data);
    //If not, the entire tree is empty and the root is generated
    if (NULL == pOldNode)
    {
        pOldNode = new CLeafNode;
        if(this->buffer_flag==1) pOldNode->Set_buf(1);    //drush8
        m_pLeafHead = pOldNode;
        m_pLeafTail = pOldNode;
        SetRoot(pOldNode);
    }

    if(pOldNode->if_buf ==1){pOldNode->Buffersort();pOldNode->Buffermerge();}
    //Leaf node is not full, just insert
    if (pOldNode->GetCount() < MAXNUM_DATA)
    {
        //cout << "Insert directly into leaf node" << endl;
        return pOldNode->Insert(data, Reg_data);
    }

    //Leaf node is full, create a new leaf node and cut the data of the last half of the original node to the new node
    CLeafNode *pNewNode = new CLeafNode;
    if(this->buffer_flag==1) pNewNode->Set_buf(1);  //drush8
    KEY_TYPE key = INVALID;
    //printf("split begin\n");
    key = pOldNode->Split(pNewNode);
    //printf("split succeed\n");

    //Insert node in Double-linked list
    CLeafNode *pOldNext = pOldNode->m_pNextNode;

    pOldNode->m_pNextNode = pNewNode;
    pNewNode->m_pNextNode = pOldNext;
    pNewNode->m_pPrevNode = pOldNode;
    //printf("1\n");
    if (NULL == pOldNext)
    {
        m_pLeafTail = pNewNode;
    }
    else
    {
        pOldNext->m_pPrevNode = pNewNode;
    }
    //printf("2\n");

    //Determine whether to insert into the old node or into a new node
    if (data < key)
    {
        pOldNode->Insert(data, Reg_data);
    }
    else
    {
        pNewNode->Insert(data, Reg_data);
    }
    //printf("3\n");

    CInternalNode *pFather = (CInternalNode *)(pOldNode->GetFather());
    //printf("3.1\n");

    //If the original node is the root node
    if (NULL == pFather)
    {
        //printf("3.2\n");
        CNode *pNode1 = new CInternalNode;
        pNode1->SetPointer(1, pOldNode); 
        pNode1->SetElement(1, key);     
        pNode1->SetPointer(2, pNewNode); 
        pOldNode->SetFather(pNode1);    
        pNewNode->SetFather(pNode1);     
        pNode1->SetCount(1);

        SetRoot(pNode1); //set new root
        //printf("3.3\n");
        return true;
    }
    //printf("4\n");

    bool ret = InsertInternalNode(pFather, key, pNewNode);
    //printf("5\n");
    return ret;
}

//Delete data
bool BPlusTree::Delete(KEY_TYPE data)
{
    //Search corresponding leaf node
    CLeafNode *pOldNode = SearchLeafNode(data);

    if (NULL == pOldNode)
    {
        return false;
    }
    //printf("Search leaf node success\n");

    bool success = pOldNode->Delete(data);
    if (false == success)
    {
        return false;
    }
    //printf("Delete data success\n");

    CInternalNode *pFather = (CInternalNode *)(pOldNode->GetFather());
    if (NULL == pFather)
    {
        //If none of the data is left, delete the root
        if (0 == pOldNode->GetCount())
        {
            delete pOldNode;
            m_pLeafHead = NULL;
            m_pLeafTail = NULL;
            SetRoot(NULL);
            cout << "Delete root successfully!\n" << "Tree is empty!" << endl;
        }

        return true;
    }
    //printf("Get father success\n");

    //After deletion, the filling degree of leaf node is still >= 50%
    if (pOldNode->GetCount() >= ORDER_V)
    {
        //printf("S1\n");
        for (int i = 1; (data >= pFather->GetElement(i)) && (i <= pFather->GetCount()); i++)
        {
            //If the parent key is deleted, need to change the key
            if (pFather->GetElement(i) == data)
            {
                pFather->SetElement(i, pOldNode->GetElement(1)); //Change to the new first element of the leaf node
                //printf("Change father's key success\n");
            }
        }

        return true;
    }

    //Find a nearby brother
    int flag = FLAG_LEFT;
    CLeafNode *pBrother = (CLeafNode *)(pOldNode->GetBrother(flag));
    // if (pBrother == NULL)
    // {
    //     cout << "Brother is NULL!!!" << endl;
    // }
    // else
    // {
    //     printf("Get brother success\n");
    // }
    

    //Brother filling degree > 50%
    KEY_TYPE NewData = INVALID;
    Registration *New_Reg_Data = NULL;

    if (pBrother->GetCount() > ORDER_V)
    {
        if(pBrother->if_buf ==1) {  //buffer mode necessary   drush8
        pBrother->Buffersort();
        pBrother->Buffermerge();
    }
    //    printf("S2A\n");
        if (FLAG_LEFT == flag) 
        {
            NewData = pBrother->GetElement(pBrother->GetCount());
            New_Reg_Data = pBrother->Reg_Datas[pBrother->GetCount()-1]; // Modified(new)
        }
        else
        {
            NewData = pBrother->GetElement(1);
            New_Reg_Data = pBrother->Reg_Datas[0]; // Modified(new)
        }

        pOldNode->Insert(NewData, New_Reg_Data);
        pBrother->Delete(NewData);
        //printf("Move one element from brother success\n");

        //Change father's key
        if (FLAG_LEFT == flag)
        {
            for (int i = 1; i <= pFather->GetCount() + 1; i++)
            {
                if (pFather->GetPointer(i) == pOldNode && i > 1)
                {
                    pFather->SetElement(i - 1, pOldNode->GetElement(1));
                    //printf("Change father's key success\n");
                }
            }
        }
        else
        {
            for (int i = 1; i <= pFather->GetCount() + 1; i++)
            {
                if (pFather->GetPointer(i) == pOldNode && i > 1)
                {
                    pFather->SetElement(i - 1, pOldNode->GetElement(1));
                    //printf("Change father's key success\n");
                }
                if (pFather->GetPointer(i) == pBrother && i > 1)
                {
                    pFather->SetElement(i - 1, pBrother->GetElement(1));
                    //printf("Change father's key success\n");
                }
            }
        }

        return true;
    }

    //printf("S2B\n");
    KEY_TYPE NewKey = NULL;

    //Merge this node with its siblings, anyway, into smaller nodes

    if (FLAG_LEFT == flag)
    {
        (void)pBrother->Combine(pOldNode);
        NewKey = pOldNode->GetElement(1);

        CLeafNode *pOldNext = pOldNode->m_pNextNode;
        pBrother->m_pNextNode = pOldNext;
        //Delete node in Double-linked list
        if (NULL == pOldNext)
        {
            m_pLeafTail = pBrother;
        }
        else
        {
            pOldNext->m_pPrevNode = pBrother;
        }
        delete pOldNode;
        //printf("Combine success\n");
    }
    else
    {
        // Modified
        for (int i = 1; i <= pFather->GetCount(); i++)
        {
            if (pFather->GetElement(i) == data)
            {
                pFather->SetElement(i, pOldNode->GetElement(1));
                //cout << "Change father's key successfully!" << endl;
            }
        }
        //

        (void)pOldNode->Combine(pBrother);
        NewKey = pBrother->GetElement(1);

        CLeafNode *pOldNext = pBrother->m_pNextNode;
        pOldNode->m_pNextNode = pOldNext;
        //Delete node in Double-linked list
        if (NULL == pOldNext)
        {
            m_pLeafTail = pOldNode;
        }
        else
        {
            pOldNext->m_pPrevNode = pOldNode;
        }
        delete pBrother;
        //printf("Combine success\n");
    }

    return DeleteInternalNode(pFather, NewKey);
}

//Delete all nodes
void BPlusTree::ClearTree()
{
    CNode *pNode = GetRoot();
    if (NULL != pNode)
    {
        pNode->DeleteChildren();
        // cout << "Delete root!" << endl;
        delete pNode;
    }

    m_pLeafHead = NULL;
    m_pLeafTail = NULL;
    SetRoot(NULL);
}

// ��ת������ƽ�⣬ʵ�����ǰ��������ع�һ��,��������룬�����¿���
/*
BPlusTree *BPlusTree::RotateTree()
{
    BPlusTree *pNewTree = new BPlusTree;
    int i = 0;
    CLeafNode *pNode = m_pLeafHead;
    while (NULL != pNode)
    {
        if(pNode->if_buf ==1) {  //buffer mode necessary  drush8
        pNode->Buffersort();
        pNode->Buffermerge();
        }


        for (int i = 1; i <= pNode->GetCount(); i++)
        {
            (void)pNewTree->Insert(pNode->GetElement(i), pNode->Reg_Datas[i-1]); // Modified(new)
        }

        pNode = pNode->m_pNextNode;
    }

    return pNewTree;
}
// ������Ƿ�����B+���Ķ���
*/
bool BPlusTree::CheckTree()
{
    CLeafNode *pThisNode = m_pLeafHead;
    CLeafNode *pNextNode = NULL;
    while (NULL != pThisNode)
    {
        pNextNode = pThisNode->m_pNextNode;
        if (NULL != pNextNode)
        {
            if(this->buffer_flag==1){   //drush8
                pThisNode->Buffersort();pThisNode->Buffermerge();
                pNextNode->Buffersort();pThisNode->Buffermerge();
            }

            if (pThisNode->GetElement(pThisNode->GetCount()) > pNextNode->GetElement(1))
            {
                return false;
            }
        }
        pThisNode = pNextNode;
    }

    return CheckNode(GetRoot());
}

bool BPlusTree::CheckNode(CNode *pNode)
{
    if (NULL == pNode)
    {
        return true;
    }

    int i = 0;
    bool ret = false;

    //Check whether the 50% fill is satisfied
    if ((pNode->GetCount() < ORDER_V) && (pNode != GetRoot()))
    {
        return false;
    }

    //Checks whether keys or data are sorted by size
    for (i = 1; i < pNode->GetCount(); i++)
    {
        if (pNode->GetElement(i) > pNode->GetElement(i + 1))
        {
            return false;
        }
    }

    if (NODE_TYPE_LEAF == pNode->GetType())
    {
        return true;
    }

    for (i = 1; i <= pNode->GetCount() + 1; i++)
    {
        ret = CheckNode(pNode->GetPointer(i));

        if (false == ret)
        {
            return false;
        }
    }

    return true;
}

//Print the whole tree
void BPlusTree::PrintTree()
{
    CNode *pRoot = GetRoot();
    if (NULL == pRoot)
    {
        cout << "Tree is empty" << endl;
        return;
    }    

    CNode *p1, *p2, *p3;
    int i, j, k;
    int total = 0;

    printf("\nFirst layer\n | ");
    PrintNode(pRoot);
    total = 0;
    printf("\nSecond layer\n | ");
    for (i = 1; i <= MAXNUM_POINTER; i++)
    {
        p1 = pRoot->GetPointer(i);
        if (NULL == p1)
            continue;
        PrintNode(p1);
        total++;
        if (total % 4 == 0)
            printf("\n | ");
    }
    total = 0;
    printf("\nThird layer\n | ");
    for (i = 1; i <= MAXNUM_POINTER; i++)
    {
        p1 = pRoot->GetPointer(i);
        if (NULL == p1)
            continue;
        for (j = 1; j <= MAXNUM_POINTER; j++)
        {
            p2 = p1->GetPointer(j);
            if (NULL == p2)
                continue;
            PrintNode(p2);
            total++;
            if (total % 4 == 0)
                printf("\n | ");
        }
    }
    total = 0;
    printf("\nFourth layer\n | ");
    for (i = 1; i <= MAXNUM_POINTER; i++)
    {
        p1 = pRoot->GetPointer(i);
        if (NULL == p1)
            continue;
        for (j = 1; j <= MAXNUM_POINTER; j++)
        {
            p2 = p1->GetPointer(j);
            if (NULL == p2)
                continue;
            for (k = 1; k <= MAXNUM_POINTER; k++)
            {
                p3 = p2->GetPointer(k);
                if (NULL == p3)
                    continue;
                PrintNode(p3);
                total++;
                if (total % 4 == 0)
                    printf("\n | ");
            }
        }
    }
}

//Print node
void BPlusTree::PrintNode(CNode *pNode)
{
    if (NULL == pNode)
    {
        return;
    }

    for (int i = 1; i <= MAXNUM_KEY; i++)
    {
        printf("%3d ", pNode->GetElement(i));
        if (pNode->m_Type == NODE_TYPE_LEAF && i<=pNode->GetCount())
        {
            CLeafNode *p = (CLeafNode*)pNode;

            if(p->if_buf ==1) {  //buffer mode necessary drush8
                p->Buffersort();
                p->Buffermerge();
            }

            cout << "(name:" << p->Reg_Datas[i-1]->person->name << ")";//Print information you want here~
        }
        
        if (i >= MAXNUM_KEY)
        {
            printf(" | ");
        }
    }
}

//Search corresponding leaf node
CLeafNode *BPlusTree::SearchLeafNode(KEY_TYPE data)
{
    int i = 0;

    CNode *pNode = GetRoot();

    while (NULL != pNode)
    {
        //The node is the leaf node, and the loop ends
        if (NODE_TYPE_LEAF == pNode->GetType())
        {
            //cout << "Find corresponding leaf node" <<endl;
            break;
        }

        //Find the first location where the key value is greater than or equal to key
        for (i = 1; i <= pNode->GetCount(); i++)
        {
            if (data < pNode->GetElement(i))
            {
                break;
            }
        }

        pNode = pNode->GetPointer(i);
    }

    return (CLeafNode *)pNode;
}

//Insert key into internal node
bool BPlusTree::InsertInternalNode(CInternalNode *pNode, KEY_TYPE key, CNode *pRightSon)
{
    if (NULL == pNode || NODE_TYPE_LEAF == pNode->GetType())
    {
        return false;
    }

    //Node is not full, just insert
    if (pNode->GetCount() < MAXNUM_KEY)
    {
        return pNode->Insert(key, pRightSon);
    }

    CInternalNode *pBrother = new CInternalNode;
    KEY_TYPE NewKey = INVALID;
    //Split node
    NewKey = pNode->Split(pBrother, key);

    if (pNode->GetCount() < pBrother->GetCount())
    {
        pNode->Insert(key, pRightSon);
    }
    else if (pNode->GetCount() > pBrother->GetCount())
    {
        pBrother->Insert(key, pRightSon);
    }
    else //key(v) = key(v+1)
    {
        pBrother->SetPointer(1, pRightSon);
        pRightSon->SetFather(pBrother);
    }

    CInternalNode *pFather = (CInternalNode *)(pNode->GetFather());
    //Until the root is full, new root is generated
    if (NULL == pFather)
    {
        pFather = new CInternalNode;
        pFather->SetPointer(1, pNode);   
        pFather->SetElement(1, NewKey);  
        pFather->SetPointer(2, pBrother); 
        pNode->SetFather(pFather);        
        pBrother->SetFather(pFather);     
        pFather->SetCount(1);

        SetRoot(pFather);
        return true;
    }

    return InsertInternalNode(pFather, NewKey, pBrother);
}

// Delete key in internal node
bool BPlusTree::DeleteInternalNode(CInternalNode *pNode, KEY_TYPE key)
{
    bool success = pNode->Delete(key);
    if (false == success)
    {
        //printf("failed to delete internal node\n");
        return false;
    }
    //printf("Delete internal data success\n");

    CInternalNode *pFather = (CInternalNode *)(pNode->GetFather());
    if (NULL == pFather)
    {
        //If none of the data is left, the first node of the root is used as the root
        if (0 == pNode->GetCount())
        {
            SetRoot(pNode->GetPointer(1));
            delete pNode;
        }

        return true;
    }

    //After deletion, the node filling degree is still >= 50%
    if (pNode->GetCount() >= ORDER_V)
    {
        //printf("s1\n");
        for (int i = 1; (key >= pFather->GetElement(i)) && (i <= pFather->GetCount()); i++)
        {
            if (pFather->GetElement(i) == key)
            {
                pFather->SetElement(i, pNode->GetElement(1));
                //printf("Change father's key success\n");
            }
        }

        return true;
    }

    //Find a nearby brother
    int flag = FLAG_LEFT;
    CInternalNode *pBrother = (CInternalNode *)(pNode->GetBrother(flag));

    //Brother filling degree > 50%
    // Modified(new)
    KEY_TYPE NewData_l = pBrother->GetElement(pBrother->GetCount());
    KEY_TYPE NewData_r = pBrother->GetElement(1);
    
    if (pBrother->GetCount() > ORDER_V)
    {
        //printf("s2\n");
        pNode->MoveOneElement(pBrother);
        //printf("Move element success\n");

        if (FLAG_LEFT == flag)
        {
            for (int i = 1; i <= pFather->GetCount() + 1; i++)
            {
                if (pFather->GetPointer(i) == pNode && i > 1)
                {
                    pFather->SetElement(i - 1, NewData_l); // Modified(new)
                    //printf("Change father's key success\n");
                }
            }
        }
        else
        {
            for (int i = 1; i <= pFather->GetCount() + 1; i++)
            {
                // if (pFather->GetPointer(i) == pNode && i > 1)
                // {
                //     pFather->SetElement(i - 1, pNode->GetElement(1));
                // }
                if (pFather->GetPointer(i) == pBrother && i > 1)
                {
                    pFather->SetElement(i - 1, NewData_r); //Modified(new)
                    //printf("Change father's key success\n");
                }
            }
        }

        return true;
    }

    //Keys to be deleted from the parent node: if none of the sibling nodes is greater than 50,
    //the nodes need to be merged, and the parent node needs to delete the key
    KEY_TYPE NewKey = NULL;
    //printf("s3\n");

    //Merge this node with its siblings, anyway, into smaller nodes
    if (FLAG_LEFT == flag)
    {
        (void)pBrother->Combine(pNode);
        NewKey = pNode->GetElement(1);
        delete pNode;
        //printf("Combine success\n");
    }
    else
    {
        (void)pNode->Combine(pBrother);
        NewKey = pBrother->GetElement(1);
        delete pBrother;
        //printf("Combine success\n");
    }

    return DeleteInternalNode(pFather, NewKey);
}
//drush8::
Registration* BPlusTree::idsearch(KEY_TYPE id)
{
    CLeafNode * tar_leaf = SearchLeafNode(id);
    if(this->buffer_flag ==1) {
        tar_leaf->Buffersort();
        tar_leaf->Buffermerge();
    }
    for(int i=0;i<tar_leaf->m_Count;i++){
        if(tar_leaf->m_Datas[i] == id){
            return tar_leaf->Reg_Datas[i];
        }
    }
    return NULL;
}

void CLeafNode::Buffermerge()
{
    KEY_TYPE final_value[MAXNUM_DATA];
    Registration* final_Reigs[MAXNUM_DATA];
    int i=0,j=0,k=0;
    int mm_Count = m_Count - buffer_Count;
    while(i<mm_Count || j<buffer_Count){
        if(i == mm_Count||(j!= buffer_Count && m_Datas[i]>=Buffer_Value[j])){
            final_value[k] = Buffer_Value[j];
            final_Reigs[k] = Buffer_Block[j];
            k++;j++; continue;
        }
        if(j == buffer_Count||(i!= mm_Count && m_Datas[i]<=Buffer_Value[j])){
            final_value[k] = m_Datas[i];
            final_Reigs[k] = Reg_Datas[i];
            k++;i++;
        }    
    }

    for (int l=0;l<k;l++){
        m_Datas[l] = final_value[l];
        Reg_Datas[l] = final_Reigs[l];        
    }
    buffer_Count =0;
}

void CLeafNode::Buffersort(){
    for (int i =0;i<buffer_Count;i++){
        int value = Buffer_Value[i],j=i;
        Registration* reg = Buffer_Block[i];
        while(j>0 && value < Buffer_Value[j-1]){
            j--;
            Buffer_Value[j+1] = Buffer_Value[j];
            Buffer_Block[j+1] = Buffer_Block[j];
        }

        Buffer_Value[j] = value;
        Buffer_Block[j] = reg;
    }
}

bool CLeafNode::Bufferdelete(KEY_TYPE value)
{
    int i, j;
    bool found = false;
    for (i = 0; i < buffer_Count; i++)
    {
        if (value == Buffer_Value[i])
        {
            found = true;
            break;
        }
    }
    // ���û���ҵ�������ʧ��
    if (false == found)
    {
        return false;
    }

    // �������������ǰ��
    for (j = i; j < buffer_Count - 1; j++)
    {
        Buffer_Value[j] = Buffer_Value[j + 1];
        Buffer_Block[j] = Buffer_Block[j + 1]; // Modified(new)
    }

    Buffer_Value[j] = INVALID;
    Buffer_Block[j] = NULL; // Modified(new)
    m_Count--;
    buffer_Count--;
    // ���سɹ�
    return true;
}
