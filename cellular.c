// cellular.c
// a 1D cellular automaton
//
// Manuel Planton
//
//*                                       *                                     
// *                                     ***                                    
// **                                   **  *                                   
// * *                                 ** ****                                 *
// * **                               **  *   *                               **
// * * *                             ** **** ***                             ** 
// * * **                           **  *    *  *                           **  
// * * * *                         ** ****  ******                         ** **
// * * * **                       **  *   ***     *                       **  * 
// * * * * *                     ** **** **  *   ***                     ** ****
// * * * * **                   **  *    * **** **  *                   **  *   
// * * * * * *                 ** ****  ** *    * ****                 ** ****  
// * * * * * **               **  *   ***  **  ** *   *               **  *   **
// * * * * * * *             ** **** **  *** ***  ** ***             ** **** ** 
// * * * * * * **           **  *    * ***   *  ***  *  *           **  *    * *
// * * * * * * * *         ** ****  ** *  * *****  *******         ** ****  ** *
// * * * * * * * **       **  *   ***  **** *    ***      *       **  *   ***  *
// * * * * * * * * *     ** **** **  ***    **  **  *    ***     ** **** **  ***
// * * * * * * * * **   **  *    * ***  *  ** *** ****  **  *   **  *    * ***  
// * * * * * * * * * * ** ****  ** *  ******  *   *   *** **** ** ****  ** *  * 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct params
{
  char ALIVE;
  char DEAD;
  char RULE;
  unsigned int NUM_CELLS;
  unsigned int CYCLES;
} Params;

void init(char *cells, Params *params)
{
  unsigned int len = params->NUM_CELLS;
  memset(cells, params->DEAD, len);
  cells[len+1] = '\0';
  cells[(int)(len/2)] = params->ALIVE;
  //DEBUG
  cells[0] = params->ALIVE;
  cells[len] = params->ALIVE;
}

void update(char *cells, char *old_cells, Params *params)
{
  unsigned int len = params->NUM_CELLS;
  memcpy(old_cells, cells, (len)*sizeof(char));
  
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
      if(c == params->ALIVE)
        state = 1;
      else
        state = 0;
      
      pattern |= state << (-j+1);
    }
    
    // apply rule
    state = (params->RULE & (1 << pattern)) != 0;

    if(state == 1)
      cells[i] = params->ALIVE;
    else
      cells[i] = params->DEAD;
  }
}

void printHelp()
{
  printf("usage: cellular [-h] [-r <rule>] [-c <number of cells>]\n" \
         "                [-t <number of life cycles>] [-l <alive char>]\n" \
         "                [-d <dead char>] [-s <start configuration>]\n\n" \
         
         "-h\tshow this message\n" \
         "-r\tthe rule of the cellular automaton\n" \
         "-c\tthe number of cells in the row\n" \
         "-t\tthe number of computed rows (time)\n" \
         "-l\tthe character which represents the living cells\n" \
         "-d\tthe character which represents the dead cells\n" \
/*         "-s\tthe starting configuration of the first line of cells\n" \*/
/*         "\tTo choose individual living cells, pass a list of white space\n" \*/
/*         "\tseparated integers.\n" \*/
/*         "\t  cellular -s 3 12 42 ...\n" \*/
/*         "\tYou can choose the middle cell with 'm' and the last cell with 'l'.\n" \*/
/*         "\t  cellular -s 0 m l\n" \*/
/*         "\tTo choose a random distribution, use 'r' and the probability in percent.\n" \*/
/*         "\t  cellular -s r 50\n"*/
         );
}

void getArgs(int argc, char **argv, Params *params)
{
  if(argc <= 1)
    return;
  
  int i = 1;
  while(i < argc)
  {
    if(argv[i][0] == '-')
    {
      switch(argv[i][1])
      {
        case 'r':
          params->RULE = atoi(argv[i+1]);
          i += 2;
          break;
        case 'c':
          params->NUM_CELLS = atoi(argv[i+1]);
          i += 2;
          break;
        case 't':
          params->CYCLES = atoi(argv[i+1]);
          i += 2;
          break;
        case 'l':
          params->ALIVE = argv[i+1][0];
          i += 2;
          break;
        case 'd':
          params->DEAD = argv[i+1][0];
          i += 2;
          break;
        case 'h':
        default:
          printHelp();
          exit(EXIT_SUCCESS);
      }
    }
  }
}

char *cpCells(char *src, unsigned int len)
{
  char *dest = (char*) malloc((len)*sizeof(char));
  if(dest == NULL)
  {
    printf("ERROR: No memory for cpCells!\n");
    exit(EXIT_FAILURE);
  }
  memcpy(dest, src, (len)*sizeof(char));
  return dest;
}

int main(int argc, char **argv)
{
  Params params;
  // default values
  params.ALIVE = '*';
  params.DEAD = ' ';
  params.RULE = 30;
  params.NUM_CELLS = 80;
  params.CYCLES = 20;
  
  getArgs(argc, argv, &params);
  
  char cells[params.NUM_CELLS+1];
  
  init(cells, &params); // TODO: auch mit random
  
  char *old_cells = cpCells(cells, params.NUM_CELLS+1);
  
  unsigned int cycle;
  for(cycle=0; cycle < params.CYCLES; cycle++)
  {
    puts(cells);
    update(cells, old_cells, &params);
  }
  
  if(old_cells != 0)
    free(old_cells);
  
  return 0;
}
