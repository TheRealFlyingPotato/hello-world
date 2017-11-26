#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;
const int LOWER_INCREMENT = 3,
          UPPER_INCREMENT = 3,
          LOWER_START = 1,
          UPPER_START = 20 ,
          LOOP_COUNT = 5;

const string COMMAND = "simulator ";

int main (int argc, char *argv[])
{
  int lowerInc = LOWER_INCREMENT, 
      upperInc = UPPER_INCREMENT,
      loops = LOOP_COUNT,
      lowerBound = LOWER_START,
      upperBound = UPPER_START;
  char commandstr[60];
  //char commandstr[21], upperstr[21]; //enough to hold all numbers up to 64 bytes
  ofstream fout;
  fout.open("multiOutput");
  ifstream fin;
  string name, abbrv, value; // for reading from the output file

  switch (argc)
  {
    case 4:
      loops = atoi(argv[3]);
    case 3:
      lowerInc = atoi(argv[1]);
      upperInc = atoi(argv[2]);
    default:
      break; 
  }
  for (int i = 0; i < loops; i++)
  {
      cout << "Working on bounds [" << lowerBound << "," << upperBound << "]" << endl;
      //sprintf(lowerstr, "%d", lowerBound);
      //sprintf(upperstr, "%d", upperBound);
      //command = COMMAND;
      //command += lowerstr;
      //command += 
      //command += upperstr;
      sprintf(commandstr, "simulator %d %d", lowerBound, upperBound); 
      system(commandstr);

      // output information
      fout << "!" << lowerBound << " " << upperBound << endl;
      fin.open("output");
      while (getline(fin, name))
      {
        getline(fin, abbrv);
        getline(fin, value);
        fout << abbrv << endl << value << endl;
      }
      
      
      lowerBound += lowerInc;
      upperBound += upperInc; 
      fin.close();
  } 
}
