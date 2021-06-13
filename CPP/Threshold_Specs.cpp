#include <iostream>
#include <fstream>
using namespace std;

class ThresholdSpecs {
public:
	int numRows{ -1 }, numCols{ -1 }, minVal{ -1 }, maxVal{ -1 };
	int** body;

public:
	ThresholdSpecs(ifstream &input) {
		read_header(input);
		this->body = new int*[this->numRows];
		for (int i{ 0 }; i < this->numRows; i++) {
			body[i] = new int[this->numCols];
			for (int j{ 0 }; j < this->numCols; j++) {
				body[i][j] = 0;
			}
		}
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

	void output_img(ofstream &output, int thresholdValue) {
		cout << "Threshold value is: " << thresholdValue << endl;
		this->minVal = 0;
		this->maxVal = 1;
		output << this->numRows << " " << this->numCols << " " << this->minVal << " " << this->maxVal << endl;


		for (int i{ 0 }; i < this->numRows; i++) {
			for (int j{ 0 }; j < this->numCols; j++) {
				if (this->body[i][j] <= thresholdValue){
					output << 0;
				} else {
				 	output << 1;
				}
				//output << this->body[i][j];
				if ((j + 1) % (this->numCols) != 0) output << " ";
			}
			if ((i + 1) % (this->numRows) != 0) output << endl;
		}
	}

	void pretty_print(ofstream &output, int thresholdValue) {
		this->minVal = 0;
		this->maxVal = 1;
		output << this->numRows << " " << this->numCols << " " << this->minVal << " " << this->maxVal << endl;

		for (int i{ 0 }; i < this->numRows; i++) {
			for (int j{ 0 }; j < this->numCols; j++) {
				if (this->body[i][j] <= thresholdValue){
					output << " ";
				} else {
				 	output << 1;
				}
				//output << this->body[i][j];
				if ((j + 1) % (this->numCols) != 0) output << " ";
			}
			if ((i + 1) % (this->numRows) != 0) output << endl;
		}
	}
	
	void free_Heap() {
		for (int i{ 0 }; i < this->numRows; i++) {
			delete[] this->body[i];
		}
		delete[] this->body;
	}
};

int main(int argc, const char *argv[]) {
	string inputName = argv[1];
	ifstream input;
	input.open(inputName);
	
	int thresholdValue = atoi(argv[2]);

	string outputName1 = argv[3];
	ofstream output1;
	output1.open(outputName1);

	string outputName2 = argv[4];
	ofstream output2;
	output2.open(outputName2);

	if (input.is_open()) {
		if (output1.is_open()) {
			ThresholdSpecs * read_obj = new ThresholdSpecs(input);
			read_obj->read_body(input);
			read_obj->output_img(output1, thresholdValue);
			read_obj->pretty_print(output2, thresholdValue);

			input.close();
			output1.close();
			output2.close();
			
			read_obj->free_Heap();
			delete read_obj;
		}
		else {
			cout << "Error: output error" << outputName1 << endl;
		}
	}
	else {
		cout << "Error: input error" << inputName << endl;
	}

	return 0;
}