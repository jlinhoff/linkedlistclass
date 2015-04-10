
struct DoubleLinkNode {
   // doubly linked, circular list w/sentinel
   DoubleLinkNode *next;
   DoubleLinkNode *prev;
   int sentinel; // zero signals list head, other values for application use

   struct {
      int x;
      int y;
      char c;
   } data;

   DoubleLinkNode(int sentinel)
   {
      // write this -- link to self
      this->sentinel = sentinel; // set sentinel (zero signals head)
   } // node constructor
   ~DoubleLinkNode() {
      this->unlink(); // destructor unlinks
   } // node destructor
   void listDestroy() // destroy list
   {
      // important point: two ways nodes are used: 1) node, 2) list-head
      // must call with list head
      DoubleLinkNode *n;
      while ((n = this->next) && n->sentinel)
         delete n; // destroy node -- node unlinks itself
   } // listDestroy()
   void unlink()
   {
      // write this -- unlink any node
      // link to self for safety, multiple unlinks OK
   } // unlink()
   void linkBefore(DoubleLinkNode *n)
   {
     // call from list head to link before head (last in list)
     // write this
   } // linkBefore()
}; // struct DoubleLinkNode
