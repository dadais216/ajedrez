#ifndef MEMORIALOCAL_H
#define MEMORIALOCAL_H


struct MemoriaLocal{
    ///@optim meterlos en un mismo espacio seria mas rapido (usando memcpys y demas). No creo que valga la pena para esta version, para la compilada si
    vector<int> pieza;
    vector<int> local;
};

#endif // MEMORIALOCAL_H
