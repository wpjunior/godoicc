
all:
	gcc -g godoi.c -o godoi
tests:
	make
	./godoi tests/declaracoes_leia.txt
	./godoi tests/declaracoes_variaveis.txt

test01:
	make
	./godoi tests/repita.txt
test02:
	make
	./godoi tests/erro_parametros.txt
