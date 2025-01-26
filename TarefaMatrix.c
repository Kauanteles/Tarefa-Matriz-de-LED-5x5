#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_matrix.pio.h"

// Definições
#define NUM_PIXELS 25
#define OUT_PIN 7
#define LINHA_QNTD 4
#define COLUNA_QNTD 4
#define FPS 10 // Frames por segundo (100 ms por frame)

// Mapas de GPIOs para teclado
const uint gpioCol[COLUNA_QNTD] = {4, 3, 2, 1};
const uint gpioLinha[LINHA_QNTD] = {10, 9, 8, 5};

// Mapeamento das teclas do teclado
const char teclado[LINHA_QNTD][COLUNA_QNTD] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

// Funções auxiliares
uint32_t rgb_color(double r, double g, double b) {
    unsigned char R = r * 255;
    unsigned char G = g * 255;
    unsigned char B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Inicialização do GPIO
void init_gpio() {
    for (int i = 0; i < LINHA_QNTD; i++) {
        gpio_init(gpioLinha[i]);
        gpio_set_dir(gpioLinha[i], GPIO_OUT);
        gpio_put(gpioLinha[i], 1);
    }
    for (int i = 0; i < COLUNA_QNTD; i++) {
        gpio_init(gpioCol[i]);
        gpio_set_dir(gpioCol[i], GPIO_IN);
        gpio_pull_up(gpioCol[i]);
    }
}

// Escaneia o teclado
char escanear_teclado() {
    for (int row = 0; row < LINHA_QNTD; row++) {
        gpio_put(gpioLinha[row], 0);
        for (int col = 0; col < COLUNA_QNTD; col++) {
            if (!gpio_get(gpioCol[col])) {
                gpio_put(gpioLinha[row], 1);
                return teclado[row][col];
            }
        }
        gpio_put(gpioLinha[row], 1);
    }
    return 0; // Nenhuma tecla pressionada
}


// Função alterar a cor do vermelho para o verde
uint32_t cor_transicao(int frame, int max_frames) {
    // Valor do vermelho começa em 1 (máximo) e vai diminuindo
    double r = 1.0 - (double)frame / max_frames; 
    // Valor do verde começa em 0 e vai aumentando
    double g = (double)frame / max_frames; 
    // Azul permanece em 0
    double b = 0.0; 

    // Retorna a cor com a interpolação de vermelho para verde
    return rgb_color(r, g, b);
}

// Função de animação com transição de vermelho para verde
void animacao_1(PIO pio, uint sm) {
    double frames[5][25] = {
        {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    int max_frames = 5; // Número de frames na animação

    for (int frame = 0; frame < max_frames; frame++) {
        for (int i = 0; i < NUM_PIXELS; i++) {
            // Se o valor for 1 (LED aceso), atribuímos a cor com transição
            if (frames[frame][i] == 1) {
                uint32_t color = cor_transicao(frame, max_frames);  // Cor com transição
                pio_sm_put_blocking(pio, sm, color); // Mostra o LED com a cor
            } else {
                // Se for 0 (LED apagado), atribuímos a cor preta
                uint32_t color = rgb_color(0, 0, 0); // Cor preta (apagado)
                pio_sm_put_blocking(pio, sm, color); // Desliga o LED
            }
        }
        sleep_ms(1000 / FPS);  // Delay entre os frames para controlar a animação
    }
}
// Função para desligar todos os LEDs

void desligar_leds(PIO pio, uint sm) {
    // Todos os LEDs desligados (cor preta)
    for (int i = 0; i < NUM_PIXELS; i++) {
        uint32_t color = rgb_color(0, 0, 0); // Cor preta (apagado)
        pio_sm_put_blocking(pio, sm, color); // Envia a cor preta para todos os LEDs
    }
}

// Função principal
int main() {
    stdio_init_all();

    // Inicializa PIO e configura
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    // Inicializa teclado
    init_gpio();

    printf("Sistema iniciado.\n");

    while (true) {
        char tecla = escanear_teclado();
        switch (tecla)
        {
        case '1':
            animacao_1(pio, sm); // Simboliza o carregamento de uma bateria
            
            break;
        case 'A':
            desligar_leds(pio, sm);
        default:
            break;
        }
        sleep_ms(100); // Debounce
    }
}