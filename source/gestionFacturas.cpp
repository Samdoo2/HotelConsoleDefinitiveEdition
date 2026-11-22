#include "../header/gestionFacturas.h"
#include "../header/gestionReservas.h"
#include "../header/gestionConsumos.h"
#include "../header/utilidades.h"
#include "../header/Fecha.h"
#include <iostream>
#include <stdexcept>
using namespace std;

// Impuesto aplicado a la factura (19%). Si cambia, se cambia solo aqui.
static const double TASA_IMPUESTO = 0.19;

static string rutaFacturas() { return rutaArchivo("facturas.csv"); }

vector<Factura> cargarFacturas() {
    vector<string> lineas = leerLineas(rutaFacturas());
    vector<Factura> lista;
    for (size_t i = 0; i < lineas.size(); i++) {
        try {
            Factura f = Factura::deserializar(lineas[i]);
            for (size_t j = 0; j < lista.size(); j++)
                if (lista[j].getCodigo() == f.getCodigo())
                    throw invalid_argument("Codigo de factura repetido.");
            lista.push_back(f);
        } catch (const exception& e) {
            throw runtime_error("facturas.csv, linea " + to_string(i + 1) + ": " + e.what());
        }
    }
    return lista;
}

static void guardarFacturas(const vector<Factura>& lista) {
    vector<string> lineas;
    for (size_t i = 0; i < lista.size(); i++)
        lineas.push_back(lista[i].serializar());
    escribirLineas(rutaFacturas(), lineas);
}

void inicializarArchivoFacturas() {
    if (!existeArchivo(rutaFacturas())) {
        vector<string> vacio;
        escribirLineas(rutaFacturas(), vacio);
    }
    cargarFacturas(); // valida el formato del archivo
}

bool reservaTieneFactura(const string& codigoReserva) {
    vector<Factura> lista = cargarFacturas();
    for (size_t i = 0; i < lista.size(); i++)
        if (lista[i].getCodigoReserva() == codigoReserva) return true;
    return false;
}

static void mostrarDetalleFactura(const Factura& f) {
    Reserva r = obtenerReserva(f.getCodigoReserva());
    cout << "\n===== FACTURA " << f.getCodigo() << " =====\n"
         << "Fecha de emision: " << f.getFechaEmision() << "\n"
         << "Reserva: " << r.getCodigo() << " | Huesped: " << r.getDocumentoHuesped()
         << " | Habitacion: " << r.getCodigoHabitacion() << "\n"
         << "Estadia: " << r.getFechaEntrada() << " a " << r.getFechaSalida() << "\n"
         << "Noches: " << r.calcularNoches() << " x $" << dinero(r.getTarifaPorNoche())
         << " = $" << dinero(f.getValorNoches()) << "\n"
         << "Servicios consumidos:\n";
    mostrarConsumosDeReserva(r.getCodigo());
    cout << "Subtotal servicios: $" << dinero(f.getSubtotalServicios()) << "\n"
         << "Subtotal (noches + servicios): $"
         << dinero(f.getValorNoches() + f.getSubtotalServicios()) << "\n"
         << "Impuestos (" << (int)(TASA_IMPUESTO * 100) << "%): $" << dinero(f.getImpuestos()) << "\n"
         << "TOTAL A PAGAR: $" << dinero(f.getTotalPagar()) << "\n"
         << "Estado de pago: " << f.getEstadoPago() << "\n";
}

static void generarFacturaMenu() {
    string codigoReserva = leerTextoObligatorio("Codigo de la reserva a facturar: ");
    Reserva r = obtenerReserva(codigoReserva);
    if (!r.estaActiva())
        throw invalid_argument("No se puede facturar una reserva cancelada.");
    if (reservaTieneFactura(codigoReserva))
        throw invalid_argument("Esa reserva ya tiene factura.");

    vector<Factura> lista = cargarFacturas();
    Factura nueva(generarCodigo("F", (int)lista.size() + 1), codigoReserva, Fecha::hoy(),
                  r.calcularValorEstimado(), subtotalServicios(codigoReserva), TASA_IMPUESTO);
    mostrarDetalleFactura(nueva);
    if (!confirmar("Guardar la factura")) {
        cout << "Factura descartada.\n";
        return;
    }
    lista.push_back(nueva);
    guardarFacturas(lista);
    cout << "Factura guardada con codigo " << nueva.getCodigo() << ".\n";
}

static void listarFacturas() {
    vector<Factura> lista = cargarFacturas();
    cout << "\n=== FACTURAS (" << lista.size() << ") ===\n";
    for (size_t i = 0; i < lista.size(); i++) {
        cout << lista[i].getCodigo() << " | Reserva " << lista[i].getCodigoReserva()
             << " | " << lista[i].getFechaEmision()
             << " | Total $" << dinero(lista[i].getTotalPagar())
             << " | " << lista[i].getEstadoPago() << "\n";
    }
}

static void buscarPorCodigoFactura() {
    string codigo = leerTextoObligatorio("Codigo de la factura: ");
    vector<Factura> lista = cargarFacturas();
    for (size_t i = 0; i < lista.size(); i++) {
        if (lista[i].getCodigo() == codigo) {
            mostrarDetalleFactura(lista[i]);
            return;
        }
    }
    cout << "No existe esa factura.\n";
}

static void buscarPorReservaFactura() {
    string codigoReserva = leerTextoObligatorio("Codigo de la reserva: ");
    vector<Factura> lista = cargarFacturas();
    for (size_t i = 0; i < lista.size(); i++) {
        if (lista[i].getCodigoReserva() == codigoReserva) {
            mostrarDetalleFactura(lista[i]);
            return;
        }
    }
    cout << "Esa reserva no tiene factura.\n";
}

static void registrarPagoMenu() {
    string codigo = leerTextoObligatorio("Codigo de la factura: ");
    vector<Factura> lista = cargarFacturas();
    for (size_t i = 0; i < lista.size(); i++) {
        if (lista[i].getCodigo() != codigo) continue;
        if (lista[i].getEstadoPagoEnum() == EstadoPago::Pagada)
            throw invalid_argument("Esa factura ya estaba pagada.");
        lista[i].registrarPago();
        guardarFacturas(lista);
        cout << "Pago registrado. La factura quedo como Pagada.\n";
        return;
    }
    throw invalid_argument("No existe esa factura.");
}

void menuFacturacion() {
    int opcion;
    do {
        cout << "\n=== FACTURACION ===\n"
             << "1. Generar factura de una reserva\n"
             << "2. Ver todas las facturas\n"
             << "3. Ver detalle por codigo de factura\n"
             << "4. Buscar factura por codigo de reserva\n"
             << "5. Registrar pago de una factura\n"
             << "0. Volver\n";
        opcion = leerOpcion();
        try {
            switch (opcion) {
                case 1: generarFacturaMenu(); break;
                case 2: listarFacturas(); break;
                case 3: buscarPorCodigoFactura(); break;
                case 4: buscarPorReservaFactura(); break;
                case 5: registrarPagoMenu(); break;
                case 0: break;
                default: cout << "Opcion no valida.\n";
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    } while (opcion != 0);
}
