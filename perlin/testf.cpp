#include <iostream>
using namespace std;

template<typename T, typename K>
struct fibnode {
  // constructor
  fibnode(const T& item, const K& key, fibnode<T, K> *next = NULL, fibnode<T, K> *prev = NULL) {
    this->item = item;
    this->key = key;
    this->next = next;
    this->prev = prev;
  }

  // new nodes should always have no parents or children since fib heap
  fibnode *parent = NULL;    
  fibnode *child = NULL;     // any one child of the current node 
  fibnode *next;             
  fibnode *prev;             
  K key;                    // stores the key of the node
  T item;                   // stores the item of the the node
  unsigned int deg = 0;     // the degree of the node *ONLY RELEVANT FOR ROOT NODES*
  unsigned int num = 0;     // the number of nodes underneath
  bool mark = 0;            // determines if node is marked
};

int main() {
    fibnode<T, K> *min;
    min = new fibnode<T, K>(item, key);
    min->prev = min;
    min->next = min;
    
    current_node = new fibnode<T, K>(item, key, min, min->prev);
    min->prev->next = current_node;
    min->prev = current_node;

    cout << &min << " " << &current_node << endl;
    return 0;
}