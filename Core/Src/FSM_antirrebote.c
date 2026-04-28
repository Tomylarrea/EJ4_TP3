#include <stdint.h>
#include "main.h"
#include "FSM_antirrebote.h"

#define TIEMPO_VALIDACION 20

Estado_pulsador antirrebote(Pulsador *p){
    if (HAL_GetTick() - p->ultimo_tick < 1) {
        // Actualizar el estado interno de la FSM para evitar que quede varada
        if (p->estado == ESTADO_PRESIONADO) {
            p->estado = ESTADO_MANTENIDO;
            return ESTADO_MANTENIDO;
        }
        if (p->estado == ESTADO_LIBERADO) {
            p->estado = ESTADO_REPOSO;
            return ESTADO_REPOSO;
        }
        return p->estado;
    }

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
                p->t = 0; // Inicia el temporizador
            }
            break;

        case ESTADO_VAL_PRESION:
            p->t++;
            if (p->t >= TIEMPO_VALIDACION) {
                // Se evalúa el pin exclusivamente al terminar el tiempo de enmascaramiento
                if (p->boton == 1) {
                    siguiente = ESTADO_PRESIONADO;
                } else {
                    siguiente = ESTADO_REPOSO; // Falso disparo o ruido espurio
                }
            }
            break;

        case ESTADO_PRESIONADO:
            siguiente = ESTADO_MANTENIDO;
            break;

        case ESTADO_MANTENIDO:
            if (p->boton == 0) {
                siguiente = ESTADO_VAL_LIBERACION;
                p->t = 0;
            }
            break;

        case ESTADO_VAL_LIBERACION:
            p->t++;
            if (p->t >= TIEMPO_VALIDACION) {
                if (p->boton == 0) {
                    siguiente = ESTADO_LIBERADO;
                } else {
                    siguiente = ESTADO_MANTENIDO; // Fue un rebote mientras se mantenía presionado
                }
            }
            break;

        case ESTADO_LIBERADO:
            siguiente = ESTADO_REPOSO;
            break;

        default:
            break;
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
