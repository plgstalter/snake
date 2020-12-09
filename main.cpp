#include <iostream>
#include "sys/ioctl.h"

#include "stdlib.h"

#include "config.h"
#include "internals.h"
#include "display.h"
#include "food.h"


// Première partie : Setup avant le début de la partie

void backgroundSetup(const int nx, const int ny, int background[]){
  // on va ajouter les '#' sur les bords
  // or dans `display.cpp`, les '#' correspondent à '1'
  for (int i=0; i<nx; i++) {
    background[i] = 1;
    background[(ny-1)*nx + i] = 1;
  }
  for (int j=0; j<ny; j++){
    background[j*nx] = 1;
    background[j*nx+nx-1] = 1;
  }
}

void setupSnake(int snake[], int len, int nx, int ny){
  // le + 1 est pour éviter d'être sur la frontière ; idem pour le -2 (dans l'autre sens)
  snake[0] = 8;
  snake[SNAKE_LEN] = ny-10;
  for (int i=1; i < len; i++) {
    // on initialise un serpent horizontal
    snake[i] = snake[i-1] - 1;
    snake[SNAKE_LEN+i] = snake[SNAKE_LEN+i-1]; 
  }
}


// 2e partie : ajouter et supprimer le serpent

void add_snake(int* snake, int bg[], int snl, const int nx, const int ny){
  //on ajoute le snake sur le background ; un '1' affiche un '#'
  for (int i=0; i<snl; i++) {
    bg[snake[i] + nx*snake[SNAKE_LEN+i]] = 1;
  }
}

void remove_snake(int* snake, int bg[], int snl, const int nx, const int ny){
  // on retire le snake pour avoir un fond vierge
  for (int i=0; i<snl; i++) {
    bg[snake[i] + nx*snake[SNAKE_LEN+i]] = -1;
  }
}


// 3e partie : faire évoluer le snake

void snakegrowth(int* snake, int snl, int dxdy[]) {
  for (int i=0; i<snl; i++) {
    snake[snl - i] = snake[snl - i - 1];
    snake[SNAKE_LEN + snl - i] = snake[SNAKE_LEN + snl - i - 1];
  }
  snake[0] = snake[1] + dxdy[0];
  snake[SNAKE_LEN] = snake[SNAKE_LEN + 1] + dxdy[1];
}

void snake_movement(char key, int dxdy[]){
  // on considère que l'on joue avec les touches a, z, e et s sur un clavier AZERTY
  if (key == 'a') {
    dxdy[0] = -1;
    dxdy[1] = 0;
  }
  if (key == 'e') {
    dxdy[0] = 1;
    dxdy[1] = 0;
  }
  if (key == 'z') {
    dxdy[0] = 0;
    dxdy[1] = -1;
  }
  if (key == 's') {
    dxdy[0] = 0;
    dxdy[1] = 1;
  }
}

void update_snake_coordinates(int snake[], int snl, int dxdy[]){
  for (int i=0;i<(snl - 1);i++) {
    snake[i] = snake[i+1];
    snake[SNAKE_LEN+i] = snake[SNAKE_LEN+i+1];
  }
  snake[snl-1] = snake[snl - 1] + dxdy[0];
  snake[SNAKE_LEN+snl-1] = snake[SNAKE_LEN+snl - 1] + dxdy[1];
}


// 4e partie : game over

bool snake_eats_itself(int* snake, int nx, int ny, int snl, int dxdy[]){

}

bool verifyBorder(int snake[], int nx, int ny, int snl, int dxdy[]){

  // // 1ere partie : vérifier que l'on évite les bords
  // if (dxdy[0]== 1 && snake[snl-1] == nx-1) {
  //   return false;
  // }
  // if (dxdy[0]== -1 && snake[snl-1] == 0) {
  //   return false;
  // }
  // if (dxdy[1]== 1 && snake[snl+SNAKE_LEN-1] == ny-1) {
  //   return false;
  // }
  // if (dxdy[1]== -1 && snake[snl+SNAKE_LEN-1] == 0) {
  //   return false;
  // }

  // // 2nde partie : vérifier que le serpent ne se mord pas la queue
  // // idée : on regarde si la dernière case + dxdy fonce sur le serpent (i.e. vaut une des autres cases)
  // int test_1 = snake[snl-1] + dxdy[0];
  // int test_2 = snake[snl-1+SNAKE_LEN] + dxdy[1];
  // for (int i=0; i<snl-1; i++) {
  //   if (snake[i] == test_1 && snake[i+SNAKE_LEN] == test_2) {
  //     return false;
  //   }
  // si aucun problème, on est bon
  return true;
}


// Dernière partie : la partie elle-même :)

void startGame(const int& lap, const int& nx, const int& ny, int& snl, int* snake, int* bg){
    char key;
    int dxdy[2] = {1,0};
    int food[2] = {0,0};

    createFood( bg, food, nx, ny );

    while( true ) {
        internal::frameSleep(lap);
        if( internal::keyEvent() ){
            std::cin >> key; 
            snake_movement(key, dxdy);
        }
        backgroundClear();
        add_snake(snake, bg, snl, nx, ny );
        printFrame(nx, ny, bg);
        remove_snake(snake, bg, snl, nx, ny);
        bool out_1 = verifyBorder(snake, nx, ny, snl, dxdy);
        bool out_2 = snake_eats_itself(snake, nx, ny, snl, dxdy);
        if (!(out_1 && out_2)) {
            std::cerr << "" << std::endl;
            exit(1);
        }
        bool eat = eatFood(food, snake);
        if(eat){
            createFood(bg, food, nx, ny);
            snakegrowth(snake, snl, dxdy);
            snl++;
        }
        update_snake_coordinates(snake, snl, dxdy);

    }
}



int main(){
    const int nx = 50;
    const int ny = 25;
    const int lap=200;

    int snake_len = 3;
    int background[nx*ny] ;
    for( int i=0; i<nx*ny; i++){
        background[i] = -1;
    }
    int snake[2*SNAKE_LEN];
    backgroundSetup(nx, ny, background);
    setupSnake(snake, snake_len, nx, ny);
    add_snake(snake, background, snake_len, nx, ny);
    startGame(lap, nx, ny, snake_len, snake, background);
    return 0;
}