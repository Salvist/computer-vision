#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Reader
{
public:
  int numRows{-1}, numCols{-1}, minVal{-1}, maxVal{-1};
  int framedRows{-1}, framedCols{-1};
  int label{-1};
  int numBoundaryPts{-1};
  int **zeroFramedAry;
  int **imgAry;

  class ChainCode
  {
    struct Point
    {
      int row{-1}, col{-1};

      Point()
      {
        row = 0;
        col = 0;
      }

      void setRC(int r, int c)
      {
        row = r;
        col = c;
      }

      void setNextP(int dir)
      {
        // direction
        // [3][2][1]
        // [4][x][0]
        // [5][6][7]
        switch (dir)
        {
        case 0:
          col++;
          break;
        case 1:
          row--;
          col++;
          break;
        case 2:
          row--;
          break;
        case 3:
          row--;
          col--;
          break;
        case 4:
          col--;
          break;
        case 5:
          row++;
          col--;
          break;
        case 6:
          row++;
          break;
        case 7:
          row++;
          col++;
          break;
        }
      }
    };

  public:
    Point *startP;
    Point *currentP;
    //Point *nextP; not needed
    int *neighborCoord;
    int lastQ;
    int chainDir;
    int label;

    ChainCode()
    {
      startP = new Point();
      currentP = new Point();

      neighborCoord = new int[8];
      lastQ = 0;
      chainDir = 0;
    }

    void getChainCode(ofstream &output, int **inAry, int inAryNumRows, int inAryNumCols)
    {
      setChainCode(output, inAry, inAryNumRows, inAryNumCols);

      do
      {
        setNeighbor(inAry, currentP->row, currentP->col);
        chainDir = findNextPixel(lastQ, currentP->row, currentP->col);
        output << chainDir << " ";
        currentP->setNextP(chainDir);
        lastQ = (chainDir + 6) % 8;
      } while (currentP->row != startP->row || currentP->col != startP->col);
    }

    void setChainCode(ofstream &output, int **inAry, int inAryNumRows, int inAryNumCols)
    {
      cout << "setting chain code\n";
      for (int i = 1; i <= inAryNumRows; i++)
      {
        for (int j = 1; j <= inAryNumCols; j++)
        {
          if (inAry[i][j] > 0)
          {
            startP->setRC(i, j);
            output << i << " " << j << " " << inAry[i][j] << endl;
            currentP->setRC(i, j);
            lastQ = 4;
            return;
          }
        }
      }
    }

    int findNextPixel(int dir, int r, int c)
    {
      for (int loop = 0; loop < 8; loop++)
      {
        dir = (++dir) % 8;

        switch (dir)
        {
        case 0:
          if (neighborCoord[4] > 0)
            return 0;
          break;
        case 1:
          if (neighborCoord[2] > 0)
            return 1;
          break;
        case 2:
          if (neighborCoord[1] > 0)
            return 2;
          break;
        case 3:
          if (neighborCoord[0] > 0)
            return 3;
          break;
        case 4:
          if (neighborCoord[3] > 0)
            return 4;
          break;
        case 5:
          if (neighborCoord[5] > 0)
            return 5;
          break;
        case 6:
          if (neighborCoord[6] > 0)
            return 6;
          break;
        case 7:
          if (neighborCoord[7] > 0)
            return 7;
          break;
        default:
          break;
        }
      }
      return -1;
    }

    // [0][1][2]
    // [3][x][4]
    // [5][6][7]
    void setNeighbor(int **inAry, int r, int c)
    {
      neighborCoord[0] = inAry[r - 1][c - 1];
      neighborCoord[1] = inAry[r - 1][c];
      neighborCoord[2] = inAry[r - 1][c + 1];
      neighborCoord[3] = inAry[r][c - 1];
      neighborCoord[4] = inAry[r][c + 1];
      neighborCoord[5] = inAry[r + 1][c - 1];
      neighborCoord[6] = inAry[r + 1][c];
      neighborCoord[7] = inAry[r + 1][c + 1];
    }

    void constructBoundary(ifstream &input, ofstream &output, int **inAry)
    {
      int startRow = 0, startCol = 0;
      input >> startRow >> startCol >> label;

      currentP->setRC(startRow, startCol);

      label;
      int dir;
      if(label <= 4) label += 4;
      inAry[startRow][startCol] = label;
      while (!input.eof())
      {
        input >> dir;
        currentP->setNextP(dir);
        if (currentP->row == startRow && currentP->col == startCol)
          break;
        inAry[currentP->row][currentP->col] = label;
      }
    }

    int numRows, numCols;
    void fillBot(int **inAry, int r, int c){
      if(inAry[r][c] == label || r > numRows || c > numCols) return;
      inAry[r][c]++;
      fillBot(inAry, r+1, c);
    }
    void fillRight(int **inAry, int r, int c){
      if(inAry[r][c] == label || r > numRows || c > numCols) return;
      inAry[r][c]++;
      fillRight(inAry, r, c+1);
    }

    void fillTop(int **inAry, int r, int c){
      if(inAry[r][c] == label || r < 1 || c < 1) return;
      inAry[r][c]++;
      fillTop(inAry, r-1, c);
    }

    void fillLeft(int **inAry, int r, int c){
      if(inAry[r][c] == label || r < 1|| c < 1) return;
      inAry[r][c]++;
      fillLeft(inAry, r, c-1);
    }

    void fillTest(int** inAry, int numR, int numC){
      numRows = numR;
      numCols = numC;

      for(int i = 1; i <= numRows; i++){
        for(int j = 1; j <= numCols; j++){
          if(inAry[i][j] == label){
            fillBot(inAry, i+1, j);
            fillRight(inAry, i, j+1);
          }
        }
      }
      for(int i = numRows; i > 0; i--){
        for(int j = numCols; j > 0; j--){
          if(inAry[i][j] == label){
            fillTop(inAry, i-1, j);
            fillLeft(inAry, i, j-1);
          }
        }
      }
      
      for(int i = 1; i <= numRows; i++){
        for(int j = 1; j <= numCols; j++){
          if(inAry[i][j] >= 4) inAry[i][j] = label;
          else inAry[i][j] = 0;
        }
      }
    }

    void fill_intetior(int **inAry, int numRows, int numCols)
    {
      bool increment = false;
      for(int i = 1; i <= numRows; i++){
        for(int j = 1; j <= numCols; j++){
          if(inAry[i][j] == label && inAry[i][j+1] < 4){
            inAry[i][j] = 4;
          }
        }
        increment = false;
        for(int j = 1; j <= numCols; j++){
          if(increment && inAry[i][j] < 4) inAry[i][j]++;
          if(inAry[i][j] == 4) increment = !increment;
        }
      }
      increment = false;
      for(int j = 1; j <= numCols; j++){
        for(int i = 1; i <= numRows; i++){
          if((inAry[i][j] == 4 ||inAry[i][j] == label) && inAry[i+1][j] < 4){
            inAry[i][j] = 5;
          }
        }
        increment = false;
        for(int i = 1; i <= numRows; i++){
          if(increment && inAry[i][j] < 5) inAry[i][j] += 2;
          if(inAry[i][j] == 5) increment = !increment;
        }
      }
      
      for(int i = 1; i <= numRows; i++){
        for(int j = 1; j <= numCols; j++){
          if(inAry[i][j] >= 2) inAry[i][j] = label;
          else inAry[i][j] = 0;
        }
      }
      
    }
  };

  ChainCode *cc;

public:
  Reader(ifstream &inputImg)
  {
    load_img_header(inputImg);
    cout << "load header done" << endl;
    this->framedRows = this->numRows + 2;
    this->framedCols = this->numCols + 2;

    this->zeroFramedAry = new int *[this->framedRows];
    this->imgAry = new int *[this->framedRows];
    for (int i = 0; i < this->framedRows; i++)
    {
      this->zeroFramedAry[i] = new int[this->framedCols];
      this->imgAry[i] = new int[this->framedCols];
      for (int j = 0; j < this->framedCols; j++)
      {
        this->zeroFramedAry[i][j] = 0;
        this->imgAry[i][j] = 0;
      }
    }

    //Chain Code initialization
    cc = new ChainCode();

    cout << "zero initialization done" << endl;
  }
  void load_img_header(ifstream &input)
  {
    input >> this->numRows >> this->numCols >> this->minVal >> this->maxVal;
  }
  void load_img(ifstream &input)
  {
    for (int i = 1; i <= this->numRows; i++)
    {
      for (int j = 1; j <= this->numCols; j++)
      {
        input >> this->zeroFramedAry[i][j];
      }
    }
  }

  void set_zero_2d_ary(int **ary)
  {
    for (int i = 0; i < this->framedRows; i++)
    {
      for (int j = 0; j < this->framedCols; j++)
      {
        ary[i][j] = 0;
      }
    }
  }

  void getChainCode(ofstream &output)
  {
    output << this->numRows << " " << this->numCols << " " << this->minVal << " " << this->maxVal << endl;

    cout << "starting chain code...\n";
    cc->getChainCode(output, this->zeroFramedAry, this->numRows, this->numCols);
    cout << "chain code done\n";
  }

  void constructBoundary(ifstream &input, ofstream &output)
  {
    load_img_header(input);
    cc->constructBoundary(input, output, imgAry);
  }

  void fill_interior()
  {
    cc->fillTest(imgAry, numRows, numCols);
  }

  void ary_to_file(ofstream &output, int **inAry)
  {
    output << this->numRows << " " << this->numCols << " " << this->minVal << " " << this->maxVal << endl;
    for (int i = 1; i <= this->numRows; i++)
    {
      for (int j = 1; j <= this->numCols; j++)
      {
        output << inAry[i][j];
        if ((j + 1) % (this->framedCols - 1) != 0)
          output << " ";
      }
      if ((i + 1) % (this->framedRows - 1) != 0)
        output << endl;
    }
  }

  void pretty_print(ofstream &output, int **inAry)
  {
    output << this->numRows << " " << this->numCols << " " << this->minVal << " "
           << this->maxVal << endl;
    for (int i = 1; i <= this->numRows; i++)
    {
      for (int j = 1; j <= this->numCols; j++)
      {
        if (inAry[i][j] > 0)
        {
          output << inAry[i][j];
        }
        else
        {
          output << ".";
        }
        if ((j + 1) % (this->framedCols - 1) != 0)
          output << " ";
      }
      if ((i + 1) % (this->framedRows - 1) != 0)
        output << endl;
    }
  }
  void free_Heap()
  {
    for (int i{0}; i < this->framedRows; i++)
    {
      delete[] this->zeroFramedAry[i];
    }
    delete[] this->zeroFramedAry;
  }
};
int main(int argc, const char *argv[])
{
  string inputImgName = argv[1];
  ifstream inputInFile;
  inputInFile.open(inputImgName);

  string outImgName = inputImgName.substr(0, inputImgName.size() - 4);
  string outCCName = outImgName + "_chainCode.txt";
  ofstream outCC;
  outCC.open(outCCName);

  string outBoundaryName = outImgName + "_Boundary.txt";
  ofstream outBoundary;
  outBoundary.open(outBoundaryName);

  string outDecompressName = outImgName + "_chainCodeDecompressed.txt";
  ofstream outDecom;
  outDecom.open(outDecompressName);

  if (inputInFile.is_open())
  {
    Reader *read_obj = new Reader(inputInFile);
    read_obj->load_img(inputInFile);
    read_obj->getChainCode(outCC);
    outCC.close();

    string inputCCName = outCCName;
    ifstream inCCFile;
    inCCFile.open(inputCCName);
    read_obj->constructBoundary(inCCFile, outBoundary);

    read_obj->ary_to_file(outBoundary, read_obj->imgAry);
    outBoundary.close();

    read_obj->fill_interior();
    read_obj->ary_to_file(outDecom, read_obj->imgAry);
    outDecom.close();

    inputInFile.close();

    read_obj->free_Heap();
  }
}
