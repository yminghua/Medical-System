/**
 * C++: Fibonacci heap
 *
 * @author skywang
 * @date 2014/04/06
 * URL: https://www.cnblogs.com/skywang12345/p/3659069.html#a3
 * Modified by CS225 Group D3
 */


#ifndef _FIBONACCI_TREE_HPP_
#define _FIBONACCI_TREE_HPP_

#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <typeinfo>
#include "Data_Structure.h"
using namespace std;

template <class T>
class FibNode {
    public:
        T priority;       // key
        int degree;            // degree
        FibNode<T> *left;      // left sibling
        FibNode<T> *right;     // right sibling
        FibNode<T> *child;     // first child node
        FibNode<T> *parent;    // parent node
        bool marked;           // whether the first child is deleted

        Registration *registration;

        FibNode(Registration *data_init,T priority_want): priority(priority_want), registration(data_init), degree(0), marked(false),left(NULL),right(NULL),child(NULL),parent(NULL) {
            priority = priority_want;
            registration = data_init;
            degree = 0;
            marked = false;
            left   = this;
            right  = this;
            parent = NULL;
            child  = NULL;
        }
};

template <class T>
class FibHeap {
    public:
        int total_n;         // Total number of nodes in the heap
        int maxDegree;       // maximum degree
        FibNode<T> *min;     // minimum node (root node of a minimum heap)
        FibNode<T> **cons;   // memory area if maxDegree

    public:
        FibHeap();
        ~FibHeap();

        // Removes the smallest node in the Fibonacci heap
        void removeMin();
        // Merge 'other' into the current heap
        void combine(FibHeap<T> *other);
        // Get the minimum key value in Fibonacci heap and save it in pkey; Return true if success, false otherwise.
        bool minimum(T *pkey);
        // Update oldKey to newkey
        void update(T oldkey, T newkey);
        // Delete the node whose key value is 'key'
        void remove(T key);
        // Whether the Fibonacci heap contains a key value 'key'
        bool contains(T key);
        // Print the Fibonacci heap
        void print();
        // Destroy
        void destroy();

    public:
        // Remove node from the double-linked list
        void removeNode(FibNode<T> *node);
        // Add node to the front of root(in loop list)
        void addNode(FibNode<T> *node, FibNode<T> *root);
        // Link bidirectional linked list B to the end of bidirectional linked list A
        void catList(FibNode<T> *a, FibNode<T> *b);
        // Insert node into the Fibonacci heap
        void insert(FibNode<T> *node);
        // Create a new node for Registration data with priority = p_want and return pointer of node
        FibNode<T>* insert(Registration *data, T p_want);
        // Remove the smallest node of the heap from root list
        FibNode<T>* extractMin();
        // Link node to root
        void link(FibNode<T>* node, FibNode<T>* root);
        // Create space for consolidating
        void makeCons();
        // Merges the left and right trees of the same degree in the root list of the Fibonacci heap
        void consolidate();
        // Modify the degree
        void renewDegree(FibNode<T> *parent, int degree);
        // Detach node from the child links of parent node and make node a member of the root list
        void cut(FibNode<T> *node, FibNode<T> *parent);
        // Perform "cascading_cut" on node
        void cascadingCut(FibNode<T> *node) ;
        // Decrease the node value to key
        void decrease(FibNode<T> *node, T key);
        // Increase the node value to key
        void increase(FibNode<T> *node, T key);
        // Update the key value of node to key
        void update(FibNode<T> *node, T key);
        // Find the node with key value in the minimum heap root
        FibNode<T>* search(FibNode<T> *root, T key);
        // Find the node with key value 'key' in the Fibonacci heap
        FibNode<T>* search(T key);
        // Delete node
        //void remove(FibNode<T> *node);  -> use decrease(node, key) + removeMin() instead
        // Destroy the Fibonacci heap
        void destroyNode(FibNode<T> *node);
        // Print Fibonacci Heap
        void print(FibNode<T> *node, FibNode<T> *prev, int direction);
};

/*
 * Constructor
 */
template <class T>
FibHeap<T>::FibHeap()
{
    total_n = 0;
    maxDegree = 0;
    min = NULL;
    cons = NULL;
}

/*
 * Destructor
 */
template <class T>
FibHeap<T>::~FibHeap()
{
}

/*
 * Remove node from the double-linked list
 */
template <class T>
void FibHeap<T>::removeNode(FibNode<T> *node)
{
    node->left->right = node->right;
    node->right->left = node->left;
}

/*
 * Add node to the front of root(in loop list)
 *   a …… root
 *   a …… node …… root
*/
template <class T>
void FibHeap<T>::addNode(FibNode<T> *node, FibNode<T> *root)
{
    node->left        = root->left;
    root->left->right = node;
    node->right       = root;
    root->left        = node;
}

/*
 * Insert node into the Fibonacci heap
 */
template <class T>
void FibHeap<T>::insert(FibNode<T> *node)
{
    if (total_n == 0)
        min = node;
    else
       {
        addNode(node, min);
        if (node->priority < min->priority)
            min = node;
    }
    total_n++;
}


/*
 * Create a new node for Registration data
 */
template <class T>
FibNode<T>* FibHeap<T>::insert(Registration *data,T p_want)
{
    FibNode<T> *node;

    node = new FibNode<T>(data, p_want);
    if (node != NULL)
        insert(node);
    
    return node;

}

/*
 * Link bidirectional linked list B to the end of bidirectional linked list A
 */
template <class T>
void FibHeap<T>::catList(FibNode<T> *a, FibNode<T> *b)
{
    FibNode<T> *tmp;

    tmp            = a->right;
    a->right       = b->right;
    b->right->left = a;
    b->right       = tmp;
    tmp->left      = b;
}


/*
 * Merge 'other' into the current heap
 */
template <class T>
void FibHeap<T>::combine(FibHeap<T> *other)
{
    if (other==NULL)
        return ;

    if(other->maxDegree > this->maxDegree)
        swap(*this, *other);

    if((this->min) == NULL)                // this has no "minimum node"
    {
        this->min = other->min;
        this->total_n = other->total_n;
        free(other->cons);
        other->min = NULL;
        other->total_n = 0;
        other->maxDegree = 0;
        /*delete other;
        other = new FibHeap<T>;*/
    }
    else if((other->min) == NULL)           // this has "minimum node" && other has no "minimum node"
    {
        free(other->cons);
        other->min = NULL;
        other->total_n = 0;
        other->maxDegree = 0;
        /*delete other;
        other = new FibHeap<T>;*/
    }                                       // this has "minimum node" && other has "minimum node"
    else
    {
        // Add root list in 'other' to 'this'
        catList(this->min, other->min);

        if (this->min->priority > other->min->priority)
            this->min = other->min;
        this->total_n += other->total_n;
        free(other->cons);
        other->min = NULL;
        other->total_n = 0;
        other->maxDegree = 0;
        /*delete other;
        other = new FibHeap<T>;*/
    }
}

/*
 * Remove the smallest node of the heap from root list(Remove the tree of the smallest node from the heap)
 */
template <class T>
FibNode<T>* FibHeap<T>::extractMin()
{
    FibNode<T> *p = min;

    if (p == p->right)
        min = NULL;
    else
    {
        removeNode(p);
        min = p->right;
    }
    p->left = p->right = p;

    return p;
}

/*
 * Link node to root
 */
template <class T>
void FibHeap<T>::link(FibNode<T>* node, FibNode<T>* root)
{
    // Remove node from the double-linked list
    removeNode(node);
    // set node to the child of root
    if (root->child == NULL)
        root->child = node;
    else
        addNode(node, root->child);

    node->parent = root;
    root->degree++;
    node->marked = false;
}

/*
 * Create space for consolidating
 */
template <class T>
void FibHeap<T>::makeCons()
{
    int old = maxDegree;

    // Calculate log2(total_n)，"+1" means round up to an integer
    // ex. log2(13) = 3，round up: 3+1=4
    maxDegree = (log(total_n)/log(2.0)) + 1;
    if (old >= maxDegree)
        return ;

    // Since degree maxDegree may be merged, maxDegree+1 is required
    cons = (FibNode<T> **)realloc(cons,
            sizeof(FibHeap<T> *) * (maxDegree + 1));
}

/*
 * Merges the left and right trees of the same degree in the root list of the Fibonacci heap
 */
template <class T>
void FibHeap<T>::consolidate()
{
    int i, d, D;
    FibNode<T> *x, *y, *tmp;

    makeCons();// create space for hashing
    D = maxDegree + 1;

    for (i = 0; i < D; i++)
        cons[i] = NULL;

    // Merge root nodes of the same degree so that each degree of the tree is unique
    while (min != NULL)
    {
        x = extractMin();                // Fetch the smallest tree in the heap (the tree of the smallest node)
        d = x->degree;                    // Get the degree of the minimum tree
        // cons[d] != NULL，means that there are two trees (x and y) that have the same degree
        while (cons[d] != NULL)
        {
            y = cons[d];                // y is a tree of the same degree as x
            if (x->priority > y->priority)        // Make sure x has a smaller key value than y
                swap(x, y);

            link(y, x);    // link y to x
            cons[d] = NULL;
            d++;
        }
        cons[d] = x;
    }
    min = NULL;

    // Add the nodes in cons back to the root
    for (i=0; i<D; i++)
    {
        if (cons[i] != NULL)
        {
            if (min == NULL)
                min = cons[i];
            else
            {
                addNode(cons[i], min);
                if ((cons[i])->priority < min->priority)
                    min = cons[i];
            }
        }
    }
}

/*
 * Removes the smallest node in the Fibonacci heap
 */
template <class T>
void FibHeap<T>::removeMin()
{
    if (min==NULL)
        return ;

    FibNode<T> *child = NULL;
    FibNode<T> *m = min;
    // Add each of min's sons (son and son's sibling) to the Fibonacci Heap root list
    while (m->child != NULL)
    {
        child = m->child;
        removeNode(child);
        if (child->right == child)
            m->child = NULL;
        else
            m->child = child->right;

        addNode(child, min);
        child->parent = NULL;
    }

    // Remove m from the root list
    removeNode(m);
    // If m is the only node in the heap, set the minimum node of the heap to NULL
    // Otherwise, set the  node of the heap to a non-empty node (m->right), and then adjust
    if (m->right == m)
        min = NULL;
    else
    {
        min = m->right;
        consolidate();
    }
    total_n--;

    delete m;
}

/*
 * Get the minimum key value in the Fibonacci heap and save it in pkey; Returns true on success, false otherwise
 */
template <class T>
bool FibHeap<T>::minimum(T *pkey)
{
    if (min==NULL || pkey==NULL)
        return false;

    *pkey = min->priority;
    return true;
}

/*
 * Modify the degree
 */
template <class T>
void FibHeap<T>::renewDegree(FibNode<T> *parent, int degree)
{
    parent->degree -= degree;
    if (parent-> parent != NULL)
        renewDegree(parent->parent, degree);
}

/*
 * Detach node from the child link of the parent node and make node a member of the root list
 */
template <class T>
void FibHeap<T>::cut(FibNode<T> *node, FibNode<T> *parent)
{
    removeNode(node);
    renewDegree(parent, node->degree);
    // node has no sibling
    if (node == node->right)
        parent->child = NULL;
    else
        parent->child = node->right;

    node->parent = NULL;
    node->left = node->right = node;
    node->marked = false;
    // Add the tree containing node to "root list"
    addNode(node, min);
}

/*
 * Perform "cascading cut" on nodes
 */
template <class T>
void FibHeap<T>::cascadingCut(FibNode<T> *node)
{
    FibNode<T> *parent = node->parent;
    if (parent != NULL)
    {
        if (node->marked == false)
            node->marked = true;
        else
        {
            cut(node, parent);
            cascadingCut(parent);
        }
    }
}

/*
 * Decrease the node value to key
 */
template <class T>
void FibHeap<T>::decrease(FibNode<T> *node, T key)
{
    FibNode<T> *parent;

    if (min==NULL ||node==NULL)
        return ;

    if ( key>=node->priority)
    {
        cout << "decrease failed: the new key(" << key <<") "
             << "is no smaller than current key(" << node->priority <<")" << endl;
        return ;
    }

    node->priority = key;
    parent = node->parent;
    if (parent!=NULL && node->priority < parent->priority)
    {
        // Detach node from parent and add node to the root list
        cut(node, parent);
        cascadingCut(parent);
    }

    // Update minimum node
    if (node->priority < min->priority)
        min = node;
}

/*
 * Increase the node value to key
 */
template <class T>
void FibHeap<T>::increase(FibNode<T> *node, T key)
{
    FibNode<T> *child, *parent, *right;

    if (min==NULL ||node==NULL)
        return ;

    if (key <= node->priority)
    {
        cout << "increase failed: the new key(" << key <<") "
             << "is no greater than current key(" << node->priority <<")" << endl;
        return ;
    }

    // Add each son of node (excluding grandchildren, great-grandchildren...) to root list
    while (node->child != NULL)
    {
        child = node->child;
        removeNode(child);               // Remove child from node's child list
        if (child->right == child)
            node->child = NULL;
        else
            node->child = child->right;

        addNode(child, min);       // Add child to the root list
        child->parent = NULL;
    }
    node->degree = 0;
    node->priority = key;

    // If node is not in the root list, remove it from the child link
    // of the parent node and make it a member of the root list, and then perform "cascading cut"
    // Otherwise, determine whether the smallest node of the heap needs to be updated
    parent = node->parent;
    if(parent != NULL)
    {
        cut(node, parent);
        cascadingCut(parent);
    }
    else if(min == node)
    {
        right = node->right;
        while(right != node)
        {
            if(node->priority > right->priority)
                min = right;
            right = right->right;
        }
    }
}

/*
 * Update the key value of node to key
 */
template <class T>
void FibHeap<T>::update(FibNode<T> *node, T key)
{
    if(key < node->priority)
        decrease(node, key);
    else if(key > node->priority)
        increase(node, key);
    else
        cout << "No need to update!!!" << endl;
}

/*
 * Update oldKey to newkey
 */
template <class T>
void FibHeap<T>::update(T oldkey, T newkey)
{
    FibNode<T> *node;

    node = search(oldkey);
    if (node!=NULL)
        update(node, newkey);
}

/*
 * Find the node with key value in the minimum heap root
 */
template <class T>
FibNode<T>* FibHeap<T>::search(FibNode<T> *root, T key)
{
    FibNode<T> *t = root;    // Temporary node
    FibNode<T> *p = NULL;    // Node to be searched

    if (root==NULL)
        return root;

    do
    {
        if (t->priority == key)
        {
            p = t;
            break;
        }
        else
        {
            if ((p = search(t->child, key)) != NULL)
                break;
        }
        t = t->right;
    } while (t != root);

    return p;
}

/*
 * Find the node with key value 'key' in the Fibonacci heap
 */
template <class T>
FibNode<T>* FibHeap<T>::search(T key)
{
    if (min==NULL)
        return NULL;

    return search(min, key);
}

/*
 * Whether the Fibonacci heap contains a key value 'key'. Return true if it exists, false otherwise
 */
template <class T>
bool FibHeap<T>::contains(T key)
{
    return search(key)!=NULL ? true: false;
}

/*
 * Delete node
 */
// template <class T>
// void FibHeap<T>::remove(FibNode<T> *node)
// {
//     decrease(node, " "); // when type(priority)=string
//     // or
//     decrease(node, 0); // when type(priority)=int

//     removeMin();
// }

/*
 * Delete the node whose key value is 'key'
 */
template <class T>
void FibHeap<T>::remove(T key)
{
    FibNode<T> *node;

    if (min==NULL)
        return ;

    node = search(key);
    if (node==NULL)
        return ;

    remove(node);
    
}

/*
 * Destroy the Fibonacci heap
 */
template <class T>
void FibHeap<T>::destroyNode(FibNode<T> *node)
{
    FibNode<T> *start = node;

    if(node == NULL)
        return;

    do {
        destroyNode(node->child);
        // Destroy node and point it to the next
        node = node->right;
        delete node->left;
    } while(node != start);
}

template <class T>
void FibHeap<T>::destroy()
{
    destroyNode(min);
    free(cons);
}

/*
 * Print Fibonacci heap 
 * node: current node; prev: previous node, parent/sibling; 
 * direction: 1-Indicates that the current node is a left child, 2-Indicates that the current node is a sibling node
 */
template <class T>
void FibHeap<T>::print(FibNode<T> *node, FibNode<T> *prev, int direction)
{
    FibNode<T> *start=node;

    if (node==NULL)
        return ;
    do
    {
        if (direction == 1)
            cout << setw(8) << node->priority << "(" << node->degree << ") is "<< setw(2) << prev->priority << "'s child" << endl;
        else
            cout << setw(8) << node->priority << "(" << node->degree << ") is "<< setw(2) << prev->priority << "'s next" << endl;

        if (node->child != NULL)
            print(node->child, node, 1);

        // sibling node
        prev = node;
        node = node->right;
        direction = 2;
    } while(node != start);
}

template <class T>
void FibHeap<T>::print()
{
    int i=0;
    FibNode<T> *p;

    if (min==NULL) {
        cout << "The heap is emptied\n";
        return ;
    }

    cout << "== Details of Fibonacci heap: ==" << endl;
    p = min;
    do {
        i++;
        cout << setw(2) << i << ". " << setw(4) << p->priority << "(" << p->degree << ") is root" << endl;

        print(p->child, p, 1);
        p = p->right;
    } while (p != min);
    cout << endl;
}


#endif