#ifndef NEWBTREE_H
#define NEWBTREE_H
#include <iostream>
#include <iomanip>
using namespace std;

template <class T>
class BTree
{
public:
    BTree(bool dups = false);
    //big three:
    BTree(const BTree<T>& other);
    ~BTree();
    BTree<T>& operator =(const BTree<T>& RHS);

    void insert(const T& entry);                //insert entry into the tree
    void remove(const T& entry);                //remove entry from the tree

    void clear_tree();                          //clear this object (delete all nodes etc.)
    void copy_tree(const BTree<T>& other);      //copy other into this object

    bool contains(const T& entry);              //true if entry can be found in the array
    T& get(const T& entry);                     //return a reference to entry in the tree
    T* find(const T& entry);                    //return a pointer to this key. NULL if not there.

    int size() const;                           //count the number of elements in the tree
    bool empty() const;                         //true if the tree is empty

    void print_tree(int level = 0, ostream &outs=cout) const;
    //print a readable version of the tree
    template<typename U>
    friend ostream& operator<<(ostream& outs, const BTree<U>& print_me);

public:
    static const int MINIMUM = 1;
    static const int MAXIMUM = 2 * MINIMUM;

    bool dups_ok;                                   //true if duplicate keys may be inserted
    int data_count;                                 //number of data elements
    T data[MAXIMUM + 1];                            //holds the keys
    int child_count;                                //number of children
    BTree* subset[MAXIMUM + 2];                     //subtrees

    bool is_leaf() const {return child_count==0;}   //true if this is a leaf node

    void copy_tree(BTree<T>* left,const BTree<T>* right);   //copys tree

    //inserts the entry into the correct position in data, adjusting data to correct positions if necessary
    void insertPositionLeftMost(const T& entry);
    void insertPositionRightMost(const T& entry);
    void insertPositionMiddle(const T& entry, int h);

    void insertNoChildrenEver(const T& entry);      //if inserting into tree with no children
    void insertChildrenExist(const T& entry);       //if inserting into a tree with children , not first insert, not at the root
    void insertChildrenExistRoot(const T& entry);   //if insesring into a tree with children first insert, at the root
    void insertNoChildrenNow(const T& entry);       //if inserting and node now has no children

    void fix_excessNoChildrenEver();                //fix excess in root in tree with no children

    void fix_excessRootWithChildren();              //fix excess if root with children

    void fix_excessLeftMost();                      //fix excess in leftmost child
    void fix_excessRightMost();                     //fix excess in rightmost child
    void fix_excessMiddle(int i);                   //fix excess in middle child

    //insert element functions
    void loose_insert(const T& entry);              //allows MAXIMUM+1 data elements in the root
    void fix_excess(int i);                         //fix excess of data elements in child i

    //remove element functions:
    void loose_remove(const T& entry);              //allows MINIMUM-1 data elements in the root
    void fix_shortage(int i);                       //fix shortage of data elements in child i

    void remove_biggest(T& entry);                  //remove the biggest child of this tree->entry
    void rotate_left(int i);                        //transfer one element LEFT from child i
    void rotate_right(int i);                       //transfer one element RIGHT from child i
    void merge_with_next_subset(int i);             //merge subset i with subset i+1

};

template<typename T>
void BTree<T>::insertPositionLeftMost(const T& entry){
    //Copy data into sub
    T sub[data_count];
    for(int p = 0; p < data_count; ++p){
        sub[p] = data[p];
    }
    //insert new entry into data[0]
    data[0] = entry;
    //add one to data_count
    ++data_count;
    //Copy all data one position to the right
    int s = 0;
    for(int p = 1; p < data_count; ++p){
        data[p] = sub[s];
        ++s;
    }
}

template<typename T>
void BTree<T>::insertPositionRightMost(const T& entry){
    //Add one to data_count
    ++data_count;
    //Add entry to last position in data
    data[data_count - 1] = entry;
}

template<typename T>
void BTree<T>::insertPositionMiddle(const T& entry, int p){
    //copy the data array right of p
    int u = p;
    T sub[data_count - p];
    for(int i = 0; i < data_count - p; ++i){
        sub[i] = data[u];
        ++u;
    }
    //insert entry into position data[p]
    data[p] = entry;
    //add one to data_count
    ++data_count;
    //move data entries one position to the right
    u = 0;
    for(int i = p + 1; i < data_count; ++i){
        data[i] = sub[u];
        ++u;
    }
}

template<typename T>
void BTree<T>::insertNoChildrenEver(const T& entry){
    //if data_count is empty, enter data and exit
    if(0 == data_count){
        data[0] = entry;
        data_count = 1;
        return;
    }
    //if data_count is not empty, enter data in the correct position
    else {
        //if entry is less than data[0]
        if(entry <= data[0]){
            //check if duplicates are allowed
            if(entry == data[0]){
                if(dups_ok == false){
                    cout << "Duplicated not allowed" << endl;
                    return;
                }
            }
            insertPositionLeftMost(entry);
        }
        //if entry is greater than the last data position
        else if(entry >= data[data_count - 1]){
            //check if duplicates are allowed
            if(entry == data[data_count - 1]){
                if(dups_ok == false){
                    cout << "Duplicates not allowed" << endl;
                    return;
                }
            }
            insertPositionRightMost(entry);
        }
        //if entry is in between first and last data entries
        else{
            //Find out what index the entry should be inserted into
            int p = 0;
            for(int i = 0; i < data_count; ++i){
                if(entry <= data[i]){
                    //check if duplicates are allowed
                    if(entry == data[i]){
                        if(dups_ok == false){
                            cout << "Duplicates not allowed" << endl;
                            return;
                        }
                    }
                    p = i;
                    //if entry is greater than data[i], exit loop
                    if(entry > data[i]){
                        i = data_count - 1;
                    }
                }
            }
            insertPositionMiddle(entry,p);
        }
    }
    //If data_count is above MAXIMUM, tree needs to be adjusted
    if(data_count == MAXIMUM + 1){
        fix_excessNoChildrenEver();
    }
}

template<typename T>
void BTree<T>::insertNoChildrenNow(const T& entry){
    //if entry is less than data[0]
    if(entry <= data[0]){
        //check if duplicates are allowed
        if(entry == data[0]){
            if(dups_ok == false){
                cout << "Duplicated not allowed" << endl;
                return;
            }
        }
        insertPositionLeftMost(entry);
    }
    //if entry is greater than the last data position
    else if(entry >= data[data_count - 1]){
        //check if duplicates are allowed
        if(entry == data[data_count - 1]){
            if(dups_ok == false){
                cout << "Duplicates not allowed" << endl;
                return;
            }
        }
        insertPositionRightMost(entry);
    }
    //if entry is in between first and last data entries
    else{
        //Find out what index the entry should be inserted into
        int p = 0;
        for(int i = 0; i < data_count; ++i){
            if(entry <= data[i]){
                //check if duplicates are allowed
                if(entry == data[i]){
                    if(dups_ok == false){
                        cout << "Duplicates not allowed" << endl;
                        return;
                    }
                }
                p = i;
                //If entry is greater than data[i] exit loop
                if(entry > data[i]){
                    i = data_count - 1;
                }
            }
        }
        insertPositionMiddle(entry,p);
    }
}

template<typename T>
void BTree<T>::insert(const T& entry){
    //If tree has no children
    if(is_leaf() == true){
        insertNoChildrenEver(entry);
        return;
    }
    //If tree has children
    insertChildrenExistRoot(entry);
}

template<typename T>
void BTree<T>::insertChildrenExistRoot(const T& entry){
    //if entry is less than first position in data
    if(entry <= data[0]){
        //check if duplicates are allowed
        if(entry == data[0]){
            if(dups_ok == false){
                cout << "Duplicates not allowed" << endl;
                return;
            }
        }
        //insert entry into subset[0]
        subset[0]->insertChildrenExist(entry);
        //check if subset[0] data_count is not above MAXIMUM
        if(subset[0]->data_count == MAXIMUM + 1){
            fix_excess(0);
        }
    }
    //if entry is greater or equal too last position in data
    else if(entry >= data[data_count-1]){
        //check for duplicates are allowed
        if(entry == data[data_count-1]){
            if(dups_ok == false){
                cout << "Duplicates not allowed" << endl;
                return;
            }
        }
        //insert entry into subset[child_count - 1];
        subset[child_count-1]->insertChildrenExist(entry);
        //check if subset[child_count-1] data_count is not above MAXIMUM
        if(subset[child_count - 1]->data_count == MAXIMUM + 1){
            fix_excess(child_count - 1);
        }
    }
    //if entry is inbetween first and last position in data
    else{
        //Find out what index of the subset we should go into
        int p = 0;
        for(int i = 0; i < data_count; ++i){
            if(entry <= data[i]){
                //check if duplicates are allowed
                if(entry == data[i]){
                    if(dups_ok == false){
                        cout << "Duplicates not allowed" << endl;
                        return;
                    }
                }
                p = i;
            }
            if(entry > data[i]){
                i = data_count - 1;
            }
        }
        //insert entry into subset[p];
        subset[p]->insertChildrenExist(entry);
        //check if subset[child_count-1] data_count is not above MAXIMUM
        if(subset[p]->data_count == MAXIMUM + 1){
            fix_excess(p);
        }
    }

    //check that data_count in the root is not equal to maximum + 1
    if(data_count == MAXIMUM + 1){
        fix_excessRootWithChildren();
    }
}

template<typename T>
void BTree<T>::fix_excessRootWithChildren(){
    //Put tree into a new tree, clear root , then split tree and move up data
    BTree<T> *subTree = new BTree<T>;

    //copy data in root
    for(int j = 0; j < data_count; ++j){
        subTree->insertNoChildrenNow(data[j]);
    }
    //copy children in root
    for(int j = 0; j < child_count; ++j){
        subTree->subset[j] = new BTree<T>;
        copy_tree(subTree->subset[j], subset[j]);
        subTree->child_count += 1;
    }
    //clear tree
    clear_tree();
    //rebuild tree, take middle of data values and put it in root, take the left and right values of the middle
    //and make them the two new children of the root
    //Then split the subsets as children of the new children of the root
    //CREATE NEW FIRST SUBSETS and ENTER DATA FOR ROOT
    T capture = subTree->data[subTree->data_count / 2];
    insert(capture);
    subset[0] = new BTree<T>;
    subset[1] = new BTree<T>;
    child_count = 2;
    for(int j = 0; j < subTree->data_count / 2; ++j){
        subset[0]->insert(subTree->data[j]);
    }
    for(int j = (subTree->data_count / 2) + 1; j < subTree->data_count; ++j){
        subset[1]->insert(subTree->data[j]);
    }
    //SPLIT AND COPY CHILDREN FOR FIRST SUBSETS OF TREE
    for(int j = 0; j < subTree->child_count / 2; ++j){
        subset[0]->subset[j] = new BTree<T>;
        copy_tree(subset[0]->subset[j],subTree->subset[j]);
        subset[0]->child_count += 1;
    }
    int p = 0;
    for(int j = subTree->child_count / 2; j < subTree->child_count; ++j){
        subset[1]->subset[p] = new BTree<T>;
        copy_tree(subset[1]->subset[p],subTree->subset[j]);
        subset[1]->child_count += 1;
        ++p;
    }
}

template <typename T>
void BTree<T>::fix_excess(int i){
    if(0 == i){
        fix_excessLeftMost();
    }
    else if( i == (child_count - 1)){
        fix_excessRightMost();
    }
    else{
        fix_excessMiddle(i);
    }
}

template<typename T>
void BTree<T>::fix_excessRightMost(){
    //cc = child_count
    //move right half of data from index cc-1 into new child cc
    //delete all but left half of data from index cc-1
    //fix data-count on index cc-1 and new cc
    //move middle data entry from cc-1 into data
    //copy right half of children from cc-1
    //delete all but left half of children from cc-1
    //copy right half of children from cc-1 to cc
    //change child_counts and data_counts where necessary

    //move entry in subset[child_count - 1]->data[data_count / 2]
    int moveUp = subset[child_count - 1]->data_count / 2;
    //entry T to be moved up
    T moveEntry = subset[child_count - 1]->data[moveUp];
    //take moveEntry and insert it into data
    insertNoChildrenNow(moveEntry);

    //copy right half of data from cc-1
    T copyRight[moveUp];
    int p = 0;
    int capture = subset[child_count - 1]->data_count;
    for(int j = moveUp + 1; j < subset[child_count-1]->data_count; ++j){
        copyRight[p] = subset[child_count - 1]->data[j];
        ++p;
    }

    //copy right half of children from cc-1
    BTree<T> *copyRightChildren[subset[child_count-1]->child_count / 2];
    int l = 0;
    for(int j = subset[child_count - 1]->child_count / 2; j < subset[child_count - 1]->child_count; ++j){
       copyRightChildren[l] = NULL;
    }
    l = 0;
    for(int j = subset[child_count - 1]->child_count / 2; j < subset[child_count - 1]->child_count; ++j){
       copyRightChildren[l] = new BTree<T>;
       copy_tree(copyRightChildren[l], subset[child_count - 1]->subset[j]);
    }

    //add a child, insert data into new child, change data_count on old child
    child_count += 1;
    subset[child_count - 1] = new BTree<T>;
    for(int j = (capture/2) + 1; j < capture; ++j){
        subset[child_count - 1]->insert(subset[child_count - 2]->data[j]);
    }
    subset[child_count - 2]->data_count = capture / 2;

    //move right half of subset[child_count-2] children over to subset[child_count-1]
    //change child_count for subset[child_count-2]
    l = subset[child_count - 2]->child_count / 2;
    for(int j = 0; j < subset[child_count - 2]->child_count / 2; ++j){
        subset[child_count - 1]->subset[j] = new BTree<T>;
        copy_tree(subset[child_count - 1]->subset[j], subset[child_count - 2]->subset[l]);
        delete subset[child_count - 2]->subset[l];
        subset[child_count - 2]->subset[l] = NULL;
        subset[child_count-2]->child_count -= 1;
    }

}

template<typename T>
void BTree<T>::fix_excessMiddle(int i){

    cout << "I = " << i << endl;
    cout << "subset[i]->data[data_count / 2] = " << subset[i]->data[data_count / 2] << endl;

    //Insert appropriate value from subset to move up, into current data
    insertNoChildrenNow(subset[i]->data[data_count / 2]);



}

template<typename T>
void BTree<T>::fix_excessLeftMost(){
    //Add child, move all children to the right one spot, split data in half,
    //half to index 0. half to index 1, and the extra data extry should be moved into data set.
    //add one to data_count
    //add one to child_count
    //copy children(if any) of child to be split, it will be split into two

    //variable for what index in child[0] data should be moved up
    int moveUp = subset[0]->data_count / 2;
    //variable T to be moved up
    T moveEntry = subset[0]->data[moveUp];
    //take moveEntry and insert it into data
    insertNoChildrenNow(moveEntry);

    //copy data from before moveUp index and after moveUp index
    T copyLeft[moveUp];
    T copyRight[moveUp];
    for(int j = 0; j < moveUp; ++j){
        copyLeft[j] = subset[0]->data[j];
    }
    int p = 0;
    for(int j = moveUp + 1; j < subset[0]->data_count; ++j){
        copyRight[p] = subset[0]->data[j];
        ++p;
    }

    //copy all children right of index 0
    BTree<T> *childrenCopied[child_count - 1];
    for(int j = 0; j < child_count - 1; ++j){
        childrenCopied[j] = NULL;
    }
    for(int j = 0; j < child_count - 1; ++j){
        childrenCopied[j] = new BTree<T>;
        copy_tree(childrenCopied[j], subset[j+1]);
    }

    //copy left-half, then right-half of children(if any) of index 0
    int captureIndexZeroChildCountSplit = subset[0]->child_count / 2;
    BTree<T> *indexZeroLeftChildren[subset[0]->child_count / 2];
    BTree<T> *indexZeroRightChildren[subset[0]->child_count / 2];
    for(int j = 0; j < subset[0]->child_count / 2; ++j){
        indexZeroLeftChildren[j] = new BTree<T>;
        copy_tree(indexZeroLeftChildren[j], subset[0]->subset[j]);
    }
    for(int j = subset[0]->child_count / 2; j < subset[0]->child_count; ++j){
        indexZeroRightChildren[j] = new BTree<T>;
        copy_tree(indexZeroRightChildren[j], subset[0]->subset[j]);
    }

    //clear children
    for(int j = 0; j < child_count; ++j){
        subset[j]->clear_tree();
        subset[j] = NULL;
    }

    //create new child, add one to child_count
    child_count +=1;
    for(int j = 0; j < child_count; ++j){
        subset[j] = new BTree<T>;
    }

    //add data to subset[0] and subset[1]
    for(int j = 0; j < moveUp; ++j){
        subset[0]->insertNoChildrenNow(copyLeft[j]);
    }
    for(int j = 0; j < moveUp; ++j){
        subset[1]->insertNoChildrenNow(copyRight[j]);
    }

    //copy children for subset[0] and subset[1] (if any)
    for(int j = 0; j < captureIndexZeroChildCountSplit; ++j){
        subset[0]->subset[j] = new BTree<T>;
        copy_tree(subset[0]->subset[j], indexZeroLeftChildren[j]);
    }
    for(int j = 0; j < captureIndexZeroChildCountSplit; ++j){
        subset[1]->subset[j] = new BTree<T>;
        copy_tree(subset[1]->subset[j], indexZeroRightChildren[j]);
    }

    //copy subset[2 - max]
    int l = 0;
    for(int j = 2; j < child_count; ++j){
        copy_tree(subset[j], childrenCopied[l]);
        ++l;
    }



}

template<typename T>
void BTree<T>::copy_tree(BTree<T>* left,const BTree<T>* right){

    left->child_count = right->child_count;
    for(int i = 0; i < right->data_count; ++i){
        left->insertNoChildrenNow(right->data[i]);
    }
    //Copy children
    if(0 != right->child_count){
        for(int i = 0; i < right->child_count; ++i){
            left->subset[i] = new BTree<T>;
            copy_tree(left->subset[i],right->subset[i]);

        }
    }
}

template<typename T>
void BTree<T>::clear_tree(){
    //Clear Tree
    for(int i = 0; i < data_count; ++i){
        data[i] = T();
    }
    data_count = 0;
    for(int i = 0; i < child_count; ++i){
        subset[i]->clear_tree();
        delete  subset[i];
        subset[i] = NULL;
    }
    child_count = 0;
}

template<typename T>
void BTree<T>::insertChildrenExist(const T& entry){
    //If node has no children
    if(is_leaf() == true){
        insertNoChildrenNow(entry);
        return;
    }
    //If node has children
    //if entry is less than first position in data
    if(entry <= data[0]){
        //check if duplicates are allowed
        if(entry == data[0]){
            if(dups_ok == false){
                cout << "Duplicates not allowed" << endl;
                return;
            }
        }
        //insert entry into subset[0]
        subset[0]->insertChildrenExist(entry);
        //check if subset[0] data_count is not above MAXIMUM
        if(subset[0]->data_count == MAXIMUM + 1){
            fix_excess(0);
        }
    }
    //if entry is greater or equal too last position in data
    else if(entry >= data[data_count-1]){
        //check for duplicates are allowed
        if(entry == data[data_count-1]){
            if(dups_ok == false){
                cout << "Duplicates not allowed" << endl;
                return;
            }
        }
        //insert entry into subset[child_count - 1];        
        subset[child_count-1]->insertChildrenExist(entry);
        //check if subset[child_count-1] data_count is not above MAXIMUM
        if(subset[child_count - 1]->data_count == MAXIMUM + 1){
            fix_excess(child_count - 1);
        }
    }
    //if entry is inbetween first and last position in data
    else{
        //Find out what index of the subset we should go into
        int p = 0;
        for(int i = 0; i < data_count; ++i){
            if(entry <= data[i]){
                //check if duplicates are allowed
                if(entry == data[i]){
                    if(dups_ok == false){
                        cout << "Duplicates not allowed" << endl;
                        return;
                    }
                }
                p = i;
                if(data[i+1] > entry){
                    i = data_count - 1;
                }
            }
        }
        //insert entry into subset[p];
        subset[p]->insertChildrenExist(entry);
        //check if subset[child_count-1] data_count is not above MAXIMUM
        if(subset[p]->data_count == MAXIMUM + 1){
            fix_excess(p);
        }
    }
}

template<typename T>
void BTree<T>::fix_excessNoChildrenEver(){
    //Find out where to split the node
    int j;
    j = data_count / 2;
    //copy data from left of j
    T sub[data_count / 2];
    for(int i = 0; i < data_count / 2; ++i){
        sub[i] = data[i];
    }
    //copy data from right of j
    int p = (data_count / 2) + 1;
    T sub2[data_count / 2];
    for(int i = 0; i < data_count / 2; ++i){
        sub2[i] = data[p];
        ++p;
    }
    //Create subsets, change child_count, fix data, change data_count
    subset[0] = new BTree<T>;
    subset[1] = new BTree<T>;
    child_count = 2;
    data[0] = data[data_count / 2];
    data_count = 1;
    //Copy data into the children
    for(int i = 0; i < j; ++ i){
        subset[0]->insert(sub[i]);
        subset[1]->insert(sub2[i]);
    }
}

template<typename T>
BTree<T>::BTree(bool dups){
    //Constructor
    //Set data_count and child_count to zero, Set subset[i] = NULL, Set dups_ok to dups
    data_count = 0;
    child_count = 0;
    for(int i = 0; i < MAXIMUM + 2; ++i){
        subset[i] = NULL;
    }
    dups_ok = dups;
}
template<typename T>
BTree<T>::~BTree(){

}

template<typename T>
void BTree<T>::print_tree(int level, ostream& outs) const{
    //Print Tree from book
    const int EXTRA_INDENTATION = 4;
    size_t i;
    outs << setw(level) << "";

    for(i = 0;i < data_count; ++i){
        outs << data[i] << " " ;
    }
    outs << endl;

    for(i = 0; i < child_count; ++i){
        subset[i]->print_tree(level + EXTRA_INDENTATION);
    }
}

template<typename U>
ostream& operator <<(ostream& outs, const BTree<U>& print_me) {
    print_me.print_tree();
    return outs;
}

#endif // NEWBTREE_H
