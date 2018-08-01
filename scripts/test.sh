RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

cd ../build

printf "${BLUE}Blank ${NC}\n"
./a.out < ../tests/blank

printf "${BLUE}Blank Program ${NC}\n"
./a.out < ../tests/blank_program

printf "${BLUE}Simple Assigment ${NC}\n"
./a.out < ../tests/calc_assigment

printf "${BLUE}Calculation Assigment ${NC}\n"
./a.out < ../tests/simple_assigment

printf "${BLUE}Simple Print ${NC}\n"
./a.out < ../tests/simple_print

printf "${BLUE}Simple Control ${NC}\n"
./a.out < ../tests/simple_control

# printf "${BLUE}Number tests ${NC}\n"
# ./a.out < ./tests/number-test
#
# printf "\n"
# printf "${BLUE}Letter tests ${NC}\n"
# ./a.out < ./tests/letter-test
#
# printf "\n"
# printf "${BLUE}Program test 1 ${NC}\n"
# ./a.out < ./tests/program1
