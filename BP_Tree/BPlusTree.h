/* BPlusTree.h

B+�������ļ���������ʵ��һ���򵥵�B+��

Definition (from http://www.seanster.com/BplusTree/BplusTree.html ):
(1) A B+ tree of order v consists of a root, internal nodes and leaves.
(2) The root my be either leaf or node with two or more children.
(3) Internal nodes contain between v and 2v keys, and a node with k keys has k + 1 children.
(4) Leaves are always on the same level.
(5) If a leaf is a primary index, it consists of a bucket of records, sorted by search key. If it is a secondary index, it will have many short records consisting of a key and a pointer to the actual record.

(1) һ��v�׵�B+���ɸ���㡢�ڲ�����Ҷ�ӽ����ɡ�
(2) ����������Ҷ�ӽ�㣬Ҳ������������������������ڲ���㡣
(3) ÿ���ڲ�������v - 2v���������һ���ڲ�������k������������ֻ��k+1��ָ��������ָ�롣
(4) Ҷ�ӽ������������ͬһ���ϡ�
(5) ���Ҷ�ӽ������������������һ�鰴��ֵ����ļ�¼�����Ҷ�ӽ���Ǵ�������������һ��̼�¼��ÿ���̼�¼����һ�����Լ�ָ��ʵ�ʼ�¼��ָ�롣
(6) �ڲ����ļ�ֵ��Ҷ�ӽ�������ֵ���Ǵ�С��������ġ�
(7) ���м����У�ÿ�������������е����еļ���С���������ÿ�������������е����еļ������ڵ����������

*/

/* B+ ���Ľף����ڲ�����м�����С��Ŀv��
   Ҳ��Щ�˰ѽ׶���Ϊ�ڲ�����м��������Ŀ����2v��
   һ����ԣ�Ҷ�ӽ����������ݸ������ڲ����������������һ���ģ�Ҳ��2v��(������������Ŀ����Ϊ�˰��ڲ�����Ҷ�ӽ��ͳһ��ͬһ���ṹ�а�)
*/
#define ORDER_V 2 /* Ϊ���������v�̶�Ϊ2��ʵ�ʵ�B+��vֵӦ���ǿ���ġ������v���ڲ��ڵ��м�����Сֵ */

#define MAXNUM_KEY (ORDER_V * 2)        /* �ڲ����������������Ϊ2v */
#define MAXNUM_POINTER (MAXNUM_KEY + 1) /* �ڲ���������ָ��������ָ�������Ϊ2v+1 */
#define MAXNUM_DATA (ORDER_V * 2)       /* Ҷ�ӽ����������ݸ�����Ϊ2v */
#define MAXNUM_BUFFER 2

/* ��ֵ������*/
typedef int KEY_TYPE; /* Ϊ�����������Ϊint���ͣ�ʵ�ʵ�B+����ֵ����Ӧ���ǿ���� */
/*��ע�� Ϊ�������Ҷ�ӽ�������Ҳֻ�洢��ֵ*/

/* ������� */
enum NODE_TYPE
{
    NODE_TYPE_ROOT = 1,     // �����
    NODE_TYPE_INTERNAL = 2, // �ڲ����
    NODE_TYPE_LEAF = 3,     // Ҷ�ӽ��
};

// #define NULL 0
#define INVALID 0

#define FLAG_LEFT 1
#define FLAG_RIGHT 2

#include "../code/Data_Structure.h"
//Declare Registration datatype
// class Registration;

/* ������ݽṹ��Ϊ�ڲ�����Ҷ�ӽ��ĸ��� */
class CNode
{
public:
    CNode();
    virtual ~CNode();

    //��ȡ�����ý������
    NODE_TYPE GetType() { return m_Type; }
    void SetType(NODE_TYPE type) { m_Type = type; }

    // ��ȡ��������Ч���ݸ���
    int GetCount() { return m_Count; }
    void SetCount(int i) { m_Count = i; }

    // ��ȡ������ĳ��Ԫ�أ����м���ָ��ֵ����Ҷ�ӽ��ָ����
    virtual KEY_TYPE GetElement(int i) { return 0; }
    virtual void SetElement(int i, KEY_TYPE value) {}

    // ��ȡ������ĳ��ָ�룬���м���ָָ�룬��Ҷ�ӽ��������
    virtual CNode *GetPointer(int i) { return NULL; }
    virtual void SetPointer(int i, CNode *pointer) {}

    // ��ȡ�����ø����
    CNode *GetFather() { return m_pFather; }
    void SetFather(CNode *father) { m_pFather = father; }

    // ��ȡһ��������ֵܽ��
    CNode *GetBrother(int &flag);

    // ɾ�����
    void DeleteChildren();

public:
    NODE_TYPE m_Type; // ������ͣ�ȡֵΪNODE_TYPE����

    int m_Count; // ��Ч���ݸ��������м���ָ����������Ҷ�ӽ��ָ���ݸ���
    int buffer_Count;
    CNode *m_pFather; // ָ�򸸽���ָ�룬��׼B+���в�û�и�ָ�룬������Ϊ�˸����ʵ�ֽ����Ѻ���ת�Ȳ���
};

/* �ڲ�������ݽṹ */
class CInternalNode : public CNode
{
public:
    CInternalNode();
    virtual ~CInternalNode();

    // ��ȡ�����ü�ֵ�����û���˵�����ִ�1��ʼ��ʵ���ڽ�����Ǵ�0��ʼ��
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

    // ��ȡ������ָ�룬���û���˵�����ִ�1��ʼ
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

    // �ڽ��pNode�ϲ����value
    bool Insert(KEY_TYPE value, CNode *pNode);
    // ɾ����value
    bool Delete(KEY_TYPE value);

    // ���ѽ��
    KEY_TYPE Split(CInternalNode *pNode, KEY_TYPE key);
    // ��Ͻ��(�ϲ����)
    bool Combine(CNode *pNode);
    // ����һ�����һ��Ԫ�ص������
    bool MoveOneElement(CNode *pNode);

public:
    KEY_TYPE m_Keys[MAXNUM_KEY];       // ������
    CNode *m_Pointers[MAXNUM_POINTER]; // ָ������
};

/* Ҷ�ӽ�����ݽṹ */
class CLeafNode : public CNode
{
public:
    
    int if_buf =0;

    void Set_buf(int i){
        if(i == 1) if_buf =1;
    }

    CLeafNode();
    virtual ~CLeafNode();

    // ��ȡ����������
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

    // ��ȡ������ָ�룬��Ҷ�ӽ�������壬ֻ��ʵ�и�����麯��
    CNode *GetPointer(int i)
    {
        return NULL;
    }

    // ��������
    bool Insert(KEY_TYPE value, Registration *data); //modified
    // ɾ������
    bool Delete(KEY_TYPE value);

    // ���ѽ��
    KEY_TYPE Split(CLeafNode *pNode);
    // ��Ͻ��
    bool Combine(CLeafNode *pNode);

public:
    // ����������������ʵ��˫������
    CLeafNode *m_pPrevNode; // ǰһ�����
    CLeafNode *m_pNextNode; // ��һ�����

public:
    KEY_TYPE m_Datas[MAXNUM_DATA]; // ��������
    
public:
    Registration *Reg_Datas[MAXNUM_DATA]; // modified

public:
    Registration *Buffer_Block[MAXNUM_BUFFER];   //for KD standard tree: with buffer
    KEY_TYPE Buffer_Value[MAXNUM_BUFFER]; 

public: //for the overflow block, that means, combine/merge
    void Buffermerge();
    void Buffersort();
    bool Bufferdelete(KEY_TYPE value);
};

/* B+�����ݽṹ */
class BPlusTree
{
public:
    BPlusTree();
    virtual ~BPlusTree();

    // ����ָ��������
    Registration *Search(KEY_TYPE data); //modified
    // ����ָ��������
    bool Insert(KEY_TYPE data, Registration *Reg_data); //modified
    // ɾ��ָ��������
    bool Delete(KEY_TYPE data);

    // �����
    void ClearTree();

    // ��ӡ��
    void PrintTree();

    // ��ת��
    BPlusTree *RotateTree();

    // ������Ƿ�����B+���Ķ���
    bool CheckTree();

    void PrintNode(CNode *pNode);

    // �ݹ����㼰�������Ƿ�����B+���Ķ���
    bool CheckNode(CNode *pNode);

    // ��ȡ�����ø����
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

    // ��ȡ���������
    int GetDepth()
    {
        return m_Depth;
    }

    void SetDepth(int depth)
    {
        m_Depth = depth;
    }

    // ��ȼ�һ
    void IncDepth()
    {
        m_Depth = m_Depth + 1;
    }

    // ��ȼ�һ
    void DecDepth()
    {
        if (m_Depth > 0)
        {
            m_Depth = m_Depth - 1;
        }
    }

public:
    // ����������������ʵ��˫������
    CLeafNode *m_pLeafHead; // ͷ���
    CLeafNode *m_pLeafTail; // β���

public:
    // Ϊ���������Ҷ�ӽ��
    CLeafNode *SearchLeafNode(KEY_TYPE data);
    //��������м���
    bool InsertInternalNode(CInternalNode *pNode, KEY_TYPE key, CNode *pRightSon);
    // ���м�����ɾ����
    bool DeleteInternalNode(CInternalNode *pNode, KEY_TYPE key);

    Registration *idsearch(KEY_TYPE id);
    CNode *m_Root; // �����
    int m_Depth;   // �������

    int buffer_flag =0;
    void buffermode(int open){     //warning: once buffer mode is open, key value of tree will be exchanged.
        if(open == 1) buffer_flag =1;   //id in this mode will becomes age_group id.
        else buffer_flag =0;
    }
};