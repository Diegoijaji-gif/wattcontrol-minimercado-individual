// ============================================================================
// WattControl - Gestor inteligente de energia para un minimercado
// EA2 POO: herencia, polimorfismo, composicion, agregacion y asociacion
//
// Este archivo sigue la metodologia didactica de AquaControl PASO_A_PASO.md:
// primero se preparan las librerias y constantes; despues se construyen las
// clases; luego las relaciones; y finalmente el menu, la simulacion y resumen.
// ============================================================================

// ---------------------------------------------------------------------------
// 1. LIBRERIAS
// ---------------------------------------------------------------------------
#include <iostream>   // cout y cin: mostrar informacion y leer datos del usuario.
#include <iomanip>    // fixed, setprecision y setw: dar formato a los resultados.
#include <string>     // string: guardar nombres de las cargas y del negocio.
#include <limits>     // numeric_limits: limpiar correctamente entradas incorrectas.
#include <cstdlib>    // rand y srand: generar precios y fallas simuladas.
#include <ctime>      // time: cambiar la semilla aleatoria en cada ejecucion.
#include <cmath>      // isfinite: rechazar valores NaN e infinitos.

// Permite escribir cout, cin y string sin anteponer std::.
using namespace std;

// ---------------------------------------------------------------------------
// 2. DIRECTIVAS DEL PREPROCESADOR Y CONSTANTES DEL NEGOCIO
// ---------------------------------------------------------------------------
// #define: antes de compilar, el preprocesador reemplaza HORAS_DIA por 24.
// Se utiliza para indicar que un dia tiene 24 horas.
#define HORAS_DIA 24

// Cantidad de cargas del minimercado: refrigeradores, aire y calentador.
#define MAX_CARGAS 3

// Una de cada 20 horas, en promedio, se simula una falla de lectura del precio.
#define FALLA_CADA 20

// Las constantes const guardan valores que no deben cambiar durante el programa.
// Las potencias se expresan en kW (kilovatios), no en kWh.
const float POTENCIA_REFRIGERADORES = 2.5f; // Carga critica: refrigeradores.
const float POTENCIA_AIRE = 3.5f;           // Carga aplazable: aire acondicionado.
const float POTENCIA_CALENTADOR = 2.0f;     // Carga aplazable: calentador de agua.

// Potencia contratada personalizada para el minimercado: 10 kW.
// Las tres cargas suman 8 kW, por lo que pueden funcionar juntas sin superar el limite.
const float POTENCIA_CONTRATADA = 7.0f;

// Si el precio SUPERA 800 COP/kWh, las cargas aplazables esperan.
// Este es el valor personalizado acordado para nuestro proyecto.
const float PRECIO_CARO = 800.0f; // Umbral del minimercado segun la tabla del proyecto de aula.

// Si el precio supera 1500 y los embalses estan por debajo del 40 %,
// se declara una emergencia y se interrumpe la simulacion con break.
const float PRECIO_EMERGENCIA = 1500.0f;

// Limite de alerta del nivel de los embalses, expresado en porcentaje.
const float NIVEL_EMBALSE_ALERTA = 40.0f;

// La hora pico incluye las horas 18, 19 y 20.
const int HORA_PICO_INICIO = 18;
const int HORA_PICO_FIN = 20;

// Limites de las demas franjas horarias para seleccionar el rango de precios.
const int HORA_MADRUGADA_FIN = 6;
const int HORA_MANANA_FIN = 14;
const int HORA_TARDE_FIN = 17;

// Rangos minimo y maximo observados en los datos horarios del proyecto de aula.
const int PRECIO_MADRUGADA_MIN = 398;
const int PRECIO_MADRUGADA_MAX = 640;
const int PRECIO_MANANA_MIN = 456;
const int PRECIO_MANANA_MAX = 970;
const int PRECIO_TARDE_MIN = 596;
const int PRECIO_TARDE_MAX = 1040;
const int PRECIO_PICO_MIN = 837;
const int PRECIO_PICO_MAX = 1612;
const int PRECIO_NOCHE_MIN = 565;
const int PRECIO_NOCHE_MAX = 970;

// Horas minimas de funcionamiento que definimos para las cargas aplazables.
// Justificacion del equipo: el aire debe funcionar al menos 6 horas para
// mantener condiciones adecuadas en el minimercado; el calentador necesita
// al menos 2 horas para cubrir la necesidad diaria de agua caliente.
const int HORAS_MINIMAS_AIRE = 6;
const int HORAS_MINIMAS_CALENTADOR = 2;

// ---------------------------------------------------------------------------
// 3. FUNCIONES PARA VALIDAR LAS ENTRADAS
// ---------------------------------------------------------------------------
// Estas funciones evitan que el programa siga trabajando con datos invalidos.
// cin.fail() detecta, por ejemplo, cuando se escribe una palabra donde se espera
// un numero. cin.clear() quita el estado de error y cin.ignore() descarta la linea.

// Lee un numero entero y repite la pregunta hasta recibir un entero valido.
int leerEntero(const string& mensaje) {
    int valor;

    while (true) {
        cout << mensaje;
        if (cin >> valor) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }

        if (cin.eof()) {
            cout << "\nFin de la entrada. El programa terminara.\n";
            exit(0);
        }

        cout << "Entrada invalida. Escriba un numero entero.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Lee un decimal finito y mayor que cero (por ejemplo, el precio de energia).
float leerPrecio(const string& mensaje) {
    float valor;

    while (true) {
        cout << mensaje;
        if (cin >> valor) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (isfinite(valor) && valor > 0.0f) {
                return valor;
            }
            cout << "El precio debe ser un numero finito mayor que cero.\n";
        } else {
            if (cin.eof()) {
                cout << "\nFin de la entrada. El programa terminara.\n";
                exit(0);
            }
            cout << "Entrada invalida. Escriba un valor numerico.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// Lee un porcentaje valido entre 0 y 100, inclusive.
float leerEmbalses(const string& mensaje) {
    float valor;

    while (true) {
        cout << mensaje;
        if (cin >> valor) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (isfinite(valor) && valor >= 0.0f && valor <= 100.0f) {
                return valor;
            }
            cout << "Nivel invalido. Debe ser un numero entre 0 y 100.\n";
        } else {
            if (cin.eof()) {
                cout << "\nFin de la entrada. El programa terminara.\n";
                exit(0);
            }
            cout << "Entrada invalida. Escriba un porcentaje numerico.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// ---------------------------------------------------------------------------
// 4. CLASE BASE ABSTRACTA: Carga
// ---------------------------------------------------------------------------
// Carga representa cualquier equipo electrico del minimercado.
// Es abstracta porque cada tipo de carga debe definir su propia decision.
// No se crean objetos Carga directamente: se crean objetos de sus clases hijas.
class Carga {
protected:
    // protected: estos atributos pueden ser utilizados por Carga y sus clases hijas,
    // pero no se pueden modificar directamente desde main.
    string nombre;       // Nombre que identifica la carga.
    float potenciaKW;    // Potencia que consume cuando esta encendida.
    bool encendida;      // Estado actual: true = encendida, false = apagada.
    int horasUso;        // Cantidad de horas que funciono en la simulacion.
    float energiaKWh;    // Energia acumulada: potencia (kW) por horas (h).

public:
    // Constructor: inicializa los atributos al crear cada objeto.
    // La carga comienza apagada y sus contadores empiezan en cero.
    Carga(string nombreCarga, float potencia)
        : nombre(nombreCarga), potenciaKW(potencia), encendida(false),
          horasUso(0), energiaKWh(0.0f) {}

    // Destructor virtual: permite destruir correctamente objetos derivados
    // cuando se trabaja con direcciones guardadas como Carga*.
    virtual ~Carga() {}

    // Metodo virtual puro: cada clase hija DEBE implementar su decision.
    // El = 0 hace que Carga sea una clase abstracta.
    virtual bool debeEncender(int hora, float precio, float nivelEmbalses) = 0;

    // Cambia el estado de la carga a encendida. No suma horas aqui:
    // la hora se registra en registrarUsoHora(), una vez por cada hora simulada.
    void encender() {
        encendida = true;
    }

    // Cambia el estado de la carga a apagada.
    void apagar() {
        encendida = false;
    }

    // Registra el consumo de UNA hora. Si esta encendida, suma una hora de uso
    // y potenciaKW kWh, porque una carga de 1 kW encendida durante 1 hora
    // consume 1 kWh.
    void registrarUsoHora() {
        if (encendida) {
            horasUso++;
            energiaKWh += potenciaKW;
        }
    }

    // Reinicia los datos de uso para que cada simulacion empiece desde cero.
    void reiniciarEstadisticas() {
        encendida = false;
        horasUso = 0;
        energiaKWh = 0.0f;
    }

    // Getters: permiten consultar atributos privados/protegidos sin modificarlos.
    string getNombre() const { return nombre; }
    float getPotencia() const { return potenciaKW; }
    bool estaEncendida() const { return encendida; }
    int getHorasUso() const { return horasUso; }
    float getEnergiaKWh() const { return energiaKWh; }
};

// ---------------------------------------------------------------------------
// 5. HERENCIA: CargaCritica ES una Carga
// ---------------------------------------------------------------------------
// Los refrigeradores son indispensables para conservar los productos.
// Por eso su metodo debeEncender() siempre devuelve true.
class CargaCritica : public Carga {
public:
    // El constructor de la clase hija llama al constructor de Carga
    // para guardar el nombre y la potencia heredados.
    CargaCritica(string nombreCarga, float potencia)
        : Carga(nombreCarga, potencia) {}

    // override indica que esta funcion implementa el metodo virtual puro
    // declarado en la clase base.
    bool debeEncender(int hora, float precio, float nivelEmbalses) override {
        // Los parametros se reciben para mantener la misma firma del metodo,
        // aunque la carga critica no depende de hora, precio ni embalses.
        (void)hora;
        (void)precio;
        (void)nivelEmbalses;
        return true; // Regla 1: la carga critica nunca se apaga.
    }
};

// ---------------------------------------------------------------------------
// 6. HERENCIA: CargaAplazable ES una Carga
// ---------------------------------------------------------------------------
// El aire acondicionado y el calentador pueden esperar cuando la energia
// esta cara, es hora pico o los embalses estan en nivel de alerta.
class CargaAplazable : public Carga {
private:
    // Cada carga aplazable conoce su minimo de horas para la produccion.
    int horasMinimas;

public:
    // Ademas del nombre y la potencia, recibe las horas minimas del equipo.
    CargaAplazable(string nombreCarga, float potencia, int minimoHoras)
        : Carga(nombreCarga, potencia), horasMinimas(minimoHoras) {}

    // Implementacion de la decision para las cargas aplazables.
    bool debeEncender(int hora, float precio, float nivelEmbalses) override {
        // && significa Y: las dos comparaciones deben cumplirse para ser hora pico.
        bool horaPico = (hora >= HORA_PICO_INICIO && hora <= HORA_PICO_FIN);

        // La condicion compuesta representa las reglas del proyecto:
        // se debe aplazar si ocurre AL MENOS una de estas situaciones:
        // 1) embalses por debajo del nivel de alerta;
        // 2) la hora corresponde a la franja pico;
        // 3) el precio supera el umbral de energia cara.
        // || significa O. El ! niega la condicion completa de aplazamiento:
        // si NO se cumple ninguna causa de espera, la carga puede encenderse.
        bool debeAplazarse = (nivelEmbalses < NIVEL_EMBALSE_ALERTA) ||
                             horaPico ||
                             (precio > PRECIO_CARO);

        return !debeAplazarse;
    }

    // Devuelve el minimo de horas definido para esta carga.
    int getHorasMinimas() const { return horasMinimas; }
};

// ---------------------------------------------------------------------------
// 7. COMPOSICION: clase Medidor
// ---------------------------------------------------------------------------
// Medidor acumula la energia y el costo que genera el negocio.
// La composicion se declara en Negocio mediante el atributo objeto Medidor.
class Medidor {
private:
    float energiaTotalKWh; // Energia consumida por todas las cargas.
    float costoTotalCOP;   // Costo acumulado en pesos colombianos.

public:
    // Constructor: el medidor empieza sin consumo ni costo.
    Medidor() : energiaTotalKWh(0.0f), costoTotalCOP(0.0f) {}

    // Registra el consumo de una carga durante una hora.
    // Energia = potencia (kW) x 1 hora; costo = energia (kWh) x precio.
    void registrar(float potenciaKW, float precioCOP) {
        float energiaHora = potenciaKW;
        energiaTotalKWh += energiaHora;
        costoTotalCOP += energiaHora * precioCOP;
    }

    // Reinicia los acumuladores al iniciar una nueva simulacion.
    void reiniciar() {
        energiaTotalKWh = 0.0f;
        costoTotalCOP = 0.0f;
    }

    float getEnergiaTotal() const { return energiaTotalKWh; }
    float getCostoTotal() const { return costoTotalCOP; }
};

// ---------------------------------------------------------------------------
// 8. ASOCIACION: clase Mercado
// ---------------------------------------------------------------------------
// Mercado representa la bolsa de energia. Negocio conoce un objeto Mercado
// mediante un puntero, pero no lo crea ni es propietario de el.
class Mercado {
private:
    // Mercado conserva la ultima lectura manual y el nivel usado en simulacion.
    int horaManual;
    float precioManual;
    float nivelEmbalses;
    bool existeLecturaManual;

public:
    Mercado() : horaManual(0), precioManual(0.0f), nivelEmbalses(100.0f),
                existeLecturaManual(false) {}

    // Guarda la lectura manual: asi Negocio consulta al Mercado por sus datos.
    void guardarLectura(int hora, float precio, float embalses) {
        horaManual = hora;
        precioManual = precio;
        nivelEmbalses = embalses;
        existeLecturaManual = true;
    }

    bool tieneLecturaManual() const { return existeLecturaManual; }
    int getHoraManual() const { return horaManual; }
    float getPrecioManual() const { return precioManual; }
    float getNivelEmbalses() const { return nivelEmbalses; }

    void establecerNivelEmbalses(float nivel) {
        nivelEmbalses = nivel;
    }

    // Devuelve un precio aleatorio dentro del rango observado para la franja.
    // Los rangos corresponden a la tabla de precios reales de XM del proyecto.
    float generarPrecio(int hora) {
        int minimo = 0;
        int maximo = 0;

        if (hora <= HORA_MADRUGADA_FIN) {
            minimo = PRECIO_MADRUGADA_MIN;
            maximo = PRECIO_MADRUGADA_MAX;
        } else if (hora <= HORA_MANANA_FIN) {
            minimo = PRECIO_MANANA_MIN;
            maximo = PRECIO_MANANA_MAX;
        } else if (hora <= HORA_TARDE_FIN) {
            minimo = PRECIO_TARDE_MIN;
            maximo = PRECIO_TARDE_MAX;
        } else if (hora <= HORA_PICO_FIN) {
            minimo = PRECIO_PICO_MIN;
            maximo = PRECIO_PICO_MAX;
        } else {
            minimo = PRECIO_NOCHE_MIN;
            maximo = PRECIO_NOCHE_MAX;
        }

        return static_cast<float>(minimo + rand() % (maximo - minimo + 1));
    }
};

// ---------------------------------------------------------------------------
// 9. CLASE Negocio: administra las cargas, el medidor y el mercado
// ---------------------------------------------------------------------------
class Negocio {
private:
    string nombre;

    // COMPOSICION: Medidor es un objeto que pertenece al Negocio.
    // Se crea automaticamente cuando se crea el Negocio y vive con el.
    Medidor medidor;

    // AGREGACION: arreglo de punteros a Carga. Las cargas se crean en main()
    // y Negocio solo guarda sus direcciones; no crea ni destruye esos objetos.
    Carga* cargas[MAX_CARGAS];
    int cantidadCargas;

    // ASOCIACION: Negocio conoce al Mercado mediante su direccion.
    // El Mercado se crea afuera y se le pasa al constructor.
    Mercado* mercado;

    // Resultados de la ultima simulacion.
    float costoSinGestor;
    int horasSimuladas;
    int fallasLectura;
    bool huboEmergencia;
    bool produccionIncompleta;
    bool existeSimulacion;

public:
    // Constructor: recibe el nombre del negocio y la direccion del mercado.
    // La lista de inicializacion prepara el puntero y los atributos de control.
    Negocio(string nombreNegocio, Mercado* mercadoExterno)
        : nombre(nombreNegocio), cantidadCargas(0), mercado(mercadoExterno),
          costoSinGestor(0.0f), horasSimuladas(0),
          fallasLectura(0), huboEmergencia(false), produccionIncompleta(false),
          existeSimulacion(false) {}

    // Agrega la direccion de una carga que ya fue creada en main().
    // &objeto obtiene la direccion y Carga* permite guardar cargas derivadas.
    void agregarCarga(Carga* carga) {
        if (cantidadCargas < MAX_CARGAS) {
            cargas[cantidadCargas] = carga;
            cantidadCargas++;
        } else {
            cout << "No se pueden agregar mas cargas al negocio.\n";
        }
    }

    // Guarda la lectura manual recibida desde el menu.
    void guardarLecturaManual(int hora, float precio, float embalses) {
        mercado->guardarLectura(hora, precio, embalses);
    }

    // Muestra las cargas que administra el negocio y sus potencias.
    void mostrarCargas() const {
        cout << "\nNegocio: " << nombre << "\n";
        cout << "Cargas registradas:\n";
        for (int i = 0; i < cantidadCargas; i++) {
            cout << "  - " << cargas[i]->getNombre() << " ("
                 << cargas[i]->getPotencia() << " kW)\n";
        }
    }

    // Evalua una lectura manual sin sumarla a las estadisticas de la simulacion.
    // Primero pregunta a todas las cargas usando el mismo ciclo y el metodo virtual.
    void evaluarLecturaManual() {
        if (!mercado->tieneLecturaManual()) {
            cout << "Primero debe ingresar una lectura con la opcion 1.\n";
            return;
        }

        int horaManual = mercado->getHoraManual();
        float precioManual = mercado->getPrecioManual();
        float embalsesManual = mercado->getNivelEmbalses();

        cout << "\nEvaluacion para la hora " << horaManual
             << ", precio " << precioManual << " COP/kWh y embalses al "
             << embalsesManual << "%:\n";

        // La opcion 2 es una evaluacion puntual, no una simulacion con un for.
        // Se informa si la lectura cumple la condicion de emergencia; el break
        // se ejecuta realmente en la opcion 3, dentro del ciclo de 24 horas.
        if (precioManual > PRECIO_EMERGENCIA &&
            embalsesManual < NIVEL_EMBALSE_ALERTA) {
            cout << "EMERGENCIA DE RACIONAMIENTO: condiciones detectadas en la lectura manual.\n";
            cout << "La interrupcion con break se demuestra en la opcion 3 (simulacion).\n";
        }

        float potenciaEncendida = 0.0f;

        for (int i = 0; i < cantidadCargas; i++) {
            // Polimorfismo: se usa el mismo llamado ->debeEncender(), pero CargaCritica
            // y CargaAplazable ejecutan su propia implementacion.
            if (cargas[i]->debeEncender(horaManual, precioManual, embalsesManual)) {
                cargas[i]->encender();
            } else {
                cargas[i]->apagar();
            }

            if (cargas[i]->estaEncendida()) {
                potenciaEncendida += cargas[i]->getPotencia();
            }

        }

        // Regla 6: si la potencia excede la contratada, se apaga la carga aplazable
        // de mayor potencia. Se identifica revisando las cargas encendidas desde el final
        // y seleccionando la de mayor potencia que NO sea critica.
        if (potenciaEncendida > POTENCIA_CONTRATADA) {
            int indiceMayor = -1;
            float mayorPotencia = -1.0f;

            for (int i = 0; i < cantidadCargas; i++) {
                CargaAplazable* aplazable = dynamic_cast<CargaAplazable*>(cargas[i]);
                if (aplazable != nullptr && cargas[i]->estaEncendida() &&
                    cargas[i]->getPotencia() > mayorPotencia) {
                    mayorPotencia = cargas[i]->getPotencia();
                    indiceMayor = i;
                }
            }

            if (indiceMayor != -1) {
                cargas[indiceMayor]->apagar();
                cout << "ALERTA: se superaba la potencia contratada ("
                     << POTENCIA_CONTRATADA << " kW). Se apaga la carga aplazable de mayor potencia: "
                     << cargas[indiceMayor]->getNombre() << ".\n";
            }
        }

        // Se muestran las decisiones FINALES, despues de aplicar el limite de potencia.
        // Asi el resultado no muestra como encendida una carga que se tuvo que apagar.
        cout << "\nEstado final de las cargas:\n";
        for (int i = 0; i < cantidadCargas; i++) {
            cout << "  " << setw(22) << left << cargas[i]->getNombre()
                 << (cargas[i]->estaEncendida() ? "ENCENDER" : "APLAZAR / APAGAR") << "\n";
        }
    }

    // Registra una hora normal: aplica las reglas y suma consumo/costo.
    void operarHora(int hora, float precio, float embalses) {
        // Primero se decide el estado de cada carga.
        for (int i = 0; i < cantidadCargas; i++) {
            if (cargas[i]->debeEncender(hora, precio, embalses)) {
                cargas[i]->encender();
            } else {
                cargas[i]->apagar();
            }
        }

        // Luego se suma la potencia encendida.
        float potenciaEncendida = 0.0f;
        for (int i = 0; i < cantidadCargas; i++) {
            if (cargas[i]->estaEncendida()) {
                potenciaEncendida += cargas[i]->getPotencia();
            }
        }

        // Si se supera la potencia contratada, se apaga la carga aplazable mayor.
        if (potenciaEncendida > POTENCIA_CONTRATADA) {
            int indiceMayor = -1;
            float mayorPotencia = -1.0f;

            for (int i = 0; i < cantidadCargas; i++) {
                CargaAplazable* aplazable = dynamic_cast<CargaAplazable*>(cargas[i]);
                if (aplazable != nullptr && cargas[i]->estaEncendida() &&
                    cargas[i]->getPotencia() > mayorPotencia) {
                    mayorPotencia = cargas[i]->getPotencia();
                    indiceMayor = i;
                }
            }

            if (indiceMayor != -1) {
                cargas[indiceMayor]->apagar();
                cout << "  ALERTA DE POTENCIA: se apaga "
                     << cargas[indiceMayor]->getNombre() << " por superar "
                     << POTENCIA_CONTRATADA << " kW contratados.\n";
            }
        }

        // Finalmente se registra el consumo de cada carga que quedo encendida.
        for (int i = 0; i < cantidadCargas; i++) {
            if (cargas[i]->estaEncendida()) {
                cargas[i]->registrarUsoHora();
                medidor.registrar(cargas[i]->getPotencia(), precio);
            }
        }
    }

    // Simula un dia completo. El nivel de embalses se solicita una sola vez
    // y se mantiene constante durante las 24 horas porque el enunciado no define
    // un modelo de evolucion horaria de los embalses.
    void simularDia(float nivelEmbalses) {
        // El Mercado conserva el nivel de embalses que se usara en las 24 horas.
        mercado->establecerNivelEmbalses(nivelEmbalses);
        nivelEmbalses = mercado->getNivelEmbalses();

        // Cada simulacion comienza con contadores limpios.
        medidor.reiniciar();
        for (int i = 0; i < cantidadCargas; i++) {
            cargas[i]->reiniciarEstadisticas();
        }

        costoSinGestor = 0.0f;
        horasSimuladas = 0;
        fallasLectura = 0;
        huboEmergencia = false;
        produccionIncompleta = false;
        existeSimulacion = true;

        // Primero se generan y guardan los 24 precios. Asi ambos escenarios
        // (con y sin gestor) usan exactamente el mismo precio en cada hora.
        float preciosDia[HORAS_DIA];
        for (int hora = 0; hora < HORAS_DIA; hora++) {
            preciosDia[hora] = mercado->generarPrecio(hora);
        }

        // El enunciado define el escenario sin gestor con las tres cargas
        // encendidas TODO el dia. Por eso se calcula para las 24 horas, aunque
        // el gestor llegue a detenerse antes por una emergencia (break).
        const float potenciaTotalInstalada = POTENCIA_REFRIGERADORES +
                                             POTENCIA_AIRE + POTENCIA_CALENTADOR;
        for (int hora = 0; hora < HORAS_DIA; hora++) {
            costoSinGestor += potenciaTotalInstalada * preciosDia[hora];
        }

        cout << "\nIniciando simulacion de " << HORAS_DIA << " horas...\n";
        cout << "Los precios se generan automaticamente para cada hora.\n";
        cout << "Nivel de embalses constante durante la simulacion: "
             << nivelEmbalses << "%\n";
        if (nivelEmbalses < NIVEL_EMBALSE_ALERTA) {
            cout << "ALERTA: embalses por debajo del 40%; solo funcionan los refrigeradores.\n";
        }
        cout << "Costo sin gestor: tres cargas encendidas durante las 24 horas.\n\n";
        cout << left << setw(6) << "Hora" << right << setw(9) << "Precio"
             << setw(13) << "Refrig."
             << setw(13) << "Aire"
             << setw(13) << "Calentador"
             << setw(15) << "Costo hora" << "\n";
        cout << string(82, '-') << "\n";

        // for: recorre las horas desde 0 hasta 23.
        for (int hora = 0; hora < HORAS_DIA; hora++) {
            float precio = preciosDia[hora];

            // break: si coinciden precio superior a 1500 y embalses menores
            // al 40 %, se declara emergencia y se termina el ciclo.
            if (precio > PRECIO_EMERGENCIA &&
                nivelEmbalses < NIVEL_EMBALSE_ALERTA) {
                cout << left << setw(6) << hora << right << setw(9) << fixed << setprecision(0) << precio
                     << "  EMERGENCIA DE RACIONAMIENTO: se detiene la simulacion (break).\n";
                huboEmergencia = true;
                break;
            }

            // continue: una falla de lectura (probabilidad 1/20 por hora)
            // deja funcionando unicamente la carga critica.
            bool fallaLectura = (rand() % FALLA_CADA == 0);
            if (fallaLectura) {
                fallasLectura++;
                for (int i = 0; i < cantidadCargas; i++) {
                    cargas[i]->apagar();
                }

                // La carga critica es la primera que se agrego al Negocio.
                cargas[0]->encender();
                cargas[0]->registrarUsoHora();
                medidor.registrar(cargas[0]->getPotencia(), precio);

                horasSimuladas++;
                cout << left << setw(6) << hora << right << setw(9) << fixed << setprecision(0) << precio
                     << setw(13) << "ON" << setw(13) << "OFF" << setw(13) << "OFF"
                     << setw(15) << fixed << setprecision(0) << (POTENCIA_REFRIGERADORES * precio)
                     << "  Falla de lectura: continue.\n";
                continue;
            }

            // Si no hubo falla, se aplican las reglas normales del negocio.
            operarHora(hora, precio, nivelEmbalses);

            // El estado se muestra para CADA carga, incluso cuando esta apagada.
            // Esto permite ver claramente cuando aire o calentador se aplazan.
            float costoHora = 0.0f;
            for (int i = 0; i < cantidadCargas; i++) {
                if (cargas[i]->estaEncendida()) {
                    costoHora += cargas[i]->getPotencia() * precio;
                }
            }

            horasSimuladas++;
            cout << left << setw(6) << hora << right << setw(9) << fixed << setprecision(0) << precio
                 << setw(13) << (cargas[0]->estaEncendida() ? "ON" : "OFF")
                 << setw(13) << (cargas[1]->estaEncendida() ? "ON" : "OFF")
                 << setw(13) << (cargas[2]->estaEncendida() ? "ON" : "OFF")
                 << setw(15) << fixed << setprecision(0) << costoHora << "\n";
        }

        // Al terminar el ciclo, incluso si hubo break, se revisan las horas minimas.
        for (int i = 0; i < cantidadCargas; i++) {
            CargaAplazable* aplazable = dynamic_cast<CargaAplazable*>(cargas[i]);
            if (aplazable != nullptr &&
                aplazable->getHorasUso() < aplazable->getHorasMinimas()) {
                produccionIncompleta = true;
            }
        }

        cout << "\nSimulacion terminada. Consulte la opcion 4 para ver el resumen.\n";
    }

    // Presenta los resultados de la ultima simulacion y calcula el ahorro.
    void mostrarResumen() const {
        if (!existeSimulacion) {
            cout << "Todavia no se ha realizado una simulacion. Use la opcion 3.\n";
            return;
        }

        cout << "\n================ RESUMEN DE WATTCONTROL ================\n";
        cout << "Negocio: " << nombre << "\n";
        cout << "Horas procesadas por el gestor: " << horasSimuladas << " de " << HORAS_DIA << "\n";
        cout << "Fallas de lectura: " << fallasLectura << "\n";
        cout << "Estado de emergencia: " << (huboEmergencia ? "SI" : "NO") << "\n\n";

        cout << left << setw(24) << "Carga" << right << setw(12) << "Horas"
             << setw(14) << "Energia kWh" << "\n";
        cout << string(50, '-') << "\n";

        for (int i = 0; i < cantidadCargas; i++) {
            cout << left << setw(24) << cargas[i]->getNombre()
                 << right << setw(12) << cargas[i]->getHorasUso()
                 << setw(14) << fixed << setprecision(2) << cargas[i]->getEnergiaKWh()
                 << "\n";
        }

        float costoConGestor = medidor.getCostoTotal();
        float ahorroPesos = costoSinGestor - costoConGestor;
        float ahorroPorcentaje = 0.0f;
        if (costoSinGestor > 0.0f) {
            ahorroPorcentaje = (ahorroPesos / costoSinGestor) * 100.0f;
        }

        cout << "\nEnergia total con gestor: " << fixed << setprecision(2)
             << medidor.getEnergiaTotal() << " kWh\n";
        cout << "Costo con gestor:         $" << fixed << setprecision(0)
             << costoConGestor << " COP\n";
        cout << "Costo sin gestor:         $" << fixed << setprecision(0)
             << costoSinGestor << " COP\n";
        cout << "Ahorro:                   $" << fixed << setprecision(0)
             << ahorroPesos << " COP (" << setprecision(2) << ahorroPorcentaje << " %)\n";
        cout << "Nota: el costo sin gestor representa las 3 cargas encendidas durante las 24 horas,\n";
        cout << "      usando los mismos precios horarios del escenario con gestor.\n";

        if (produccionIncompleta) {
            cout << "ALERTA: produccion incompleta. Una o mas cargas aplazables no cumplieron sus horas minimas.\n";
        } else {
            cout << "Produccion: se cumplieron las horas minimas de las cargas aplazables.\n";
        }

        if (huboEmergencia) {
            cout << "Nota: la simulacion se detuvo antes de completar las 24 horas por emergencia de racionamiento.\n";
        }
        cout << "=========================================================\n";
    }
};

// ---------------------------------------------------------------------------
// 10. FUNCION PRINCIPAL: main
// ---------------------------------------------------------------------------
// Aqui se crean los objetos concretos, se conectan las relaciones y se muestra
// el menu. El ciclo do-while garantiza que el menu aparezca al menos una vez.
int main() {
    // srand(time(0)) cambia la semilla de rand() en cada ejecucion.
    // Por eso los precios y las fallas simuladas pueden variar entre ejecuciones.
    srand(static_cast<unsigned int>(time(0)));

    // ASOCIACION: Mercado se crea por fuera y luego se le entrega su direccion
    // al objeto Negocio mediante &mercado.
    Mercado mercado;

    // AGREGACION: las tres cargas se crean fuera del Negocio.
    // El Negocio solamente guardara sus direcciones en el arreglo Carga*.
    CargaCritica refrigeradores("Refrigeradores", POTENCIA_REFRIGERADORES);
    CargaAplazable aire("Aire acondicionado", POTENCIA_AIRE, HORAS_MINIMAS_AIRE);
    CargaAplazable calentador("Calentador de agua", POTENCIA_CALENTADOR,
                              HORAS_MINIMAS_CALENTADOR);

    // Se crea el negocio y se le pasa la direccion del mercado.
    // El Medidor se crea automaticamente dentro del Negocio (composicion).
    Negocio minimercado("Minimercado", &mercado);

    // Se agregan las direcciones de las tres cargas al Negocio.
    minimercado.agregarCarga(&refrigeradores);
    minimercado.agregarCarga(&aire);
    minimercado.agregarCarga(&calentador);

    minimercado.mostrarCargas();

    int opcion;

    do {
        cout << "\n================ MENU WATTCONTROL ================\n";
        cout << "1. Ingresar lectura manual (hora, precio, embalses)\n";
        cout << "2. Evaluar decision de cada carga\n";
        cout << "3. Simular un dia completo (24 horas)\n";
        cout << "4. Ver resumen de la ultima simulacion\n";
        cout << "0. Salir\n";
        cout << "===================================================\n";

        opcion = leerEntero("Seleccione una opcion: ");

        // switch ejecuta el bloque que corresponde al numero seleccionado.
        switch (opcion) {
            case 1: {
                int hora;
                float precio;
                float embalses;

                // Validacion con do-while: la hora debe estar entre 0 y 23.
                do {
                    hora = leerEntero("Hora (0 a 23): ");
                    if (hora < 0 || hora >= HORAS_DIA) {
                        cout << "Hora invalida. Debe estar entre 0 y 23.\n";
                    }
                } while (hora < 0 || hora >= HORAS_DIA);

                precio = leerPrecio("Precio de energia (COP/kWh, mayor que 0): ");
                embalses = leerEmbalses("Nivel de embalses (0 a 100 %): ");

                minimercado.guardarLecturaManual(hora, precio, embalses);
                cout << "Lectura manual guardada correctamente.\n";
                break;
            }

            case 2:
                minimercado.evaluarLecturaManual();
                break;

            case 3: {
                float embalses = leerEmbalses("Nivel de embalses para la simulacion (0 a 100 %): ");

                minimercado.simularDia(embalses);
                break;
            }

            case 4:
                minimercado.mostrarResumen();
                break;

            case 0:
                cout << "Hasta luego. WattControl ha finalizado.\n";
                break;

            default:
                // default se ejecuta cuando la opcion no coincide con ningun case.
                cout << "Opcion invalida. Seleccione 1, 2, 3, 4 o 0.\n";
        }

    } while (opcion != 0); // El menu se repite mientras el usuario no elija salir.

    return 0; // Indica que el programa termino correctamente.
}
