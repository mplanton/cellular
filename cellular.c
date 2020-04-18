#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_CELLS 80
#define CYCLES 80
#define ALIVE '*'
#define DEAD ' '

void init(char *cells, unsigned int len)
{
  memset(cells, DEAD, len);
  cells[len+1] = '\0';
  cells[(int)(len/2)] = ALIVE;
  //DEBUG
  cells[0] = ALIVE;
  cells[len] = ALIVE;
}

void update(char *cells, unsigned int len, char rule)
{
  char *old_cells = (char*) malloc((len+1)*sizeof(char));
  if(old_cells == NULL)
  {
    printf("ERROR: No memory for old_cells!\n");
    exit(EXIT_FAILURE);
  }
  memcpy(old_cells, cells, (len+1)*sizeof(char));
  
  int i;
  for(i=0; i<=len; i++)
  {
    int j;
    int pattern = 0;
    char c, state;
    
    for(j=-1; j<=1; j++)
    {
      // wrap
      if(i+j < 0)
        c = old_cells[len];
      else if(i+j > len)
        c = old_cells[0];
      else
        c = old_cells[i+j];

      // check symbol
      if(c == ALIVE)
        state = 1;
      else
        state = 0;
      
      pattern |= state << (-j+1);
    }
    
    // apply rule
    state = (rule & (1 << pattern)) != 0;

    if(state == 1)
      cells[i] = ALIVE;
    else
      cells[i] = DEAD;
  }
  
  free(old_cells);
}

int main()
{
  char rule = 30;
  
  char cells[NUM_CELLS+1];
  
  init(cells, NUM_CELLS); // TODO: auch mit random
  
  unsigned int cycle;
  for(cycle=0; cycle < CYCLES; cycle++)
  {
    puts(cells);
    update(cells, NUM_CELLS, rule);
  }
  
  return 0;
}
