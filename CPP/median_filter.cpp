#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Reader {
public:
	int numRows{ -1 }, numCols{ -1 }, minVal{ -1 }, maxVal{ -1 };
	int maskRows{ -1 }, maskCols{ -1 }, maskMin{ -1 }, maskMax{ -1 };
  int newMin{ -1 }, newMax{ -1 };
  int thrVal{ -1 };

	int** body;
  int** mirrorFramedAry;
  int** medianAry;
  int** gaussAry;
  int** thrAry;
  int** maskAry;
  int neighborAry[9]{ -1 };

public:
	Reader(ifstream &input, ifstream&inputMask, int thresholdValue) {
		read_header(input);
		this->body = new int*[this->numRows];
		this->mirrorFramedAry = new int*[this->numRows + 2];
		this->medianAry = new int*[this->numRows + 2];
		this->gaussAry = new int*[this->numRows + 2];
		this->thrAry = new int*[this->numRows + 2];
		for (int i{ 0 }; i < this->numRows; i++) {
			body[i] = new int[this->numCols];
			for (int j{ 0 }; j < this->numCols; j++) {
				body[i][j] = 0;
			}
		}

		for (int i{ 0 }; i < this->numRows + 2; i++) {
			mirrorFramedAry[i] = new int[this->numCols + 2];
			medianAry[i] = new int[this->numCols + 2];
			gaussAry[i] = new int[this->numCols + 2];
		  thrAry[i] = new int[this->numCols + 2];
			for (int j{ 0 }; j < this->numCols + 2; j++) {
				mirrorFramedAry[i][j] = 0;
				medianAry[i][j] = 0;
				gaussAry[i][j] = 0;
				thrAry[i][j] = 0;
			}
		}
    
    read_mask_header(inputMask);
    this->maskAry = new int*[this->maskRows];
    for (int i{0}; i < this->maskRows; i++){
      maskAry[i] = new int[this->maskCols];
      for(int j{0}; j < this->maskCols; j++){
        maskAry[i][j] = 0;
      }
    }

    for(int i{0}; i < 9; i++){
      neighborAry[i] = 0;
    }

    read_threshold_value(thresholdValue);
	}

	void read_header(ifstream &input) {
		input >> this->numRows >> this->numCols >> this->minVal >> this->maxVal;
 	}

	void read_body(ifstream &input) {
		for (int i{ 0 }; i < this->numRows; i++) {
			for (int j{ 0 }; j < this->numCols; j++) {
				input >> body[i][j];
			}
		}
	}

  void read_mask_header(ifstream &input){
    input >> this->maskRows >> this->maskCols >> this->maskMin >> this->maskMax;
  }

  void read_mask_body(ifstream &input){
    for (int i{ 0 }; i < this->maskRows; i++) {
			for (int j{ 0 }; j < this->maskCols; j++) {
				input >> maskAry[i][j];
			}
		}
  }

  void read_threshold_value(int thresholdValue){
    this->thrVal = thresholdValue;
  }

  void swap(int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
  }

  void selection_sort(int ary[], int n){
    int i, j, min;
    for(i = 0; i < n-1; i++){
      min = i;
      for(j = i+1; j < n; j++){
        if(ary[min] > ary[j]){
          min = j;
        }
      }
      swap(&ary[min], &ary[i]);
    }
  }

  void mirror_frame(){
    cout << "initializing mirror frame" << endl;
    this->newMin = this->minVal;
    this->newMax = this->maxVal;

    //four corners mirror
    mirrorFramedAry[0][0] = body[0][0];
    mirrorFramedAry[0][this->numCols+1] = body[0][this->numCols-1];
    mirrorFramedAry[this->numRows+1][0] = body[this->numRows-1][0];
    mirrorFramedAry[this->numRows+1][this->numCols+1] = body[this->numRows-1][this->numCols-1];

    //top row and bottom row mirror
    for(int i{0}; i < this->numCols; i++ ){
      mirrorFramedAry[0][i+1] = body[0][i];
      mirrorFramedAry[this->numRows+1][i+1] = body[this->numRows-1][i];
    }

    //left column and right column mirror
    for(int i{0}; i < this->numRows; i++ ){
      mirrorFramedAry[i+1][0] = body[i][0];
      mirrorFramedAry[i+1][this->numCols+1] = body[i][this->numCols-1];
    }

    //copying body to mirror array
    for (int i{ 0 }; i < this->numRows; i++) {
			for (int j{ 0 }; j < this->numCols; j++) {
				mirrorFramedAry[i+1][j+1] = body[i][j];
			}
		}
    cout << "mirror frame done" <<endl;
  }

  void load_neighbors(int r, int c){
    int k = 0;
    for(int i = -1; i <= 1; ++i){
      for(int j = -1; j <= 1; ++j){
        this->neighborAry[k] = mirrorFramedAry[r+i][c+j];
        k++;
      }
    }
  }

  void compute_median(){
    cout << "computing median" << endl;
    newMin = 9999;
    newMax = 0;

    int i = 1;
    while(i <= numRows){
      int j = 1;
      while(j <= numCols){
        load_neighbors(i, j);
        selection_sort(neighborAry, 9);
        this->medianAry[i][j] = neighborAry[4];
        if(newMin > medianAry[i][j])
          newMin = medianAry[i][j];

        if(newMax < medianAry[i][j])
          newMax = medianAry[i][j];

        j++;
      }
      i++;
    }
    cout << "computing median done" << endl;
  }

  void image_reformat(ofstream &output, int** inAry){
    cout << "printing image" << endl;
    output << this->numRows << " " << this->numCols << " " << this->newMin << " " << this->newMax << endl;
    string str = to_string(this->newMax);
    int width = (int) str.length();
    int ww; //current width on the pixel value

    for(int r = 1; r <= this->numRows; r++ ){
      for(int c = 1; c <= this->numCols; c++){
        str = to_string(inAry[r][c]);
        ww = (int) str.length();
        
        while(ww < width){
          output << " ";
          ww++;
        }
        output << inAry[r][c] << " ";
      }
      if ((r + 1) % (this->numRows+2) != 0) output << endl;
    }
    cout << "printing image done" << endl;
  }

  int convolution(int r, int c){
    int conv = 0;

    int totalWeight = 0;
    for(int i = 0; i < 9; i++){
      totalWeight += maskAry[i/3][i%3];
    }
    load_neighbors(r, c);
    for(int x = 0; x < 9; x++){
      conv += maskAry[x/3][x%3] * this->neighborAry[x];
    }

    return conv / totalWeight;
  };

  void compute_gauss(){
    cout << "computing gaussian" << endl;
    newMin = 9999;
    newMax = 0;
    int i = 1;
    int j = 1;

    for(int i = 1; i <= this->numRows; i++){
      for(int j = 1; j <= this->numCols; j++){
        this->gaussAry[i][j] = convolution(i, j);
        if(newMin > gaussAry[i][j])
          newMin = gaussAry[i][j];

        if(newMax < gaussAry[i][j])
          newMax = gaussAry[i][j];
      }
    }
    cout << "computing gaussian done" << endl;
  }

  void threshold_image(int** inAry1, int** inAry2){
    cout << "thresholding image..." << endl;
    newMin = 0;
    newMax = 1;

    for(int i = 1; i <= numRows; i++){
      for(int j = 1; j <= numCols; j++){
        if(inAry1[i][j] >= this->thrVal){
          inAry2[i][j] = 1;
        } else {
          inAry2[i][j] = 0;
        }
      }
    }
    cout << "thresholding image done" << endl;
  }

	void pretty_print(ofstream &output, int** inAry) {
    cout << "BEAUTIFUL PRINT" << endl;
		output << this->numRows << " " << this->numCols << " " << this->newMin << " " << this->newMax << endl;

		for (int i{ 1 }; i <= this->numRows; i++) {
			for (int j{ 1 }; j <= this->numCols; j++) {
				if (inAry[i][j] > 0){
					output << inAry[i][j];
				} else {
				 	output << ".";
				}
				//output << this->body[i][j];
				if ((j + 1) % (this->numCols+2) != 0) output << " ";
			}
			if ((i + 1) % (this->numRows+2) != 0) output << endl;
		}
    cout << "BEAUTIFUL PRINT done" << endl;
	}
	
	void free_Heap() {
		for (int i{ 0 }; i < this->numRows; i++) {
			delete[] this->body[i];
		}
		delete[] this->body;

    for(int i{0}; i< this->numRows+2; i++){
      delete[] this->mirrorFramedAry[i];
      delete[] this->medianAry[i];
      delete[] this->gaussAry[i];
      delete[] this->thrAry[i];
    }
    delete[] this->mirrorFramedAry;
    delete[] this->medianAry;
    delete[] this->gaussAry;
    delete[] this->thrAry;
	}
};

int main(int argc, const char *argv[]) {
  //inFile
	string inputName = argv[1];
	ifstream inputInFile;
	inputInFile.open(inputName);
	
  //maskFile
  string inputMaskName = argv[2];
  ifstream inputMask;
  inputMask.open(inputMaskName);

	int thresholdValue = atoi(argv[3]); //use 38

  //inputImg
	string outputImgName = argv[4];
	ofstream outputImg;
	outputImg.open(outputImgName);
  
  //MedianOutImg
	string outputMedianImgName = argv[5];
	ofstream outputMedianImg;
	outputMedianImg.open(outputMedianImgName);
  
  //MedianThrImg
  string outputMedianThrImgName = argv[6];
  ofstream outputMedianThrImg;
  outputMedianThrImg.open(outputMedianThrImgName);
  
  //MedianPrettyPrint
  string outputMedianPrettyPrintName = argv[7];
  ofstream outputMedianPrettyPrint;
  outputMedianPrettyPrint.open(outputMedianPrettyPrintName);
  
  //GaussOutImg
  string outputGaussImgName = argv[8];
  ofstream outputGaussImg;
  outputGaussImg.open(outputGaussImgName);
  
  //GaussThrImg
  string outputGaussThrImgName = argv[9];
  ofstream outputGaussThrImg;
  outputGaussThrImg.open(outputGaussThrImgName);
  
  //GaussPrettyPrint
  string outputGaussPrettyPrintName = argv[10];
  ofstream outputGaussPrettyPrint;
  outputGaussPrettyPrint.open(outputGaussPrettyPrintName);
  
	if (inputInFile.is_open()) {
    Reader * read_obj = new Reader(inputInFile, inputMask, thresholdValue);
    read_obj->read_body(inputInFile);
    read_obj->read_mask_body(inputMask);
    read_obj->mirror_frame();
    read_obj->image_reformat(outputImg, read_obj->mirrorFramedAry);
    read_obj->compute_median();
    read_obj->image_reformat(outputMedianImg, read_obj->medianAry);
    read_obj->threshold_image(read_obj->medianAry, read_obj->thrAry);
    read_obj->image_reformat(outputMedianThrImg, read_obj->thrAry);
    read_obj->pretty_print(outputMedianPrettyPrint, read_obj->thrAry);
    read_obj->compute_gauss();
    read_obj->image_reformat(outputGaussImg, read_obj->gaussAry);
    read_obj->threshold_image(read_obj->gaussAry, read_obj->thrAry);
    read_obj->image_reformat(outputGaussThrImg, read_obj->thrAry);
    read_obj->pretty_print(outputGaussPrettyPrint, read_obj->thrAry);

    inputInFile.close();
    inputMask.close();
    outputImg.close();
    outputMedianImg.close();
    outputMedianThrImg.close();
    outputMedianPrettyPrint.close();
    outputGaussImg.close();
    outputGaussThrImg.close();
    outputGaussPrettyPrint.close();

    read_obj->free_Heap();
    delete read_obj;
	}
	else {
		cout << "Error: input error" << inputName << endl;
	}

	return 0;
}