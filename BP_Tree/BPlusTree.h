/**
 * C++: BPlusTree
 * 
 * @author Mr.Yang
 * URL: https://www.cnblogs.com/yangj-Blog/p/12992124.html
 * Modified by CS225 Group D3
*/


#define ORDER_V 2

#define MAXNUM_KEY (ORDER_V * 2)        //Maximum number of key in an internal node
#define MAXNUM_POINTER (MAXNUM_KEY + 1) //Maximum number of pointers in an internal node
#define MAXNUM_DATA (ORDER_V * 2)       //Maximum number of data in a leaf node

//Key type
typedef int KEY_TYPE;

//Node type
enum NODE_TYPE
{
    NODE_TYPE_ROOT = 1,     // Root
    NODE_TYPE_INTERNAL = 2, // Internal node
    NODE_TYPE_LEAF = 3,     // Leaf node
};

// #define NULL 0
#define INVALID 0

#define FLAG_LEFT 1
#define FLAG_RIGHT 2

#include "../code/Data_Structure.h" //Declare Registration datatype

/* Parent class of internal and leaf node */
class CNode
{
public:
    CNode();
    virtual ~CNode();

    //Get and set the type of node
    NODE_TYPE GetType() { return m_Type; }
    void SetType(NODE_TYPE type) { m_Type = type; }

    //Get and set the number of key/data
    int GetCount() { return m_Count; }
    void SetCount(int i) { m_Count = i; }

    //Get and set one specific key/data
    virtual KEY_TYPE GetElement(int i) { return 0; }
    virtual void SetElement(int i, KEY_TYPE value) {}

    //Get and set one specific pointer
    virtual CNode *GetPointer(int i) { return NULL; }
    virtual void SetPointer(int i, CNode *pointer) {}

    //Get and set the father node
    CNode *GetFather() { return m_pFather; }
    void SetFather(CNode *father) { m_pFather = father; }

    //Get a brother node
    CNode *GetBrother(int &flag);

    //Delete node
    void DeleteChildren();

public:
    NODE_TYPE m_Type; // Node type

    int m_Count; // Valid key/data number

    CNode *m_pFather; // Pointer to the father node
};

/* Internal node */
class CInternalNode : public CNode
{
public:
    CInternalNode();
    virtual ~CInternalNode();

    //Get and set key (starting from 1)
    KEY_TYPE GetElement(int i)
    {
        if ((i > 0) && (i <= MAXNUM_KEY))
        {
            return m_Keys[i - 1];
        }
        else
        {
            return INVALID;
        }
    }

    void SetElement(int i, KEY_TYPE key)
    {
        if ((i > 0) && (i <= MAXNUM_KEY))
        {
            m_Keys[i - 1] = key;
        }
    }

    //Get and set pointer (starting from 1)
    CNode *GetPointer(int i)
    {
        if ((i > 0) && (i <= MAXNUM_POINTER))
        {
            return m_Pointers[i - 1];
        }
        else
        {
            return NULL;
        }
    }

    void SetPointer(int i, CNode *pointer)
    {
        if ((i > 0) && (i <= MAXNUM_POINTER))
        {
            m_Pointers[i - 1] = pointer;
        }
    }

    //Insert key into node
    bool Insert(KEY_TYPE value, CNode *pNode);
    //Delete key value
    bool Delete(KEY_TYPE value);

    //Split node
    KEY_TYPE Split(CInternalNode *pNode, KEY_TYPE key);
    //Combine node
    bool Combine(CNode *pNode);
    //Move one element from another node
    bool MoveOneElement(CNode *pNode);

public:
    KEY_TYPE m_Keys[MAXNUM_KEY];       //keys
    CNode *m_Pointers[MAXNUM_POINTER]; //pointers
};

/* Leaf node */
class CLeafNode : public CNode
{
public:
    CLeafNode();
    virtual ~CLeafNode();

    //Get and set data
    KEY_TYPE GetElement(int i)
    {
        if ((i > 0) && (i <= MAXNUM_DATA))
        {
            return m_Datas[i - 1];
        }
        else
        {
            return INVALID;
        }
    }

    void SetElement(int i, KEY_TYPE data)
    {
        if ((i > 0) && (i <= MAXNUM_DATA))
        {
            m_Datas[i - 1] = data;
        }
    }

    //Get and set pointer (NULL)
    CNode *GetPointer(int i)
    {
        return NULL;
    }

    //Insert data
    bool Insert(KEY_TYPE value, Registration *data); //modified
    //Delete data
    bool Delete(KEY_TYPE value);

    //Split node
    KEY_TYPE Split(CLeafNode *pNode);
    //Combine node
    bool Combine(CLeafNode *pNode);

public:
    //Double-linked list
    CLeafNode *m_pPrevNode;
    CLeafNode *m_pNextNode;

public:
    KEY_TYPE m_Datas[MAXNUM_DATA]; // Data (Reg_id)
public:
    Registration *Reg_Datas[MAXNUM_DATA]; // Regestration information // modified
};

/* B+ Tree */
class BPlusTree
{
public:
    BPlusTree();
    virtual ~BPlusTree();

    //Search data
    Registration *Search(KEY_TYPE data); //modified
    //Insert data
    bool Insert(KEY_TYPE data, Registration *Reg_data); //modified
    //Delete data
    bool Delete(KEY_TYPE data);

    //Clear Tree
    void ClearTree();

    //Print Tree
    void PrintTree();

    // BPlusTree *RotateTree();

    //Check whether the tree satisfies the definition of B+ Tree
    bool CheckTree();

    void PrintNode(CNode *pNode);

    //Check whether nodes satisfy the definition of B+ Tree
    bool CheckNode(CNode *pNode);

    //Get and set root node
    CNode *GetRoot()
    {
        return m_Root;
    }

    void SetRoot(CNode *root)
    {
        //Modified
        if (root != NULL)
        {
            m_Root = root;
            root->m_pFather = NULL;
            return;
        }
        //
        m_Root = root;
    }

    //Get and set depth
    int GetDepth()
    {
        return m_Depth;
    }

    void SetDepth(int depth)
    {
        m_Depth = depth;
    }

    //Increase depth
    void IncDepth()
    {
        m_Depth = m_Depth + 1;
    }

    //Decrease depth
    void DecDepth()
    {
        if (m_Depth > 0)
        {
            m_Depth = m_Depth - 1;
        }
    }

public:
    //Double-liinked list
    CLeafNode *m_pLeafHead;
    CLeafNode *m_pLeafTail;

public:
    //Search leaf node for insert
    CLeafNode *SearchLeafNode(KEY_TYPE data);
    //Insert key into internal node
    bool InsertInternalNode(CInternalNode *pNode, KEY_TYPE key, CNode *pRightSon);
    //Delete key in internal node
    bool DeleteInternalNode(CInternalNode *pNode, KEY_TYPE key);

    CNode *m_Root; // root node
    int m_Depth;   // depth of tree
};