#include <stdio.h>
#include <stdlib.h>

#define STACK_SIZE 512
#define DATA_SIZE 30000

#define INC '>'
#define DEC '<'
#define INC_B '+'
#define DEC_B '-'
#define OUT '.'
#define IN ','
#define JMP_F '['
#define JMP_B ']'

int interpret(char *input) {
	unsigned int data[DATA_SIZE] = { 0 }; /* Program data */
	unsigned int ip = 0; /* Input pointer */
	unsigned int dp = 0; /* Data pointer */
	unsigned int stack[STACK_SIZE] = { 0 }; /* Stack for matching brackets */
	unsigned int jump[DATA_SIZE] = { 0 }; /* Hold jump positions */
	unsigned int sp = 0; /* Stack pointer */

	/* Precompute jumps */
	while (input[ip] != '\0') {
		switch (input[ip]) {
			case JMP_F:
				stack[sp++] = ip; /* Push position to stack */
				break;
			case JMP_B:
				if (sp == 0) {
					/* Cannot start program with a closed bracket */
					printf("Error: Unmatched ']' at character %d.", ip);
					exit(1);
				} else {
					jump[ip] = stack[--sp]; /* Add jump position and pop stack */
					jump[jump[ip]] = ip; /* Add corresponding jump */
				}
			break;
		}
		ip++;
	}
	if (sp > 0) {
		/* If the stack pointer is not zero there are unmatched brackets */
		printf("Error: Unmatched '[' at character %d.", stack[--sp]);
		exit(1);
	}

	/* Execute program */
	ip = 0;
	while (input[ip] != '\0') {
		switch (input[ip]) {
			case INC: ++dp; break; /* Increment data pointer */
			case DEC: --dp; break; /* Decrement data pointer */
			case INC_B: ++data[dp]; break; /* Increment byte at data pointer */
			case DEC_B: --data[dp]; break; /* Decrement byte at data pointer */
			case OUT: putchar(data[dp]); break; /* Output byte at data pointer */
			case IN: data[dp] = getchar(); break; /* Input to byte at data pointer */
			case JMP_F:
				/* If byte at pointer is zero, jump forward */
				if (!data[dp]) {
					ip = jump[ip];
				}
				break;
			case JMP_B:
				/* If byte at pointer is non-zero, jump backward */
				if (data[dp]) {
					ip = jump[ip];
				}
				break;
			default: break;
		}
		ip++;
	}
	return 0;
}

int main(int argc, char **argv) {
	/* Check arguments */
	if (argc != 2) {
		printf("Usage: bf filename");
		return 0;
	}
	
	/* Read file from argument */
	char *buffer = NULL;
	size_t size = 0;
	FILE *fp = fopen(argv[1], "r");
	/* Get buffer size */
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	rewind(fp); /* Set stream to beginning */
	buffer = malloc((size + 1) * sizeof(*buffer)); /* Allocate buffer */
	fread(buffer, size, 1, fp); /* Read the file into the buffer */
	buffer[size] = '\0'; /* Null terminate the buffer */
	fclose(fp); /* Close the file */
	
	/* Interpret the file */
	if (interpret(buffer) == 1) {
		printf("Error: Critical failure.");
		exit(1);
	}
	free(buffer); /* Free buffer */
	
	return 0;
}
