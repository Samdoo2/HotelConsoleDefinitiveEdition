#include "../header/Factura.h"
#include "../header/Fecha.h"
#include "../header/utilidades.h"
#include <stdexcept>
#include <vector>
using namespace std;



Factura::Factura():valorNoches(0),subtotalServicios(0),impuestos(0),totalPagar(0),estadoPago("Pendiente") {}
Factura::Factura(string codigo, string codigoReserva, string fechaEmision,double valorNoches, double subtotalServicios, double tasaImpuesto)
:codigo(codigo), codigoReserva(codigoReserva), fechaEmision(fechaEmision),valorNoches(valorNoches),subtotalServicios(subtotalServicios),
impuestos(0), totalPagar(0), estadoPago("Pendiente") {
    if (codigo.empty() || codigoReserva.empty() || !Fecha::esValida(fechaEmision))
     throw invalid_argument("Datos de factura invalidos.");
    if (valorNoches < 0 || subtotalServicios < 0 || tasaImpuesto < 0 || tasaImpuesto > 1)
        throw invalid_argument("Los valores de factura o la tasa de impuesto son invalidos.");
    impuestos = (valorNoches + subtotalServicios) * tasaImpuesto;
    totalPagar = valorNoches + subtotalServicios + impuestos;
}

// Getters
string Factura::getCodigo() const { return codigo; }
string Factura::getCodigoReserva() const { return codigoReserva; }
string Factura::getFechaEmision() const { return fechaEmision; }
double Factura::getValorNoches() const { return valorNoches; }
double Factura::getSubtotalServicios() const { return subtotalServicios; }
double Factura::getImpuestos() const { return impuestos; }
double Factura::getTotalPagar() const { return totalPagar; }
string Factura::getEstadoPago() const { return estadoPago; }
EstadoPago Factura::getEstadoPagoEnum() const { return convertirEstadoPago(estadoPago); }
void Factura::registrarPago() { estadoPago = "Pagada"; }
void Factura::setEstadoPago(string nuevoEstado) {
    convertirEstadoPago(nuevoEstado);
    estadoPago = nuevoEstado;
}

// Formato del CSV: codigo,codigoReserva,fecha,valorNoches,subtotalServicios,impuestos,total,estadoPago
string Factura::serializar() const {
    return codigo + "," + codigoReserva + "," + fechaEmision + "," +
           to_string(valorNoches) + "," + to_string(subtotalServicios) + "," +
           to_string(impuestos) + "," + to_string(totalPagar) + "," + estadoPago;
}

Factura Factura::deserializar(const string& linea) {
    vector<string> c = dividirCampos(linea);
    if (c.size() != 8)
        throw invalid_argument("La linea de factura debe tener 8 campos.");
    if (c[0].empty() || c[1].empty() || !Fecha::esValida(c[2]))
        throw invalid_argument("Datos de factura invalidos.");
    Factura f;
    try {
        f.valorNoches = stod(c[3]);
        f.subtotalServicios = stod(c[4]);
        f.impuestos = stod(c[5]);
        f.totalPagar = stod(c[6]);
    } catch (...) {
        throw invalid_argument("Valores de factura con formato incorrecto.");
    }
    convertirEstadoPago(c[7]); // lanza error si el estado no es valido
    f.codigo = c[0];
    f.codigoReserva = c[1];
    f.fechaEmision = c[2];
    f.estadoPago = c[7];
    return f;
}
