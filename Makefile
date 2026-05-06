CPP := clang++
CPP_FLAGS := -std=c++20 -Wall -Wextra

SOURCE := bst.cpp
EXE := bst

bst: bst.cpp
	$(CPP) $(CPP_FLAGS) $(SOURCE) -o $(EXE)

clean:
	rm -f $(EXE)
