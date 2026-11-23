#include "../header/ConsumoServicio.h"
#include "../header/utilidades.h"
#include <stdexcept>
#include <vector>

ConsumoServicio::ConsumoServicio() : cantidad(0), precioAplicado(0), valorTotal(0) {}
ConsumoServicio::ConsumoServicio(string codigo, string codigoReserva, string codigoServicio,int cantidad, double precioAplicado): codigo(codigo), codigoReserva(codigoReserva), codigoServicio(codigoServicio),
cantidad(0), precioAplicado(precioAplicado), valorTotal(0) {
if (codigo.empty() || codigoReserva.empty() || codigoServicio.empty() || precioAplicado <= 0)
   throw invalid_argument("Datos de consumo de servicio invalidos.");
    actualizarCantidad(cantidad);
}
string ConsumoServicio::getCodigo() const { return codigo; }
string ConsumoServicio::getCodigoReserva() const { return codigoReserva; }
string ConsumoServicio::getCodigoServicio() const { return codigoServicio; }
int ConsumoServicio::getCantidad() const { return cantidad; }
double ConsumoServicio::getPrecioAplicado() const { return precioAplicado; }
double ConsumoServicio::getValorTotal() const { return valorTotal; }



void ConsumoServicio::actualizarCantidad(int nuevaCantidad) {
    if (nuevaCantidad <= 0) throw invalid_argument("La cantidad debe ser positiva.");
    cantidad = nuevaCantidad;
    valorTotal = precioAplicado * cantidad;
}

// Formato del CSV: codigo,codigoReserva,codigoServicio,cantidad,precioAplicado
string ConsumoServicio::serializar() const {
    return codigo + "," + codigoReserva + "," + codigoServicio + "," +
           to_string(cantidad) + "," + to_string(precioAplicado);
}


// Esta funcion convierte una linea de texto en un objeto ConsumoServicio
ConsumoServicio ConsumoServicio::deserializar(const string& linea) {
    vector<string> c = dividirCampos(linea);
    if (c.size() != 5)
        throw invalid_argument("La linea de consumo debe tener 5 campos.");
    int cantidad = 0;
    double precio = 0;
    try {
        cantidad = stoi(c[3]);
        precio = stod(c[4]);
    } catch (...) {
        throw invalid_argument("Cantidad o precio del consumo con formato incorrecto.");
    }
    return ConsumoServicio(c[0], c[1], c[2], cantidad, precio);
}
