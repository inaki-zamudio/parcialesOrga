section .text

; Marca un ejercicio como aún no completado (esto hace que no corran sus tests)
FALSE EQU 0
; Marca un ejercicio como hecho
TRUE  EQU 1

; Marca el ejercicio 2A como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - ej2a
global EJERCICIO_2A_HECHO
EJERCICIO_2A_HECHO: db FALSE ; Cambiar por `TRUE` para correr los tests.

; Dada una imagen origen escribe en el destino `scale * px + offset` por cada
; píxel en la imagen.
;
; Parámetros:
;   - dst_depth: La imagen destino (mapa de profundidad). Está en escala de
;                grises a 32 bits con signo por canal.
;   - src_depth: La imagen origen (mapa de profundidad). Está en escala de
;                grises a 8 bits sin signo por canal.
;   - scale:     El factor de escala. Es un entero con signo de 32 bits.
;                Multiplica a cada pixel de la entrada.
;   - offset:    El factor de corrimiento. Es un entero con signo de 32 bits.
;                Se suma a todos los píxeles luego de escalarlos.
;   - width:     El ancho en píxeles de `src_depth` y `dst_depth`.
;   - height:    El alto en píxeles de `src_depth` y `dst_depth`.
global ej2a
ej2a:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits.
	;
	; r/m64 = int32_t* dst_depth
	; r/m64 = uint8_t* src_depth
	; r/m32 = int32_t  scale
	; r/m32 = int32_t  offset
	; r/m32 = int      width
	; r/m32 = int      height

	ret

; Marca el ejercicio 2B como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - ej2b
global EJERCICIO_2B_HECHO
EJERCICIO_2B_HECHO: db FALSE ; Cambiar por `TRUE` para correr los tests.

; Dadas dos imágenes de origen (`a` y `b`) en conjunto con sus mapas de
; profundidad escribe en el destino el pixel de menor profundidad por cada
; píxel de la imagen. En caso de empate se escribe el píxel de `b`.
;
; Parámetros:
;   - dst:     La imagen destino. Está a color (RGBA) en 8 bits sin signo por
;              canal.
;   - a:       La imagen origen A. Está a color (RGBA) en 8 bits sin signo por
;              canal.
;   - depth_a: El mapa de profundidad de A. Está en escala de grises a 32 bits
;              con signo por canal.
;   - b:       La imagen origen B. Está a color (RGBA) en 8 bits sin signo por
;              canal.
;   - depth_b: El mapa de profundidad de B. Está en escala de grises a 32 bits
;              con signo por canal.
;   - width:  El ancho en píxeles de todas las imágenes parámetro.
;   - height: El alto en píxeles de todas las imágenes parámetro.
global ej2b
ej2b:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits.
	;
	; r/m64 = rgba_t*  dst
	; r/m64 = rgba_t*  a
	; r/m64 = int32_t* depth_a
	; r/m64 = rgba_t*  b
	; r/m64 = int32_t* depth_b
	; r/m32 = int      width
	; r/m32 = int      height

	ret
