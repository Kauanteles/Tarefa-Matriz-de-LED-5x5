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

typedef struct {
    double frame[NUM_PIXELS];
    double color[3];
    uint32_t ms_time;
} scene;

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

// Animação de um "X" acendendo em uma matriz de LEDs 5x5
void animacao_2(PIO pio, uint sm) {
    // Frames do "X" acendendo
    double x_pattern[5][25] = {
        {1, 0, 0, 0, 1,  // Primeiro frame
        0, 1, 0, 1, 0,
        0, 0, 1, 0, 0,
        0, 1, 0, 1, 0,
        1, 0, 0, 0, 1},

        {0, 0, 0, 0, 0,  // Segundo frame
        0, 1, 0, 1, 0,
        0, 0, 1, 0, 0,
        0, 1, 0, 1, 0,
        0, 0, 0, 0, 0},

        {0, 0, 0, 0, 0,  // Terceiro frame
        0, 0, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0},

        {0, 0, 0, 0, 0,  // Quarto frame
        0, 0, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0},

        {0, 0, 0, 0, 0,  // Quinto frame
        0, 0, 0, 0, 0,
        0, 1, 0, 1, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 0, 0}
    };

    int max_frames = 5; // Número de frames na animação

    for (int frame = 0; frame < max_frames; frame++) {
        for (int i = 0; i < NUM_PIXELS; i++) {
            // Se o valor for 1 (LED aceso), atribuímos a cor com transição
            if (x_pattern[frame][i] == 1) {
                uint32_t color = cor_transicao(frame, max_frames); // Cor com transição
                pio_sm_put_blocking(pio, sm, color); // Mostra o LED com a cor
            } else {
                // Se for 0 (LED apagado), atribuímos a cor preta
                uint32_t color = rgb_color(0, 0, 0); // Cor preta (apagado)
                pio_sm_put_blocking(pio, sm, color); // Desliga o LED
            }
        }
        sleep_ms(1000 / FPS); // Delay entre os frames para controlar a animação
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

void animacao_0(PIO pio, uint sm){
    int max_frames = 7;

    scene animation [] = {
        {
            {
                0, 0, 0, 0, 0,
                0, 1, 1, 1, 0,
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0,
                0, 1, 0, 1, 0
            },
            {0,0,0.5},
            500
        },{
            {
                0, 0, 0, 0, 0,
                0, 1, 1, 1, 0,
                1, 0, 0, 0, 1,
                0, 0, 0, 0, 0,
                0, 1, 0, 1, 0
            },
            {0,0,0.5},
            500
        },{
            {
                0, 0, 0, 0, 0,
                0, 1, 1, 1, 0,
                1, 0, 0, 0, 1,
                0, 0, 0, 0, 0,
                0, 0, 0, 1, 0
            },
            {0,0,0.5},
            200
        },{
            {
                0, 0, 0, 0, 0,
                0, 1, 1, 1, 0,
                1, 0, 0, 0, 1,
                0, 0, 0, 0, 0,
                0, 1, 0, 1, 0
            },
            {0,0,0.5},
            500
        },{
            {
                0, 0, 0, 0, 0,
                0, 1, 1, 1, 0,
                1, 0, 0, 0, 1,
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0
            },
            {0,0,0.5},
            200
        },{
            {
                0, 0, 0, 0, 0,
                0, 1, 1, 1, 0,
                1, 0, 0, 0, 1,
                0, 0, 0, 0, 0,
                0, 1, 0, 1, 0
            },
            {0,0,0.5},
            200
        },{
            {
                0, 1, 1, 1, 0,
                1, 0, 0, 0, 1,
                1, 1, 1, 1, 1,
                0, 0, 0, 0, 0,
                0, 1, 0, 1, 0
            },
            {0,0,0.5},
            1000
        }
    };

    for (int frame = 0; frame < max_frames; frame++) {
        for (int i = 0; i < NUM_PIXELS; i++) {
            if (animation[frame].frame[i]) {
                uint32_t color = rgb_color(animation[frame].color[0], animation[frame].color[1], animation[frame].color[2]);
                pio_sm_put_blocking(pio, sm, color);
            } else {
                uint32_t color = rgb_color(0, 0, 0);
                pio_sm_put_blocking(pio, sm, color);
            }
        }
        sleep_ms(animation[frame].ms_time);
    }
    sleep_ms(100);
    desligar_leds(pio, sm);
}

// Função para ligar todos os LEDs na cor azul
void ligar_azul(PIO pio, uint sm) {
    // Todos os LEDs acesos com cor azul em intensidade máxima
    for (int i = 0; i < NUM_PIXELS; i++) {
        uint32_t color = rgb_color(0, 0, 1.0); // Azul com intensidade máxima
        pio_sm_put_blocking(pio, sm, color);  // Envia a cor azul para todos os LEDs
    }
}

// Função para ligar todos os LEDs na cor vermelha com 80% de intensidade
void ligar_vermelho(PIO pio, uint sm) {
    // Todos os LEDs acesos com cor vermelha em 80% de intensidade
    for (int i = 0; i < NUM_PIXELS; i++) {
        uint32_t color = rgb_color(0.8, 0, 0); // Vermelho com 80% de intensidade
        pio_sm_put_blocking(pio, sm, color);  // Envia a cor vermelha para todos os LEDs
    }
}

void animacao_cobra(PIO pio, uint sm) {
    // Frames da cobra atravessando a matriz
    double cobra_frames[20][25] = {
        {1, 0, 0, 0, 0,  // Primeiro frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {1, 1, 0, 0, 0,  // Segundo frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {1, 1, 1, 0, 0,  // Terceiro frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {0, 1, 1, 1, 0,  // Quarto frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {0, 0, 1, 1, 1,  // Quinto frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {0, 0, 0, 1, 1,  // Sexto frame
         1, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {0, 0, 0, 0, 1,  // Sétimo frame
         1, 0, 0, 0, 0,
         0, 0, 0, 0, 1,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {0, 0, 0, 0, 0,  // Oitavo frame
         1, 0, 0, 0, 0,
         0, 0, 0, 1, 1,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {0, 0, 0, 0, 0,  // Nono frame
         0, 0, 0, 0, 0,
         0, 0, 1, 1, 1,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {0, 0, 0, 0, 0,  // Décimo frame
         0, 0, 0, 0, 0,
         0, 1, 1, 1, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {0, 0, 0, 0, 0,  // Décimo Primeiro frame
         0, 0, 0, 0, 0,
         1, 1, 1, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {0, 0, 0, 0, 0,  // Décimo Segundo frame
         0, 0, 0, 0, 0,
         1, 1, 0, 0, 0,
         0, 0, 0, 0, 1,
         0, 0, 0, 0, 0},

        {0, 0, 0, 0, 0,  // Décimo Terceiro frame
         0, 0, 0, 0, 0,
         1, 0, 0, 0, 0,
         0, 0, 0, 0, 1,
         1, 0, 0, 0, 0},

        {0, 0, 0, 0, 0,  // Décimo Quarto frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 1,
         1, 1, 0, 0, 0},

        {0, 0, 0, 0, 0,  // Décimo Quinto frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         1, 1, 1, 0, 0},

        {0, 0, 0, 0, 0,  // Décimo Sexto frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 1, 1, 1, 0},

        {0, 0, 0, 0, 0,  // Décimo Sétimo frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 1, 1, 1},

        {0, 0, 0, 0, 0,  // Décimo Oitavo frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 1, 1},

        {0, 0, 0, 0, 0,  // Décimo Nono frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 1},

        {0, 0, 0, 0, 0,  // Vigésimo frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},
    };

    int num_frames = 20; // Número total de frames na animação

    for (int frame = 0; frame < num_frames; frame++) {
        // Exibe o frame atual
        for (int i = 0; i < NUM_PIXELS; i++) {
            if (cobra_frames[frame][i] == 1) {
                uint32_t color = rgb_color(0, 255, 0); // Cor verde para a cobra
                pio_sm_put_blocking(pio, sm, color);
            } else {
                uint32_t color = rgb_color(0, 0, 0);   // Cor preta (apagado)
                pio_sm_put_blocking(pio, sm, color);
            }
        }
        sleep_ms(200); // Delay entre os frames
    }
}
void animacao_timer(PIO pio, uint sm){
    
        double timer_frames [9][25] = {
        {0, 0, 1, 0, 0,  // Primeiro frame
         0, 0, 1, 0, 0,
         0, 0, 1, 0, 0,
         0, 0, 1, 0, 0,
         0, 0, 1, 1, 0},
       
        {0, 1, 1, 1, 0,  // Segundo frame
         0, 1, 0, 0, 0,
         0, 0, 1, 0, 0,
         0, 0, 0, 1, 0,
         0, 1, 1, 1, 0},    
        
        {0, 1, 1, 1, 0,  // terceiro frame
         0, 0, 0, 1, 0,
         0, 1, 1, 1, 0,
         0, 0, 0, 1, 0,
         0, 1, 1, 1, 0},
        
        {0, 1, 0, 0, 0,  // Quarto frame
         0, 0, 0, 1, 0,
         0, 1, 1, 1, 0,
         0, 1, 0, 1, 0,
         0, 1, 0, 1, 0},
        
        {0, 1, 1, 1, 0,  // Quinto frame
         0, 0, 0, 1, 0,
         0, 1, 1, 1, 0,
         0, 1, 0, 0, 0,
         0, 1, 1, 1, 0},

        {0, 1, 1, 1, 0,  // Sexto frame
         0, 1, 0, 1, 0,
         0, 1, 1, 1, 0,
         0, 1, 0, 0, 0,
         0, 0, 1, 0, 0},

    
        
        {0, 1, 0, 0, 0,  // Setimo frame
         0, 0, 0, 1, 0,
         0, 1, 0 , 0, 0,
         0, 0, 0, 1, 0,
         0, 1, 1, 1, 0},
        
        {0, 1, 1, 1, 0,  // Oitavo frame
         0, 1, 0, 1, 0,
         0, 1, 1, 1, 0,
         0, 1, 0, 1, 0,
         0, 1, 1, 1, 0},
        
        {0, 0, 1, 0, 0,  // 9Sexto frame
         0, 0, 0, 1, 0,
         0, 1, 1, 1, 0,
         0, 1, 0, 1, 0,
         0, 1, 1, 1, 0},
        
    
      
    };
    
    int total_frames=9;

    for (int frame = 0; frame < total_frames; frame++) {
        // Exibe o frame atual
        for (int i = 0; i < NUM_PIXELS; i++) {
            if (timer_frames[frame][i] == 1) {
                uint32_t color = rgb_color(255, 0, 0); // Cor vermelha 
                pio_sm_put_blocking(pio, sm, color);
            } else {
                uint32_t color = rgb_color(0, 0, 0);   // Cor preta (apagado)
                pio_sm_put_blocking(pio, sm, color);
            }
        }
        sleep_ms(1000); // Delay entre os frames
    }

}
// Animação de "Ondas Crescentes" na matriz de LEDs 5x5
void animacao_ondas(PIO pio, uint sm) {
    // Frames representando as ondas crescentes
    double ondas[6][25] = {
        {0, 0, 0, 0, 0,  // Nenhum LED aceso (estado inicial)
         0, 0, 0, 0, 0,
         0, 0, 1, 0, 0,  // Apenas o LED central aceso
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {0, 0, 0, 0, 0,  // Primeira expansão
         0, 1, 1, 1, 0,
         0, 1, 1, 1, 0,
         0, 1, 1, 1, 0,
         0, 0, 0, 0, 0},

        {1, 0, 0, 0, 1,  // Segunda expansão
         0, 1, 1, 1, 0,
         0, 1, 1, 1, 0,
         0, 1, 1, 1, 0,
         1, 0, 0, 0, 1},

        {1, 1, 1, 1, 1,  // Terceira expansão (bordas completas)
         1, 1, 1, 1, 1,
         1, 1, 1, 1, 1,
         1, 1, 1, 1, 1,
         1, 1, 1, 1, 1},

        {0, 0, 0, 0, 0,  // Recolhimento - bordas desligando
         0, 1, 1, 1, 0,
         0, 1, 1, 1, 0,
         0, 1, 1, 1, 0,
         0, 0, 0, 0, 0},

        {0, 0, 0, 0, 0,  // Última etapa (só o LED central aceso)
         0, 0, 0, 0, 0,
         0, 0, 1, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0}
    };

    int max_frames = 6;  // Número total de frames na animação

    // Iteração sobre os frames
    for (int frame = 0; frame < max_frames; frame++) {
        for (int i = 0; i < NUM_PIXELS; i++) {
            if (ondas[frame][i] == 1) {
                // LEDs acesos com cor azul crescente
                uint32_t color = rgb_color(0, 0, (double)frame / max_frames);
                pio_sm_put_blocking(pio, sm, color);
            } else {
                // LEDs apagados
                uint32_t color = rgb_color(0, 0, 0);
                pio_sm_put_blocking(pio, sm, color);
            }
        }
        sleep_ms(500);  // Delay entre os frames
    }

    sleep_ms(100);  // Pausa ao final
    desligar_leds(pio, sm);  // Garantir que todos os LEDs desliguem
}

void animacao_e(PIO pio, uint sm){
        // Frames da cobra atravessando a matriz
    double e_frames[21][25] = {
        {1, 0, 0, 0, 0,  // Primeiro frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {1, 1, 0, 0, 0,  // Segundo frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {1, 1, 1, 0, 0,  // Terceiro frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {1, 1, 1, 0, 0,  // Quarto frame
         0, 1, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {1, 1, 1, 0, 0,  // Quinto frame
         0, 1, 0, 0, 0,
         0, 0, 0, 1, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {1, 1, 1, 0, 0,  // Sexto frame
         0, 1, 0, 0, 0,
         0, 0, 0, 1, 0,
         0, 1, 0, 0, 0,
         0, 0, 0, 0, 0},

        {1, 1, 1, 0, 0,  // Sétimo frame
         0, 1, 0, 0, 0,
         0, 0, 0, 1, 0,
         0, 1, 0, 0, 0,
         0, 0, 1, 0, 0},

        {1, 1, 1, 0, 0,  // Oitavo frame
         0, 1, 0, 0, 0,
         0, 0, 0, 1, 0,
         0, 1, 0, 0, 0,
         0, 1, 1, 0, 0},

        {1, 1, 1, 0, 0,  // Nono frame
         0, 1, 0, 0, 0,
         0, 0, 0, 1, 0,
         0, 1, 0, 0, 1,
         0, 1, 1, 0, 0},

        {1, 1, 1, 0, 0,  // Décimo frame
         0, 1, 0, 0, 0,
         1, 0, 0, 1, 0,
         0, 1, 0, 0, 1,
         0, 1, 1, 0, 0},

        {1, 1, 1, 0, 0,  // Décimo Primeiro frame
         0, 1, 0, 0, 0,
         1, 1, 0, 1, 0,
         0, 1, 0, 0, 1,
         0, 1, 1, 0, 0},

        {1, 1, 1, 0, 0,  // Décimo Segundo frame
         0, 1, 0, 0, 0,
         1, 1, 1, 1, 0,
         0, 1, 0, 0, 1,
         0, 1, 1, 0, 0},

        {1, 1, 1, 0, 0,  // Décimo Terceiro frame
         0, 1, 0, 0, 0,
         1, 1, 1, 1, 1,
         0, 1, 0, 0, 1,
         0, 1, 1, 0, 0},

        {0, 0, 0, 0, 0,  // Décimo Quarto frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 1,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {0, 0, 0, 0, 0,  // Décimo Quinto frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {0, 0, 0, 0, 0,  // Décimo Sexto frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 1,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},

        {0, 0, 0, 0, 0,  // Décimo Sétimo frame
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 0, 0, 0, 0},
        
        {1, 1, 1, 0, 0,  // Décimo Oitavo frame
         0, 1, 0, 0, 0,
         1, 1, 1, 1, 1,
         0, 1, 0, 0, 1,
         0, 1, 1, 0, 0},

         {1, 1, 1, 0, 0,  // Décimo Nono frame
         0, 1, 0, 0, 0,
         1, 1, 1, 1, 1,
         0, 1, 0, 0, 1,
         0, 1, 1, 0, 0},

         {1, 1, 1, 0, 0,  // Vigésimo frame
         0, 1, 0, 0, 0,
         1, 1, 1, 1, 1,
         0, 1, 0, 0, 1,
         0, 1, 1, 0, 0},

         {1, 1, 1, 0, 0,  // Vigésimo Primeiro frame
         0, 1, 0, 0, 0,
         1, 1, 1, 1, 1,
         0, 1, 0, 0, 1,
         0, 1, 1, 0, 0}
    };

    int num_frames = 22; // Número total de frames na animação

    for (int frame = 0; frame < num_frames; frame++) {
        // Exibe o frame atual
        for (int i = 0; i < NUM_PIXELS; i++) {
            if (e_frames[frame][i] == 1) {
                uint32_t color = rgb_color(0, 0, 255); // Cor azul para a letra
                pio_sm_put_blocking(pio, sm, color);
            } else {
                uint32_t color = rgb_color(0, 0, 0);   // Cor preta (apagado)
                pio_sm_put_blocking(pio, sm, color);
            }
        }
        if(frame < 13 || frame > 18){ // Delay entre os frames, dependendo do frame
            sleep_ms(200);
        }else{
            sleep_ms(500);
        }
    }
}


// uma cobrinha correndo em volta de um led no centro
void animacao_9(PIO pio,uint sm){
    double frame[][NUM_PIXELS] = {
        {1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,1,1,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,1,1,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,1,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,1,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,1,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,1,1,0,0},
        {0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,1,1,1,0,0,0},
        {0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,1,0,0,0,0,0},
        {1,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,1,0,0,0,0,0},
        {1,1,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
        {1,1,1,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0}
    };
    int num_frames = 16;
    int k = 0;
    while (k < 7){
        for (int j = 0; j < num_frames; j++) {
            for (int i = 0; i < NUM_PIXELS; i++) {
                if (frame[j][i]) {
                    uint32_t color = rgb_color(0.2, 0, 0);
                    pio_sm_put_blocking(pio, sm, color);
                } else {
                    uint32_t color = rgb_color(0, 0, 0);
                    pio_sm_put_blocking(pio, sm, color);
                }
            }
            sleep_ms(50);
        }
        k++;
    }
    desligar_leds(pio, sm);
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
            
        case '2':
            animacao_2(pio, sm); // Simboliza um X na matriz
            break;

        case '3':
            animacao_cobra(pio, sm); // Simboliza uma cobra atravessando a matriz
            break;   
        case '4':
            animacao_timer(pio, sm); // Simboliza um timer de 1 a 9
            break;
            
        case '5':
            animacao_e(pio, sm); // Letra 'e' da embarcatech aparece
            break;

        case '6':
            animacao_ondas(pio, sm); // Simboliza ondas crescentes
            break;
            
        case '9':
            animacao_9(pio, sm);
            break;

        case '0':
            animacao_0(pio, sm); // rosto feliz piscando
            break;

        case 'A':
            desligar_leds(pio, sm);
            break;

        case 'B':
            ligar_azul(pio, sm);
            break;

        case 'C':
            ligar_vermelho(pio, sm); // Aciona LEDs na cor vermelha com 80% de intensidade
            break;

        case 'D': // liga os leds em verde com 50% de intensidade
            for (int i = 0; i < NUM_PIXELS; i++) {
                uint32_t color = rgb_color(0, 0.5, 0); // verde com 50% de intensidade
                pio_sm_put_blocking(pio, sm, color); 
            }
            break;  

        case '#': // liga leds com cor branca em 20% de intensidade
            for (int i = 0; i < NUM_PIXELS; i++) {
                uint32_t color = rgb_color(0.2, 0.2, 0.2); // branco com 20% de intensidade
                pio_sm_put_blocking(pio, sm, color); 
            }
            break;
        default:
            break;
        }
        sleep_ms(100); // Debounce
    }
}
