#pragma once
#include <string>
using namespace std;

// Un servicio usado durante una estadia. Siempre pertenece a una reserva.
class ConsumoServicio {
protected:
    string codigo;
    string codigoReserva;
    string codigoServicio;
    int cantidad;
    double precioAplicado;   // precio por unidad con el que se cobro
    double valorTotal;       // precioAplicado * cantidad

public:
    ConsumoServicio();
    ConsumoServicio(string codigo, string codigoReserva, string codigoServicio,int cantidad, double precioAplicado);
    string getCodigo() const;
    string getCodigoReserva() const;
    string getCodigoServicio() const;
    int getCantidad() const;
    double getPrecioAplicado() const;
    double getValorTotal() const;
    void actualizarCantidad(int nuevaCantidad);

    string serializar() const;
    static ConsumoServicio deserializar(const string& linea);
};
