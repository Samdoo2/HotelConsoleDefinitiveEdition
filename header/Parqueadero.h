#pragma once

#include "Servicio.h"
#include <string>
using namespace std;

// Servicio de parqueadero: se cobra por dia.
class Parqueadero : public Servicio {
private:
    string tipoVehiculo;

public:
    Parqueadero();
    Parqueadero(string codigo, string nombre, string descripcion,
                double precio, bool activo, string tipoVehiculo);

    string getTipoVehiculo() const;
    string getUnidadCobro() const;

    string getTipoServicio() const override;
    string describir() const override;
    double calcularCosto(int cantidad) const override;
    string serializar() const override;
    void deserializar(const string& linea);
};
