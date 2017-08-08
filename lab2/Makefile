all: th_alloc test1 test2 test3 test4 test5 test6 test7 test8 test9 test10 test11

th_alloc: th_alloc.c
	gcc -fPIC -pedantic -Wall -Wextra -g -shared th_alloc.c -o th_alloc.so

test1: tests/test01.c
	gcc -pedantic -Wall -Wextra -g tests/test01.c -o tests_exe/test01 -lm
test2: tests/test02.c
	gcc -pedantic -Wall -Wextra -g tests/test02.c -o tests_exe/test02 -lm
test3: tests/test03.c
	gcc -pedantic -Wall -Wextra -g tests/test03.c -o tests_exe/test03 -lm
test4: tests/test04.c
	gcc -pedantic -Wall -Wextra -g tests/test04.c -o tests_exe/test04 -lm
test5: tests/test05.c
	gcc -pedantic -Wall -Wextra -g tests/test05.c -o tests_exe/test05 -lm
test6: tests/test06.c
	gcc -pedantic -Wall -Wextra -g tests/test06.c -o tests_exe/test06 -lm
test7: tests/test07.c
	gcc -pedantic -Wall -Wextra -g tests/test07.c -o tests_exe/test07 -lm
test8: tests/test08.c
	gcc -pedantic -Wall -Wextra -g tests/test08.c -o tests_exe/test08 -lm
test9: tests/test09.c
	gcc -pedantic -Wall -Wextra -g tests/test09.c -o tests_exe/test09 -lm
test10: tests/test10.c
	gcc -pedantic -Wall -Wextra -g tests/test10.c -o tests_exe/test10 -lm
test11: tests/test11.c
	gcc -pedantic -Wall -Wextra -g tests/test11.c -o tests_exe/test11 -lm

clean:
	rm test th_alloc.so

