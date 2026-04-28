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
            }
            break;
        case ESTADO_VAL_PRESION:
            if (p->boton == 0) {
                siguiente = ESTADO_REPOSO;
            } else if (p->t >= 5 && p->boton == 1) {
                siguiente = ESTADO_PRESIONADO;
            } else if (p->t < 5 && p->boton == 1) {
                p->t++;
            }
            break;
        case ESTADO_PRESIONADO:
            // Estado transitorio de 1 ciclo. Evoluciona de forma automática.
            siguiente = ESTADO_MANTENIDO;
            break;
        case ESTADO_MANTENIDO:
            if (p->boton == 0) {
                siguiente = ESTADO_VAL_LIBERACION;
            }
            break;
        case ESTADO_VAL_LIBERACION:
            if (p->boton == 1) {
                siguiente = ESTADO_MANTENIDO;
            } else if (p->t >= 5 && p->boton == 0) {
                siguiente = ESTADO_LIBERADO;
            } else if (p->t < 5 && p->boton == 0) {
                p->t++;
            }
            break;
        case ESTADO_LIBERADO:
            // Estado transitorio de 1 ciclo. Evoluciona de forma automática.
            siguiente = ESTADO_REPOSO;
            break;
        default:
            break;
    }

    // Simplificación: si hay cualquier cambio de estado, el temporizador se reinicia.
    if (siguiente != p->estado) {
        p->t = 0;
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
