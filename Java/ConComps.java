package project_4;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;

public class ConComps {
	class Property{
		int label;
		int numPixels;
		int minR, minC, maxR, maxC;
		
		public Property() {
			this.label = 0;
			this.numPixels = 0;
			this.minR = 0;
			this.minC = 0;
			this.maxR = 0;
			this.maxC = 0;
		}
		
		public void setBB() {
			for(int i = 1; i <= numRows; i++) {
				if(minR != 0) break;
				for(int j = 1; j <= numCols; j++) {
					if(zeroFramedAry[i][j] == label) {
						minR = i;
					}
				}
			}
			for(int j = 1; j <= numCols; j++) {
				if(minC != 0) break;
				for(int i = 1; i <= numRows; i++) {
					if(zeroFramedAry[i][j] == label) {
						minC = j;
					}
				}
			}
			for(int i = 1; i <= numRows; i++) {
				for(int j = 1; j <= numCols; j++) {
					if(zeroFramedAry[i][j] == label) {
						if(maxR < i) maxR = i;
						if(maxC < j) maxC = j;
					}
				}
			}
		}
		
		public void printLabel() throws IOException {
			System.out.println("Printing CCProperty...");
			FileWriter outputWriter = null;
			BufferedWriter output = null;
			
			try {
				outputWriter = new FileWriter("CCProperty.txt");
				output = new BufferedWriter(outputWriter);
				
				output.write(Integer.toString(numRows) + " ");
				output.write(Integer.toString(numCols) + " ");
				output.write(Integer.toString(newMin) + " ");
				output.write(Integer.toString(newMax) + "\n");
				
				for(int i = 1; i <= numRows; i++) {
					for(int j = 1; j <= numCols; j++) {
						output.write(Integer.toString(zeroFramedAry[i][j]) + " ");
					}
					output.write("\n");
				}
				
			} finally {
				if(output != null) output.close();
			}
		}
	}
	
	Property[] ccProperty;
	
	public int numRows, numCols, minVal, maxVal;
	public int newMin, newMax;
	public int newLabel;
	public int trueNumCC;
	public int eqArySize;
	
	public int[][] zeroFramedAry;
	public int[] nonZeroNeighborAry;
	public int[] eqAry;
	
	
	public ConComps(int rows, int cols, int min, int max) {
		this.numRows = rows;
		this.numCols = cols;
		this.minVal = min;
		this.maxVal = max;
		this.eqArySize = numRows * numCols / 4;
		
		this.zeroFramedAry = new int[numRows+2][numCols+2];
		for(int i = 0; i < numRows+2; i++) {
			for(int j = 0; j < numCols+2; j++) {
				this.zeroFramedAry[i][j] = 0;
			}
		}
		
		this.nonZeroNeighborAry = new int[5];
		for(int i = 0; i < 5; i++) {
			this.nonZeroNeighborAry[i] = 0;
		}
		
		this.eqAry = new int[eqArySize];
		for(int i = 0; i < eqArySize; i++) {
			this.eqAry[i] = i;
		}
	}
	
	public void connect8Pass1() {
		System.out.println("Processing Pass 1...");
		this.newMin = this.minVal;
		this.newLabel = minVal;
		
		for(int i = 1; i <= this.numRows; i++) {
			for(int j = 1; j <= this.numCols; j++) {
				
				if(this.zeroFramedAry[i][j] > 0) {
					//System.out.print("pixel at " + i + ", " + j + " ");
					switch(getCase1(i, j)) {
					case 1:
						this.newLabel++;
						this.zeroFramedAry[i][j] = this.newLabel;
						break;
					case 2:
						this.zeroFramedAry[i][j] = sameLabel(this.nonZeroNeighborAry);
						break;
					case 3:
						this.zeroFramedAry[i][j] = min(this.nonZeroNeighborAry);
						//System.out.println("from case 3 pos " + i + ", " + j + " updating eq table at " + this.newLabel + " to " + this.eqAry[this.newLabel]);
						//this.eqAry[this.newLabel] = this.zeroFramedAry[i][j];
						break;
					}
				}
			}
		}
		this.newMax = this.newLabel;
		System.out.println("Pass 1 done!");
	}
	
	public void connect8Pass2() {
		System.out.println("Processing Connect 8 Pass 2...");
		this.newMin = this.minVal;
		
		for(int i = numRows; i > 0; i--) {
			for(int j = numCols; j > 0; j--) {
				
				if(this.zeroFramedAry[i][j] > 0) {
					switch(getCase2(i, j)) {
					case 1:
						
						//System.out.println("does nothing at " + i + ", " + j + " with " + this.zeroFramedAry[i][j]);
						break;
					case 2:
						int minLabel = min(this.nonZeroNeighborAry);
						if(this.zeroFramedAry[i][j] > minLabel) {
							//if(this.eqAry[zeroFramedAry[i][j]] > )
							this.eqAry[this.zeroFramedAry[i][j]] = minLabel;
							this.zeroFramedAry[i][j] = minLabel;
						}
						break;
					}
				}
				
			}
		}
		System.out.println("Connect 8 Pass 2 done!");
	}
	
	public int sameLabel(int[] neighborAry) {
		for(int i = 0; i < 5; i++) {
			if(neighborAry[i] > 0) return neighborAry[i];
		}
		return 0;
	}
	
	public int min(int[] neighborAry) {
		int min = 0;
		
		for(int i = 0; i < 5; i++) {
			if(neighborAry[i] > 0) {
				min = neighborAry[i];
				break;
			}
		}
		for(int i = 0; i < 5; i++) {
			if(neighborAry[i] < min && neighborAry[i] != 0) min = neighborAry[i];
		}
		
		return min;
	}
	
	public int getCase1(int i, int j) {
		int diffLabel = 0;
		
		this.nonZeroNeighborAry[0] = this.zeroFramedAry[i-1][j-1];
		this.nonZeroNeighborAry[1] = this.zeroFramedAry[i-1][j];
		this.nonZeroNeighborAry[2] = this.zeroFramedAry[i-1][j+1];
		this.nonZeroNeighborAry[3] = this.zeroFramedAry[i][j-1];

		int temp = 0;
		for(int k = 0; k < 4; k++) {
			if(diffLabel != 0) {
				if(this.nonZeroNeighborAry[k] != temp && this.nonZeroNeighborAry[k] != 0) {
					diffLabel++;
				}
			}
			if(diffLabel == 0) {
				if(this.nonZeroNeighborAry[k] > 0) {
					temp = this.nonZeroNeighborAry[k];
					diffLabel++;
				}
			}
		}
		if(diffLabel == 1) return 2;
		if(diffLabel > 1) return 3;
		return 1;
	}
	
	public int getCase2(int i, int j) {
		int diffLabel = 0;
		
		this.nonZeroNeighborAry[0] = this.zeroFramedAry[i][j];
		this.nonZeroNeighborAry[1] = this.zeroFramedAry[i][j+1];
		this.nonZeroNeighborAry[2] = this.zeroFramedAry[i+1][j-1];
		this.nonZeroNeighborAry[3] = this.zeroFramedAry[i+1][j];
		this.nonZeroNeighborAry[4] = this.zeroFramedAry[i+1][j+1];

		int temp = 0;
		for(int k = 0; k < 5; k++) {
			if(diffLabel != 0) {
				if(this.nonZeroNeighborAry[k] != temp && this.nonZeroNeighborAry[k] != 0) {
					diffLabel++;
				}
			}
			if(diffLabel == 0) {
				if(this.nonZeroNeighborAry[k] > 0) {
					temp = this.nonZeroNeighborAry[k];
					diffLabel++;
				}
			}
		}
		if(diffLabel > 1) return 2;
		return 1;
	}
	
	//MANAGE EQ ARY
	public void manageEQAry() {
		System.out.println("Managing EQAry...");
		int readLabel = 0;
		int index = 1;
		while(index < this.newLabel) {
			if(index != this.eqAry[index]) {
				this.eqAry[index] = this.eqAry[this.eqAry[index]];
			}
			else {
				readLabel++;
				eqAry[index] = readLabel;
			}
			index++;
		}
		this.trueNumCC = readLabel;
		System.out.println("EQAry managed!");
	}
	
	//CONNECT 4
	public void connect4Pass1() {
		System.out.println("Processing Connect 4 Pass 1...");
		this.newMin = this.minVal;
		this.newLabel = minVal;
		
		for(int i = 1; i <= this.numRows; i++) {
			for(int j = 1; j <= this.numCols; j++) {
				
				if(this.zeroFramedAry[i][j] > 0) {
					//System.out.print("pixel at " + i + ", " + j + " ");
					switch(con4GetCase1(i, j)) {
					case 1:
						this.newLabel++;
						this.zeroFramedAry[i][j] = this.newLabel;
						break;
					case 2:
						this.zeroFramedAry[i][j] = sameLabel(this.nonZeroNeighborAry);
						break;
					case 3:
						this.zeroFramedAry[i][j] = min(this.nonZeroNeighborAry);
						//updateEQAry(this.newLabel);
						//System.out.println("from case 3 pos " + i + ", " + j + " updating eq table at " + this.newLabel + " to " + this.eqAry[this.newLabel]);
						this.eqAry[this.newLabel] = this.zeroFramedAry[i][j];
						break;
					}
				}
			}
		}
		this.newMax = this.newLabel;
		System.out.println("Pass 1 done!");
	}
	
	public void connect4Pass2() {
		System.out.println("Processing Connect 4 Pass 2...");
		this.newMin = this.minVal;
		
		for(int i = this.numRows; i > 0; i--) {
			for(int j = this.numCols; j > 0; j--) {
				
				if(this.zeroFramedAry[i][j] > 0) {
					//System.out.print("pixel at " + i + ", " + j + " ");
					switch(con4GetCase2(i, j)) {
					case 1:
						//System.out.println("does nothing at " + i + ", " + j + " with " + this.zeroFramedAry[i][j]);
						break;
					case 2:
						int minLabel = min(this.nonZeroNeighborAry);
						//this.zeroFramedAry[i][j] = min(this.nonZeroNeighborAry);
						if(this.zeroFramedAry[i][j] > minLabel) {
							this.eqAry[this.zeroFramedAry[i][j]] = minLabel;
							this.zeroFramedAry[i][j] = minLabel;
						}
						break;
					}
				}
			}
		}
		System.out.println("Pass 2 done!");
	}
	
	//PASS 3
	public void connectPass3() {
		this.ccProperty = new Property[this.trueNumCC + 1];
		
		for(int i = 0; i < this.trueNumCC + 1; i++) {
			ccProperty[i] = new Property();
			ccProperty[i].label = i+1;
		}
		this.newMax = this.trueNumCC;
		
		for(int i = 1; i <= this.numRows; i++) {
			for(int j = 1; j <= this.numCols; j++) {
				if(this.zeroFramedAry[i][j] > 0) {
					this.zeroFramedAry[i][j] = this.eqAry[this.zeroFramedAry[i][j]];
					//System.out.println(this.zeroFramedAry[i][j]);
					this.ccProperty[this.zeroFramedAry[i][j]-1].numPixels++;
				}
			}
		}
		
		for(int i = 0; i < this.trueNumCC; i++) {
			ccProperty[i].setBB();
		}
	}
	
	public void drawBoxes(Property[] ccProperty) {
		int index = 0;
		
		while(index < this.trueNumCC) {
			for(int i = ccProperty[index].minR; i <= ccProperty[index].maxR; i++) {
				this.zeroFramedAry[i][ccProperty[index].minC] = ccProperty[index].label;
				this.zeroFramedAry[i][ccProperty[index].maxC] = ccProperty[index].label;
			}
			for(int j = ccProperty[index].minC; j <= ccProperty[index].maxC; j++) {
				this.zeroFramedAry[ccProperty[index].minR][j] = ccProperty[index].label;
				this.zeroFramedAry[ccProperty[index].maxR][j] = ccProperty[index].label;
			}
			
			index++;
		}
	}
	
	public int con4GetCase1(int i, int j) {
		this.nonZeroNeighborAry[0] = this.zeroFramedAry[i-1][j];
		this.nonZeroNeighborAry[1] = this.zeroFramedAry[i][j-1];
		
		if(this.nonZeroNeighborAry[0] == 0 &&
				this.nonZeroNeighborAry[1] == 0) return 1;
		
		if(this.nonZeroNeighborAry[0] != this.nonZeroNeighborAry[1]) {
			if(this.nonZeroNeighborAry[0] == 0 || this.nonZeroNeighborAry[1] == 0) return 2;
		}
		return 3;
	}
	
	public int con4GetCase2(int i, int j) {
		this.nonZeroNeighborAry[0] = this.zeroFramedAry[i][j];
		this.nonZeroNeighborAry[1] = this.zeroFramedAry[i][j+1];
		this.nonZeroNeighborAry[2] = this.zeroFramedAry[i+1][j];
		
		if(this.nonZeroNeighborAry[1] == 0 &&
				this.nonZeroNeighborAry[2] == 0) return 1;
		
		if(this.nonZeroNeighborAry[0] != this.nonZeroNeighborAry[1] ||
				this.nonZeroNeighborAry[1] != this.nonZeroNeighborAry[2] ||
				this.nonZeroNeighborAry[0] != this.nonZeroNeighborAry[2]) return 2;
		return 1;
	}
	
	public void zero2D() {
		for(int i = 0; i < numRows+2; i++) {
			for(int j = 0; j < numCols+2; j++) {
				this.zeroFramedAry[i][j] = 0;
			}
		}
	}
	
	public static void printEQAry(ConComps conComps, String outputName) throws IOException {
		System.out.println("Printing " + outputName + "...");
		FileWriter outputWriter = null;
		BufferedWriter output = null;
		
		try {
			outputWriter = new FileWriter(outputName);
			output = new BufferedWriter(outputWriter);
			
			output.write(Integer.toString(conComps.numRows) + " ");
			output.write(Integer.toString(conComps.numCols) + " ");
			output.write(Integer.toString(conComps.newMin) + " ");
			output.write(Integer.toString(conComps.newMax) + "\n");
			
			for(int i = 1; i < conComps.newLabel; i++) {
				output.write(i + ": " + conComps.eqAry[i] + "\n");
			}
			
		} finally {
			if(output != null) output.close();
		}
		System.out.println(outputName + " printed!");
	}
	
	public static void imageReformat(ConComps conComps, String outputName) throws IOException {
		System.out.println("Printing " + outputName + "...");
		FileWriter outputWriter = null;
		BufferedWriter output = null;
		
		String str = Integer.toString(conComps.newMax);
		int width = str.length();
		int ww;
		
		try {
			outputWriter = new FileWriter(outputName);
			output = new BufferedWriter(outputWriter);
			
			output.write(Integer.toString(conComps.numRows) + " ");
			output.write(Integer.toString(conComps.numCols) + " ");
			output.write(Integer.toString(conComps.newMin) + " ");
			output.write(Integer.toString(conComps.newMax) + "\n");
			
			for(int i = 1; i <= conComps.numRows; i++) {
				for(int j = 1; j <= conComps.numCols; j++) {
					str = Integer.toString(conComps.zeroFramedAry[i][j]);
					ww = str.length();
					
					while(ww < width) {
						output.write(" ");
						ww++;
					}
					if(conComps.zeroFramedAry[i][j] != 0) output.write(Integer.toString(conComps.zeroFramedAry[i][j]) + " ");
					else output.write("  ");
				}
				output.write("\n");
			}
			
		} finally {
			if(output != null) output.close();
		}
		System.out.println(outputName + " printed!");
	}
	
	public static void printLabel(ConComps conComps) throws IOException {
		System.out.println("Printing CCProperty...");
		FileWriter outputWriter = null;
		BufferedWriter output = null;
		
		try {
			outputWriter = new FileWriter("CCProperty.txt");
			output = new BufferedWriter(outputWriter);
			
			output.write(Integer.toString(conComps.numRows) + " ");
			output.write(Integer.toString(conComps.numCols) + " ");
			output.write(Integer.toString(conComps.newMin) + " ");
			output.write(Integer.toString(conComps.newMax) + "\n");
			output.write(Integer.toString(conComps.trueNumCC) + "\n");
			
			for(int i = 0; i < conComps.trueNumCC; i++) {
				output.write("....................\n");
				output.write(conComps.ccProperty[i].label + "\n");
				output.write(conComps.ccProperty[i].numPixels + "\n");
				output.write(conComps.ccProperty[i].minR + " " + conComps.ccProperty[i].minC + "\n");
				output.write(conComps.ccProperty[i].maxR + " " + conComps.ccProperty[i].maxC + "\n");
			}
			
		} finally {
			if(output != null) output.close();
		}
		System.out.println("CCProperty printed!");
	}
	
	public static ConComps loadImage(String inFile) throws FileNotFoundException {
		FileReader inFileReader = new FileReader(inFile);
		BufferedReader buffInReader = new BufferedReader(inFileReader);
		Scanner input = new Scanner(buffInReader);
		
		int r = 0, c = 0, min = 0, max = 0;
		if(input.hasNextInt()) r = input.nextInt();
		if(input.hasNextInt()) c = input.nextInt();
		if(input.hasNextInt()) min = input.nextInt();
		if(input.hasNextInt()) max = input.nextInt();
		
		ConComps conComps = new ConComps(r, c, min, max);
		for(int i = 1; i <= conComps.numRows; i++) {
			for(int j = 1; j <= conComps.numCols; j++) {
				if(input.hasNextInt()) conComps.zeroFramedAry[i][j] = input.nextInt();
				else {
					System.out.println("Corrupted input data");
					System.exit(0);
				}
			}
		}
		input.close();
		return conComps;
	}
	
	public static void main(String[] args) throws IOException {
		String inFileName = args[0];
		
		System.out.println("Which connectedness algorithm do you want to apply? Input either 4 or 8.");
		Scanner scan = new Scanner(System.in);
		String connect = scan.next();
		System.out.println(connect);
		if(Integer.parseInt(connect) != 4 && Integer.parseInt(connect) != 8) {
			System.out.println("Invalid input. Please re-run the program and input 4 or 8");
			System.exit(0);
		}
		if(Integer.parseInt(connect) == 4) {
			ConComps conComps = loadImage(inFileName);
			imageReformat(conComps, "outFile.txt");
			conComps.connect4Pass1();
			imageReformat(conComps, "connect4pass1.txt");

			conComps.connect4Pass2();
			imageReformat(conComps, "connect4pass2.txt");
			
			conComps.manageEQAry();
			printEQAry(conComps, "connect4EQAry.txt");
			
			conComps.connectPass3();
			imageReformat(conComps, "connect4pass3.txt");
			
			printLabel(conComps);
			
			conComps.drawBoxes(conComps.ccProperty);
			imageReformat(conComps, "connect4DrawBox.txt");
		}
		
		if(Integer.parseInt(connect) == 8) {
			ConComps conComps = loadImage(inFileName);
			imageReformat(conComps, "outFile.txt");
			conComps.connect8Pass1();
			imageReformat(conComps, "connect8pass1.txt");

			conComps.connect8Pass2();
			imageReformat(conComps, "connect8pass2.txt");
			
			conComps.manageEQAry();
			printEQAry(conComps, "connect8EQAry.txt");
			
			conComps.connectPass3();
			imageReformat(conComps, "connect8pass3.txt");
			
			printLabel(conComps);
			
			conComps.drawBoxes(conComps.ccProperty);
			imageReformat(conComps, "connect8DrawBox.txt");
		}
		scan.close();
	}
}