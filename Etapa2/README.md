# Segunda Etapa

## Dependências
 - Linux
 - CMake 3.17+
 - C++11

## Compilação e instalação
Todos os comandos são executados a partir do diretório "Etapa2"
```
cd Etapa2
```
 1. Crie o diretório onde o CMake configurará e compilará o projeto
```
mkdir build
cd build
```
 2. Execute o comando para configuração do CMake
```
cmake ..
```
 3. Compile o código (```-jn``` é a flag para usar n threads no processo de compilação)
```
cmake --build . -j4
```
 4. "Instale" os binários no diretório ```Etapa2/bin/``` (na prática ele apenas copia os binários para a pasta)
```
cmake --install .
```

## Teste
Os testes automáticos são executados a partir do diretório de compilação
```
cd build
ctest
```

Para executá-los em modo verbose, adiciona a flag ```--verbose```
```
ctest --verbose
```

## Execução
Para facilitar o processo de execução, um Makefile está disponível na pasta ```Etapa2/bin/``` com scripts para lançar os servidores e limpar os dados, se necessário. É necessário possuir o ```gnome-terminal``` instalado.

Para lançar os servidores, execute
```
cd bin
make launch1
```

Todas as aplicações possuem flags de ajuda (```--help``` ou ```-h```) que apresentam seus comandos.
