#if 1 // one.cc
// one.cc -- Joe Linhoff for CS393, linked lists
#include <stdio.h>
#include <string.h>
#include <malloc.h>

// macros
#define BRK() __debugbreak()
#define ret(_r_) do{r=_r_;goto BAIL;}while(0)
#define bret(_r_) do{BRK();r=_r_;goto BAIL;}while(0)

class Map {
   // character map class for example game
public:
   int width;
   int height;
   int rowcells; // cells in each row
   char *cells; // all cells

   Map() :cells(NULL) {} // constructor
   ~Map() // destructor
   {
      if (this->cells)
         free(this->cells);
   } // ~Map
   int create(int w, int h)
   {
      int r, s;
      if (this->cells)
         bret(-1); // already allocated
      this->width = w;
      this->rowcells = 1 + w; // width + 1 for string term
      this->height = h;
      s = sizeof(*this->cells); // size of one cell
      s *= this->rowcells*h; // allocate w/room for EOS

      if (!(this->cells = (char*)malloc(s)))
         bret(-2); // failed alloc

      this->clear();

      r = 0;
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
      int r, y;
      char *row;
      if (!this->cells)
         bret(-1); // not created yet
      for (y = 0; y<this->height; y++)
      {
         row = this->cells + y*this->rowcells;
         printf("%s\n", row);
      } // for

      r = 0;
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

// JFL 05 Apr 15
int main(int argc,char *argv[])
{
   int r,x,y,c,len;
   Map *map=NULL; // non-NULL if valid
   FILE *fh=NULL; // non-NULL if valid
   char line[256],*s1,*s2;

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
   if((r=map->create(x,y))<0) // x,y is width,height here
      goto BAIL;

   //
   // SCAN MAP
   //

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

      // set in the map
      map->xySet(x, y, c);
   } // for

   //
   //
   //

   map->print();

   // questions:
   // - how to find, move all objects
   // - what is O notation analysis for moving all objects?
   // - how would you collide objects? (players P, gold G, bombs B)

BAIL:
   getchar(); // wait for enter-key to exit
   if(fh) // close if valid
      fclose(fh);
   if (map) // free if valid
      delete map;
   return 0;
} // main()

// EOF
#endif // one.cc
