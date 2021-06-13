package project_6;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;

public class Thinning {
	
	public int numRows, numCols, minVal, maxVal;
	public int changeFlag;
	public int[][] firstAry;
	public int[][] secondAry;
	
	public Thinning(int rows, int cols, int min, int max) {
		this.numRows = rows;
		this.numCols = cols;
		this.minVal = min;
		this.maxVal = max;
		
		this.firstAry = new int[numRows + 2][numCols + 2];
		this.secondAry = new int[numRows + 2][numCols + 2];
		
		for(int i = 0; i < numRows + 2; i++) {
			for(int j = 0; j < numCols + 2; j++) {
				this.firstAry[i][j] = 0;
				this.secondAry[i][j] = 0;
			}
		}
	}
	
	public void zeroFrame(int[][] inAry) {
		for(int i = 0; i < numRows; i++) {
			inAry[i][0] = 0;
			inAry[i][numCols-1] = 0;
		}
		for(int j = 0; j < numCols; j++) {
			inAry[0][j] = 0;
			inAry[numRows-1][j] = 0;
		}
	}
	
	public void copyAry() {
		for(int i = 0; i <= numRows; i++) {
			for(int j = 0; j <= numCols; j++) {
				this.firstAry[i][j] = this.secondAry[i][j];
			}
		}
	}
	
	public boolean checkCondition(int CODE, int r, int c) {
		int neighborCount = 0;
		for(int i = -1; i <= 1; i++) {
			for(int j = -1; j <= 1; j++) {
				if(i == 0 && j == 0) continue;
				if(firstAry[r+i][c+j] > 0) neighborCount++;
			}
		}
		
		//NORTH THINNING
		if(CODE == 1) {
			if(firstAry[r-1][c] != 0) return false;
			if(neighborCount < 4) return false;
			if(checkAddConComp(r, c)) return false;
		}
		
		//SOUTH THINNING
		if(CODE == 2) {
			if(firstAry[r+1][c] != 0) return false;
			if(neighborCount < 4) return false;
			if(checkAddConComp(r, c)) return false;
		}
		
		//WEST THINNING
		if(CODE == 3) {
			if(firstAry[r][c-1] != 0) return false;
			if(neighborCount < 3) return false;
			if(checkAddConComp(r, c)) return false;
		}
		
		//EAST THINNING
		if(CODE == 4) {
			if(firstAry[r][c+1] != 0) return false;
			if(neighborCount < 3) return false;
			if(checkAddConComp(r, c)) return false;
		}
		
		changeFlag++;
		return true;
	}
	
	public boolean checkAddConComp(int r, int c) {
		int[] neighborAry = new int[9];
		for(int i = 0; i < 9; i++) neighborAry[i] = 0;
		
		int counter = 0;
		for(int i = -1; i <= 1; i++) {
			for(int j = -1; j <= 1; j++) {
				neighborAry[counter] = firstAry[r+i][c+j];
				counter++;
			}
		}
		// [0], [1], [2]
		// [3], [4], [5]
		// [6], [7], [8]

		if(neighborAry[1] == 0 && neighborAry[3] == 0 && neighborAry[0] == 1) return true;
		if(neighborAry[7] == 0 && neighborAry[3] == 0 && neighborAry[6] == 1) return true;
		if(neighborAry[1] == 0 && neighborAry[5] == 0 && neighborAry[2] == 1) return true;
		if(neighborAry[7] == 0 && neighborAry[5] == 0 && neighborAry[8] == 1) return true;

		if(neighborAry[3] == 0 && neighborAry[5] == 0) {
			if((neighborAry[0] == 1 || neighborAry[1] == 1 || neighborAry[2] == 1) &&
					(neighborAry[6] == 1 || neighborAry[7] == 1 || neighborAry[8] == 1))
				return true;
		}
		if(neighborAry[1] == 0 && neighborAry[7] == 0) {
			if((neighborAry[0] == 1 || neighborAry[3] == 1 || neighborAry[6] == 1) &&
					(neighborAry[2] == 1 || neighborAry[5] == 1 || neighborAry[8] == 1))
				return true;
		}
		
		return false;
	}
	
	public void northThinning() {
		for(int i = 0; i < numRows; i++) {
			for(int j = 0; j < numCols; j++) {
				if(secondAry[i][j] > 0) {
					if(checkCondition(1, i, j)) {
						secondAry[i][j] = 0;
					}
				}
			}
		}
	}
	
	public void southThinning() {
		for(int i = numRows; i >= 0; i--) {
			for(int j = numCols; j >= 0; j--) {
				if(secondAry[i][j] > 0) {
					if(checkCondition(2, i, j)) secondAry[i][j] = 0;
				}
			}
		}
	}
	
	public void westThinning() {
		for(int i = 0; i < numRows; i++) {
			for(int j = 0; j < numCols; j++) {
				if(secondAry[i][j] > 0) {
					if(checkCondition(3, i, j)) secondAry[i][j] = 0;
				}
			}
		}
	}

	public void eastThinning() {
		for(int i = numRows; i >= 0; i--) {
			for(int j = numCols; j >= 0; j--) {
				if(secondAry[i][j] > 0) {
					if(checkCondition(4, i, j)) secondAry[i][j] = 0;
				}
			}
		}
	}
	
	public static Thinning loadImage(String inFile) throws FileNotFoundException {
		FileReader inFileReader = new FileReader(inFile);
		BufferedReader buffInReader = new BufferedReader(inFileReader);
		Scanner input = new Scanner(buffInReader);
		
		int r = 0, c = 0, min = 0, max = 0;
		if(input.hasNextInt()) r = input.nextInt();
		if(input.hasNextInt()) c = input.nextInt();
		if(input.hasNextInt()) min = input.nextInt();
		if(input.hasNextInt()) max = input.nextInt();
		
		Thinning thin = new Thinning(r, c, min, max);
		for(int i = 1; i <= thin.numRows; i++) {
			for(int j = 1; j <= thin.numCols; j++) {
				if(input.hasNextInt()) {
					thin.firstAry[i][j] = input.nextInt();
					thin.secondAry[i][j] = thin.firstAry[i][j];
				}
				else {
					System.out.println("Corrupted input data");
					System.exit(0);
				}
			}
		}
		input.close();
		return thin;
	}
	
	public static void imageReformat(Thinning thin, String outputName) throws IOException {
		System.out.println("Printing " + outputName + "...");
		FileWriter outputWriter = null;
		BufferedWriter output = null;
		
		String str = Integer.toString(thin.maxVal);
		int width = str.length();
		int ww;
		
		try {
			outputWriter = new FileWriter(outputName);
			output = new BufferedWriter(outputWriter);
			
			output.write(Integer.toString(thin.numRows) + " ");
			output.write(Integer.toString(thin.numCols) + " ");
			output.write(Integer.toString(thin.minVal) + " ");
			output.write(Integer.toString(thin.maxVal) + "\n");
			
			for(int i = 1; i <= thin.numRows; i++) {
				for(int j = 1; j <= thin.numCols; j++) {
					str = Integer.toString(thin.firstAry[i][j]);
					ww = str.length();
					
					while(ww < width) {
						output.write(" ");
						ww++;
					}
					if(thin.firstAry[i][j] != 0) output.write(Integer.toString(thin.firstAry[i][j]) + " ");
					else output.write("  ");
				}
				output.write("\n");
			}
			
		} finally {
			if(output != null) output.close();
		}
		System.out.println(outputName + " printed!");
	}
	
	public static void main(String[] args) throws IOException {
		String inFileName = args[0];
		
		Thinning thin = loadImage(inFileName);
		imageReformat(thin, "PrettyPrint-" + args[0]);
		
		int cycle = 0;
		do {
			thin.changeFlag = 0;
			
			thin.northThinning();
			thin.copyAry();
			//imageReformat(thin, "PrettyPrint-" + inFileName.substring(0, inFileName.length()-4) + "-cycleN" + cycle + ".txt" );
			
			thin.southThinning();
			thin.copyAry();
			//imageReformat(thin, "PrettyPrint-" + inFileName.substring(0, inFileName.length()-4) + "-cycleS" + cycle + ".txt" );
			
			thin.westThinning();
			thin.copyAry();
			//imageReformat(thin, "PrettyPrint-" + inFileName.substring(0, inFileName.length()-4) + "-cycleW" + cycle + ".txt" );
			
			thin.eastThinning();
			thin.copyAry();
			//imageReformat(thin, "PrettyPrint-" + inFileName.substring(0, inFileName.length()-4) + "-cycleE" + cycle + ".txt" );
			
			cycle++;
			
			imageReformat(thin, "PrettyPrint-" + inFileName.substring(0, inFileName.length()-4) + "-cycle" + cycle + ".txt" );
		} while(thin.changeFlag > 0);
		
	}
}