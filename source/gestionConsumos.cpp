#include "../header/gestionConsumos.h"
#include "../header/gestionReservas.h"
#include "../header/gestionServicios.h"
#include "../header/gestionFacturas.h"
#include "../header/utilidades.h"
#include <iostream>
#include <stdexcept>
using namespace std;

static string rutaConsumos() { return rutaArchivo("consumos.csv"); }

vector<ConsumoServicio> cargarConsumos() {
    vector<string> lineas = leerLineas(rutaConsumos());
    vector<ConsumoServicio> lista;
    for (size_t i = 0; i < lineas.size(); i++) {
        try {
            ConsumoServicio c = ConsumoServicio::deserializar(lineas[i]);
            for (size_t j = 0; j < lista.size(); j++)
                if (lista[j].getCodigo() == c.getCodigo())
                    throw invalid_argument("Codigo de consumo repetido.");
            lista.push_back(c);
        } catch (const exception& e) {
            throw runtime_error("consumos.csv, linea " + to_string(i + 1) + ": " + e.what());
        }
    }
    return lista;
}

static void guardarConsumos(const vector<ConsumoServicio>& lista) {
    vector<string> lineas;
    for (size_t i = 0; i < lista.size(); i++)
        lineas.push_back(lista[i].serializar());
    escribirLineas(rutaConsumos(), lineas);
}

void inicializarArchivoConsumos() {
    if (!existeArchivo(rutaConsumos())) {
        vector<string> vacio;
        escribirLineas(rutaConsumos(), vacio);
    }
    cargarConsumos(); // valida el formato del archivo
}

double subtotalServicios(const string& codigoReserva) {
    vector<ConsumoServicio> lista = cargarConsumos();
    double total = 0;
    for (size_t i = 0; i < lista.size(); i++)
        if (lista[i].getCodigoReserva() == codigoReserva)
            total = total + lista[i].getValorTotal();
    return total;
}

static void mostrarConsumo(const ConsumoServicio& c) {
    cout << c.getCodigo() << " | Reserva " << c.getCodigoReserva()
         << " | Servicio " << c.getCodigoServicio()
         << " | Cantidad " << c.getCantidad()
         << " | $" << dinero(c.getPrecioAplicado()) << " c/u"
         << " | Total $" << dinero(c.getValorTotal()) << "\n";
}

void mostrarConsumosDeReserva(const string& codigoReserva) {
    vector<ConsumoServicio> lista = cargarConsumos();
    int cantidad = 0;
    for (size_t i = 0; i < lista.size(); i++) {
        if (lista[i].getCodigoReserva() == codigoReserva) {
            cout << "   - ";
            mostrarConsumo(lista[i]);
            cantidad++;
        }
    }
    if (cantidad == 0) cout << "   (sin servicios consumidos)\n";
}

static void registrarConsumoMenu() {
    string codigoReserva = leerTextoObligatorio("Codigo de la reserva: ");
    Reserva r = obtenerReserva(codigoReserva); // si no existe, lanza error: no hay cobros sin reserva
    if (!r.estaActiva())
        throw invalid_argument("La reserva esta cancelada.");
    if (reservaTieneFactura(codigoReserva))
        throw invalid_argument("La reserva ya tiene factura, no se pueden agregar servicios.");

    cout << "\nServicios disponibles:\n";
    mostrarServiciosActivos();
    string codigoServicio = leerTextoObligatorio("Codigo del servicio: ");
    int cantidad = leerEntero("Cantidad (platos, prendas o dias): ", 1, 100);

    double costo = calcularCostoServicio(codigoServicio, cantidad);
    double precioAplicado = costo / cantidad;   // precio por unidad (incluye recargos)

    vector<ConsumoServicio> lista = cargarConsumos();
    ConsumoServicio nuevo(generarCodigo("C", (int)lista.size() + 1), codigoReserva,
                          codigoServicio, cantidad, precioAplicado);
    cout << "Valor total del consumo: $" << dinero(nuevo.getValorTotal()) << "\n";
    if (!confirmar("Guardar el consumo")) {
        cout << "Consumo descartado.\n";
        return;
    }
    lista.push_back(nuevo);
    guardarConsumos(lista);
    cout << "Consumo guardado con codigo " << nuevo.getCodigo() << ".\n";
}

static void verConsumosDeUnaReserva() {
    string codigoReserva = leerTextoObligatorio("Codigo de la reserva: ");
    obtenerReserva(codigoReserva); // verifica que exista
    cout << "\nConsumos de la reserva " << codigoReserva << ":\n";
    mostrarConsumosDeReserva(codigoReserva);
    cout << "Subtotal servicios: $" << dinero(subtotalServicios(codigoReserva)) << "\n";
}

static void verTodosLosConsumos() {
    vector<ConsumoServicio> lista = cargarConsumos();
    cout << "\n=== CONSUMOS (" << lista.size() << ") ===\n";
    for (size_t i = 0; i < lista.size(); i++) mostrarConsumo(lista[i]);
}

void menuConsumos() {
    int opcion;
    do {
        cout << "\n=== CONSUMOS DE SERVICIOS ===\n"
             << "1. Ver servicios disponibles\n"
             << "2. Registrar consumo en una reserva\n"
             << "3. Ver consumos de una reserva\n"
             << "4. Ver todos los consumos\n"
             << "0. Volver\n";
        opcion = leerOpcion();
        try {
            switch (opcion) {
                case 1: cout << "\n"; mostrarServiciosActivos(); break;
                case 2: registrarConsumoMenu(); break;
                case 3: verConsumosDeUnaReserva(); break;
                case 4: verTodosLosConsumos(); break;
                case 0: break;
                default: cout << "Opcion no valida.\n";
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    } while (opcion != 0);
}
