#ifndef OUT_H
#define OUT_H

#define BBLACK "\033[1;30m"
#define UBLACK "\033[4;30m"
#define ONBLACK "\033[0;37m\033[40m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define LGREEN "\033[1;32m"
#define NC "\033[0m"

#define VARDEFINED 1
#define VARNOTDEFINED 2
#define DIVBYZERO 3

#define ALWAYSTRUE 4
#define ALWAYSFALSE 5

#define WHEREIF 6

void error(int error, int line_number, char* variable_name) {
  printf("%s", RED);
  printf("ERROR at line %d: ", line_number);
  printf("%s", NC);

  switch (error) {
    case VARDEFINED:
            printf("Variable '%s' already defined.\n", variable_name);
            break;
    case VARNOTDEFINED:
            printf("Variable '%s' not defined.\n", variable_name);
            break;
    case DIVBYZERO:
            printf("Division by zero.\n");
  }
}

void warning(int warning, int line_number, int where) {
  printf("%s", YELLOW);
  printf("WARNING at line %d: ", line_number);
  printf("%s", NC);

  switch (warning) {
    case ALWAYSTRUE:
      switch (where) {
        case WHEREIF:
          printf("The content of this if statement will always execute.\n");
          break;
      }
      break;
    case ALWAYSFALSE:
      switch (where) {
        case WHEREIF:
          printf("The content of this if statement will not execute.\n");
          break;
      }
      break;
  }
}

void error_message(char* message, int line_number, char* variable_name, char* after) {
  printf("%s", RED);
  if (line_number > 0)
    printf("ERROR at line %d: ", line_number);
  else
    printf("ERROR: ");
  printf("%s", NC);
  printf("%s", message);

  if (variable_name) {
    printf("'%s'", variable_name);
  }
  if (after) {
    printf("%s", after);
  }
  printf("\n");
}

void print_variable(char* name) {
  printf("%s", LGREEN);
  printf("%s", name);
  printf("%s\n", NC);
}

void print_assign_operator(char* name) {
  printf("%s", ONBLACK);
  printf("%s", name);
  printf("%s\n", NC);
}

void print_binary_operator(char* name) {
  printf("%s", BBLACK);
  printf("%s", name);
  printf("%s\n", NC);
}

void print_un_operator(char* name) {
  printf("%s", UBLACK);
  printf("%s", name);
  printf("%s\n", NC);
}

#endif
