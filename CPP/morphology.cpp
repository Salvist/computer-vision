#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Reader {
public:
  int numImgRows{-1}, numImgCols {-1}, imgMin{-1}, imgMax{-1};
  int numStructRows{-1}, numStructCols{-1}, structMin{-1}, structMax{-1};
  int rowOrigin{-1}, colOrigin{-1};
  int rowFrameSize{-1}, colFrameSize{-1};
  int extraRows{-1}, extraCols{-1};

  int** zeroFramedAry;
  int** morphAry;
  int** tempAry;
  int** structAry;

  public:
  Reader(ifstream &inputImg, ifstream &inputStruct){
    load_img_header(inputImg);
    cout << "load header done" << endl;
    load_struct_header(inputStruct);
    cout << "load elem done" << endl;
    
    //Setting frame size and extra size
    this->rowFrameSize = this->numStructRows / 2;
    this->colFrameSize = this->numStructCols / 2;
    this->extraRows = this->rowFrameSize * 2;
    this->extraCols = this->colFrameSize * 2;
    cout << "setting frame and extra done" << endl;

    this->zeroFramedAry = new int*[this->numImgRows + this->extraRows];
    this->morphAry = new int*[this->numImgRows + this->extraRows];
    this->tempAry = new int*[this->numImgRows + this->extraRows];
    cout << "setting rows done" << endl;

    for(int i = 0; i < this->numImgRows + extraRows; i++){
      zeroFramedAry[i] = new int[this->numImgCols + this->extraCols];
      morphAry[i] = new int[this->numImgCols + this->extraCols];
      tempAry[i] = new int[this->numImgCols + this->extraCols];
      for(int j = 0; j < this->numImgCols + extraCols; j++){
        zeroFramedAry[i][j] = 0;
        morphAry[i][j] = 0;
        tempAry[i][j] = 0;
      }
    }
    cout << "setting cols done" << endl;

    this->structAry = new int*[this->numStructRows];
    for(int i = 0; i < this->numStructRows; i++){
      structAry[i] = new int[this->numStructCols];
      for(int j = 0; j < this->numStructCols; j++){
        structAry[i][j] = 0;
      }
    }
    cout << "zero initialization done" << endl;
  }

  
	void load_img_header(ifstream &input) {
		input >> this->numImgRows >> this->numImgCols >> this->imgMin >> this->imgMax;
 	}

  void load_struct_header(ifstream &input){
    input >> this->numStructRows >> this->numStructCols >> this->structMin >> this->structMax;
    input >> this->rowOrigin >> this->colOrigin;
    cout << this->numStructRows <<  " " << this->numStructCols << " " << this->structMin << " " <<this->structMax << endl;
    cout << this->rowOrigin << " " << this->colOrigin << endl;
  }
	void load_img(ifstream &input) {
		for (int i = rowFrameSize; i < this->numImgRows + this->rowFrameSize; i++) {
			for (int j = colFrameSize; j < this->numImgCols + this->colFrameSize; j++) {
				input >> this->zeroFramedAry[i][j];
			}
		}
	}

  void load_struct(ifstream &input){
    for (int i{ 0 }; i < this->numStructRows; i++) {
			for (int j{ 0 }; j < this->numStructCols; j++) {
				input >> this->structAry[i][j];
        cout << this->structAry[i][j] << " ";
			}
      cout << endl;
		}
  }

  void zero_2d_ary(int** ary){
    for(int i = 0; i < this->numImgRows + extraRows; i++){
      for(int j = 0; j < this->numImgCols + extraCols; j++){
        ary[i][j] = 0;
      }
    }
  }

  void dilation(int** inAry, int** outAry, int r, int c){
    int rowOffset = r - this->rowOrigin;
    int colOffset = c - this->colOrigin;

    for(int rowIndex = 0; rowIndex < numStructRows; rowIndex++){
      for(int colIndex = 0; colIndex < numStructCols; colIndex++){
        //if(this->zeroFramedAry[rowOffset + rowIndex][colOffset + colIndex] == 1){
        //  this->morphAry[rowOffset + rowIndex][colOffset + colIndex] = 1;
        //}
        if(outAry[rowOffset + rowIndex][colOffset + colIndex] == 1) continue;
        outAry[rowOffset + rowIndex][colOffset + colIndex] = structAry[rowIndex][colIndex];
      }
    }
  }

  void compute_dilation(int** inAry, int** outAry){
    for(int i = rowFrameSize; i < this->numImgRows + this->extraRows; i++){
      for(int j = colFrameSize; j < this->numImgCols + this->extraCols; j++){
        if(inAry[i][j] > 0){
          dilation(inAry, outAry, i, j);
        }
      }
    }
  }

  void erosion(int** inAry, int** outAry, int r, int c){
    int rowOffset = r - this->rowOrigin;
    int colOffset = c - this->colOrigin;
    bool matchFlag = true;

    int rowIndex = 0;

    while(rowIndex < numStructRows && matchFlag){
      int colIndex = 0;
      while(colIndex < numStructCols && matchFlag){
        if(structAry[rowIndex][colIndex] > 0 && inAry[rowOffset + rowIndex][colOffset + colIndex] <= 0){
          matchFlag = false;
        }
        colIndex++;
      }
      rowIndex++;
    }
    if(matchFlag == true){
      outAry[rowOffset + this->rowOrigin][colOffset + this->colOrigin] = 1;
    } else {
      outAry[rowOffset + this->rowOrigin][colOffset + this->colOrigin] = 0;
    }
  }

  void compute_erosion(int** inAry, int** outAry){
    for(int i = rowFrameSize; i < this->numImgRows + this->extraRows; i++){
      for(int j = colFrameSize; j < this->numImgCols + this->extraCols; j++){
        if(inAry[i][j] > 0){
          erosion(inAry, outAry, i, j);
        }
      }
    }
  }

  void compute_closing(int** inAry, int** outAry, int** tempAry){
    compute_dilation(inAry, tempAry);
    compute_erosion(tempAry, outAry);
  }

  void compute_opening(int** inAry, int** outAry, int** tempAry){
    compute_erosion(inAry, tempAry);
    compute_dilation(tempAry, outAry);
  }

  void ary_to_file(ofstream &output, int** outAry){
    output << this->numImgRows << " " << this->numImgCols << " " << this->imgMin << " " << this->imgMax << endl;
    
    for(int r = rowFrameSize; r < numImgRows + rowFrameSize; r++){
      for(int c = colFrameSize; c < numImgCols + colFrameSize; c++){
        output << outAry[r][c];
        if ((c + 1) % (this->numImgCols + colFrameSize) != 0) output << " ";
      }
      if ((r + 1) % (this->numImgRows + rowFrameSize) != 0) output << endl;

    }
  }

  void pretty_print(ofstream &output, int** inAry) {
    cout << "BEAUTIFUL PRINT" << endl;
		output << this->numImgRows << " " << this->numImgCols << " " << this->imgMin << " " << this->imgMax << endl;

		for (int i = rowFrameSize; i < this->numImgRows + this->rowFrameSize; i++) {
			for (int j = colFrameSize; j < this->numImgCols + this->colFrameSize; j++) {
				if (inAry[i][j] > 0){
					output << inAry[i][j];
				} else {
				 	output << ".";
				}
        if ((j + 1) % (this->numImgCols + colFrameSize) != 0) output << " ";
			}
      if ((i + 1) % (this->numImgRows + rowFrameSize) != 0) output << endl;
		}
    cout << "BEAUTIFUL PRINT done" << endl;
	}

  
  void pretty_print_struct(ofstream &output, int** inAry) {
    cout << "BEAUTIFUL PRINT STRUCT" << endl;
		output << this->numStructRows << " " << this->numStructCols << " " << this->structMin << " " << this->structMax << endl;
    output << this->rowOrigin << " " << this->colOrigin << endl;
		for (int i = 0; i < this->numStructRows; i++) {
			for (int j = 0; j < this->numStructCols; j++) {
				if (inAry[i][j] > 0){
					output << inAry[i][j];
				} else {
				 	output << ".";
				}
        if ((j + 1) % (this->numStructCols) != 0) output << " ";
			}
      if ((i + 1) % (this->numStructRows) != 0) output << endl;
		}
    cout << "BEAUTIFUL PRINT STRUCT done" << endl;
	}

  void free_Heap() {
		for (int i{ 0 }; i < this->numImgRows + extraRows; i++) {
			delete[] this->zeroFramedAry[i];
      delete[] this->morphAry[i];
      delete[] this->tempAry[i];
		}
		delete[] this->zeroFramedAry;
    delete[] this->morphAry;
    delete[] this->tempAry;

    for(int i{0}; i < this->numStructRows; i++){
      delete[] this->structAry[i];
    }
    delete[] this->structAry;
	}
};

int main(int argc, const char *argv[]) {
  //inFile
	string inputImgName = argv[1];
	ifstream inputInFile;
	inputInFile.open(inputImgName);

  string subImgName = inputImgName.substr(0, inputImgName.size()-4) + "-";
	
  //structFile
  string inputStructName = argv[2];
  ifstream inputStruct;
  inputStruct.open(inputStructName);
  
  //dilationFile
  string outputDilationName = subImgName + argv[3];
  ofstream outputDilation;
  outputDilation.open(outputDilationName);

  //erosionFile
  string outputErosionName = subImgName + argv[4];
  ofstream outputErosion;
  outputErosion.open(outputErosionName);
  
  //closingFile
  string outputClosingName = subImgName + argv[5];
  ofstream outputClosing;
  outputClosing.open(outputClosingName);

  //openingFile
  string outputOpeningName = subImgName + argv[6];
  ofstream outputOpening;
  outputOpening.open(outputOpeningName);

  //prettyPrintImg
  string outputPrettyImgName = subImgName + "prettyImg.txt";
  ofstream outputPrettyImg;
  outputPrettyImg.open(outputPrettyImgName);

  //prettyStruct
  string outputPrettyStructName = subImgName + "prettyStruct.txt";
  ofstream outputPrettyStruct;
  outputPrettyStruct.open(outputPrettyStructName);

  //prettyPrintDilation
  string outputPrettyDilationName = subImgName + "prettyDilation.txt";
  ofstream outputPrettyDilation;
  outputPrettyDilation.open(outputPrettyDilationName);

  //prettyPrintErosion
  string outputPrettyErosionName = subImgName + "prettyErosion.txt";
  ofstream outputPrettyErosion;
  outputPrettyErosion.open(outputPrettyErosionName);

  //prettyPrintOpening
  string outputPrettyOpeningName = subImgName + "prettyOpening.txt";
  ofstream outputPrettyOpening;
  outputPrettyOpening.open(outputPrettyOpeningName);

  //prettyPrintClosing
  string outputPrettyClosingName = subImgName + "prettyClosing.txt";
  ofstream outputPrettyClosing;
  outputPrettyClosing.open(outputPrettyClosingName);


	if (inputInFile.is_open()) {
    Reader * read_obj = new Reader(inputInFile, inputStruct);
    read_obj->load_img(inputInFile);
    read_obj->load_struct(inputStruct);
    read_obj->pretty_print(outputPrettyImg, read_obj->zeroFramedAry);

    read_obj->pretty_print_struct(outputPrettyStruct, read_obj->structAry);

    read_obj->compute_dilation(read_obj->zeroFramedAry, read_obj->morphAry);
    read_obj->ary_to_file(outputDilation, read_obj->morphAry);
    read_obj->pretty_print(outputPrettyDilation, read_obj->morphAry);

    read_obj->zero_2d_ary(read_obj->morphAry);
    read_obj->compute_erosion(read_obj->zeroFramedAry, read_obj->morphAry);
    read_obj->ary_to_file(outputErosion, read_obj->morphAry);
    read_obj->pretty_print(outputPrettyErosion, read_obj->morphAry);

    read_obj->zero_2d_ary(read_obj->morphAry);
    read_obj->compute_opening(read_obj->zeroFramedAry, read_obj->morphAry, read_obj->tempAry);
    read_obj->ary_to_file(outputOpening, read_obj->morphAry);
    read_obj->pretty_print(outputPrettyOpening, read_obj->morphAry);

    read_obj->zero_2d_ary(read_obj->morphAry);
    read_obj->zero_2d_ary(read_obj->tempAry);
    read_obj->compute_closing(read_obj->zeroFramedAry, read_obj->morphAry, read_obj->tempAry);
    read_obj->ary_to_file(outputClosing, read_obj->morphAry);
    read_obj->pretty_print(outputPrettyClosing, read_obj->morphAry);

    inputInFile.close();
    inputStruct.close();
    outputPrettyImg.close();
    outputPrettyStruct.close();
    outputDilation.close();
    outputErosion.close();
    outputClosing.close();
    outputOpening.close();
    outputPrettyDilation.close();
    outputPrettyErosion.close();
    outputPrettyClosing.close();
    outputPrettyOpening.close();

    read_obj->free_Heap();
    delete read_obj;
	}
	else {
		cout << "Error: input error" << inputImgName << endl;
	}

	return 0;
}