#ifndef ARUCO_RECEPTOR_H
#define ARUCO_RECEPTOR_H

#include <Arduino.h>

struct ArucoReceptor {
  int id = -1;
  char buffer[8];
  byte indice = 0;

  void iniciar(long baudios = 9600) {
    Serial.begin(baudios);
  }

  bool actualizar() {
    bool recibido = false;

    while (Serial.available() > 0) {
      char c = Serial.read();

      if (c == '\n') {
        if (indice > 0) {
          buffer[indice] = '\0';
          id = atoi(buffer);
          recibido = true;
        }
        indice = 0;
      }
      else if ((c >= '0' && c <= '9') || c == '-') {
        if (indice < sizeof(buffer) - 1) {
          buffer[indice++] = c;
        }
      }
    }

    return recibido;
  }

  int getId() {
    return id;
  }
};

#endif