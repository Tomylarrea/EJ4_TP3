/*
 * FSM_antirrebote.h
 *
 *  Created on: 27 abr 2026
 *      Author: tomyl
 */

#ifndef INC_FSM_ANTIRREBOTE_H_
#define INC_FSM_ANTIRREBOTE_H_

#include "main.h"
#include <stdint.h>

typedef enum {
    PULLDOWN = 0,
    PULLUP   = 1
} Tipo_pull;

typedef enum {
    ESTADO_REPOSO         = 0,
    ESTADO_VAL_PRESION    = 1,
    ESTADO_PRESIONADO     = 2,  // Evento transitorio: equivale a flanco de bajada validado
    ESTADO_MANTENIDO      = 3,  // Estado estable: botón mantenido (nivel)
    ESTADO_VAL_LIBERACION = 4,
    ESTADO_LIBERADO       = 5   // Evento transitorio: equivale a flanco de subida validado
} Estado_pulsador;

typedef struct {
    GPIO_TypeDef   *port;
    uint16_t        PIN;
    GPIO_PinState   estado_pin;
    uint8_t         boton;
    uint8_t         t;
    Estado_pulsador estado;
    Estado_pulsador estado_anterior; // <-- Nuevo campo
    Tipo_pull       pull;
    uint32_t        ultimo_tick;
} Pulsador;

Estado_pulsador antirrebote(Pulsador *p);
Estado_pulsador ANTIRREBOTE_evento(Pulsador *p);
void ANTIRREBOTE_iniciar(Pulsador *p, GPIO_TypeDef *port, uint16_t PIN, Tipo_pull pull);

#endif /* INC_FSM_ANTIRREBOTE_H_ */
