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

// ��ȡһ��������ֵܽ��
CNode *CNode::GetBrother(int &flag)
{
    // CNode *pFather = GetFather(); //��ȡ�丸���ָ��
    CInternalNode *pFather = (CInternalNode *)(GetFather()); //��ȡ�丸���ָ��
    if (NULL == pFather)
    {
        printf("Father is NULL!!!\n");
        return NULL;
    }

    CNode *pBrother = NULL;
    cout << "father's first key is: " << pFather->GetElement(1) << endl;
    
    for (int i = 1; i <= pFather->GetCount() + 1; i++) // GetCount()��ʾ��ȡ���ݻ�ؼ�������Ҫ��ָ����С1��
    {
        // �ҵ�������λ��
        if (pFather->GetPointer(i) == this)
        {
            cout << "Find!" << endl;
            if (i == (pFather->GetCount() + 1)) //��ʾ��Ϊ���������ұߺ��ӡ�
            {
                pBrother = pFather->GetPointer(i - 1); // ���������һ��ָ�룬ֻ����ǰһ��ָ��
                if (pBrother == NULL)
                {
                    cout << "Brother is NULL" << endl;
                }
                flag = FLAG_LEFT;
                cout << "Brother's first key: " << pBrother->GetElement(1) << endl;
            }
            else
            {
                pBrother = pFather->GetPointer(i + 1); // �����Һ�һ��ָ��
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

// �ݹ�ɾ���ӽ��
void CNode::DeleteChildren()
{
    // GetCount()Ϊ���ؽ���йؼ��ּ����ݵĸ���
    for (int i = 1; i <= GetCount() + 1; i++) // Modified(new)
    {
        CNode *pNode = GetPointer(i);
        if (NULL != pNode) // Ҷ�ӽ��û��ָ��
        {
            pNode->DeleteChildren();
        }

        delete pNode;
    }
}

//���ڲ��ڵ�Ĺؼ��ֺ�ָ��ֱ��ʼ��Ϊ0�Ϳ�
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

// ���м����в������
/*���ʣ��м�����Ҫ����ֵ���ڲ���ֵʱ��ͨ���������ҵ���Ҷ�ӽ���е�λ�ã�Ȼ���ٲ��롣
�м���ͨ����Ҷ�ӽ����Ҫ����ʱ�����Ѻ���������ӽ���������*/
bool CInternalNode::Insert(KEY_TYPE value, CNode *pNode)
{
    int i;
    // ����м���������ֱ�ӷ���ʧ��
    if (GetCount() >= MAXNUM_KEY)
    {
        return false;
    }

    int j = 0;

    // �ҵ�Ҫ�������λ��
    for (i = 0; (value > m_Keys[i]) && (i < m_Count); i++)
    {
    }

    // ��ǰλ�ü������ļ����κ��ƣ��ճ���ǰλ��
    for (j = m_Count; j > i; j--)
    {
        m_Keys[j] = m_Keys[j - 1];
    }

    // ��ǰλ�ü�������ָ�����κ���
    for (j = m_Count + 1; j > i + 1; j--)
    {
        m_Pointers[j] = m_Pointers[j - 1];
    }

    // �Ѽ���ָ����뵱ǰλ��
    m_Keys[i] = value;
    m_Pointers[i + 1] = pNode; // ע���ǵ�i+1��ָ������ǵ�i��ָ��
    pNode->SetFather(this);    // �ǳ���Ҫ  �ú�������˼�ǲ���ؼ���value������ָ������

    m_Count++;

    // ���سɹ�
    return true;
}

// ���м�����ɾ�������Լ��ü����ָ��
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

/* �����м���
�����м���ͷ���Ҷ�ӽ����ȫ��ͬ����Ϊ�м��㲻����2V��������2V+1ָ�룬���������һ��Ϊ2��ָ�뽫�޷��� �䡣
��˸���http://www.seanster.com/BplusTree/BplusTree.html �������� ������㷨�ǣ�
����Ҫ����ļ�key�жϣ�
(1)���keyС�ڵ�V��������ѵ�V���������,�����ҵļ��ֱ�ֵ����������
(2) ���key���ڵ�V+1��������ѵ�V+1���������,�����ҵļ��ֱ�ֵ����������
(3)���key���ڵ�V��V+1����֮�䣬���key��Ϊ Ҫ����ļ���ԭ���ļ�����һ�뵽���������
�������RetKey�����Ǳ��ں������뵽���Ƚ��
*/
KEY_TYPE CInternalNode::Split(CInternalNode *pNode, KEY_TYPE key) // key���²����ֵ��pNode�Ƿ��ѽ��
{
    int i = 0, j = 0;

    // ���Ҫ����ļ�ֵ�ڵ�V��V+1����ֵ�м䣬��Ҫ��תһ�£�����ȴ��������
    if ((key > this->GetElement(ORDER_V)) && (key < this->GetElement(ORDER_V + 1)))
    {
        // �ѵ�V+1 -- 2V�����Ƶ�ָ���Ľ����

        for (i = ORDER_V + 1; i <= MAXNUM_KEY; i++)
        {
            j++;
            pNode->SetElement(j, this->GetElement(i));
            this->SetElement(i, INVALID);
        }

        // �ѵ�V+2 -- 2V+1��ָ���Ƶ�ָ���Ľ����
        j = 0;
        for (i = ORDER_V + 2; i <= MAXNUM_POINTER; i++)
        {
            j++;
            this->GetPointer(i)->SetFather(pNode); // ���������ӽ��ĸ���
            pNode->SetPointer(j+1, this->GetPointer(i)); //Modified
            this->SetPointer(i, INVALID);
        }

        // ���ú�Count����
        this->SetCount(ORDER_V);
        pNode->SetCount(ORDER_V);

        // ��ԭ��ֵ����
        return key;
    }

    // ���´���keyС�ڵ�V����ֵ��key���ڵ�V+1����ֵ�����

    // �ж�����ȡ��V����V+1����
    int position = 0;
    if (key < this->GetElement(ORDER_V))
    {
        position = ORDER_V;
    }
    else
    {
        position = ORDER_V + 1;
    }

    // �ѵ�position�������������Ϊ�µļ�ֵ����
    KEY_TYPE RetKey = this->GetElement(position);

    // �ѵ�position+1 -- 2V�����Ƶ�ָ���Ľ����
    j = 0;
    for (i = position + 1; i <= MAXNUM_KEY; i++)
    {
        j++;
        pNode->SetElement(j, this->GetElement(i));
        this->SetElement(i, INVALID);
    }

    // �ѵ�position+1 -- 2V+1��ָ���Ƶ�ָ���Ľ����(ע��ָ��ȼ���һ��)
    j = 0;
    for (i = position + 1; i <= MAXNUM_POINTER; i++)
    {
        j++;
        this->GetPointer(i)->SetFather(pNode); // ���������ӽ��ĸ���
        pNode->SetPointer(j, this->GetPointer(i));
        this->SetPointer(i, INVALID);
    }

    // �����ȡ����λ��
    this->SetElement(position, INVALID);

    // ���ú�Count����
    this->SetCount(position - 1);
    pNode->SetCount(MAXNUM_KEY - position);

    return RetKey;
}

//��Ͻ�㣬��ָ���м��������ȫ�����е����м���
bool CInternalNode::Combine(CNode *pNode)
{
    // �������
    if (this->GetCount() + pNode->GetCount() + 1 > MAXNUM_DATA) // Ԥ��һ���¼���λ��
    {
        return false;
    }

    // ȡ���ϲ����ĵ�һ�����ӵĵ�һ��Ԫ����Ϊ�¼�ֵ
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

// ����һ�����һ��Ԫ�ص������
bool CInternalNode::MoveOneElement(CNode *pNode)
{
    // �������
    if (this->GetCount() >= MAXNUM_DATA)
    {
        return false;
    }

    int i, j;

    // �ֵܽ���ڱ�������
    if (pNode->GetElement(1) < this->GetElement(1))
    {
        // ���ڳ�λ��
        for (i = m_Count; i > 0; i--)
        {
            m_Keys[i] = m_Keys[i - 1];
        }
        for (j = m_Count + 1; j > 0; j--)
        {
            m_Pointers[j] = m_Pointers[j - 1];
        }

        // ��ֵ
        // ��һ����ֵ�����ֵܽ������һ����ֵ�����Ǳ�����һ���ӽ��ĵ�һ��Ԫ�ص�ֵ
        // Modified(new)
        CNode *k = this->GetPointer(1);
        while (k->GetType() != NODE_TYPE_LEAF)
        {
            k = k->GetPointer(1);
        }
        m_Keys[0] = k->GetElement(1);
        //
        // ��һ���ӽ��Ϊ�ֵܽ������һ���ӽ��
        m_Pointers[0] = pNode->GetPointer(pNode->GetCount() + 1);
        pNode->GetPointer(pNode->GetCount() + 1)->SetFather(this); // Modified(new)

        // �޸��ֵܽ��
        pNode->SetElement(pNode->GetCount(), INVALID);
        pNode->SetPointer(pNode->GetCount() + 1, NULL);
    }
    else // �ֵܽ���ڱ�����ұ�
    {
        // ��ֵ
        // ���һ����ֵ�����ֵܽ��ĵ�һ����ֵ�������ֵܽ���һ���ӽ��ĵ�һ��Ԫ�ص�ֵ
        // Modified(new)
        CNode *k = pNode->GetPointer(1);
        while (k->GetType() != NODE_TYPE_LEAF)
        {
            k = k->GetPointer(1);
        }
        m_Keys[m_Count] = k->GetElement(1);

        // ���һ���ӽ��Ϊ�ֵܽ��ĵ�һ���ӽ��
        m_Pointers[m_Count + 1] = pNode->GetPointer(1);
        pNode->GetPointer(1)->SetFather(this); //Modified(new)

        // �޸��ֵܽ��
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
        //
    }

    // ������Ŀ
    this->SetCount(this->GetCount() + 1);
    pNode->SetCount(pNode->GetCount() - 1);

    return true;
}

// ���Ҷ�ӽ���е�����
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

// ��Ҷ�ӽ���в�������
bool CLeafNode::Insert(KEY_TYPE value, Registration *data)
{
    int i, j;
    // ���Ҷ�ӽ��������ֱ�ӷ���ʧ��
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
    // �ҵ�Ҫ�������ݵ�λ��
    for (i = 0; (value > m_Datas[i]) && (i < m_Count); i++)
    {
    }

    // ��ǰλ�ü��������������κ��ƣ��ճ���ǰλ��
    for (j = m_Count; j > i; j--)
    {
        m_Datas[j] = m_Datas[j - 1];
        Reg_Datas[j] = Reg_Datas[j - 1]; // Modified
    }

    // �����ݴ��뵱ǰλ��
    m_Datas[i] = value;
    Reg_Datas[i] = data; //modified

    m_Count++;

    // ���سɹ�
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
    // ���û���ҵ�������ʧ��
    if (false == found)
    {
        //Buffer mode judging & work!  drush8
        if(this->if_buf ==1) return Bufferdelete(value); 
        return false;
    }

    // �������������ǰ��
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

// ����Ҷ�ӽ�㣬�ѱ�Ҷ�ӽ��ĺ�һ�����ݼ��е�ָ����Ҷ�ӽ����
KEY_TYPE CLeafNode::Split(CLeafNode *pNode)
{
    if(this->if_buf ==1) {
        Buffersort();
        Buffermerge();   
        pNode->Buffersort();
        pNode->Buffermerge();
           //drush8
    }

    // �ѱ�Ҷ�ӽ��ĺ�һ�������Ƶ�ָ���Ľ����
    int j = 0;
    for (int i = ORDER_V + 1; i <= MAXNUM_DATA; i++)
    {
        j++;
        pNode->SetElement(j, this->GetElement(i));
        pNode->Reg_Datas[j-1] = this->Reg_Datas[i-1]; // Modified(new)
        this->SetElement(i, INVALID);
        this->Reg_Datas[i-1] = NULL; // Modified(new)
    }
    // ���ú�Count����
    this->SetCount(this->GetCount() - j);
    pNode->SetCount(pNode->GetCount() + j);

    // �����½��ĵ�һ��Ԫ����Ϊ��
    return pNode->GetElement(1);
}

// ��Ͻ�㣬��ָ��Ҷ�ӽ�������ȫ�����е���Ҷ�ӽ��
bool CLeafNode::Combine(CLeafNode *pNode)
{

    // �������
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

// �����в�������
Registration* BPlusTree::Search(KEY_TYPE data)
{
    printf("search begin\n");
    Registration *Reg_result = NULL;
    int i = 0;
    // int offset = 0;
    /*if (NULL != sPath)
    {
        (void)sprintf(sPath + offset, "The serach path is:");
        offset += 19;
    }*/

    printf("1\n");
    CNode *pNode = GetRoot();
    // ѭ�����Ҷ�Ӧ��Ҷ�ӽ��
    printf("2\n");
    while (NULL != pNode)
    {
        printf("3\n");
        // ���ΪҶ�ӽ�㣬ѭ������
        if (NODE_TYPE_LEAF == pNode->GetType())
        {
            break;
        }

        // �ҵ���һ����ֵ���ڵ���key��λ��
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
    printf("4\n");

    // û�ҵ�
    if (NULL == pNode)
    {
        printf("This Reg_id does not exist\n");
        return NULL; // Modified(new)
    }

    /*if (NULL != sPath)
    {
        (void)sprintf(sPath + offset, "%3d", pNode->GetElement(1));
        offset += 3;
    }*/

    // ��Ҷ�ӽ���м�����
    printf("5\n");
    CLeafNode *pNode_leaf = (CLeafNode *)pNode;

    if(pNode_leaf->if_buf ==1) {          ///for buffer merge   drush8
        pNode_leaf->Buffersort();
        pNode_leaf->Buffermerge();
    }
    printf("6\n");
    bool found = false;
    for (i = 1; (i <= pNode_leaf->GetCount()); i++)
    {
        printf("7\n");
        if (data == pNode_leaf->GetElement(i))
        {
            printf("8\n");
            found = true;
            Reg_result = pNode_leaf->Reg_Datas[i-1]; // Modified(new)
            if (Reg_result == NULL)
            {
                cout << "Error!" << endl;
            }
            
        }
        printf("9\n");
    }
    
    // Modified(new)
    if (found == false)
    {
        cout << "This Reg_id does not exist!" << endl;
    }
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
    printf("10\n");
    return Reg_result;
}

/* ��B+���в�������
������������Ҫ�ҵ�������Ҫ�����Ҷ�ӽ�㣬Ȼ������������
(1) Ҷ�ӽ��δ����ֱ���ڸý���в��뼴�ɣ�
(2) Ҷ�ӽ�����������޸����(���������Ҷ�ӽ��)����Ҫ���Ȱ�Ҷ�ӽ����ѣ�Ȼ��ѡ�����ԭ�����½�㣬Ȼ�������ɸ��ڵ㣻
(3) Ҷ�ӽ�����������丸���δ������Ҫ���Ȱ�Ҷ�ӽ����ѣ�Ȼ��ѡ�����ԭ�����½�㣬���޸ĸ�����ָ�룻
(4) Ҷ�ӽ�����������丸�����������Ҫ���Ȱ�Ҷ�ӽ����ѣ�Ȼ��ѡ�����ԭ�����½�㣬���ŰѸ������ѣ����޸��游����ָ�롣
    ��Ϊ�游���Ҳ�����������Կ�����Ҫһֱ�ݹ鵽δ�������Ƚ��Ϊֹ��
*/
bool BPlusTree::Insert(KEY_TYPE data, Registration *Reg_data) //
{
    // ����Ƿ��ظ�����

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

    // ���������Ҷ�ӽ��
    CLeafNode *pOldNode = SearchLeafNode(data);
    // ���û���ҵ���˵���������ǿյģ����ɸ����
    if (NULL == pOldNode)
    {
        pOldNode = new CLeafNode;
        if(this->buffer_flag==1) pOldNode->Set_buf(1);    //drush8
        m_pLeafHead = pOldNode;
        m_pLeafTail = pOldNode;
        SetRoot(pOldNode);
    }

    if(pOldNode->if_buf ==1){pOldNode->Buffersort();pOldNode->Buffermerge();}
    // Ҷ�ӽ��δ������Ӧ���1��ֱ�Ӳ���
    if (pOldNode->GetCount() < MAXNUM_DATA)
    {
        cout << "Insert directly into leaf node" << endl;
        return pOldNode->Insert(data, Reg_data);
    }

    // ԭҶ�ӽ���������½�Ҷ�ӽ�㣬����ԭ����һ�����ݼ��е��½��
    CLeafNode *pNewNode = new CLeafNode;
    if(this->buffer_flag==1) pNewNode->Set_buf(1);  //drush8
    KEY_TYPE key = INVALID;
    printf("split begin\n");
    key = pOldNode->Split(pNewNode);
    printf("split succeed\n");

    // ��˫�������в�����
    CLeafNode *pOldNext = pOldNode->m_pNextNode;

    pOldNode->m_pNextNode = pNewNode;
    pNewNode->m_pNextNode = pOldNext;
    pNewNode->m_pPrevNode = pOldNode;
    printf("1\n");
    if (NULL == pOldNext)
    {
        m_pLeafTail = pNewNode;
    }
    else
    {
        pOldNext->m_pPrevNode = pNewNode;
    }
    printf("2\n");

    // �ж��ǲ��뵽ԭ��㻹���½���У�ȷ���ǰ�����ֵ�����
    if (data < key)
    {
        pOldNode->Insert(data, Reg_data); // ����ԭ���
    }
    else
    {
        pNewNode->Insert(data, Reg_data); // �����½��
    }
    printf("3\n");

    // �����
    CInternalNode *pFather = (CInternalNode *)(pOldNode->GetFather());
    //printf("3.1\n");

    // ���ԭ����Ǹ��ڵ㣬��Ӧ���2
    if (NULL == pFather)
    {
        printf("3.2\n");
        CNode *pNode1 = new CInternalNode;
        pNode1->SetPointer(1, pOldNode); // ָ��1ָ��ԭ���
        pNode1->SetElement(1, key);      // ���ü�
        pNode1->SetPointer(2, pNewNode); // ָ��2ָ���½��
        pOldNode->SetFather(pNode1);     // ָ�������
        pNewNode->SetFather(pNode1);     // ָ�������
        pNode1->SetCount(1);

        SetRoot(pNode1); // ָ���µĸ����
        printf("3.3\n");
        return true;
    }
    printf("4\n");

    // ���3�����4������ʵ��
    bool ret = InsertInternalNode(pFather, key, pNewNode);
    printf("5\n");
    return ret;
}

/* ɾ��ĳ����
ɾ�����ݵ��㷨���£�
(1) ���ɾ����Ҷ�ӽ��������>=50%��ֻ��Ҫ�޸�Ҷ�ӽ�㣬���ɾ�����Ǹ����ļ��������ҲҪ��Ӧ�޸ģ�
(2) ���ɾ����Ҷ�ӽ������<50%����Ҫ���ҵ�һ��������ֵܽ��(���Ҿ���)��Ȼ������������
    A. ������ֵܽ������>50%���Ѹ��ֵܽ������һ�����ݼ��е�����㣬�����ļ�ֵҲҪ��Ӧ�޸ġ�
    B. ������ֵܽ�������=50%������������ϲ���������Ҳ��Ӧ�ϲ���(����ϲ��󸸽�������<50%������Ҫ�ݹ�)
*/
bool BPlusTree::Delete(KEY_TYPE data)
{
    // ���������Ҷ�ӽ��
    CLeafNode *pOldNode = SearchLeafNode(data);
    // ���û���ҵ�������ʧ��
    if (NULL == pOldNode)
    {
        return false;
    }
    printf("Search leaf node success\n");

    // ɾ�����ݣ����ʧ��һ����û���ҵ���ֱ�ӷ���ʧ��
    bool success = pOldNode->Delete(data);
    if (false == success)
    {
        return false;
    }
    printf("Delete data success\n");

    // ��ȡ�����
    CInternalNode *pFather = (CInternalNode *)(pOldNode->GetFather());
    if (NULL == pFather)
    {
        // ���һ�����ݶ�û���ˣ�ɾ�������(ֻ�и��ڵ���ܳ��ִ����)
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
    printf("Get father success\n");

    // ɾ����Ҷ�ӽ��������>=50%����Ӧ���1
    if (pOldNode->GetCount() >= ORDER_V)
    {
        printf("S1\n");
        for (int i = 1; (data >= pFather->GetElement(i)) && (i <= pFather->GetCount()); i++)
        {
            // ���ɾ�����Ǹ����ļ�ֵ����Ҫ���ĸü�
            if (pFather->GetElement(i) == data)
            {
                pFather->SetElement(i, pOldNode->GetElement(1)); // ����ΪҶ�ӽ���µĵ�һ��Ԫ��
                printf("Change father's key success\n");
            }
        }

        return true;
    }

    // �ҵ�һ��������ֵܽ��(����B+���Ķ��壬����Ҷ�ӽ�㣬�������ҵ���)
    int flag = FLAG_LEFT;
    CLeafNode *pBrother = (CLeafNode *)(pOldNode->GetBrother(flag));



    if (pBrother == NULL)
    {
        cout << "Brother is NULL!!!" << endl;
    }
    else
    {
        printf("Get brother success\n");
    }
    

    // �ֵܽ������>50%����Ӧ���2A
    KEY_TYPE NewData = INVALID;
    Registration *New_Reg_Data = NULL;

    if (pBrother->GetCount() > ORDER_V)
    {
        if(pBrother->if_buf ==1) {  //buffer mode necessary   drush8
        pBrother->Buffersort();
        pBrother->Buffermerge();
    }
        printf("S2A\n");
        if (FLAG_LEFT == flag) // �ֵ�����ߣ������һ�����ݹ���
        {
            NewData = pBrother->GetElement(pBrother->GetCount());
            New_Reg_Data = pBrother->Reg_Datas[pBrother->GetCount()-1]; // Modified(new)
        }
        else // �ֵ����ұߣ��Ƶ�һ�����ݹ���
        {
            NewData = pBrother->GetElement(1);
            New_Reg_Data = pBrother->Reg_Datas[0]; // Modified(new)
        }

        pOldNode->Insert(NewData, New_Reg_Data);
        pBrother->Delete(NewData);
        printf("Move one element from brother success\n");

        // �޸ĸ����ļ�ֵ
        if (FLAG_LEFT == flag)
        {
            for (int i = 1; i <= pFather->GetCount() + 1; i++)
            {
                if (pFather->GetPointer(i) == pOldNode && i > 1)
                {
                    pFather->SetElement(i - 1, pOldNode->GetElement(1)); // ���ı�����Ӧ�ļ�
                    printf("Change father's key success\n");
                }
            }
        }
        else
        {
            for (int i = 1; i <= pFather->GetCount() + 1; i++)
            {
                if (pFather->GetPointer(i) == pOldNode && i > 1)
                {
                    pFather->SetElement(i - 1, pOldNode->GetElement(1)); // ���ı�����Ӧ�ļ�
                    printf("Change father's key success\n");
                }
                if (pFather->GetPointer(i) == pBrother && i > 1)
                {
                    pFather->SetElement(i - 1, pBrother->GetElement(1)); // �����ֵܽ���Ӧ�ļ�
                    printf("Change father's key success\n");
                }
            }
        }

        return true;
    }

    // ���2B
    printf("S2B\n");
    // �������Ҫɾ���ļ�
    KEY_TYPE NewKey = NULL;

    // �ѱ�������ֵܽ��ϲ���������κϲ������ݽ�С�Ľ�㣬���������������޸�ָ��

    if (FLAG_LEFT == flag)
    {
        (void)pBrother->Combine(pOldNode);
        NewKey = pOldNode->GetElement(1);

        CLeafNode *pOldNext = pOldNode->m_pNextNode;
        pBrother->m_pNextNode = pOldNext;
        // ��˫��������ɾ�����
        if (NULL == pOldNext)
        {
            m_pLeafTail = pBrother;
        }
        else
        {
            pOldNext->m_pPrevNode = pBrother;
        }
        // ɾ�������
        delete pOldNode;
        printf("Combine success\n");
    }
    else
    {
        // Modified
        for (int i = 1; i <= pFather->GetCount(); i++)
        {
            if (pFather->GetElement(i) == data)
            {
                pFather->SetElement(i, pOldNode->GetElement(1));
                cout << "Change father's key successfully!" << endl;
            }
        }
        //

        (void)pOldNode->Combine(pBrother);
        NewKey = pBrother->GetElement(1);

        CLeafNode *pOldNext = pBrother->m_pNextNode;
        pOldNode->m_pNextNode = pOldNext;
        // ��˫��������ɾ�����
        if (NULL == pOldNext)
        {
            m_pLeafTail = pOldNode;
        }
        else
        {
            pOldNext->m_pPrevNode = pOldNode;
        }
        // ɾ�������
        delete pBrother;
        printf("Combine success\n");
    }

    return DeleteInternalNode(pFather, NewKey);
}

// �����������ɾ�����н��
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

// �ݹ����㼰�������Ƿ�����B+���Ķ���
bool BPlusTree::CheckNode(CNode *pNode)
{
    if (NULL == pNode)
    {
        return true;
    }

    int i = 0;
    bool ret = false;

    // ����Ƿ�����50%������
    if ((pNode->GetCount() < ORDER_V) && (pNode != GetRoot()))
    {
        return false;
    }

    // �����������Ƿ񰴴�С����
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

    // ���м��㣬�ݹ�������
    for (i = 1; i <= pNode->GetCount() + 1; i++)
    {
        ret = CheckNode(pNode->GetPointer(i));
        // ֻҪ��һ�����Ϸ��ͷ��ز��Ϸ�
        if (false == ret)
        {
            return false;
        }
    }

    return true;
}

// ��ӡ������
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

    printf("\n��һ��\n | ");
    PrintNode(pRoot);
    total = 0;
    printf("\n�ڶ���\n | ");
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
    printf("\n������\n | ");
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
    printf("\n���Ĳ�\n | ");
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

// ��ӡĳ���
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

            cout << "(name:" << p->Reg_Datas[i-1]->person->name << ")";
        }
        
        if (i >= MAXNUM_KEY)
        {
            printf(" | ");
        }
    }
}

// ���Ҷ�Ӧ��Ҷ�ӽ��
CLeafNode *BPlusTree::SearchLeafNode(KEY_TYPE data)
{
    int i = 0;

    CNode *pNode = GetRoot();
    // ѭ�����Ҷ�Ӧ��Ҷ�ӽ��
    while (NULL != pNode)
    {
        // ���ΪҶ�ӽ�㣬ѭ������
        if (NODE_TYPE_LEAF == pNode->GetType())
        {
            cout << "Find corresponding leaf node" <<endl;
            break;
        }

        // �ҵ���һ����ֵ���ڵ���key��λ��
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

//�ݹ麯������������м���
bool BPlusTree::InsertInternalNode(CInternalNode *pNode, KEY_TYPE key, CNode *pRightSon)
{
    if (NULL == pNode || NODE_TYPE_LEAF == pNode->GetType())
    {
        return false;
    }

    // ���δ����ֱ�Ӳ���
    if (pNode->GetCount() < MAXNUM_KEY)
    {
        return pNode->Insert(key, pRightSon);
    }

    CInternalNode *pBrother = new CInternalNode; // C++��new ������ʾ����һ������Ҫ���ڴ�ռ䣬���������׵�ַ��
    KEY_TYPE NewKey = INVALID;
    // ���ѱ����
    NewKey = pNode->Split(pBrother, key);

    if (pNode->GetCount() < pBrother->GetCount())
    {
        pNode->Insert(key, pRightSon);
    }
    else if (pNode->GetCount() > pBrother->GetCount())
    {
        pBrother->Insert(key, pRightSon);
    }
    else // ������ȣ�����ֵ�ڵ�V��V+1����ֵ�м����������ֽڵ�ҵ��½��ĵ�һ��ָ����
    {
        pBrother->SetPointer(1, pRightSon);
        pRightSon->SetFather(pBrother);
    }

    CInternalNode *pFather = (CInternalNode *)(pNode->GetFather());
    // ֱ������㶼���ˣ������ɸ����
    if (NULL == pFather)
    {
        pFather = new CInternalNode;
        pFather->SetPointer(1, pNode);    // ָ��1ָ��ԭ���
        pFather->SetElement(1, NewKey);   // ���ü�
        pFather->SetPointer(2, pBrother); // ָ��2ָ���½��
        pNode->SetFather(pFather);        // ָ�������
        pBrother->SetFather(pFather);     // ָ�������
        pFather->SetCount(1);

        SetRoot(pFather); // ָ���µĸ����
        return true;
    }

    // �ݹ�
    return InsertInternalNode(pFather, NewKey, pBrother);
}

// �ݹ麯�������м�����ɾ����
bool BPlusTree::DeleteInternalNode(CInternalNode *pNode, KEY_TYPE key)
{
    // ɾ���������ʧ��һ����û���ҵ���ֱ�ӷ���ʧ��
    bool success = pNode->Delete(key);
    if (false == success)
    {
        printf("failed to delete internal node\n");
        return false;
    }
    printf("Delete internal data success\n");

    // ��ȡ�����
    CInternalNode *pFather = (CInternalNode *)(pNode->GetFather());
    if (NULL == pFather)
    {
        // ���һ�����ݶ�û���ˣ��Ѹ����ĵ�һ�������Ϊ�����
        if (0 == pNode->GetCount())
        {
            SetRoot(pNode->GetPointer(1));
            delete pNode;
        }

        return true;
    }

    // ɾ������������>=50%
    if (pNode->GetCount() >= ORDER_V)
    {
        printf("s1\n");
        for (int i = 1; (key >= pFather->GetElement(i)) && (i <= pFather->GetCount()); i++)
        {
            // ���ɾ�����Ǹ����ļ�ֵ����Ҫ���ĸü�
            if (pFather->GetElement(i) == key)
            {
                pFather->SetElement(i, pNode->GetElement(1)); // ����ΪҶ�ӽ���µĵ�һ��Ԫ��
                printf("Change father's key success\n");
            }
        }

        return true;
    }

    //�ҵ�һ��������ֵܽ��(����B+���Ķ��壬���˸���㣬�������ҵ���)
    int flag = FLAG_LEFT;
    CInternalNode *pBrother = (CInternalNode *)(pNode->GetBrother(flag));

    // �ֵܽ������>50%
    // Modified(new)
    KEY_TYPE NewData_l = pBrother->GetElement(pBrother->GetCount());
    KEY_TYPE NewData_r = pBrother->GetElement(1);
    //
    
    if (pBrother->GetCount() > ORDER_V)
    {
        printf("s2\n");
        pNode->MoveOneElement(pBrother);
        printf("Move element success\n");

        // �޸ĸ����ļ�ֵ
        if (FLAG_LEFT == flag)
        {
            for (int i = 1; i <= pFather->GetCount() + 1; i++)
            {
                if (pFather->GetPointer(i) == pNode && i > 1)
                {
                    pFather->SetElement(i - 1, NewData_l); // ���ı�����Ӧ�ļ� // Modified(new)
                    printf("Change father's key success\n");
                }
            }
        }
        else
        {
            for (int i = 1; i <= pFather->GetCount() + 1; i++)
            {
                // if (pFather->GetPointer(i) == pNode && i > 1)
                // {
                //     pFather->SetElement(i - 1, pNode->GetElement(1)); // ���ı�����Ӧ�ļ�
                // }
                if (pFather->GetPointer(i) == pBrother && i > 1)
                {
                    pFather->SetElement(i - 1, NewData_r); // �����ֵܽ���Ӧ�ļ� //Modified(new)
                    printf("Change father's key success\n");
                }
            }
        }

        return true;
    }

    // �������Ҫɾ���ļ����ֵܽ�㶼������50������Ҫ�ϲ���㣬��ʱ�������Ҫɾ����
    KEY_TYPE NewKey = NULL;
    printf("s3\n");

    // �ѱ�������ֵܽ��ϲ���������κϲ������ݽ�С�Ľ�㣬���������������޸�ָ��
    if (FLAG_LEFT == flag)
    {
        (void)pBrother->Combine(pNode);
        NewKey = pNode->GetElement(1);
        delete pNode;
        printf("Combine success\n");
    }
    else
    {
        (void)pNode->Combine(pBrother);
        NewKey = pBrother->GetElement(1);
        delete pBrother;
        printf("Combine success\n");
    }

    // �ݹ�
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
