#ifndef ERRORS_H
#define ERRORS_H

#define RED "\033[0;31m"
#define NC "\033[0m"

#define VARDEFINED 1
#define VARNOTDEFINED 2


void error(int error, int line_number, char* variable_name) {
  printf("%s", RED);
  printf("ERROR on line %d: ", line_number);

  switch (error) {
    case VARDEFINED:
            printf("Variable %s already defined.\n", variable_name);
            break;
    case VARNOTDEFINED:
            printf("Variable %s not defined.\n", variable_name);
            break;
  }

  printf("%s", NC);
}

#endif
