// =============================================================
// WattControl v2 - Gestor de energia de un minimercado
// Paso 3: composicion, agregacion y asociacion
// =============================================================

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>

using namespace std;

// =============================================================
// CONSTANTES DEL PROYECTO
// =============================================================

#define HORAS_DIA 24

const float POTENCIA_REFRIGERADORES = 2.5f;
const float POTENCIA_AIRE = 3.5f;
const float POTENCIA_CALENTADOR = 2.0f;

const float POTENCIA_CONTRATADA = 7.0f;

const float PRECIO_CARO = 950.0f;

const int HORA_PICO_INICIO = 18;
const int HORA_PICO_FIN = 20;

const float NIVEL_EMBALSE_ALERTA = 40.0f;

const int HORAS_MINIMAS_AIRE = 8;
const int HORAS_MINIMAS_CALENTADOR = 2;

const float PRECIO_EMERGENCIA = 1500.0f;

const int FALLA_CADA = 20;


// =============================================================
// CLASE Carga: clase base abstracta
// =============================================================

class Carga {

protected:

    string nombre;
    float potenciaKW;
    bool encendida;
    int horasUso;

public:

    Carga(string n, float p) {
        nombre = n;
        potenciaKW = p;
        encendida = false;
        horasUso = 0;
    }

    virtual ~Carga() {}

    virtual bool debeEncender(int hora, float precio, float nivel) = 0;

    void encender() {
        encendida = true;
        horasUso++;
    }

    void apagar() {
        encendida = false;
    }

    float costoHora(float precio) {
        if (encendida) {
            return potenciaKW * precio;
        }

        return 0.0f;
    }

    string getNombre() {
        return nombre;
    }

    float getPotencia() {
        return potenciaKW;
    }

    bool estaEncendida() {
        return encendida;
    }

    int getHorasUso() {
        return horasUso;
    }
};


// =============================================================
// CLASE CargaCritica: hereda de Carga
// =============================================================

class CargaCritica : public Carga {

public:

    CargaCritica(string n, float p) : Carga(n, p) {}

    bool debeEncender(int hora, float precio, float nivel) override {

        // La carga critica siempre funciona
        return true;
    }
};


// =============================================================
// CLASE CargaAplazable: hereda de Carga
// =============================================================

class CargaAplazable : public Carga {

public:

    CargaAplazable(string n, float p) : Carga(n, p) {}

    bool debeEncender(int hora, float precio, float nivel) override {

        bool horaPico = hora >= HORA_PICO_INICIO &&
                        hora <= HORA_PICO_FIN;

        // Se aplaza por embalses bajos, hora pico o precio caro
        if (nivel < NIVEL_EMBALSE_ALERTA ||
            horaPico ||
            precio > PRECIO_CARO) {

            return false;
        }

        return true;
    }
};


// =============================================================
// CLASE Medidor: composicion con Negocio
// =============================================================

// Medidor acumula el consumo y el costo de energia
class Medidor {

private:

    // Energia acumulada en kWh
    float energiaAcumulada;

    // Costo acumulado en pesos colombianos
    float costoAcumulado;

public:

    // Constructor: inicia los acumulados en cero
    Medidor() {
        energiaAcumulada = 0.0f;
        costoAcumulado = 0.0f;
    }

    // Registra el consumo y el costo de una hora
    void registrar(float energia, float costo) {
        energiaAcumulada += energia;
        costoAcumulado += costo;
    }

    // Devuelve los kWh acumulados
    float getEnergia() {
        return energiaAcumulada;
    }

    // Devuelve el costo acumulado
    float getCosto() {
        return costoAcumulado;
    }
};


// =============================================================
// CLASE Mercado: asociacion con Negocio
// =============================================================

// Mercado proporciona el precio de energia y el nivel de embalses
class Mercado {

public:

    // Precio de prueba para esta etapa
    float obtenerPrecio(int hora) {

        // En esta prueba usamos un precio fijo
        return 700.0f;
    }

    // Nivel de embalses de prueba
    float obtenerNivelEmbalses(int hora) {

        // En esta prueba usamos un nivel fijo del 60 %
        return 60.0f;
    }
};


// =============================================================
// CLASE Negocio: agrega cargas, contiene Medidor y se asocia a Mercado
// =============================================================

class Negocio {

private:

    // Nombre del negocio
    string nombre;

    // ---------------------------------------------------------
    // COMPOSICION
    // El Medidor se crea como parte del Negocio
    // ---------------------------------------------------------

    Medidor medidor;

    // ---------------------------------------------------------
    // ASOCIACION
    // Negocio conoce a Mercado mediante un puntero
    // ---------------------------------------------------------

    Mercado* mercado;

    // ---------------------------------------------------------
    // AGREGACION
    // Negocio guarda punteros a cargas creadas externamente
    // ---------------------------------------------------------

    Carga* cargas[3];

    // Cantidad de cargas registradas
    int cantidadCargas;

public:

    // ---------------------------------------------------------
    // Constructor de Negocio
    // ---------------------------------------------------------

    Negocio(string n, Mercado* m,
            Carga* carga1, Carga* carga2, Carga* carga3) {

        nombre = n;

        // Recibe la referencia al mercado
        mercado = m;

        // Agrega las cargas que ya fueron creadas afuera
        cargas[0] = carga1;
        cargas[1] = carga2;
        cargas[2] = carga3;

        cantidadCargas = 3;
    }


    // ---------------------------------------------------------
    // Metodo para operar durante una hora
    // ---------------------------------------------------------

    void operarHora(int hora) {

        // Consulta al Mercado
        float precio = mercado->obtenerPrecio(hora);
        float nivel = mercado->obtenerNivelEmbalses(hora);

        // Variables para calcular la potencia encendida
        float potenciaTotal = 0.0f;

        // Variables para calcular consumo y costo de la hora
        float energiaHora = 0.0f;
        float costoHoraTotal = 0.0f;

        cout << endl;
        cout << "========================================" << endl;
        cout << "NEGOCIO: " << nombre << endl;
        cout << "Hora: " << hora << endl;
        cout << "Precio: " << precio << " COP/kWh" << endl;
        cout << "Embalses: " << nivel << "%" << endl;
        cout << "----------------------------------------" << endl;


        // -----------------------------------------------------
        // Consultar las decisiones de cada carga
        // -----------------------------------------------------

        for (int i = 0; i < cantidadCargas; i++) {

            bool decision = cargas[i]->debeEncender(
                hora,
                precio,
                nivel
            );

            if (decision) {

                cargas[i]->encender();

                potenciaTotal += cargas[i]->getPotencia();

                cout << cargas[i]->getNombre()
                     << ": ENCENDIDO" << endl;
            }
            else {

                cargas[i]->apagar();

                cout << cargas[i]->getNombre()
                     << ": APLAZADO" << endl;
            }
        }


        // -----------------------------------------------------
        // REGLA 6: controlar la potencia contratada
        // -----------------------------------------------------

        // Si la potencia supera los 7 kW, apagar la carga
        // aplazable de mayor potencia que este encendida

        while (potenciaTotal > POTENCIA_CONTRATADA) {

            int indiceMayor = -1;
            float mayorPotencia = 0.0f;

            // Se revisan solo las cargas aplazables
            for (int i = 1; i < cantidadCargas; i++) {

                if (cargas[i]->estaEncendida() &&
                    cargas[i]->getPotencia() > mayorPotencia) {

                    mayorPotencia = cargas[i]->getPotencia();
                    indiceMayor = i;
                }
            }

            // Si no se encuentra una carga aplazable,
            // se termina el control de potencia
            if (indiceMayor == -1) {
                break;
            }

            // Apaga la carga aplazable de mayor potencia
            cargas[indiceMayor]->apagar();

            // Resta su potencia al total
            potenciaTotal -= cargas[indiceMayor]->getPotencia();

            cout << "ALERTA: potencia contratada superada." << endl;

            cout << cargas[indiceMayor]->getNombre()
                 << " se apaga por control de potencia." << endl;
        }


        // -----------------------------------------------------
        // Calcular el consumo y costo de la hora
        // -----------------------------------------------------

        for (int i = 0; i < cantidadCargas; i++) {

            if (cargas[i]->estaEncendida()) {

                // Como estamos calculando una hora,
                // kW equivale a kWh consumidos durante esa hora
                energiaHora += cargas[i]->getPotencia();

                // Calcula el costo de cada carga encendida
                costoHoraTotal += cargas[i]->costoHora(precio);
            }
        }


        // -----------------------------------------------------
        // Registrar los datos en el Medidor
        // -----------------------------------------------------

        medidor.registrar(energiaHora, costoHoraTotal);

        cout << "----------------------------------------" << endl;
        cout << "Potencia encendida: "
             << potenciaTotal << " kW" << endl;

        cout << "Energia de esta hora: "
             << energiaHora << " kWh" << endl;

        cout << "Costo de esta hora: "
             << costoHoraTotal << " COP" << endl;

        cout << "========================================" << endl;
    }


    // ---------------------------------------------------------
    // Mostrar el resumen del Medidor
    // ---------------------------------------------------------

    void mostrarMedidor() {

        cout << endl;
        cout << "========================================" << endl;
        cout << "        RESUMEN DEL MEDIDOR             " << endl;
        cout << "========================================" << endl;

        cout << "Energia acumulada: "
             << medidor.getEnergia() << " kWh" << endl;

        cout << "Costo acumulado: "
             << medidor.getCosto() << " COP" << endl;

        cout << "========================================" << endl;
    }
};


// =============================================================
// PROGRAMA PRINCIPAL
// =============================================================

int main() {

    cout << fixed << setprecision(1);

    // ---------------------------------------------------------
    // Crear el objeto Mercado
    // ---------------------------------------------------------

    Mercado mercado;


    // ---------------------------------------------------------
    // Crear las cargas fuera de Negocio
    // Esto permite demostrar la agregacion
    // ---------------------------------------------------------

    CargaCritica refrigeradores(
        "Refrigeradores",
        POTENCIA_REFRIGERADORES
    );

    CargaAplazable aire(
        "Aire acondicionado",
        POTENCIA_AIRE
    );

    CargaAplazable calentador(
        "Calentador de agua",
        POTENCIA_CALENTADOR
    );


    // ---------------------------------------------------------
    // Crear el Negocio
    // ---------------------------------------------------------

    Negocio minimercado(
        "Minimercado",
        &mercado,
        &refrigeradores,
        &aire,
        &calentador
    );


    // ---------------------------------------------------------
    // Probar la operacion de una hora
    // ---------------------------------------------------------

    int horaPrueba = 12;

    minimercado.operarHora(horaPrueba);


    // ---------------------------------------------------------
    // Mostrar el acumulado del Medidor
    // ---------------------------------------------------------

    minimercado.mostrarMedidor();


    return 0;
}