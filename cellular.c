// cellular.c
// a 1D cellular automaton
//
// Manuel Planton
//
//    *          *                                        *                   
//   ***        ***                                      ***                  
//  **  *      **  *                                    **  *                 
// ** ****    ** ****                                  ** ****                
// *  *   *  **  *   *                                **  *   *               
// ***** ***** **** ***                              ** **** ***              
// *     *     *    *  *                            **  *    *  *             
// **   ***   ***  ******                          ** ****  ******            
// * * **  * **  ***     *                        **  *   ***     *           
// * * * *** * ***  *   ***                      ** **** **  *   ***          
// * * * *   * *  **** **  *                    **  *    * **** **  *         
// * * * ** ** ****    * ****                  ** ****  ** *    * ****        
// * * * *  *  *   *  ** *   *                **  *   ***  **  ** *   *       
// * * * ******** *****  ** ***              ** **** **  *** ***  ** ***      
// * * * *        *    ***  *  *            **  *    * ***   *  ***  *  *     
// * * * **      ***  **  *******          ** ****  ** *  * *****  *******    
// * * * * *    **  *** ***      *        **  *   ***  **** *    ***      *   
// * * * * **  ** ***   *  *    ***      ** **** **  ***    **  **  *    ***  
// * * * * * ***  *  * ******  **  *    **  *    * ***  *  ** *** ****  **  * 
// * * * * * *  ****** *     *** ****  ** ****  ** *  ******  *   *   *** ****


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

typedef struct params
{
  char ALIVE;
  char DEAD;
  char RULE;
  unsigned int NUM_CELLS;
  unsigned int CYCLES;
  unsigned int arg_pos_start;
  unsigned int arg_pos_end;
} Params;


void setRand(char *cells, int prob, Params *params)
{
  time_t t;
  srand((unsigned) time(&t));
  
  int i;
  for(i=0; i<params->NUM_CELLS; i++)
  {
    if(rand() < ((float)prob/100)*RAND_MAX)
    {
      cells[i] = params->ALIVE;
    }
  }
}

void init(char *cells, Params *params, char **argv)
{
  unsigned int len = params->NUM_CELLS;
  memset(cells, params->DEAD, len+1);
  cells[len+1] = '\0';
  
  if(params->arg_pos_start == 0 || params->arg_pos_end == 0)
  {
    // no position given
    // default
    cells[(int)(len/2)] = params->ALIVE;
    return;
  }

  char *arg;
  int i;
  for(i=params->arg_pos_start; i<=params->arg_pos_end; i++)
  {
    arg = argv[i];
    if(isdigit(arg[0]))
    {
      cells[atoi(arg)] = params->ALIVE;
    }
    else if(isalpha(arg[0]))
    {
      switch(arg[0])
      {
        case 'm':
          cells[(int)(len/2)] = params->ALIVE;
          break;
        case 'l':
          cells[len] = params->ALIVE;
          break;
        case 'r':
          setRand(cells, atoi(argv[i+1]), params);
          i += 2;
          break;
      }
    }
  }
  
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

void printHelp(Params *params)
{
  printf("usage: cellular [-h] [-r <rule>] [-c <number of cells>]\n" \
         "                [-t <number of life cycles>] [-l <alive char>]\n" \
         "                [-d <dead char>] [-s <start configuration>]\n\n" \
         
         "-h\tshow this message\n" \
         "-r\tthe rule 0-255 of the cellular automaton (default: %d)\n" \
         "-c\tthe number of cells in the row (default: %u)\n" \
         "-t\tthe number of computed rows or time (default: %u)\n" \
         "-l\tthe character which represents the living cells\n" \
         "-d\tthe character which represents the dead cells\n" \
         "-s\tthe starting configuration of the first line of cells (default: m)\n" \
         "\tTo choose individual living cells, pass a list of white space\n" \
         "\tseparated integers.\n" \
         "\t  cellular -s 3 12 42 ...\n" \
         "\tYou can choose the middle cell with 'm' and the last cell with 'l'.\n" \
         "\t  cellular -s 0 m l\n" \
         "\tTo choose a random distribution, use 'r' and the probability in percent.\n" \
         "\t  cellular -s r 50\n"
         , params->RULE, params->NUM_CELLS, params->CYCLES);
}


void getArgs(int argc, char **argv, Params *params)
{
  if(argc <= 1)
    return;
  
  int n = 0;
  int i = 1;
  while(i < argc)
  {
    if(argv[i][0] == '-')
    {
      // check if argument has one or more parameters
      if(((i+1 >= argc) || (argv[i+1][0] == '-')) && argv[i][1] != 'h')
      {
        printf("ERROR: %d. argument '%s' needs a parameter.\n\n", i, argv[i]);
        printHelp(params);
        exit(EXIT_FAILURE);
      }
        
      
      // TODO: check validity of parameters
      // TODO: add choosing if wrap on sides
      switch(argv[i][1])
      {
        case 'h':
          printHelp(params);
          exit(EXIT_SUCCESS);
        case 'r':
          if(argv[i+1] < 0 || atoi(argv[i+1]) > 255)
          {
            printf("ERROR: %d. argument. The rule must be between 0 - 255.\n\n", i);
            printHelp(params);
            exit(EXIT_FAILURE);
          }
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
        case 's':
          n = i+1;
          while(n < argc && argv[n][0] != '-')
            n++;
          params->arg_pos_start = i+1;
          params->arg_pos_end = n-1;
          i += n-1;
          break;
        default:
          printHelp(params);
          exit(EXIT_FAILURE);
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
  params.arg_pos_start = 0;
  params.arg_pos_end = 0;
  
  getArgs(argc, argv, &params);
  
  char cells[params.NUM_CELLS+1];
  
  init(cells, &params, argv);
  
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
