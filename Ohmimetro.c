/*Discente: Thalles Inácio Araújo
  Matrícula: tic370101531
  Data: 28/04/2025
  Tarfa 2 (Proposta na aula do dia 22/04)
*/

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "pico/stdlib.h"
 #include "hardware/adc.h"
 #include "hardware/i2c.h"
 #include "inc/ssd1306.h"
 #include "inc/font.h"
 #include "hardware/clocks.h"
 #include "hardware/pio.h"
 #include "pio_matrix.pio.h"

 #define I2C_PORT i2c1
 #define I2C_SDA 14
 #define I2C_SCL 15
 #define endereco 0x3C
 #define ADC_PIN 28 // GPIO para o voltímetro
 #define Botao_A 5  // GPIO para botão A
 
// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5

 // Matriz de led's
PIO pio;
uint sm;
uint32_t VALOR_LED;
unsigned char R, G, B;
#define NUM_PIXELS 25
#define OUT_PIN 7

char str_cor1[10], str_cor2[10], str_cor3[10];
int vetor[7];
int mult;
double r1 = 0, g1 = 0, b1 = 0;
double r2 = 0, g2 = 0, b2 = 0;
double r3 = 0, g3 = 0, b3 = 0;

 int R_conhecido = 510;   // Resistor de 10k ohm
 float R_x = 0.0;           // Resistor desconhecido
 float ADC_VREF = 3.31;     // Tensão de referência do ADC
 int ADC_RESOLUTION = 4095; // Resolução do ADC (12 bits)
 
 // Trecho para modo BOOTSEL com botão B
 #include "pico/bootrom.h"
 #define botaoB 6
 void gpio_irq_handler(uint gpio, uint32_t events)
 {
   reset_usb_boot(0, 0);
 }
 
// Vetor contendo todos os valores de resistência da série E24 no intervalo de 510 Ohms a 100k Ohms
 const int resistores_e24[] = {
  510, 560, 620, 680, 750, 820, 910,
  1000, 1100, 1200, 1300, 1500, 1600, 1800, 2000, 2200, 2400, 2700, 3000,
  3300, 3600, 3900, 4300, 4700, 5100, 5600, 6200, 6800, 7500, 8200, 9100,
  10000, 11000, 12000, 13000, 15000, 16000, 18000, 20000, 22000, 24000,
  27000, 30000, 33000, 36000, 39000, 43000, 47000, 51000, 56000, 62000,
  68000, 75000, 82000, 91000, 100000
};

// Aqui estão os dseenhos das colunas, efetivamente

double col_combinada[] = {
  0.1, 0.0, 0.1, 0.0, 0.1, 0.1, 0.0, 0.1, 0.0, 0.1,
  0.1, 0.0, 0.1, 0.0, 0.1, 0.1, 0.0, 0.1, 0.0, 0.1,
  0.1, 0.0, 0.1, 0.0, 0.1
};


//Neste trecho, adicionarei funções para manuseio da matriz de leds
uint32_t matrix_rgb(double b, double r, double g)
{
  R = r * 255;
  G = g * 255;
  B = b * 255;
  return (G << 24) | (R << 16) | (B << 8);
}

// Função para desenhar na matriz de leds. Perceba que ela está adaptada para usar uma combinação RGB diferente
// em cada coluna. Isto se deve ao fato que, ao longo da realização da tarefa, foi notado que a matriz é incapaz
// de sobrepor desenhos, mesmo que cada desenho use leds totalmente diferentes, como é o caso de usar 3 colunas diferentes.
// Dessa forma, foi necessário realizar essa alteração no método.
void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm){

  for (int16_t i = 0; i < NUM_PIXELS; i++) {
          if(i == 0 || i == 9 || i == 10 || i == 19 || i == 20) {
            valor_led = matrix_rgb(desenho[24-i] * b3, desenho[24-i] * r3, desenho[24-i] * g3); 
          } else if(i == 2 || i == 7 || i ==  12 || i == 17 || i ==  22) {
            valor_led = matrix_rgb(desenho[24-i] * b2, desenho[24-i] * r2, desenho[24-i] * g2);
          } else if (i == 4 || i == 5 || i ==  14 || i == 15 || i ==  24) {
            valor_led = matrix_rgb(desenho[24-i] * b1, desenho[24-i] * r1, desenho[24-i] * g1);
          } else {
            valor_led = matrix_rgb(0,0,0);
          }
          pio_sm_put_blocking(pio, sm, valor_led);
  }
}
// ------------------------------------------------------------------------------------------------------------------------
// Nesta função, pego o valor bruto da média da resistência e arredondo para o mais conhecido na série E24
int arredonda(int x) {
  int diferenca, i, idx = 0;
  diferenca = abs(x - resistores_e24[0]);

    for(i = 1; i < 55; i++) {
      if(abs(x - resistores_e24[i]) < diferenca){
        diferenca = abs(x - resistores_e24[i]);
        idx = i;
      }
    }
    return resistores_e24[idx];
}

// Aqui, estou colocando o valor já arredondado da resistência dentro d um vetor,
// visando facilitar a manipulação de dados.
void colocarNumeroNoVetor(int numero) {

  int aux = numero;

  // Inicializa o vetor com zeros
  for (int i = 0; i < 7; i++) {
      vetor[i] = 0;
  }

  // Primeiro, descobrir quantos dígitos o número tem
  int temp = numero;
  int digitos = 0;
  if (temp == 0) {
      digitos = 1;
  } else {
      while (temp > 0) {
          temp /= 10;
          digitos++;
      }
  }

  int divisor = 1;
  for (int i = 1; i < digitos; i++) {
      divisor *= 10;
  }

  int i = 0;
  while (divisor > 0 && i < 7) {
      vetor[i] = numero / divisor;
      numero %= divisor;
      divisor /= 10;
      i++;
  }

  // Verificando fator multiplicativo

  if (aux >= 100000) {
    mult = 4;
  } 
  if (aux >= 10000 && aux < 100000) {
      mult = 3;
  }
  else if (aux >= 1000 && aux < 10000) {
      mult = 2;
  }
  else if (aux < 1000){
      mult = 1;
  }
}

void cores1(int x) {
  // Definição das cores para a primeira coluna
  switch(x) {
      case 0: r1 = 0; g1 = 0; b1 = 0; strcpy(str_cor1, "black"); break;
      case 1: r1 = 0.6; g1 = 0.3; b1 = 0; strcpy(str_cor1, "brown"); break;
      case 2: r1 = 1; g1 = 0; b1 = 0; strcpy(str_cor1, "red"); break;
      case 3: r1 = 1; g1 = 0.5; b1 = 0; strcpy(str_cor1, "orange"); break;
      case 4: r1 = 1; g1 = 1; b1 = 0; strcpy(str_cor1, "yellow"); break;
      case 5: r1 = 0; g1 = 1; b1 = 0; strcpy(str_cor1, "green"); break;
      case 6: r1 = 0; g1 = 0; b1 = 1; strcpy(str_cor1, "blue"); break;
      case 7: r1 = 0.5; g1 = 0; b1 = 0.5; strcpy(str_cor1, "violet"); break;
      case 8: r1 = 0.5; g1 = 0.5; b1 = 0.5; strcpy(str_cor1, "gray"); break;
      case 9: r1 = 1; g1 = 1; b1 = 1; strcpy(str_cor1, "white"); break;
      default: strcpy(str_cor1, ""); break;
  }
}

void cores2(int x) {
  // Definição das cores para a segunda coluna
  switch(x) {
      case 0: r2 = 0; g2 = 0; b2 = 0; strcpy(str_cor2, "black"); break;
      case 1: r2 = 0.6; g2 = 0.3; b2 = 0; strcpy(str_cor2, "brown"); break;
      case 2: r2 = 1; g2 = 0; b2 = 0; strcpy(str_cor2, "red"); break;
      case 3: r2 = 1; g2 = 0.5; b2 = 0; strcpy(str_cor2, "orange"); break;
      case 4: r2 = 1; g2 = 1; b2 = 0; strcpy(str_cor2, "yellow"); break;
      case 5: r2 = 0; g2 = 1; b2 = 0; strcpy(str_cor2, "green"); break;
      case 6: r2 = 0; g2 = 0; b2 = 1; strcpy(str_cor2, "blue"); break;
      case 7: r2 = 0.5; g2 = 0; b2 = 0.5; strcpy(str_cor2, "violet"); break;
      case 8: r2 = 0.5; g2 = 0.5; b2 = 0.5; strcpy(str_cor2, "gray"); break;
      case 9: r2 = 1; g2 = 1; b2 = 1; strcpy(str_cor2, "white"); break;
      default: strcpy(str_cor2, ""); break;
  }
}

void cores3(int x) {
  // Definição das cores para a terceira coluna
  switch(x) {
      case 1: r3 = 0.6; g3 = 0.3; b3 = 0; strcpy(str_cor3, "brown"); break;
      case 2: r3 = 1; g3 = 0; b3 = 0; strcpy(str_cor3, "red"); break;
      case 3: r3 = 1; g3 = 0.5; b3 = 0; strcpy(str_cor3, "orange"); break;
      case 4: r3 = 1; g3 = 1; b3 = 0; strcpy(str_cor3, "yellow"); break;
      default: strcpy(str_cor3, ""); break;
  }
}



 int main()
 {
  stdio_init_all();

  uart_init(UART_ID, BAUD_RATE);

  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

   // Para ser utilizado o modo BOOTSEL com botão B
   gpio_init(botaoB);
   gpio_set_dir(botaoB, GPIO_IN);
   gpio_pull_up(botaoB);
   gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
   // Aqui termina o trecho para modo BOOTSEL com botão B
   
   int val;

   // Setando matriz de led's
   double r = 0.0, b = 0.0 , g = 0.0;
   bool ok;
   ok = set_sys_clock_khz(128000, false);
   pio = pio0;

   uint offset = pio_add_program(pio, &Ohmimetro_program);
   uint sm = pio_claim_unused_sm(pio, true);
   Ohmimetro_program_init(pio, sm, offset, OUT_PIN);
 
   gpio_init(Botao_A);
   gpio_set_dir(Botao_A, GPIO_IN);
   gpio_pull_up(Botao_A);
 
   // I2C Initialisation. Using it at 400Khz.
   i2c_init(I2C_PORT, 400 * 1000);
 
   gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
   gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
   gpio_pull_up(I2C_SDA);                                        // Pull up the data line
   gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
   ssd1306_t ssd;                                                // Inicializa a estrutura do display
   ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
   ssd1306_config(&ssd);                                         // Configura o display
   ssd1306_send_data(&ssd);                                      // Envia os dados para o display
 
   // Limpa o display. O display inicia com todos os pixels apagados.
   ssd1306_fill(&ssd, false);
   ssd1306_send_data(&ssd);
 
   adc_init();
   adc_gpio_init(ADC_PIN); // GPIO 28 como entrada analógica

   float tensao;
   char str_x[5]; // Buffer para armazenar a string
   char str_y[5]; // Buffer para armazenar a string
   char show[5];

   bool cor = true;
   while (true)
   {  
    
     
    
    adc_select_input(2); // Seleciona o ADC para eixo X. O pino 28 como entrada analógica
 
     float soma = 0.0f;
     for (int i = 0; i < 500; i++)
     {
       soma += adc_read();
       sleep_ms(1);
     }
     float media = soma / 500.0f;
 
       // Fórmula simplificada: R_x = R_conhecido * ADC_encontrado /(ADC_RESOLUTION - adc_encontrado)
       R_x = (R_conhecido * media) / (ADC_RESOLUTION - media);
     
    // Esteira de tratamento de dados

    val = arredonda(R_x);

    colocarNumeroNoVetor(val);

    cores1(vetor[0]);
    cores2(vetor[1]);
    cores3(mult);

    // Exibinbdo codigo de cores na matriz de leds
    desenho_pio(col_combinada, VALOR_LED, pio, sm);
    
    sprintf(show, "%d", val);
     
    desenha_molde_completo(&ssd);

    //desenho do resistor
    ssd1306_line(&ssd, 8, 53, 45, 53, cor);             // Desenha uma linha horinzotal para formar o resistor
    ssd1306_line(&ssd, 83, 53, 119, 53, cor);           // Desenha uma linha horinzotal para formar o resistor
    ssd1306_rect(&ssd, 47, 45, 38, 14, cor, !cor);      // Desenha um retângulo para formar o resistor
    ssd1306_rect(&ssd, 47, 50, 5, 14, cor, cor);      // Desenha um retângulo para formar o resistor
    ssd1306_rect(&ssd, 47, 60, 5, 14, cor, cor);      // Desenha um retângulo para formar o resistor
    ssd1306_rect(&ssd, 47, 70, 5, 14, cor, cor);      // Desenha um retângulo para formar o resistor
    
    
    ssd1306_draw_string_escala(&ssd, str_cor1, 6, 18, 0.8); // Extremo esquerdo: Cor 1
    ssd1306_draw_string_escala(&ssd, str_cor2, 45, 18, 0.8); // Centro: Cor 2
    ssd1306_draw_string_escala(&ssd, str_cor3, 85, 18, 0.8); // Extremo direito: Cor 3 (Fator multiplicativo)
    ssd1306_draw_string(&ssd, show, 50, 32);
    
    ssd1306_send_data(&ssd); 

    sleep_ms(700);
   }
 }