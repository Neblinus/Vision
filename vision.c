// Vision Brainfuck-to-C transpiler

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Prints to STDERR and exits the program execution
void ReportExit(char *errorMessage, int errorCode)
{
  fprintf(stderr, "%s\n", errorMessage);
  exit(errorCode);
}

// Ensures command-line args are passed and minimally valid
void VerifyArguments(int *argumentCount, char **arguments)
{
  // Program name, input file path and memblock size as arguments
  if (*argumentCount != 3)
  {
    ReportExit("Expected a path to the Brainfuck source and the memblock size in bytes.", 1);
  }
  // Path must be at least as lengthy as a.bf, ending in .bf
  int pathLength = strlen(arguments[1]) - 1;
  if (!(pathLength >= 3 && arguments[1][pathLength - 2] == '.' &&
  arguments[1][pathLength - 1] == 'b' && arguments[1][pathLength] == 'f'))
  {
    ReportExit("Expected a path to the input source file ending in '.bf'.", 2);
  }
  // The memblock size must be all digits
  for (int i = 0; i < strlen(arguments[2]); ++i)
  {
    if (!isdigit(arguments[2][i]) && arguments[2][i] != '\0')
    {
      ReportExit("Expected the desired memblock size (in bytes) as second argument.", 3);
    }
  }
}

// Safely gets a file pointer and return it opened
FILE *GetSafeFilePointer(char *filePath, char *mode)
{
  FILE *targetFile = fopen(filePath, mode);
  if (targetFile == NULL)
  {
    ReportExit("Failed to create the requested file.", 4);
  }
  return targetFile;
}

// Safely sets up the output file and return its pointer
FILE *GetOutputFile(char *memblockSize)
{
  FILE *outputFile = GetSafeFilePointer("c-code-output.c", "w"); // Temporary
  if (fprintf(outputFile, "#include <stdio.h>\n#include <stdint.h>\ntypedef uint8_t byte;\n"
  "int main(void)\n{\n\tbyte memblock[%s] = {0};\n\tbyte *pointer = memblock;\n\t",
  memblockSize) < 0)
  {
    ReportExit("Failed to write the base code to the C output file.", 5);
  }
  return outputFile;
}

// Writes the C analog code to the passed input character
void WriteAnalogC(char inputToken, int inputCount, FILE *cSourceFile)
{
  char *codeOutput;
  int writeOutputBytes;
  switch (inputToken)
  {
    case '>': codeOutput = "pointer += "; break;
    case '<': codeOutput = "pointer -= "; break;
    case '+': codeOutput = "*pointer += "; break;
    case '-': codeOutput = "*pointer -= "; break;
    case '.': codeOutput = "printf(\"%c\", *pointer);"; break;
    case ',': codeOutput = "scanf(\"%d\", pointer);"; break;
    case '[': codeOutput = "while (*pointer != 0)\n\t{\n\t"; break;
    case ']': codeOutput = "\n\t}"; break;
  }
  if (strchr("><+-", inputToken) != NULL)
  {
    writeOutputBytes = fprintf(cSourceFile, "%s%d;\n\t", codeOutput, inputCount);
  }
  else
  {
    writeOutputBytes = fprintf(cSourceFile, "%s\n\t", codeOutput);
  }
  if (writeOutputBytes < 0)
  {
    ReportExit("Couldn't write the C code for this token on the output file.", 7);
  }
}

// Writes the last tokens to the C file and close both files
void ShutdownTranspiling(FILE *input, FILE *output)
{
  if (fprintf(output, "\n}") < 0)
  {
    ReportExit("Couldn't write the main() function end on the C source.", 8);
  }
  fclose(input);
  fclose(output);
}

// Parses the brainfuck source code translating it to C
void ParseBrainfuckSource(char *sourcePath, char *memblockSize)
{
  int currentToken;
  int tokenCount = 0;
  int previousToken = -5;
  FILE *bfSource = GetSafeFilePointer(sourcePath, "r");
  FILE *cSource = GetOutputFile(memblockSize);
  while ((currentToken = fgetc(bfSource)) != EOF)
  {
    if (ferror(bfSource))
    {
      ReportExit("An error occurred while parsing the Brainfuck source.", 6);
    }
    else if (strchr("><+-.,[]", currentToken) == NULL)
    {
      continue;
    }
    previousToken = (previousToken == -5) ? currentToken : previousToken;
    (currentToken == previousToken) ? (tokenCount += 1) : (WriteAnalogC(previousToken,
    tokenCount, cSource), tokenCount = 1, previousToken = currentToken);
  }
  WriteAnalogC(previousToken, tokenCount, cSource);
  ShutdownTranspiling(bfSource, cSource);
}

int main(int argc, char **argv)
{
  VerifyArguments(&argc, argv);
  ParseBrainfuckSource(argv[1], argv[2]);
}