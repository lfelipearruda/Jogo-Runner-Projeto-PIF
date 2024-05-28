#include "keyboard.h"
#include "screen.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>

#define LARGURA 40
#define ALTURA 20
#define BONECO 'O'
#define PAREDE '#'
#define DESCER 1
#define SUBIR 1
#define JOGAR 0
#define SAIR  1
#define OPCOES 2

void initMap(char mapa[ALTURA][LARGURA]) {
    for (int i = 0; i < ALTURA; i++) {
        for (int j = 0; j < LARGURA; j++) {
            mapa[i][j] = ' ';
        }
    }
}

void updateParede(int *paredeX, int *parede_alto, int *parede_baixo, int *pontuacao) {
    (*paredeX)--;
    if (*paredeX < 1) {
        *paredeX = LARGURA - 2;
        *parede_alto = (rand() % (ALTURA - 6)) + 1;
        *parede_baixo = ALTURA - *parede_alto - 4;
        (*pontuacao)++;
    }
}

void updateBoneco(int *bonecoY, char input) {
    if (input == 'w' || input == 'W' || input == 32) {
        *bonecoY -= SUBIR;
    } else if (input == 's' || input == 'S') {
        *bonecoY += DESCER;
    }
}

void drawMap(char mapa[ALTURA][LARGURA], int bonecoX, int bonecoY, int paredeX, int parede_alto, int parede_baixo) {
    for (int i = 0; i < ALTURA; i++) {
        for (int j = 0; j < LARGURA; j++) {
            if (i == 0 || i == ALTURA - 1) {
                mapa[i][j] = '_';
            } else if (j == 0 || j == LARGURA - 1) {
                mapa[i][j] = '|';
            } else if (i == bonecoY && j == bonecoX) {
                mapa[i][j] = BONECO;
            } else if ((j == paredeX || j == paredeX + 1) &&
                       (i < parede_alto || i >= parede_alto + 4)) {
                mapa[i][j] = PAREDE;
            } else {
                mapa[i][j] = ' ';
            }
        }
    }
}

void printMap(char mapa[ALTURA][LARGURA], int pontuacao) {
    screenClear();
    for (int i = 0; i < ALTURA; i++) {
        for (int j = 0; j < LARGURA; j++) {
            screenGotoxy(j + SCRSTARTX, i + SCRSTARTY);
            if (mapa[i][j] == '|') {
                screenSetColor(GREEN, BLACK);
            } else if (mapa[i][j] == '_') {
                screenSetColor(GREEN, BLACK);
            } else {
                screenSetColor(WHITE, BLACK);
            }
            printf("%c", mapa[i][j]);
        }
    }
    screenGotoxy(SCRSTARTX, SCRSTARTY + ALTURA + 1);
    screenSetColor(WHITE, BLACK);
    printf("Pontuação: %d ", pontuacao);
    screenUpdate();
}

int checkCollision(int bonecoX, int bonecoY, int paredeX, int parede_alto, int abertura) {
    if (bonecoY <= 0 || bonecoY >= ALTURA - 1) {
        return 1;
    }
    if ((bonecoX == paredeX || bonecoX == paredeX + 1) &&
        (bonecoY < parede_alto || bonecoY >= parede_alto + abertura)) {
        return 1;
    }
    return 0;
}

void processInput(int *bonecoY, char *input) {
    if (keyhit()) {
        *input = readch();
        if (*input == 8 || *input == 27) {
            printf("Game Over!\n");
            exit(0);
        }
    } else {
        *input = 0;
    }
    updateBoneco(bonecoY, *input);
}

int telaInicio() {
    int opcaoSelecionada = JOGAR;

    while (1) {

        screenClear();

        printf("Selecione uma opção:\n");
        printf("%s JOGAR\n", (opcaoSelecionada == JOGAR) ? "->" : "  ");
        printf("%s SAIR\n", (opcaoSelecionada == SAIR) ? "->" : "  ");

        char input = readch();
        if (input == 'w' || input == 'W') {
            opcaoSelecionada = (opcaoSelecionada - 1 + OPCOES) % OPCOES;
        } else if (input == 's' || input == 'S') {
            opcaoSelecionada = (opcaoSelecionada + 1) % OPCOES;
        } else if (input == '\n' || input == ' ') {
            return opcaoSelecionada;
        }
    }
}

void telaGameOver(int pontuacao) {
    screenClear();
    screenGotoxy(SCRSTARTX + 10, SCRSTARTY + 8);
    screenSetColor(RED, BLACK);
    printf("GAME OVER");
    screenGotoxy(SCRSTARTX + 5, SCRSTARTY + 10);
    screenSetColor(WHITE, BLACK);
    printf("Pontuação: %d ", pontuacao);
    screenGotoxy(SCRSTARTX + 3, SCRSTARTY + 12);
    printf("Pressione ENTER para jogar novamente");
    screenGotoxy(SCRSTARTX + 5, SCRSTARTY + 13);
    printf("Pressione ESC para sair");
    screenUpdate();
}

void reiniciarJogo(char mapa[ALTURA][LARGURA], int *bonecoX, int *bonecoY, int *paredeX, int *parede_alto, int abertura, int *parede_baixo, int *pontuacao) {
    initMap(mapa);
    *bonecoX = 5;
    *bonecoY = ALTURA / 2;
    *paredeX = LARGURA - 5;
    *parede_alto = (rand() % (ALTURA - abertura - 2)) + 1;
    *parede_baixo = ALTURA - *parede_alto - abertura;
    *pontuacao = 0;
    timerPrint();
}

int main() {
    keyboardInit();
    screenInit(1);
    timerInit(50);

    int escolha = telaInicio();
    if (escolha == SAIR) {
        printf("Saindo do jogo...\n");
        screenDestroy();
        keyboardDestroy();
        return 0;
    }

    char mapa[ALTURA][LARGURA];
    int bonecoX = 5;
    int bonecoY = ALTURA / 2;
    int paredeX = LARGURA - 5;
    int abertura = 4;
    int parede_alto = (rand() % (ALTURA - abertura - 2)) + 1;
    int parede_baixo = ALTURA - parede_alto - abertura;
    int pontuacao = 0;
    char input = 0;

    initMap(mapa);

    while (1) {
        if (timerTimeOver()) {
            processInput(&bonecoY, &input);
            updateParede(&paredeX, &parede_alto, &parede_baixo, &pontuacao);
            drawMap(mapa, bonecoX, bonecoY, paredeX, parede_alto, parede_baixo);
            printMap(mapa, pontuacao);
            if (checkCollision(bonecoX, bonecoY, paredeX, parede_alto, abertura)) {
                telaGameOver(pontuacao);
                char input;
                while (1) {
                    if (keyhit()) {
                        input = readch();
                        if (input == '\n') {
                            reiniciarJogo(mapa, &bonecoX, &bonecoY, &paredeX, &parede_alto, abertura, &parede_baixo, &pontuacao);
                            break;
                        } else if (input == 27) {
                            screenDestroy();
                            keyboardDestroy();
                            return 0;
                        }
                    }
                }
            }
        }
    }

    timerDestroy();
    screenDestroy();
    keyboardDestroy();

    return 0;
}