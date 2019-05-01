#include "hash.h"
#include <string.h>
#define TAM_INICIAL 101
#define LIBRE 1
#define OCUPADO 0
#define BORRADO -1

typedef struct hash_campo {
    char *clave;
    void *valor;
    int estado;  // 1 libre => -1 => borrado 0 => ocupado
} hash_campo_t;

struct hash {
    size_t cantidad; //cantidad de elementos ocupados + desocupados          
    size_t largo; //tamaño de la tabla de hash                     
    size_t carga;  //cantidad/largo (densidad del hash)         
    hash_campo_t *tabla; 
    hash_destruir_dato_t destruir_dato; 
};


size_t funcion_hash(const char* s,size_t tam){ 
	size_t hash_val; 
	for (hash_val = 0; *s != '\0'; s++){
		hash_val = *s + 31 * hash_val;
	}
	return hash_val %  tam;
}
struct hash_iter{
	hash_t* hash_i;
	size_t contador;
	size_t pos_iter;
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
    size_t posicion = funcion_hash(clave,strlen(clave));
    size_t i;
    for (i = posicion;(!hash->tabla[posicion].estado);i++){
        if(strcmp(hash->tabla[posicion].clave, clave) != 0){
            if(hash->destruir_dato) hash->destruir_dato(hash->tabla[posicion].valor);
            hash->tabla[posicion].valor = dato;
            return true;
        }
        if(i == (hash->largo - 1)) i = 0;
    }

    strcpy(hash->tabla[i].clave,clave);
    hash->tabla[i].valor = dato;
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
    size_t posicion = funcion_hash(clave,hash->largo);
    size_t cont = posicion;
    while(strcmp(hash->tabla[cont].clave,clave) && posicion < hash->largo){
        if(cont == hash->largo - 1) posicion = 0;
        if(cont == posicion - 1) return NULL;
        cont++;
    }
    hash->tabla[cont].estado = BORRADO;
    return hash->tabla[posicion].valor;
    }

/* Obtiene el valor de un elemento del hash, si la clave no se encuentra
 * devuelve NULL.
 * Pre: La estructura hash fue inicializada
 */
void *hash_obtener(const hash_t *hash, const char *clave){
    size_t posicion = funcion_hash(clave,strlen(clave));
    while(strcmp(hash->tabla[posicion].clave,clave)){
        if (posicion == hash->largo -1) posicion = 0;
        if (posicion == (funcion_hash(clave,strlen(clave) - 1))) return NULL;
    }
    return hash->tabla[posicion].valor;
}
  
 /* Determina si clave pertenece o no al hash.
 * Pre: La estructura hash fue inicializada
 */
bool hash_pertenece(const hash_t *hash, const char *clave){
    return (!hash_obtener(hash,clave))? true:false;
}

/* Devuelve la cantidad de elementos del hash.
 * Pre: La estructura hash fue inicializada
*/
size_t hash_cantidad(const hash_t *hash){
    return (hash->cantidad);
}

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * hash_t *hash_crear(hash_destruir_dato_t destruir_dato); * Pre: La estructura hash fue inicializada
 * Post: La estructura hash fue destruida
 */
void hash_destruir(hash_t *hash){
    for (size_t i = 0; i < hash->largo ; i++){
        if(hash->tabla[i].estado &&  hash->destruir_dato ){
            hash->destruir_dato(hash->tabla[i].valor);
        }
    }
    free(hash);
}



hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t* iter= malloc(sizeof(hash_iter_t));
	if(!iter)
		return NULL;
	iter-> hash_i= hash;
	size_t i=0;
	iter-> contador=0;
	if (hash_cantidad(hash)){
		for (i ; hash->tabla[i]-> estado ; ++i);
		iter-> contador++;
	}
	iter-> pos_iter=i;
	return iter;
}

bool hash_iter_al_final(const hash_iter_t *iter){
	if(!iter)
		return false;
	if(hash_cantidad(iter->hash_i)== iter->cantidad)
		return true;
	return false;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	if(hash_iter_al_final(iter) || !iter)
		return false;
	size_t i;
	for (i = iter-> pos_iter+1 ; iter->hash_i-> tabla[i]-> estado ; ++i);

	contador++;
	iter->pos_iter= i;
	return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if(!iter || !hash_cantidad(iter->hash_i) )
		return NULL;
	return iter->hash_i->tabla[iter-> pos_iter]-> clave;
}

void hash_iter_destruir(hash_iter_t* iter){
	free(iter);
}





