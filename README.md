
# WattControl - Sistema de Gestión Energética para Minimercado

## Información del Estudiante
* **Nombre del Estudiante:** Diego Fernando Daza Ijaji
* **Asignatura:** Programación Orientada a Objetos
* **Evidencia:** EA2 - Clases con Herencia y Polimorfismo (WattControl v2)
* **Negocio Asignado:** Minimercado (Dígito 9)
* **Modalidad de Trabajo:** Individual

---

##  Descripción del Proyecto
Este proyecto consiste en un software de gestión energética inteligente escrito en C++ para mitigar el impacto de los altos costos de la energía eléctrica durante la crisis del fenómeno de El Niño. El sistema evalúa hora a hora el comportamiento del mercado de energía (precios de bolsa y nivel de embalses) para encender o aplazar de forma automatizada las cargas eléctricas de un **Minimercado**, optimizando el costo financiero sin interrumpir la operación esencial.

---

## Especificaciones Técnicas del Minimercado (Dígito 9)
* **Carga Crítica:** Refrigeradores (2.5 kW) - *Operación continua 24/7 para preservar la cadena de frío.*
* **Carga Aplazable 1:** Aire Acondicionado (3.5 kW) - *Requiere mínimo 6 horas de operación diaria.*
* **Carga Aplazable 2:** Calentador de Agua (2.0 kW) - *Requiere mínimo 4 horas de operación diaria.*
* **Potencia Contratada:** 7.0 kW (Límite máximo del negocio).
* **Umbral Precio Caro:** > 950 COP/kWh.
* **Umbral Embalse Crítico:** < 40 %.
* **Franja Hora Pico:** 18:00 h a 20:59 h (Horas 18, 19 y 20).

---

## Relaciones de Programación Orientada a Objetos (POO)
1. **Clase Abstracta:** `Carga` define los atributos comunes y el método virtual puro `debeEncender() = 0`.
2. **Herencia y Polimorfismo:** `CargaCritica` y `CargaAplazable` heredan de `Carga` e implementan sus propias reglas de encendido.
3. **Composición:** La clase `Negocio` contiene los objetos `Medidor` (nacen y mueren con el negocio).
4. **Agregación:** `Negocio` administra las cargas mediante un arreglo de punteros `Carga*` creados en el `main`.
5. **Asociación:** `Negocio` consulta precios y embalses a la clase `Mercado` a través de un puntero.

---

## Entorno de Desarrollo y Compilación
* **Lenguaje:** C++
* **Editor de Código:** Visual Studio Code
* **Compilador:** MSYS2 (g++ / MinGW-w64)
