#include "../header/gestionUsuarios.h"
#include "../header/administrador.h"
#include "../header/recepcionista.h"
#include "../header/utilidades.h"
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace std;

static string rutaUsuarios() { return rutaArchivo("usuarios.csv"); }

// Fabrica: segun el rol crea el objeto hijo correspondiente (polimorfismo).
static Usuario* crearUsuario(const string& nombre, const string& clave,
                             const string& rol, const string& turno) {
    if (rol == "Administrador" || rol == "Admin") return new Administrador(nombre, clave);
    if (rol == "Recepcionista") return new Recepcionista(nombre, clave, turno);
    throw invalid_argument("Tipo de usuario invalido.");
}

static void liberarUsuarios(vector<Usuario*>& lista) {
    for (size_t i = 0; i < lista.size(); i++) delete lista[i];
    lista.clear();
}

static int buscarUsuario(const vector<Usuario*>& lista, const string& nombre) {
    for (size_t i = 0; i < lista.size(); i++)
        if (lista[i]->getNombreUsuario() == nombre) return (int)i;
    return -1;
}

static vector<Usuario*> cargarUsuarios() {
    vector<string> lineas = leerLineas(rutaUsuarios());
    vector<Usuario*> lista;
    try {
        for (size_t i = 0; i < lineas.size(); i++) {
            vector<string> c = dividirCampos(lineas[i]);
            if (c.size() < 3)
                throw invalid_argument("linea " + to_string(i + 1) + " con formato incorrecto");
            string turno = "";
            if (c.size() >= 4) turno = c[3];
            Usuario* u = crearUsuario(c[0], c[1], c[2], turno);
            if (buscarUsuario(lista, u->getNombreUsuario()) != -1) {
                delete u;
                throw invalid_argument("usuario repetido en la linea " + to_string(i + 1));
            }
            lista.push_back(u);
        }
    } catch (const exception& e) {
        liberarUsuarios(lista);
        throw runtime_error(string("usuarios.csv: ") + e.what());
    }
    return lista;
}

static void guardarUsuarios(const vector<Usuario*>& lista) {
    vector<string> lineas;
    for (size_t i = 0; i < lista.size(); i++)
        lineas.push_back(lista[i]->serializar()); // polimorfismo
    escribirLineas(rutaUsuarios(), lineas);
}

void inicializarArchivoUsuarios() {
    bool vacio = true;
    if (existeArchivo(rutaUsuarios())) vacio = leerLineas(rutaUsuarios()).empty();
    if (vacio) {
        Administrador admin("admin", "12345");
        vector<string> lineas;
        lineas.push_back(admin.serializar());
        escribirLineas(rutaUsuarios(), lineas);
    } else {
        vector<Usuario*> lista = cargarUsuarios(); // valida el formato
        liberarUsuarios(lista);
    }
}

Usuario* iniciarSesion(const string& nombre, const string& clave) {
    vector<Usuario*> lista = cargarUsuarios();
    Usuario* encontrado = 0;
    for (size_t i = 0; i < lista.size(); i++) {
        if (lista[i]->getNombreUsuario() == nombre && lista[i]->autenticar(clave)) {
            encontrado = lista[i];
            lista.erase(lista.begin() + i);
            break;
        }
    }
    liberarUsuarios(lista);
    if (encontrado == 0) throw invalid_argument("Credenciales incorrectas.");
    return encontrado;
}

static void listarUsuarios() {
    vector<Usuario*> lista = cargarUsuarios();
    cout << "\n=== USUARIOS (" << lista.size() << ") ===\n";
    for (size_t i = 0; i < lista.size(); i++) {
        cout << lista[i]->getNombreUsuario() << " | " << lista[i]->getRol();
        if (lista[i]->getRol() == "Recepcionista")
            cout << " | Turno: " << lista[i]->getDatoExtra();
        cout << "\n";
    }
    liberarUsuarios(lista);
}

static void crearUsuarioMenu() {
    string nombre = leerTextoObligatorio("Nombre de usuario: ");
    string clave = leerTextoObligatorio("Contrasena: ");
    int tipo = leerEntero("Tipo (1=Administrador, 2=Recepcionista): ", 1, 2);
    string turno = "";
    if (tipo == 2) turno = leerTextoObligatorio("Turno (Diurno/Nocturno): ");

    vector<Usuario*> lista = cargarUsuarios();
    try {
        if (buscarUsuario(lista, nombre) != -1)
            throw invalid_argument("Ya existe un usuario con ese nombre.");
        if (tipo == 1) lista.push_back(crearUsuario(nombre, clave, "Administrador", ""));
        else lista.push_back(crearUsuario(nombre, clave, "Recepcionista", turno));
        guardarUsuarios(lista);
    } catch (...) {
        liberarUsuarios(lista);
        throw;
    }
    liberarUsuarios(lista);
    cout << "Usuario creado y guardado.\n";
}

static void editarUsuarioMenu() {
    string nombre = leerTextoObligatorio("Usuario a editar: ");
    vector<Usuario*> lista = cargarUsuarios();
    try {
        int pos = buscarUsuario(lista, nombre);
        if (pos == -1) throw invalid_argument("No existe ese usuario.");
        Usuario* u = lista[pos];
        string nuevoNombre = leerTexto("Nuevo nombre de usuario (Enter para dejarlo igual): ");
        if (!nuevoNombre.empty() && nuevoNombre != nombre) {
            if (buscarUsuario(lista, nuevoNombre) != -1)
                throw invalid_argument("Ya existe un usuario con ese nombre.");
            u->cambiarNombre(nuevoNombre);
        }
        string clave = leerTexto("Nueva contrasena (Enter para dejarla igual): ");
        if (!clave.empty()) u->cambiarContrasena(clave);
        if (u->getRol() == "Recepcionista") {
            string turno = leerTexto("Nuevo turno (Enter para dejarlo igual): ");
            if (!turno.empty()) u->setDatoExtra(turno);
        }
        guardarUsuarios(lista);
    } catch (...) {
        liberarUsuarios(lista);
        throw;
    }
    liberarUsuarios(lista);
    cout << "Usuario actualizado.\n";
}

static void eliminarUsuarioMenu(const Usuario& actual) {
    string nombre = leerTextoObligatorio("Usuario a eliminar: ");
    if (nombre == actual.getNombreUsuario())
        throw invalid_argument("No puedes eliminar tu propio usuario.");

    vector<Usuario*> lista = cargarUsuarios();
    try {
        int pos = buscarUsuario(lista, nombre);
        if (pos == -1) throw invalid_argument("No existe ese usuario.");
        int admins = 0;
        for (size_t i = 0; i < lista.size(); i++)
            if (lista[i]->getRol() == "Administrador") admins++;
        if (lista[pos]->getRol() == "Administrador" && admins <= 1)
            throw invalid_argument("Debe quedar al menos un administrador.");
        if (confirmar("Eliminar al usuario " + nombre)) {
            delete lista[pos];
            lista.erase(lista.begin() + pos);
            guardarUsuarios(lista);
            cout << "Usuario eliminado.\n";
        } else {
            cout << "Operacion cancelada.\n";
        }
    } catch (...) {
        liberarUsuarios(lista);
        throw;
    }
    liberarUsuarios(lista);
}

void menuUsuarios(const Usuario& actual) {
    if (!actual.tienePermiso("usuarios")) {
        cout << "Acceso denegado para este rol.\n";
        return;
    }
    int opcion;
    do {
        cout << "\n=== GESTION DE USUARIOS ===\n"
             << "1. Crear usuario\n"
             << "2. Listar usuarios\n"
             << "3. Editar usuario\n"
             << "4. Eliminar usuario\n"
             << "0. Volver\n";
        opcion = leerOpcion();
        try {
            switch (opcion) {
                case 1: crearUsuarioMenu(); break;
                case 2: listarUsuarios(); break;
                case 3: editarUsuarioMenu(); break;
                case 4: eliminarUsuarioMenu(actual); break;
                case 0: break;
                default: cout << "Opcion no valida.\n";
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    } while (opcion != 0);
}
