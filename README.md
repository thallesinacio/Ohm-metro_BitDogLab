# Ohmimetro BitDogLab

Este projeto foi desenvolvido como parte de uma atividade prática para representar a interface gráfica de um ohmímetro, utilizando um display OLED modelo SSD1306.

📋 Objetivo

O principal objetivo foi aplicar os conhecimentos em sistemas embarcados para criar uma interface visual simples, mas funcional, que simula a leitura de um resistor. A proposta inclui elementos gráficos e textuais que poderiam estar presentes em um dispositivo real.

🛠️ O que foi feito

Inicialização e configuração do display OLED via protocolo I2C.

Criação de funções para desenhar formas básicas como linhas, quadrados e retângulos.

Implementação de escrita de texto em dois modos: normal e em escala ampliada.

Desenho de uma moldura e elementos gráficos simulando um resistor eletrônico.

Exibição de informações como o nome "OHMIMETRO", o valor de resistência e um botão de "MEDIR".

🔍 Como funciona

O programa inicializa o display e preenche a tela com um layout pré-definido:

Um cabeçalho com o nome do projeto.

Uma linha decorativa separando o cabeçalho do corpo.

Texto indicando o valor de resistência simulada.

Um desenho representando um resistor com três faixas.

Um rodapé com o botão "MEDIR".

Todos os elementos são desenhados pixel a pixel usando uma biblioteca própria com base na manipulação do buffer interno do display.

📦 Utilidade

Apesar de ser uma simulação, esse projeto ajuda a entender:

Como escrever e posicionar textos e formas em um display gráfico.

Como organizar uma interface simples e limpa em um dispositivo com espaço limitado.

Como estruturar o código para facilitar atualizações e futuras expansões, como leitura real de sensores.
