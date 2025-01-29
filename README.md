# Instruções de uso

Para uso do software, siga os seguintes passos:

- **1°:** clone o repositório para o seu computador.

    - Ao abrir o projeto com o **VSCode**, a extensão do **CMake** irá criar a pasta ``build`` para você com os arquivos de compilação.

    - Caso não seja gerada a pasta, crie uma pasta com nome `build` e execute o seguinte comando dentro da pasta criada:
        
        ``cmake ..``

        O comando acima irá criar os arquivos de compilação.

- **2°:** execute a compilação do firmware usando a extensão do ***Raspberry Pi Pico*** do ***VSCode***.

A partir daqui, seu firmware já está compilado e pronto para uso, e só depende de onde será usado.

## Execução no ambiente de simulação Wokwi

Para utilizar o ***firmware*** no ambiente de simulação, basta ter a extensão do **Wokwi** instalada no seu ***VSCode***, além de ter configurado a sua licença.

Suprindo os requisitos citados acima, basta clicar no arquivo ``diagram.json``, e a simulação já abrirá com o esquemático pronto.

***Clique no botão de play, no canto superior esquerdo e simule!***

## Execução na *BitDogLab*

Para execução na placa de desenvolvimento ***BitDogLab***, é necessário possuir o teclado matricial mostrado abaixo.

![teclado matricial](/images/teclado.png)

Possuindo o teclado, conecte-o aos pinos de extensão da placa, de acordo com o diagrama do ***Wokwi*** presente no arquivo ``diagram.json`` e siga os passos a seguir:

- **1°:** coloque o seu ***Raspberry*** em modo de ***bootsel***, clicando no botão branco na placa e reiniciando a mesma.

- **2°:** copie o arquivo `.uf2` que foi gerado na pasta `build` para o seu ***Raspberry*** (ele aparecerá como um armazenamento externo, um Pen-Drive com nome de RPI-RP2).

    - Após isso, o seu código já vai está rodando na sua plaquinha ***BitDogLab***.

- **3°:** Está pronto, dê os comandos para executar as funcionalidades clicando nas teclas do seu teclado.


## Comandos disponívels:

`0`: mostra uma animação de um rosto feliz piscando na matriz de LEDs;

`1`: mostra uma animação simulando o carregamento de uma bateria na matriz de LEDs;

`2`: mostra a animação de um X na matriz de LEDs;

`3`: mostra a animação de uma cobra atravessando a matriz de LEDs;

`4`: mostra a animação de um timer de 1 a 9 na matriz de LEDs;

`5`:Letra 'e' da embarcatech aparece;

`6`:Simboliza ondas crescentes;

`9`: mostra a animação de uma cobra circulando a matriz LEDs, com um LED no meio;

`A`: desliga todos os LEDs da matriz;

`B`: liga todos os LEDs da matriz em 100% de intensidade na cor azul;

`C`: liga todos os LEDs da matriz na cor vermelha com 80% de intensidade;

`D`: liga todos os LEDs da matriz na cor verde com 50% de intensidade;

`#`: liga todos os LEDs da matriz na cor branca com 20% de intensidade;


## Vídeo Ensaio

Clique em ***[link do video]()*** para visualizar o vídeo ensaio do projeto.
