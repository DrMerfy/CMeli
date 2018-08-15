#ifndef OUT_H
#define OUT_H

#define RED "\033[0;31m"
#define LGREEN "\033[1;32m"
#define NC "\033[0m"

#define VARDEFINED 1
#define VARNOTDEFINED 2

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

#endif
