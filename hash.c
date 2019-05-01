#include "hash.h"
#include <string.h>
#define TAM_INICIAL 101
#define LIBRE 1
#define OCUPADO 0
#define BORRADO -1

typedef struct hash_campo {
    int *clave;
    void *valor;
    int estado;  // 1 libre => -1 => borrado 0 => ocupado
} hash_campo_t;

struct hash {
    size_t cantidad; //cantidad de elementos ocupados            
    size_t largo; //tamaño de la tabla de hash                     
    size_t carga;  //cantidad/largo                 
    hash_campo_t *tabla; 
    hash_destruir_dato_t destruir_dato; 
};

hash_campo_t *inicializar_campo(hash_t* hash){
    hash_campo_t *tabla = malloc(TAM_INICIAL * sizeof(hash_campo_t));
    if(!tabla) return NULL;
    for (size_t i = 0; i < hash->largo; i++){
        tabla[i].estado = LIBRE;
    }
    return tabla;

}

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t* hash = malloc(sizeof(hash_t));
    if (!hash) return NULL;
    hash->tabla = inicializar_campo(hash);
    if(!hash->tabla){
        free(hash);
        return NULL;
    }
    hash->destruir_dato = destruir_dato;
    hash->cantidad = 0;
    hash->largo = TAM_INICIAL;
    hash->carga = hash->cantidad / hash->largo;
    return hash;
}
 
 /*Guarda un elemento en el hash, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura hash fue inicializada
 * Post: Se almacenó el par (clave, dato)
 */
 // 1 libre => -1 => borrado 0 => ocupado
bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    size_t posicion = ;
    size_t i;
    for (i = posicion;(!hash->tabla[posicion].estado);i++){
        if(!strcmp(hash->tabla[posicion]->clave,clave)){
            if(hash->destruir_dato) hash->destruir_dato(hash->tabla[posicion].valor);
            hash->tabla[posicion].valor = dato;
            return true;
        }
        if(i == (hash->largo - 1)) i = 0;
    }
    hash->tabla[i].clave = clave;
    hash->tabla[i].dato = dato;
    hash->cantidad++;
    return true;
}
/* Borra un elemento del hash y devuelve el dato asociado.  Devuelve
 * NULL si el dato no estaba.
 * Pre: La estructura hash fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 */
void *hash_borrar(hash_t *hash, const char *clave){
    size_t posicion = funcion_hash(clave);
    size_t cont = posicion;
    while(strcmp(hash->tabla[cont].clave) && posicion < hash->largo){
        if(cont == hash->largo - 1) posicion = 0;
        if(cont == posicion - 1) return NULL;
        cont++;
    }
    hash->tabla[cont].estado = BORRADO;
    return hash->tabla[posicion].valor;
    }






