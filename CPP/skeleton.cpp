#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Reader {
public:
  int numRows{-1}, numCols {-1}, minVal{-1}, maxVal{-1};
  int newMin{-1}, newMax{-1};
  int framedRows{-1}, framedCols{-1};

  int** zeroFramedAry;
  int** skeletonAry;
  int* neighborAry;

  public:
  Reader(ifstream &inputImg){
    load_img_header(inputImg);
    cout << "load header done" << endl;

    this->framedRows = this->numRows + 2;
    this->framedCols = this->numCols + 2;
    this->newMax = this->maxVal;
    this->newMin = this->minVal;

    this->zeroFramedAry = new int*[this->framedRows];
    this->skeletonAry = new int*[this->framedRows];

    for(int i = 0; i < this->framedRows; i++){
      this->zeroFramedAry[i] = new int[this->framedCols];
      this->skeletonAry[i] = new int[this->framedCols];
      for(int j = 0; j < this->framedCols; j++){
        this->zeroFramedAry[i][j] = 0;
        this->skeletonAry[i][j] = 0;
      }
    }

    this->neighborAry = new int[8];
    for(int i = 0; i < 9; i++){
      this->neighborAry[9] = -1;
    }

    cout << "zero initialization done" << endl;
  }

  
	void load_img_header(ifstream &input) {
		input >> this->numRows >> this->numCols >> this->minVal >> this->maxVal;
 	}

	void load_img(ifstream &input) {
		for (int i = 1; i <= this->numRows; i++) {
			for (int j = 1; j <= this->numCols; j++) {
				input >> this->zeroFramedAry[i][j];
			}
		}
	}

  void load(string decomName, int** inAry){
    ifstream inDecom;
    inDecom.open(decomName);

    inDecom >> this->numRows >> this->numCols >> this->minVal >> this->maxVal;

    int r, c, val;
    while(!inDecom.eof()){
      inDecom >> r;
      inDecom >> c;
      inDecom >> val;

      inAry[r][c] = val;
    }
  }


  void set_zero_2d_ary(int** ary){
    for(int i = 0; i < this->framedRows; i++){
      for(int j = 0; j < this->framedCols; j++){
        ary[i][j] = 0;
      }
    }
  }

  void compute_8distance(ofstream &outFP, ofstream &outSP){
    first_pass_8distance();
    pretty_print(outFP, zeroFramedAry);
    second_pass_8distance();
    pretty_print(outSP, zeroFramedAry);
  }

  void first_pass_8distance(){
    cout << "processing first pass 8 distance\n";
    for(int i = 1; i <= numRows; i++){
      for(int j = 1; j <= numCols; j++){
        if(this->zeroFramedAry[i][j] > 0){
          zeroFramedAry[i][j] = min1(i, j) + 1;
          if(newMax < zeroFramedAry[i][j]) newMax = zeroFramedAry[i][j];
        }
      }
    }
    cout << "first pass 8 distance done\n";
  }

  void second_pass_8distance(){
    cout << "processing second pass 8 distance\n";
    newMax = newMin;
    for(int i = numRows; i > 0; i--){
      for(int j = numCols; j > 0; j--){
        if(this->zeroFramedAry[i][j] > 0){
          zeroFramedAry[i][j] = min3(this->zeroFramedAry[i][j], min2(i, j)+1);
          if(newMax < zeroFramedAry[i][j]) newMax = zeroFramedAry[i][j];
        }
      }
    }
    cout << "second pass 8 distance done\n";
  }

  int min1(int r, int c){
    this->neighborAry[0] = zeroFramedAry[r-1][c-1];
    this->neighborAry[1] = zeroFramedAry[r-1][c];
    this->neighborAry[2] = zeroFramedAry[r-1][c+1];
    this->neighborAry[3] = zeroFramedAry[r][c-1];

    int min = this->neighborAry[0];
    for(int i = 0; i < 4; i++){
      if(this->neighborAry[i] < min) min = this->neighborAry[i];
    }

    return min;
  }

  int min2(int r, int c){
    this->neighborAry[0] = zeroFramedAry[r][c+1];
    this->neighborAry[1] = zeroFramedAry[r+1][c-1];
    this->neighborAry[2] = zeroFramedAry[r+1][c];
    this->neighborAry[3] = zeroFramedAry[r+1][c+1];

    int min = this->neighborAry[0];
    for(int i = 0; i < 4; i++){
      if(this->neighborAry[i] < min) min = this->neighborAry[i];
    }

    return min;
  }

  int min3(int x, int min){
    if(x < min) return x;
    return min;
  }

  void compute_local_maxima(){
    cout << "computing local maxima" << endl;
    for(int i = 1; i <= numRows; i++){
      for(int j = 1; j <= numCols; j++){
        if(this->zeroFramedAry[i][j] > 0){
          skeletonAry[i][j] = is_local_maxima(i, j);
        }
      }
    }
    cout << "local maxima computed" << endl;
  }

  int is_local_maxima(int r, int c){
    this->neighborAry[0] = zeroFramedAry[r-1][c-1];
    this->neighborAry[1] = zeroFramedAry[r-1][c];
    this->neighborAry[2] = zeroFramedAry[r-1][c+1];
    this->neighborAry[3] = zeroFramedAry[r][c-1];
    this->neighborAry[4] = zeroFramedAry[r][c];
    this->neighborAry[5] = zeroFramedAry[r][c+1];
    this->neighborAry[6] = zeroFramedAry[r+1][c-1];
    this->neighborAry[7] = zeroFramedAry[r+1][c];
    this->neighborAry[8] = zeroFramedAry[r+1][c+1];

    for(int i = 0; i < 9; i++){
      if(i == 4) continue;
      if(this->neighborAry[4] < this->neighborAry[i]) return 0;
    }
    return this->neighborAry[4];
  }

  void skel_extraction(ofstream &outSkel, ofstream &outMaxima){
    compute_local_maxima();
    pretty_print(outSkel, skeletonAry);
    extract_local_maxima(outMaxima, skeletonAry);
  }

  void skel_expansion(string decomName, ofstream &outExpand1, ofstream &outExpand2){
    set_zero_2d_ary(this->zeroFramedAry);
    load(decomName, zeroFramedAry);

    first_pass_expansion();
    pretty_print(outExpand1, zeroFramedAry);
    second_pass_expansion();
    pretty_print(outExpand2, zeroFramedAry);
  }

  void first_pass_expansion(){
    cout << "processing first pass expansion\n";
    for(int i = 1; i <= numRows; i++){
      for(int j = 1; j <= numCols; j++){
        if(this->zeroFramedAry[i][j] == 0){
          int max = max1(i, j);
          if(zeroFramedAry[i][j] < max) zeroFramedAry[i][j] = max -1;
        }
      }
    }
    cout << "first pass expansion done\n";
  }

  void second_pass_expansion(){
    cout << "processing second pass expansion\n";
    for(int i = numRows; i > 0; i--){
      for(int j = numCols; j > 0; j--){
        int max = max1(i, j);
        if(zeroFramedAry[i][j] < max){
          zeroFramedAry[i][j] = max-1;
        }
      }
    }
    cout << "second pass expansion done\n";
  }

  int max1(int r, int c){
    int max = 0;

    this->neighborAry[0] = zeroFramedAry[r-1][c-1];
    this->neighborAry[1] = zeroFramedAry[r-1][c];
    this->neighborAry[2] = zeroFramedAry[r-1][c+1];
    this->neighborAry[3] = zeroFramedAry[r][c-1];
    this->neighborAry[4] = zeroFramedAry[r][c];
    this->neighborAry[5] = zeroFramedAry[r][c+1];
    this->neighborAry[6] = zeroFramedAry[r+1][c-1];
    this->neighborAry[7] = zeroFramedAry[r+1][c];
    this->neighborAry[8] = zeroFramedAry[r+1][c+1];

    for(int i = 0; i < 9; i++){
      if(i == 4) continue;
      if(max < this->neighborAry[i]) max = this->neighborAry[i];
    }
    return max;
  }

  void extract_local_maxima(ofstream &output, int** inAry){
    cout << "extracting local maxima" << endl;
    output << this->numRows << " " << this->numCols << " " << this->newMin << " " << this->newMax << endl;

    for(int i = 1; i <= numRows; i++){
      for(int j = 1; j <= numCols; j++){
        if(inAry[i][j] > 0){
          output << i << " " << j << " " << inAry[i][j] << endl;
        }
      }
    }
    cout << "local maxima extracted" << endl;
  }

  void ary_to_file(ofstream &output, int** inAry){
  this->newMax = 1;
  output << this->numRows << " " << this->numCols << " " << this->newMin << " " << this->newMax << endl;

    for (int i = 1; i <= this->numRows; i++) {
			for (int j = 1; j <= this->numCols; j++) {
				if (inAry[i][j] > 0){
					output << 1;
				} else {
				 	output << 0;
				}
        if ((j + 1) % (this->framedCols-1) != 0) output << " ";
			}
      if ((i + 1) % (this->framedRows-1) != 0) output << endl;
		}
  }

  void pretty_print(ofstream &output, int** inAry) {
		output << this->numRows << " " << this->numCols << " " << this->newMin << " " << this->newMax << endl;

		for (int i = 1; i <= this->numRows; i++) {
			for (int j = 1; j <= this->numCols; j++) {
				if (inAry[i][j] > 0){
					output << inAry[i][j];
				} else {
				 	output << ".";
				}
        if ((j + 1) % (this->framedCols-1) != 0) output << " ";
			}
      if ((i + 1) % (this->framedRows-1) != 0) output << endl;
		}
	}

  void free_Heap() {
		for (int i{ 0 }; i < this->framedRows; i++) {
			delete[] this->zeroFramedAry[i];
      delete[] this->skeletonAry[i];

		}
		delete[] this->zeroFramedAry;
    delete[] this->skeletonAry;
	}
};

int main(int argc, const char *argv[]){
  string inputImgName = argv[1];
  ifstream inputInFile;
  inputInFile.open(inputImgName);

  string outputImgName = argv[2];
  ofstream outFile;
  outFile.open(outputImgName);

  string outDecomName = argv[3];
  outDecomName = outDecomName.substr(0, outDecomName.size()-4);

  string inputName = inputImgName.substr(0, inputImgName.size()-4);
  string outputName = outputImgName.substr(0, outputImgName.size()-4);

  string outFPName = outputName + "_pass1.txt";
  ofstream outFP;
  outFP.open(outFPName);
  
  string outSPName = outputName + "_pass2.txt";
  ofstream outSP;
  outSP.open(outSPName);

  string skelName = inputName + "_skeleton.txt";
  ofstream outSkel;
  outSkel.open(skelName);

  string decomName = inputName + "_decompressed.txt";
  ofstream outDecom;
  outDecom.open(decomName);

  string outExpandName1 = outDecomName + "_expansion1.txt";
  ofstream outExpand1;
  outExpand1.open(outExpandName1);

  string outExpandName2 = outDecomName + "_expansion2.txt";
  ofstream outExpand2;
  outExpand2.open(outExpandName2);

  if(inputInFile.is_open()){
    Reader* read_obj = new Reader(inputInFile);
    read_obj->load_img(inputInFile);

    read_obj->compute_8distance(outFP, outSP);
    read_obj->skel_extraction(outSkel, outDecom);

    outFP.close();
    outSP.close();
    outSkel.close();
    outDecom.close();

    read_obj->skel_expansion(decomName, outExpand1, outExpand2);
    read_obj->ary_to_file(outFile, read_obj->zeroFramedAry);

    inputInFile.close();
    outFile.close();
    outExpand1.close();
    outExpand2.close();

    read_obj->free_Heap();
  }
}