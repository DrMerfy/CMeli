RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

cd ../build

printf "${BLUE}Blank ${NC}\n"
./compiler < ../tests/blank

printf "${BLUE}Blank Program ${NC}\n"
./compiler < ../tests/blank_program

printf "${BLUE}Simple Assigment ${NC}\n"
./compiler < ../tests/simple_assigment

printf "${BLUE}Calculation Assigment ${NC}\n"
./compiler < ../tests/calc_assigment

printf "${BLUE}Simple Print ${NC}\n"
./compiler < ../tests/simple_print

printf "${BLUE}Simple Control ${NC}\n"
./compiler < ../tests/simple_control

printf "${BLUE}Loops ${NC}\n"
./compiler < ../tests/loops

printf "${BLUE}Program test 1 ${NC}\n"
./compiler < ../tests/program1


printf "${BLUE}---->ERRORS<---- ${NC}\n"

printf "${BLUE}SIMPLE ASSIGMENT ERR ${NC}\n"
./compiler < ../tests/simple_assigment_err
