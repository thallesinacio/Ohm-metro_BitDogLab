# Ohmimetro BitDogLab

Este projeto tem como objetivo simular a interface gráfica de um ohmímetro em um display OLED SSD1306. A ideia foi explorar os recursos gráficos do display, desenvolver noções de design de interface em sistemas embarcados e montar uma base que pode futuramente evoluir para um instrumento funcional.

#📌 Objetivo


Criar a representação gráfica de um ohmímetro simples usando um display OLED.

Simular a interface de medição de resistência elétrica (valor simbólico).

Aprender e praticar a manipulação direta de pixels com a biblioteca do SSD1306.

Desenvolver uma interface intuitiva com aparência de produto real.


#🧰 O que foi feito


Título na parte superior: Exibição centralizada da palavra “OHMIMETRO”, com destaque.

Linha divisória: Uma linha horizontal foi desenhada abaixo do título para organizar visualmente o conteúdo.

Desenho de resistor: Um retângulo representando o corpo do resistor e três linhas verticais (faixas coloridas simbólicas) simulando as faixas de valor.

Valor da resistência: Texto com o valor “330 Ω” posicionado ao lado do resistor para indicar a leitura simulada.

Botão "MEDIR": Um retângulo com o texto “MEDIR” simulando um botão de ação na parte inferior da tela.


#🛠️ Como foi feito


Utilização da biblioteca do SSD1306 com funções para desenhar linhas, caixas e texto diretamente no buffer de pixels.

O projeto foi implementado de forma que cada elemento gráfico foi posicionado cuidadosamente para se adequar à resolução do display (128x64).

A comunicação com o display foi feita por meio da simulação no Wokwi, uma plataforma online para prototipagem de projetos embarcados.


#📋 Utilidade e aplicabilidade


Didática: Ótimo exemplo para iniciantes em eletrônica e programação embarcada entenderem como construir interfaces gráficas simples.

Base para expansão: Com pequenas modificações, é possível incluir medições reais de resistência usando o conversor analógico-digital da placa.

Prototipagem rápida: Ajuda a planejar visualmente como será um instrumento real, antes de adicionar componentes físicos ou lógicas complexas.
