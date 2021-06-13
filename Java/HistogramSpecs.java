package project_1;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;

public class HistogramSpecs {
	public int numRows;
	public int numCols;
	public int minVal;
	public int maxVal;
	
	public int[][] body;
	public int[] histAry;
	
	public HistogramSpecs(int rows, int cols, int min, int max) {
		this.numRows = rows;
		this.numCols = cols;
		this.minVal = min;
		this.maxVal = max;
		
		this.body = new int[this.numRows][this.numCols];
		for(int i = 0; i < this.numRows; i++) {
			for(int j = 0; j < this.numCols; j++) {
				this.body[i][j] = 0;
			}
		}
		
		this.histAry = new int[this.maxVal + 1];
		for(int i = 0; i < this.maxVal + 1; i++) {
			this.histAry[i] = 0;
		}
	}
	
	public void computeHist() {
		for(int i = 0; i < this.numRows; i++) {
			for(int j = 0; j < this.numCols; j++) {
				this.histAry[this.body[i][j]]++;
			}
		}
	}
	
	public void printHist(HistogramSpecs readObj, String outputName) throws IOException {
		FileWriter outputWriter = null;
		BufferedWriter output = null;
		
		try {
			outputWriter = new FileWriter(outputName);
			output = new BufferedWriter(outputWriter);
			
			output.write(Integer.toString(readObj.numRows) + " ");
			output.write(Integer.toString(readObj.numCols) + " ");
			output.write(Integer.toString(readObj.minVal) + " ");
			output.write(Integer.toString(readObj.maxVal) + "\n");
			

			for(int i = 0; i < readObj.maxVal + 1; i++)
				output.write(i + " " + readObj.histAry[i] + "\n");
		} finally {
			if(output != null) output.close();
		}
	}
	
	public void dispHist(HistogramSpecs readObj, String outputName) throws IOException{
		FileWriter outputWriter = null;
		BufferedWriter output = null;
		
		try {
			outputWriter = new FileWriter(outputName);
			output = new BufferedWriter(outputWriter);
			
			output.write(Integer.toString(readObj.numRows) + " ");
			output.write(Integer.toString(readObj.numCols) + " ");
			output.write(Integer.toString(readObj.minVal) + " ");
			output.write(Integer.toString(readObj.maxVal) + "\n");
			
			for(int i = 0; i < readObj.maxVal + 1; i++) {
				output.write(i + " (" + readObj.histAry[i] + ") :");
				int c = 0;
				while (c < readObj.histAry[i] && c < 60) {
					c++;
					output.write("+");
				}
				output.write("\n");
			}
			
		} finally {
			if(output != null) output.close();
		}
	}	
	
	public static void main(String[] args) throws IOException{
		String inputName = args[0]; //inFile name
		FileReader inputReader = null;
		BufferedReader buffInReader = null;
		Scanner input = null;
		
		String outputName1 = args[1]; //outFile1 name
		String outputName2 = args[2]; //outFile2 name
		
		try {
			//reading inFile and save it in HistogramSpecs object
			inputReader = new FileReader(inputName);
			buffInReader = new BufferedReader(inputReader);
			input = new Scanner(buffInReader);
			
			int numRows = 0;
			int numCols = 0;
			int minVal = 0;
			int maxVal = 0;
			
			if(input.hasNextInt()) numRows = input.nextInt();
			if(input.hasNextInt()) numCols = input.nextInt();
			if(input.hasNextInt()) minVal = input.nextInt();
			if(input.hasNextInt()) maxVal = input.nextInt();
			
			HistogramSpecs readObj = new HistogramSpecs(numRows, numCols, minVal, maxVal);
			for(int i = 0; i < readObj.numRows; i++) {
				for(int j = 0; j < readObj.numCols; j++) {
					if(input.hasNextInt()) readObj.body[i][j] = input.nextInt();
					else {
						System.out.println("Corrupted input data");
						System.exit(0);
					}
				}
			}
			
			readObj.computeHist();
			readObj.printHist(readObj, outputName1); //create output file 1
			readObj.dispHist(readObj, outputName2); //create output file 2
			
		} finally {
			if(input != null) input.close();
		}
	}
}