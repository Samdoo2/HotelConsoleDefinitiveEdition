#include "../header/gestionHuespedes.h"
#include "../header/utilidades.h"
#include "../header/gestionReservas.h"
#include "../header/gestionConsumos.h"
#include <iostream>
#include <stdexcept>
using namespace std;

static string rutaHuespedes() { return rutaArchivo("huespedes.csv"); }

vector<Huesped> cargarHuespedes() {
    vector<string> lineas = leerLineas(rutaHuespedes());
    vector<Huesped> lista;
    for (size_t i = 0; i < lineas.size(); i++) {
        try {
            Huesped h;
            h.deserializar(lineas[i]);
            for (size_t j = 0; j < lista.size(); j++)
                if (lista[j].getDocumento() == h.getDocumento())
                    throw invalid_argument("Documento duplicado.");
            lista.push_back(h);
        } catch (const exception& e) {
            throw runtime_error("huespedes.csv, linea " + to_string(i + 1) + ": " + e.what());
        }
    }
    return lista;
}

void guardarHuespedes(const vector<Huesped>& lista) {
    vector<string> lineas;
    for (size_t i = 0; i < lista.size(); i++)
        lineas.push_back(lista[i].serializar());
    escribirLineas(rutaHuespedes(), lineas);
}

void inicializarArchivoHuespedes() {
    if (!existeArchivo(rutaHuespedes())) {
        vector<string> vacio;
        escribirLineas(rutaHuespedes(), vacio);
    }
    cargarHuespedes(); // valida el formato del archivo
}

bool existeHuesped(const string& documento) {
    vector<Huesped> lista = cargarHuespedes();
    for (size_t i = 0; i < lista.size(); i++)
        if (lista[i].getDocumento() == documento) return true;
    return false;
}

void registrarHuesped(const Huesped& nuevo) {
    vector<Huesped> lista = cargarHuespedes();
    for (size_t i = 0; i < lista.size(); i++)
        if (lista[i].getDocumento() == nuevo.getDocumento())
            throw invalid_argument("Ya existe un huesped con ese documento.");
    lista.push_back(nuevo);
    guardarHuespedes(lista);
}

static void mostrarHuesped(const Huesped& h) {
    cout << h.getDocumento() << " | " << h.getNombre() << " | "
         << h.getTelefono() << " | " << h.getCorreo() << "\n";
}

static void registrarHuespedMenu() {
    string documento = leerTextoObligatorio("Documento: ");
    string nombre = leerTextoObligatorio("Nombre completo: ");
    string telefono = leerTextoObligatorio("Telefono: ");
    string correo = leerTextoObligatorio("Correo: ");
    Huesped h(documento, nombre, telefono, correo);
    registrarHuesped(h);
    cout << "Huesped registrado.\n";
}

static void listarHuespedes() {
    vector<Huesped> lista = cargarHuespedes();
    cout << "\n=== HUESPEDES (" << lista.size() << ") ===\n";
    for (size_t i = 0; i < lista.size(); i++) mostrarHuesped(lista[i]);
}

static void buscarPorDocumento() {
    string documento = leerTextoObligatorio("Documento: ");
    vector<Huesped> lista = cargarHuespedes();
    for (size_t i = 0; i < lista.size(); i++) {
        if (lista[i].getDocumento() == documento) {
            mostrarHuesped(lista[i]);
            return;
        }
    }
    cout << "No hay un huesped con ese documento.\n";
}

static void buscarPorNombre() {
    string texto = aMinusculas(leerTextoObligatorio("Nombre (o parte): "));
    vector<Huesped> lista = cargarHuespedes();
    int encontrados = 0;
    for (size_t i = 0; i < lista.size(); i++) {
        if (aMinusculas(lista[i].getNombre()).find(texto) != string::npos) {
            mostrarHuesped(lista[i]);
            encontrados++;
        }
    }
    if (encontrados == 0) cout << "No se encontraron huespedes.\n";
}

static void actualizarHuespedMenu() {
    string documento = leerTextoObligatorio("Documento del huesped a actualizar: ");
    vector<Huesped> lista = cargarHuespedes();
    for (size_t i = 0; i < lista.size(); i++) {
        if (lista[i].getDocumento() != documento) continue;
        string nombre = leerTexto("Nuevo nombre (Enter para dejarlo igual): ");
        string telefono = leerTexto("Nuevo telefono (Enter para dejarlo igual): ");
        string correo = leerTexto("Nuevo correo (Enter para dejarlo igual): ");
        if (nombre.empty()) nombre = lista[i].getNombre();
        if (telefono.empty()) telefono = lista[i].getTelefono();
        if (correo.empty()) correo = lista[i].getCorreo();
        lista[i].actualizarDatos(nombre, telefono, correo);
        guardarHuespedes(lista);
        cout << "Datos actualizados.\n";
        return;
    }
    throw invalid_argument("No existe un huesped con ese documento.");
}

// Muestra las reservas del huesped y los servicios consumidos en cada una.
static void verHistorialHuesped() {
    string documento = leerTextoObligatorio("Documento del huesped: ");
    if (!existeHuesped(documento)) throw invalid_argument("No existe un huesped con ese documento.");
    vector<Reserva> reservas = cargarReservas();
    int cantidad = 0;
    cout << "\n=== HISTORIAL DEL HUESPED " << documento << " ===\n";
    for (size_t i = 0; i < reservas.size(); i++) {
        if (reservas[i].getDocumentoHuesped() != documento) continue;
        mostrarResumenReserva(reservas[i]);
        mostrarConsumosDeReserva(reservas[i].getCodigo());
        cantidad++;
    }
    if (cantidad == 0) cout << "Este huesped no tiene reservas.\n";
}

void menuHuespedes() {
    int opcion;
    do {
        cout << "\n=== GESTION DE HUESPEDES ===\n"
             << "1. Registrar huesped\n"
             << "2. Listar huespedes\n"
             << "3. Buscar por documento\n"
             << "4. Buscar por nombre\n"
             << "5. Actualizar datos\n"
             << "6. Ver historial de reservas y consumos\n"
             << "0. Volver\n";
        opcion = leerOpcion();
        try {
            switch (opcion) {
                case 1: registrarHuespedMenu(); break;
                case 2: listarHuespedes(); break;
                case 3: buscarPorDocumento(); break;
                case 4: buscarPorNombre(); break;
                case 5: actualizarHuespedMenu(); break;
                case 6: verHistorialHuesped(); break;
                case 0: break;
                default: cout << "Opcion no valida.\n";
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    } while (opcion != 0);
}
