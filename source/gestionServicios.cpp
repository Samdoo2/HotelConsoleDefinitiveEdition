#include "../header/gestionServicios.h"
#include "../header/Alimentacion.h"
#include "../header/Lavanderia.h"
#include "../header/Parqueadero.h"
#include "../header/utilidades.h"
#include <iostream>
#include <stdexcept>
using namespace std;

static string rutaServicios() { return rutaArchivo("servicios.csv"); }

// Segun la primera palabra de la linea (el tipo) crea el objeto hijo que corresponde.
static Servicio* crearServicioDesdeLinea(const string& linea) {
    vector<string> c = dividirCampos(linea);
    if (c.empty()) throw invalid_argument("Linea de servicio vacia.");
    if (c[0] == "Alimentacion") {
        Alimentacion* s = new Alimentacion();
        try { s->deserializar(linea); } catch (...) { delete s; throw; }
        return s;
    }
    if (c[0] == "Lavanderia") {
        Lavanderia* s = new Lavanderia();
        try { s->deserializar(linea); } catch (...) { delete s; throw; }
        return s;
    }
    if (c[0] == "Parqueadero") {
        Parqueadero* s = new Parqueadero();
        try { s->deserializar(linea); } catch (...) { delete s; throw; }
        return s;
    }
    throw invalid_argument("Tipo de servicio desconocido: " + c[0]);
}

void liberarServicios(vector<Servicio*>& lista) {
    for (size_t i = 0; i < lista.size(); i++) delete lista[i];
    lista.clear();
}

static int buscarServicio(const vector<Servicio*>& lista, const string& codigo) {
    for (size_t i = 0; i < lista.size(); i++)
        if (lista[i]->getCodigo() == codigo) return (int)i;
    return -1;
}

vector<Servicio*> cargarServicios() {
    vector<string> lineas = leerLineas(rutaServicios());
    vector<Servicio*> lista;
    try {
        for (size_t i = 0; i < lineas.size(); i++) {
            Servicio* s = crearServicioDesdeLinea(lineas[i]);
            if (buscarServicio(lista, s->getCodigo()) != -1) {
                delete s;
                throw invalid_argument("codigo de servicio repetido en la linea " + to_string(i + 1));
            }
            lista.push_back(s);
        }
    } catch (const exception& e) {
        liberarServicios(lista);
        throw runtime_error(string("servicios.csv: ") + e.what());
    }
    return lista;
}

static void guardarServicios(const vector<Servicio*>& lista) {
    vector<string> lineas;
    for (size_t i = 0; i < lista.size(); i++)
        lineas.push_back(lista[i]->serializar()); // polimorfismo
    escribirLineas(rutaServicios(), lineas);
}

// Si el archivo no existe o esta vacio, crea algunos servicios de ejemplo.
void inicializarArchivoServicios() {
    bool vacio = true;
    if (existeArchivo(rutaServicios())) vacio = leerLineas(rutaServicios()).empty();
    if (vacio) {
        vector<Servicio*> lista;
        lista.push_back(new Alimentacion("AL01", "Desayuno", "Desayuno buffet", 20000, true, "Desayuno"));
        lista.push_back(new Alimentacion("AL02", "Almuerzo", "Almuerzo del dia", 35000, true, "Almuerzo"));
        lista.push_back(new Lavanderia("LA01", "Lavado normal", "Lavado por prenda", 8000, true, 0, false));
        lista.push_back(new Lavanderia("LA02", "Lavado urgente", "Lavado en 3 horas", 8000, true, 5000, true));
        lista.push_back(new Parqueadero("PA01", "Parqueadero carro", "Cobro por dia", 15000, true, "Carro"));
        lista.push_back(new Parqueadero("PA02", "Parqueadero moto", "Cobro por dia", 8000, true, "Moto"));
        guardarServicios(lista);
        liberarServicios(lista);
    } else {
        vector<Servicio*> lista = cargarServicios(); // valida el formato
        liberarServicios(lista);
    }
}

static void mostrarServicio(const Servicio* s) {
    cout << s->describir() << " | " << (s->isActivo() ? "Activo" : "Inactivo") << "\n";
}

void mostrarServiciosActivos() {
    vector<Servicio*> lista = cargarServicios();
    int cantidad = 0;
    for (size_t i = 0; i < lista.size(); i++) {
        if (lista[i]->isActivo()) {
            mostrarServicio(lista[i]);
            cantidad++;
        }
    }
    if (cantidad == 0) cout << "No hay servicios activos.\n";
    liberarServicios(lista);
}

double calcularCostoServicio(const string& codigoServicio, int cantidad) {
    vector<Servicio*> lista = cargarServicios();
    double costo = 0;
    try {
        int pos = buscarServicio(lista, codigoServicio);
        if (pos == -1) throw invalid_argument("No existe ese servicio.");
        if (!lista[pos]->isActivo()) throw invalid_argument("Ese servicio esta inactivo.");
        costo = lista[pos]->calcularCosto(cantidad); // polimorfismo: cada hija calcula distinto
    } catch (...) {
        liberarServicios(lista);
        throw;
    }
    liberarServicios(lista);
    return costo;
}

static void listarServicios() {
    vector<Servicio*> lista = cargarServicios();
    cout << "\n=== SERVICIOS (" << lista.size() << ") ===\n";
    for (size_t i = 0; i < lista.size(); i++) mostrarServicio(lista[i]);
    liberarServicios(lista);
}

static void buscarServicioMenu() {
    string texto = aMinusculas(leerTextoObligatorio("Codigo o parte del nombre: "));
    vector<Servicio*> lista = cargarServicios();
    int encontrados = 0;
    for (size_t i = 0; i < lista.size(); i++) {
        if (aMinusculas(lista[i]->getCodigo()) == texto ||
            aMinusculas(lista[i]->getNombre()).find(texto) != string::npos) {
            mostrarServicio(lista[i]);
            encontrados++;
        }
    }
    if (encontrados == 0) cout << "No se encontraron servicios.\n";
    liberarServicios(lista);
}

static void registrarServicioMenu() {
    int tipo = leerEntero("Tipo (1=Alimentacion, 2=Lavanderia, 3=Parqueadero): ", 1, 3);
    string codigo = leerTextoObligatorio("Codigo del servicio (ej. AL03): ");
    string nombre = leerTextoObligatorio("Nombre: ");
    string desc = leerTextoObligatorio("Descripcion: ");
    double precio = leerDecimal("Precio: ", 0);

    vector<Servicio*> lista = cargarServicios();
    try {
        if (buscarServicio(lista, codigo) != -1)
            throw invalid_argument("Ya existe un servicio con ese codigo.");
        if (tipo == 1) {
            string comida = leerTextoObligatorio("Tipo de comida (Desayuno/Almuerzo/Cena): ");
            lista.push_back(new Alimentacion(codigo, nombre, desc, precio, true, comida));
        } else if (tipo == 2) {
            bool urgente = confirmar("Es un servicio urgente");
            double recargo = 0;
            if (urgente) recargo = leerDecimal("Recargo por prenda: ", 0);
            lista.push_back(new Lavanderia(codigo, nombre, desc, precio, true, recargo, urgente));
        } else {
            string vehiculo = leerTextoObligatorio("Tipo de vehiculo (Carro/Moto): ");
            lista.push_back(new Parqueadero(codigo, nombre, desc, precio, true, vehiculo));
        }
        guardarServicios(lista);
    } catch (...) {
        liberarServicios(lista);
        throw;
    }
    liberarServicios(lista);
    cout << "Servicio registrado.\n";
}

static void actualizarServicioMenu() {
    string codigo = leerTextoObligatorio("Codigo del servicio a actualizar: ");
    vector<Servicio*> lista = cargarServicios();
    try {
        int pos = buscarServicio(lista, codigo);
        if (pos == -1) throw invalid_argument("No existe ese servicio.");
        string nombre = leerTextoObligatorio("Nuevo nombre: ");
        string desc = leerTextoObligatorio("Nueva descripcion: ");
        double precio = leerDecimal("Nuevo precio: ", 0);
        lista[pos]->actualizarDatos(nombre, desc, precio);
        guardarServicios(lista);
    } catch (...) {
        liberarServicios(lista);
        throw;
    }
    liberarServicios(lista);
    cout << "Servicio actualizado.\n";
}

static void activarDesactivarMenu() {
    string codigo = leerTextoObligatorio("Codigo del servicio: ");
    int opcion = leerEntero("1=Activar, 2=Desactivar: ", 1, 2);
    vector<Servicio*> lista = cargarServicios();
    try {
        int pos = buscarServicio(lista, codigo);
        if (pos == -1) throw invalid_argument("No existe ese servicio.");
        if (opcion == 1) lista[pos]->activar();
        else lista[pos]->desactivar();
        guardarServicios(lista);
    } catch (...) {
        liberarServicios(lista);
        throw;
    }
    liberarServicios(lista);
    cout << "Estado del servicio actualizado.\n";
}

void menuServicios(const Usuario& usuario) {
    if (!usuario.tienePermiso("servicios_gestion")) {
        cout << "Acceso denegado para este rol.\n";
        return;
    }
    int opcion;
    do {
        cout << "\n=== GESTION DE SERVICIOS ===\n"
             << "1. Registrar servicio\n"
             << "2. Listar servicios\n"
             << "3. Buscar servicio\n"
             << "4. Actualizar nombre, descripcion y precio\n"
             << "5. Activar o desactivar servicio\n"
             << "0. Volver\n";
        opcion = leerOpcion();
        try {
            switch (opcion) {
                case 1: registrarServicioMenu(); break;
                case 2: listarServicios(); break;
                case 3: buscarServicioMenu(); break;
                case 4: actualizarServicioMenu(); break;
                case 5: activarDesactivarMenu(); break;
                case 0: break;
                default: cout << "Opcion no valida.\n";
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    } while (opcion != 0);
}
