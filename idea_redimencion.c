size_t redimenciones[TOPE_REDIMENCION] ={ 197, 389 , 787, 1579 , 3163 , 6299, 12601, 25189, 50377 };
    size_t contador= 0;
    size_t posicion;
    if(FACTOR_CARGA_MAX < hash->carga){
        if(contador <TOPE_REDIMENCION){
            hash_redimencionar(hash ,redimenciones[contador]);
            contador++;
        }else{
            hash_redimencionar(hash ,redimenciones[contador-1] * DUPLICAR);

        }