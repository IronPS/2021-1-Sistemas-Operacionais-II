# Segunda Etapa

## Dependências
 - Linux
 - CMake 3.17+
 - C++11

## Compilação e instalação
Todos os comandos são executados a partir do diretório "Etapa1"
```
cd Etapa1
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
 4. "Instale" os binários no diretório ```Etapa1/bin/``` (na prática ele apenas copia os binários para a pasta)
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
