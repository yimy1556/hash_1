#include "hash.h"
#include <string.h>
#define TAM_INICIAL 101
#define VACIO 1
#define OCUPADO 0
#define BORRADO -1
#define FACTOR_CARGA_MAX 0.60
#define FACTOR_CARGA_MIN 0.10
#define DUPLICAR 2
#define MITAD 2


typedef struct hash_campo {
    const char *clave;
    void *valor;
    int estado;  // 1 VACIO => -1 => borrado 0 => ocupado
} hash_campo_t;

struct hash {
    size_t cantidad; //cantidad de elementos ocupados          
    size_t largo; //tamaño de la tabla de hash                     
    float carga;  //cantidad/largo (densidad del hash)         
    hash_campo_t *tabla; 
    hash_destruir_dato_t destruir_dato; 
};
struct hash_iter{
	const hash_t* hash_i;
	size_t contador;
	size_t pos_iter;
};

size_t funcion_hash(const char* s,size_t tam){ 
	size_t hash_val; 
	for (hash_val = 0; *s != '\0'; s++){
		hash_val = *s + 31 * hash_val;
	}
	return hash_val %  tam;
}

hash_campo_t *inicializar_campo(size_t largo){
    hash_campo_t *tabla = malloc(largo * sizeof(hash_campo_t));
    if(!tabla)
        return NULL;
    for (size_t i = 0; i < largo; i++){
        tabla[i].estado = VACIO;        
        tabla[i].clave = NULL;
        tabla[i].valor = NULL;
    }   
    return tabla;
}


hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t* hash = malloc(sizeof(hash_t));
    if (!hash) return NULL;
    hash->tabla = inicializar_campo(TAM_INICIAL); 
    if(!hash->tabla){
        free(hash);
        return NULL;
    }
    hash->destruir_dato = destruir_dato;
    hash->cantidad = 0;
    hash->largo = TAM_INICIAL;
    hash->carga = (float)hash->cantidad / (float)hash->largo;
    return hash;
}
bool hash_redimencionar(hash_t* hash,size_t nuevo_largo){
    hash_campo_t* tabla= malloc(nuevo_largo* sizeof(hash_campo_t) );
    if(!tabla)
        return false;
    tabla= inicializar_campo(nuevo_largo);

    for (size_t i = 0; i < hash->largo; i++){
        if(!hash->tabla[i].estado){
            size_t pos=funcion_hash(hash->tabla[i].clave, nuevo_largo);
            while(!tabla[pos].estado){
                pos++;
                if(pos==hash->largo)
                    pos=0;
            }
            tabla[pos]=hash->tabla[i];
        }
    }
    free(hash->tabla);
    hash->tabla=tabla;
    hash->largo=nuevo_largo;

    return true;
}
 
 /*Guarda un elemento en el hash, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura hash fue inicializada
 * Post: Se almacenó el par (clave, dato)
 */
 // 1 libre => -1 => borrado 0 => ocupado
bool hash_guardar(hash_t *hash, const char *clave, void *dato){

    if(FACTOR_CARGA_MAX< hash->carga){

        hash_redimencionar(hash ,hash->largo *DUPLICAR);
    }
    size_t posicion = funcion_hash(clave,hash->largo);
    size_t i;
    for (i = posicion; (hash->tabla[i].estado != VACIO) ;i++){
        if(!strcmp(hash->tabla[i].clave, clave)){
            if(hash->destruir_dato)
                hash->destruir_dato(hash->tabla[i].valor);
            hash->tabla[i].valor = dato;
            return true;
        }       
        if(i == (hash->largo - 1)) i = 0;
    }
    hash->tabla[i].estado= OCUPADO;
    hash->tabla[i].clave= clave;
    hash->tabla[i].valor = dato;
    hash->tabla[i].estado = OCUPADO;
    hash->cantidad++;
    hash->carga = (float)hash->cantidad / (float)hash->largo;
    
    return true;
}
/* Borra un elemento del hash y devuelve el dato asociado.  Devuelve    
 * NULL si el datio no estaba.
 * Pre: La estructura hash fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 */
void *hash_borrar(hash_t *hash, const char *clave){
    if(!hash_cantidad(hash))
        return NULL;
    if(TAM_INICIAL<hash->largo){
        if(hash->carga  <= FACTOR_CARGA_MIN )
            hash_redimencionar(hash ,hash->largo * MITAD);
    }
    size_t posicion = funcion_hash(clave,hash->largo);
    size_t cont = posicion;
    while(strcmp(hash->tabla[cont].clave,clave) && posicion < hash->largo){
        if(cont == hash->largo - 1)
            posicion = 0;
        if(cont == posicion - 1 || !hash->tabla[cont].estado)
            return NULL;
        cont++;
    }
    if(hash->tabla[cont].estado==BORRADO) 
        return NULL;
    hash->cantidad--;
    hash->tabla[cont].estado = BORRADO;
    return hash->tabla[posicion].valor;
    }

/* Obtiene el valor de un elemento del hash, si la clave no se encuentra
 * devuelve NULL.
 * Pre: La estructura hash fue inicializada
 */
void *hash_obtener(const hash_t *hash, const char *clave){
    if(!hash_cantidad(hash))
        return NULL;

    size_t posicion = funcion_hash(clave,hash->largo);
     while(hash->tabla[posicion].estado!=VACIO){

        if(!strcmp(hash->tabla[posicion].clave,clave)&& hash->tabla[posicion].estado== OCUPADO ){
            return hash->tabla[posicion].valor;
        }
        if (posicion == hash->largo -1) 
            posicion = 0;
        if (posicion == (funcion_hash(clave,hash->largo )-1) )
            return NULL;

        posicion++;
    }

    return NULL  ;
}
  
 /* Determina si clave pertenece o no al hash.
 * Pre: La estructura hash fue inicializada
 */
bool hash_pertenece(const hash_t *hash, const char *clave){
    if(!hash_cantidad(hash))
        return NULL;
     size_t posicion = funcion_hash(clave,hash->largo);
     while(hash->tabla[posicion].estado!=VACIO){

        if(!strcmp(hash->tabla[posicion].clave,clave)&& hash->tabla[posicion].estado== OCUPADO ){
            return true;
        }
        if (posicion == hash->largo -1) 
            posicion = 0;
        if (posicion == (funcion_hash(clave,hash->largo )-1) )
            return NULL;

        posicion++;
    }

    return false;
}

/* Devuelve la cantidad de elementos del hash.
 * Pre: La estructura hash fue inicializada
*/
size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * hash_t *hash_crear(hash_destruir_dato_t destruir_dato); * Pre: La estructura hash fue inicializada
 * Post: La estructura hash fue destruida
 */
void hash_destruir(hash_t *hash){
    for (size_t i = 0; i < hash->largo ; i++){
        if((hash->tabla[i].estado == OCUPADO) &&  hash->destruir_dato)
            hash->destruir_dato(hash->tabla[i].valor);
    }
    free(hash->tabla);
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
		for (i=0 ; hash->tabla[i].estado ; ++i);
		iter-> contador++;
	}
	iter-> pos_iter=i;
	return iter;
}

bool hash_iter_al_final(const hash_iter_t *iter){
	if(!iter)
        return false;
	if(hash_cantidad(iter->hash_i) == iter->contador) 
        return true;
	return false;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	if(hash_iter_al_final(iter) || !iter)
		return false;
	size_t i;
	for (i = iter->pos_iter + 1; iter->hash_i->tabla[i].estado ; i++);
    iter->contador++;
	iter->pos_iter = i;
	return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if(!iter || !hash_cantidad(iter->hash_i) )
		return NULL;
	return iter->hash_i->tabla[iter-> pos_iter].clave;
}

void hash_iter_destruir(hash_iter_t* iter){
    free(iter);
}





