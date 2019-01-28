#include <iostream>
#include "newbtree.h"
using namespace std;

int main()
{
    cout << "Hello World!" << endl;

    BTree<int> here;

    here.insert(25);

    here.insert(20);
    here.insert(22);
    here.insert(25);
    here.insert(10);
    here.insert(30);
    here.insert(5);
    here.insert(40);
    here.insert(50);
    here.insert(60);
    here.insert(45);
    here.insert(47);
    cout << here << endl;

//    cout << "Hello World2!" << endl;

//    BTree<char> here2;

//    here2.insert('d');

//    here2.insert('m');
//    here2.insert('s');
//    here2.insert('f');
//    here2.insert('r');
//    here2.insert('b');


//    cout << here2 << endl;
    return 0;
}
