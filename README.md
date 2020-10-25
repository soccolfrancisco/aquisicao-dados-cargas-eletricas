# Aquisição de Dados através de Cargas Elétricas 

Software desenvolvido na minha Graduação de Engenharia de Computação na Universidade Federal do Espírito Santo - UFES com objetivo de estudar as cargas elétricas de um grupo de lâmpadas de mesmo fabricante e idênticas.
## Pasta Amostras
 
  Nesta pasta encontra-se o *Banco de Dados* utilizado para a validação do processo de aquisição de dados. As amostras estão organizadas em pastas numeradas de 01 a 12 e cada pasta contém 16 arquivos de aquisição de dados.

### Nome do Arquivo

  O nome do arquivo é importante, nele esta indicando qual o momento da aquisição e qual foi a configuração naquele momento. O primeiro número (da esquerda para a direita) indica o número do momento e os quatro últimos números índica a configuração das lâmpadas naquele momento (0 para indicação de lâmpada desligada e 1 para indicação de lâmpada ligada).

*Exemplo:* 

>           1              0           0           0           1              .txt
>     Nº do momento     Lâmp. 1     Lâmp. 2     lâmp. 3     lâmp. 4      nome extensão

#### Os Dados

  Dentro de cada arquivo de texto encontra-se duas colunas de dados. A coluna da direita representa a coluna das aquisições de Tensão Elétrica e a coluna da esquerda representa a coluna das aquisições de Corrente Elétrica. Esses dados são originais adquiridas com o microcontrolador.

## Pasta Software

Nesta pasta encontra-se o programa para a aquisição de dados. O programa foi desenvolvido em *Linguagem C* utilizando a IDE Arduino.

#### Objetivo

Faz a aquisição de dados da plataforma automatizavel.

#### Como Funciona?

Verifique se o *Software Arduino* está instalado em seu computador, se não estiver basta fazer o download e instalar utilizando o  [*Software Arduino*](www.arduino.cc/en/Main/Software).

O funcionamento do programa pode ser vista no artigo com o link logo abaixo.

No programa basta fazer as configurações iniciais, transferir o código para o microcontrolador Arduino DUE e iniciar a aquisição de dados.  

- Observação: O código está comentado para facilitar o entendimento.

- Importante: O artigo do trabalho desenvolvido pode ser acessado através do link ...

## Pasta Separa Dados

Nesta pasta encontra-se o programa para geração dos arquivos de Tensão Elétrica e Corrente Elétrica para utilização no programa de geração de curvas. O programa foi desenvolvido em *Linguagem C* utilizando a IDE CodeBlock. 

#### Objetivo

Separa o arquivo de aquisição contendo as duas colunas de dados (Corrente Elétrica e Tensão Elétrica) em dois arquivos separadamente e faz o cálculo dos dados para a Tensão e Corrente Elétrica reais.

#### Como Funciona?

Verifique se o *Software CodeBlock* está instalado em seu computador, se não estiver basta fazer o download e instalar utilizando o [*Software CodeBlock*](www.codeblocks.org/downloads).

Após a instalação do *Software CodeBlock*, abre o arquivo `arquivo.c` e configure as variáveis `file`, `fileCor` e `fileTen` para abriremos os arquivos de cada configuração. 

Ao final da execução do programa, haverá dois tipos de arquivos com os dados específicos de Tensão e Corrente Elétrica, o nome do arquivo terá uma letra para identificação de quais dados se refere (C → Corrente Elétrica , T → Tensão Elétrica).

*Exemplo:*
>     C11001.txt
>     T10010.txt


## Pasta Gera Gráfico Matlab

Nesta pasta encontra-se o programa para geração das curvas de Tensão e Corrente Elétricas. O arquivo foi desenvolvido utilizando a *Linguagem MatLab*, portanto, precisa-se do *Software MatLab* para execução. 

#### Objetivo

Gera gráficos de Tensão e Corrente Elétrica para estudos.


#### Como Funciona?

Primeiramente verifique se o *Software MatLab* está instalado em seu computador, se não estiver basta fazer o download e instalar utilizando o [*Software MatLab*](www.mathworks.com/downloads).

Após a instalação do *Software MatLab*, abra o arquivo `graficos.m` e configure as variáveis `arqTesao` e `arqCorrente` para abrirem cada arquivo escolhido.

- Observação: O programa funciona apenas se estiver com os dois tipos de arquivos (arquivo de Tensão Elétrica e Corrente Elétrica).   

- Importante: Sempre que for gerar os gráfico de curvas, utilize os dois arquivos da mesma configuração.




  
