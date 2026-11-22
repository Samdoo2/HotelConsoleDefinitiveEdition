#pragma once

#include <string>
using namespace std;

// Clase base abstracta de los servicios adicionales del hotel.
// Sus hijas (Alimentacion, Lavanderia, Parqueadero) calculan el costo de forma diferente.
class Servicio {
protected:
    string codigo;
    string nombre;
    string desc;
    double precio;
    bool activo;

public:
    Servicio();
    Servicio(string codigo, string nombre, string desc, double precio, bool activo);
    virtual ~Servicio() = default;

    virtual double calcularCosto(int cantidad) const = 0;
    virtual string describir() const = 0;
    virtual string getTipoServicio() const = 0;
    virtual string serializar() const;

    void activar();
    void desactivar();
    void actualizarDatos(string nuevoNombre, string nuevaDesc, double nuevoPrecio);
    void validar() const;

    string getCodigo() const;
    string getNombre() const;
    string getDesc() const;
    double getPrecio() const;
    bool isActivo() const;
};
