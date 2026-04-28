#include <stdint.h>
#include "main.h"
#include "FSM_antirrebote.h"

Estado_pulsador antirrebote(Pulsador *p){
    if (HAL_GetTick() - p->ultimo_tick < 1) return p->estado;
    p->ultimo_tick = HAL_GetTick();

    p->estado_pin = HAL_GPIO_ReadPin(p->port, p->PIN);
    if (p->estado_pin == GPIO_PIN_SET){
        p->boton = 1;
    }
    else if (p->estado_pin == GPIO_PIN_RESET){
        p->boton = 0;
    }
    if (p->pull == PULLUP) p->boton = !p->boton;
    p->estado = ANTIRREBOTE_evento(p);
    return p->estado;
}

Estado_pulsador ANTIRREBOTE_evento(Pulsador *p) {
    Estado_pulsador siguiente = p->estado;
    switch (p->estado) {
        case ESTADO_REPOSO:
            if (p->boton == 1) {
                siguiente = ESTADO_VAL_PRESION;
                break;
            }
            break;
        case ESTADO_VAL_PRESION:
            if (p->boton == 0) {
                siguiente = ESTADO_REPOSO;
                break;
            }
            if (p->t >= 5 && p->boton == 1) {
                siguiente = ESTADO_PRESIONADO;
                break;
            }
            if (p->t < 5 && p->boton == 1) {
                p->t = p->t + 1;
                siguiente = ESTADO_VAL_PRESION;
                break;
            }
            break;
        case ESTADO_VAL_LIBERACION:
            if (p->boton == 1) {
                siguiente = ESTADO_PRESIONADO;
                break;
            }
            if (p->t >= 5 && p->boton == 0) {
                siguiente = ESTADO_REPOSO;
                break;
            }
            if (p->t < 5 && p->boton == 0) {
                p->t = p->t + 1;
                siguiente = ESTADO_VAL_LIBERACION;
                break;
            }
            break;
        case ESTADO_PRESIONADO:
            if (p->boton == 0) {
                siguiente = ESTADO_VAL_LIBERACION;
                break;
            }
            break;
        default:
            break;
    }
    if (siguiente != p->estado) {
        switch (siguiente) {
            case ESTADO_REPOSO:
                p->t = 0;
                break;
            case ESTADO_VAL_PRESION:
                p->t = 0;
                break;
            case ESTADO_VAL_LIBERACION:
                p->t = 0;
                break;
            case ESTADO_PRESIONADO:
                p->t = 0;
                break;
            default:
                break;
        }
    }
    return siguiente;
}

void ANTIRREBOTE_iniciar(Pulsador *p, GPIO_TypeDef *port, uint16_t PIN, Tipo_pull pull) {
    p->port = port;
    p->PIN = PIN;
    p->pull = pull;
    p->estado = ESTADO_REPOSO;
    p->t = 0;
    p->ultimo_tick = 0;
}
