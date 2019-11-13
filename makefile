CC = gcc

SRC = csv.c
CC_FLAGS = -Wall -Wextra
IMM_OBJ_NAME = bin/csv.o
LIB_NAME = bin/csv.a

TEST_SRC = test.c csv.c
TEST_OBJ_NAME = testing.out 
TEST_CC_FLAGS = -ggdb3 -O0 -Wall -Wextra

test :
	$(CC) $(TEST_SRC) $(TEST_CC_FLAGS) -o $(TEST_OBJ_NAME)

lib :
	$(CC) -c $(SRC) $(CC_FLAGS) -o $(IMM_OBJ_NAME)
	ar rcs $(LIB_NAME) $(IMM_OBJ_NAME)
	rm $(IMM_OBJ_NAME)
