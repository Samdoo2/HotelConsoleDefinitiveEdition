#pragma once

#include "Servicio.h"
#include <string>
using namespace std;

// Servicio de alimentacion: se cobra por plato.
class Alimentacion : public Servicio {
private:
    string tipoComida;

public:
    Alimentacion();
    Alimentacion(string codigo, string nombre, string descripcion,
                 double precio, bool activo, string tipoComida);

    string getTipoComida() const;
    string getUnidadCobro() const;

    string getTipoServicio() const override;
    string describir() const override;
    double calcularCosto(int cantidad) const override;
    string serializar() const override;
    void deserializar(const string& linea);
};
