#if 1 // two.cc
// two.cc -- Joe Linhoff for CS393, linked lists
#include <stdio.h>
#include <string.h>
#include <malloc.h>

// macros
#define BRK() __debugbreak()
#define ret(_r_) do{r=_r_;goto BAIL;}while(0)
#define bret(_r_) do{BRK();r=_r_;goto BAIL;}while(0)

class Map {
public:
   int width;
   int height;
   int rowcells; // cells in each row
   char *cells; // all cells

   Map():cells(NULL){} // constructor
   ~Map() // destructor
   {
      if(this->cells)
         free(this->cells);
   } // ~Map
   int create(int w,int h)
   {
      int r,s;
      if(this->cells)
         bret(-1); // already allocated
      this->width=w;
      this->rowcells=1+w; // width + 1 for string term
      this->height=h;
      s=sizeof(*this->cells); // size of one cell
      s*=this->rowcells*h; // allocate w/room for EOS

      if(!(this->cells=(char*)malloc(s)))
         bret(-2); // failed alloc

      this->clear();

      r=0;
   BAIL:
      return r;
   } // create()
   int clear()
   {
      int r, x, y;
      char *row;
      if (!this->cells)
         bret(-1); // not allocated

      // setup each row as a string w/terminating 0
      for (y = 0; y<this->height; y++)
      {
         row = this->cells + y*this->rowcells;
         for (x = 0; x<this->width; x++)
            row[x] = '.';
         row[x] = 0; // terminate
      } // for

      r = 0;
   BAIL:
      return r;
   } // clear()
   int print()
   {
      int r,y;
      char *row;
      if(!this->cells)
         bret(-1); // not created yet
      for(y=0;y<this->height;y++)
      {
         row=this->cells+y*this->rowcells;
         printf("%s\n",row);
      } // for

      r=0;
   BAIL:
      return r;
   } // print()
   int xySet(int x, int y, char c)
   {
      int r;
      char *row;
      if ((x < 0) || (x >= this->width))
         ret(-1);
      if ((y < 0) || (y >= this->height))
         ret(-2);
      row = this->cells + y*this->rowcells;
      row[x] = c;

      r = 0;
   BAIL:
      return r;
   } // xySet()
}; // struct Map

struct SingleLinkNode {
   SingleLinkNode *next;

   struct { // data
      int x;
      int y;
      char c;
   } data;

   SingleLinkNode():next(NULL) {} // node constructor
   ~SingleLinkNode() {} // node destructor
   void listDestroy() // destroy list
   {
      // important point: two ways nodes are used: 1) node, 2) list-head
      SingleLinkNode *n;
      while ((n = this->next))
      {
         this->next = n->next;
         delete n; // destroy node
      } // while
   } // listDestroy()
   void listAdd(SingleLinkNode *n)
   {
      n->next = this->next;
      this->next = n; 
   } // listAdd()

}; // struct SingleLinkNode

// JFL 05 Apr 15
int main(int argc,char *argv[])
{
   int r,x,y,c,len,xx,yy,moveNumber;
   Map *map = NULL; // non-NULL if valid
   FILE *fh = NULL; // non-NULL if valid
   char line[256],*s1,*s2;
   SingleLinkNode *singlehead=NULL; // list head, non-NULL if valid
   SingleLinkNode *node; // used for nodes

   if((r=fopen_s(&fh,"file1.txt","rb")))
      bret(-2);

   //
   // SCAN FOR MAP CREATION
   //

   x = y = 0; // default width & height
   for(;;)
   {
      // read a line from the file
      s1=fgets(line,sizeof(line),fh);
      if(!s1) // NULL returned when done with file
         break; // done

      // look for map WIDTH,HEIGHT
      s2="map "; // token is 'map' with space
      len=strlen(s2);
      if(strncmp(s1,s2,len))
         continue; // line doesn't start with map

      // get size
      s1+=len; // skip token
      r=sscanf_s(s1,"%d,%d",&x,&y); // get size
      if (r != 2) BRK(); // want 2 items scanned, syntax error
      break; // done with this loop

   } // for

   if (!x || !y)
      bret(-1); // not valid
                
   // create -- allocate
   if (!(map = new Map()))
      bret(-1);
   if ((r = map->create(x, y))<0) // x,y is width,height here
      goto BAIL;

   //
   // SCAN MAP
   //

   // create list head -- notice that the head is a node
   // and we will not put any data in the head
   if (!(singlehead = new SingleLinkNode()))
      bret(-1);

   for (;;)
   {
      // read a line from the file
      s1 = fgets(line, sizeof(line), fh);
      if (!s1) // NULL returned when done with file
         break; // done

      // look for xyz X,Y,C
      s2 = "xyc "; // token is 'xyc' with space
      len = strlen(s2);
      if (strncmp(s1, s2, len))
         continue; // line doesn't start with map

      // get values
      s1 += len; // skip token
      x = y = c = 0; // default to 0
      r = sscanf_s(s1, "%d,%d,%c", &x, &y,&c); // get values
      if (r != 3) BRK(); // want 3 items scanned, syntax error

      // build into list
      if (!(node = new SingleLinkNode()))
         bret(-3);
      singlehead->listAdd(node);
      node->data.x = x;
      node->data.y = y;
      node->data.c = c;
   } // for

   //
   // PLAY
   //

   // game loop
   moveNumber = 0;
   c = 1; // signal map needs to be rebuilt (dirty)
   for (;;)
   {
      if (c)
      { // dirty -- update & draw map
         c = 0; // clear dirty
         map->clear(); // clear map

         // run through all nodes, add to map
         // map is now indepent of list of nodes
         for (node = singlehead->next; node; node = node->next)
            map->xySet(node->data.x, node->data.y, node->data.c);

         // print the map & 'HUD'
         map->print();
         printf("move:%d score:\n",moveNumber);

         // every updated map counts as a move
         moveNumber++;
      } // dirty

      //
      // PLAYER INPUT
      //

      xx = yy = 0;
      r = getchar();
      switch (r)
      {
      case '?': // help
         printf("q:quite, u:up, d:down\n");
         break;
      case 'q': // quit
      case 'Q':
         ret(0); // done
      case 'u': // up
         yy--; // go up
         break;
      case 'd': // down
         yy++; // go down
         break;
	  case 'l': // left
		  xx--;
		  break;
	  case 'r': // right
		  xx++;
		  break;
      } // switch

	  if (xx || yy)
	  { // move in x or y

		  for (node = singlehead->next; node; node = node->next)
		  {
			  if (node->data.c != 'P') // look for player
				  continue; // not player, continue looping
			  node->data.x += xx; // add movement amount
			  node->data.y += yy; // add movement amount

			  // clamp to map
			  if (node->data.x < 0)
				  node->data.x = 0;
			  if (node->data.x >(map->width - 1))
				  node->data.x = map->width - 1;
			  if (node->data.y < 0)
				  node->data.y = 0;
			  if (node->data.y >(map->height - 1))
				  node->data.y = map->height - 1;
		  } // for
		  xx = yy = 0;
		  c = 1; // signal dirty
		  printf("\n");
	  }

   } // for

   // questions:
   // - how do you move players
   // - how do you collide players?
   // - how do you remove an item

BAIL:
   if (singlehead)
   {
      singlehead->listDestroy(); // destroy all nodes linked in
      delete singlehead; // delete the node
      // - why can we NOT just delete singlehead?
      // - how can we keep count of number of nodes alive?
   }
   if(fh) // close if valid
      fclose(fh);
   if (map) // free if valid
      delete map;
   return 0;
} // main()

// EOF
#endif // two.cc
