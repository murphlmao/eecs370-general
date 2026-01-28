/**
 * Project 1
 * Assembler code fragment for LC-2K
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//Every LC2K file will contain less than 1000 lines of assembly.
#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
static void checkForBlankLinesInCode(FILE *inFilePtr);
static inline int isNumber(char *);
static inline void printHexToFile(FILE *, int);
static int endsWith(char *, char *);

enum {
	OP_ADD  = 0,
	OP_NOR  = 1,
	OP_LW   = 2,
	OP_SW   = 3,
	OP_BEQ  = 4,
	OP_JALR = 5,
	OP_HALT = 6,
	OP_NOOP = 7,
	OP_FILL = -1
};

typedef struct {
	char label[MAXLINELENGTH];
	int address;
} Symbol;

static Symbol symbolTable[MAXLINELENGTH];
static int symbolCount = 0;

static int isAlpha(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static void addLabel(char* label, int address) {
	for (int i = 0; i < symbolCount; i++) {
		if (!strcmp(symbolTable[i].label, label)) {
			printf("duplicate label %s\n", label);
			exit(1);
		}
	}
	if (strlen(label) > 6) {
		printf("label too long: %s\n", label);
		exit(1);
	}
	if (!isAlpha(label[0])) {
		printf("bad label: %s\n", label);
		exit(1);
	}
	strcpy(symbolTable[symbolCount].label, label);
	symbolTable[symbolCount].address = address;
	symbolCount++;
}

static int lookupLabel(char* label) {
	for (int i = 0; i < symbolCount; i++) {
		if (!strcmp(symbolTable[i].label, label)) {
			return symbolTable[i].address;
		}
	}
	printf("undefined label: %s\n", label);
	exit(1);
}

static int getOpcode(char* opcode) {
	if (!strcmp(opcode, "add"))   return OP_ADD;
	if (!strcmp(opcode, "nor"))   return OP_NOR;
	if (!strcmp(opcode, "lw"))    return OP_LW;
	if (!strcmp(opcode, "sw"))    return OP_SW;
	if (!strcmp(opcode, "beq"))   return OP_BEQ;
	if (!strcmp(opcode, "jalr"))  return OP_JALR;
	if (!strcmp(opcode, "halt"))  return OP_HALT;
	if (!strcmp(opcode, "noop"))  return OP_NOOP;
	if (!strcmp(opcode, ".fill")) return OP_FILL;
	printf("invalid opcode: %s\n", opcode);
	exit(1);
}

static int parseReg(char* arg) {
	if (!isNumber(arg)) {
		printf("bad register: %s\n", arg);
		exit(1);
	}
	int reg = atoi(arg);
	if (reg < 0 || reg > 7) {
		printf("register out of bounds: %d\n", reg);
		exit(1);
	}
	return reg;
}

static int parseOffset(char* arg, int pc, int isBranch) {
	int offset;
	if (isNumber(arg)) {
		offset = atoi(arg);
	} else {
		int addr = lookupLabel(arg);
		offset = isBranch ? (addr - pc - 1) : addr;
	}
	if (offset < -32768 || offset > 32767) {
		printf("offset out of range: %d\n", offset);
		exit(1);
	}
	return offset & 0xFFFF;
}

static int assembleFill(char* arg) {
	return isNumber(arg) ? atoi(arg) : lookupLabel(arg);
}

static int assembleRType(int op, char* arg0, char* arg1, char* arg2) {
	int regA = parseReg(arg0);
	int regB = parseReg(arg1);
	int destReg = parseReg(arg2);
	return (op << 22) | (regA << 19) | (regB << 16) | destReg;
}

static int assembleIType(int op, char* arg0, char* arg1, char* arg2, int pc) {
	int regA = parseReg(arg0);
	int regB = parseReg(arg1);
	int offset = parseOffset(arg2, pc, op == OP_BEQ);
	return (op << 22) | (regA << 19) | (regB << 16) | offset;
}

static int assembleJType(int op, char* arg0, char* arg1) {
	int regA = parseReg(arg0);
	int regB = parseReg(arg1);
	return (op << 22) | (regA << 19) | (regB << 16);
}

static int assembleOType(int op) {
	return op << 22;
}

int main(int argc, char** argv) {
	char* inFileString;
	char* outFileString;
	FILE* inFilePtr;
	FILE* outFilePtr;
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
			arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

	if (argc != 3) {
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
			argv[0]);
		exit(1);
	}

	inFileString = argv[1];
	outFileString = argv[2];

	if (!endsWith(inFileString, ".as") &&
		!endsWith(inFileString, ".s") &&
		!endsWith(inFileString, ".lc2k")
	) {
		printf("warning: assembly code file does not end with .as, .s, or .lc2k\n");
	}

	if (!endsWith(outFileString, ".mc")) {
		printf("error: machine code file must end with .mc\n");
		exit(1);
	}

	inFilePtr = fopen(inFileString, "r");
	if (inFilePtr == NULL) {
		printf("error in opening %s\n", inFileString);
		exit(1);
	}

	checkForBlankLinesInCode(inFilePtr);

	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	// build symbol table
	int pc = 0;
	while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
		if (label[0] != '\0') {
			addLabel(label, pc);
		}
		pc++;
	}
	rewind(inFilePtr);

	// machine code generation
	pc = 0;
	while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
		int mc;
		int op = getOpcode(opcode);

		switch (op) {
		case OP_ADD:
		case OP_NOR:
			mc = assembleRType(op, arg0, arg1, arg2);
			break;
		case OP_LW:
		case OP_SW:
		case OP_BEQ:
			mc = assembleIType(op, arg0, arg1, arg2, pc);
			break;
		case OP_JALR:
			mc = assembleJType(op, arg0, arg1);
			break;
		case OP_HALT:
		case OP_NOOP:
			mc = assembleOType(op);
			break;
		case OP_FILL:
			mc = assembleFill(arg0);
			break;
		default:
			exit(1);
		}

		printHexToFile(outFilePtr, mc);
		pc++;
	}

	fclose(inFilePtr);
	fclose(outFilePtr);
	return 0;
}

// Returns non-zero if the line contains only whitespace.
static int lineIsBlank(char *line) {
	char whitespace[4] = {'\t', '\n', '\r', ' '};
	int nonempty_line = 0;
	for(int line_idx=0; line_idx < strlen(line); ++line_idx) {
		int line_char_is_whitespace = 0;
		for(int whitespace_idx = 0; whitespace_idx < 4; ++ whitespace_idx) {
			if(line[line_idx] == whitespace[whitespace_idx]) {
				line_char_is_whitespace = 1;
				break;
			}
		}
		if(!line_char_is_whitespace) {
			nonempty_line = 1;
			break;
		}
	}
	return !nonempty_line;
}

// Exits 2 if file contains an empty line anywhere other than at the end of the file.
// Note calling this function rewinds inFilePtr.
static void checkForBlankLinesInCode(FILE *inFilePtr) {
	char line[MAXLINELENGTH];
	int blank_line_encountered = 0;
	int address_of_blank_line = 0;
	rewind(inFilePtr);

	for(int address = 0; fgets(line, MAXLINELENGTH, inFilePtr) != NULL; ++address) {
		// Check for line too long
		if (strlen(line) >= MAXLINELENGTH-1) {
			printf("error: line too long\n");
			exit(1);
		}

		// Check for blank line.
		if(lineIsBlank(line)) {
			if(!blank_line_encountered) {
				blank_line_encountered = 1;
				address_of_blank_line = address;
			}
		} else {
			if(blank_line_encountered) {
				printf("Invalid Assembly: Empty line at address %d\n", address_of_blank_line);
				exit(2);
			}
		}
	}
	rewind(inFilePtr);
}


/*
* NOTE: The code defined below is not to be modifed as it is implimented correctly.
*/

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int
readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
	char *arg1, char *arg2)
{
	char line[MAXLINELENGTH];
	char *ptr = line;

	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
	/* reached end of file */
		return(0);
	}

	/* check for line too long */
	if (strlen(line) == MAXLINELENGTH-1) {
	printf("error: line too long\n");
	exit(1);
	}

	// Ignore blank lines at the end of the file.
	if(lineIsBlank(line)) {
		return 0;
	}

	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}

	/*
	 * Parse the rest of the line.  Would be nice to have real regular
	 * expressions, but scanf will suffice.
	 */
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
		opcode, arg0, arg1, arg2);

	return(1);
}

static inline int
isNumber(char *string)
{
	int num;
	char c;
	return((sscanf(string, "%d%c",&num, &c)) == 1);
}


// Prints a machine code word in the proper hex format to the file
static inline void 
printHexToFile(FILE *outFilePtr, int word) {
	fprintf(outFilePtr, "0x%08X\n", word);
}

// Returns 1 if string ends with substr, 0 otherwise
static int
endsWith(char *string, char *substr) {
	size_t stringLen = strlen(string);
	size_t substrLen = strlen(substr);
	if (stringLen < substrLen) {
		return 0; // string too short
	}
	char *stringEnd = string + stringLen - substrLen;
	if (strcmp(stringEnd, substr) == 0) {
		return 1;
	}
	return 0;
}
