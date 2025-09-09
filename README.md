# ZeroBus

[![CI](https://github.com/Jumargo/zerobus/actions/workflows/ci.yml/badge.svg)](https://github.com/Jumargo/zerobus/actions/workflows/ci.yml)

Proyecto de práctica en **C/C++ sobre Linux (Rock Pi 4)** para aprender conceptos de **sistemas distribuidos** y **comunicación con ZeroMQ**.

## 🚀 Objetivo
Implementar un bus de mensajes ligero que permita:
- Patrones **Publish/Subscribe** (telemetría).
- Patrones **Request/Reply** (control).
- Ejecución en un entorno Linux embebido (Rock Pi).
- Integración con `systemd` y CI/CD.

## 📂 Estructura del proyecto
```
zerobus/
├─ src/
│  ├─ publisher/       → Publica mensajes periódicos (ZeroMQ PUB)
│  ├─ consumer/        → Se suscribe a mensajes (ZeroMQ SUB)
│  ├─ common/          → Código compartido (logging, utils)
│  └─ exercises/       → Ejercicios de Bloque 1 (threading, IPC, sockets)
├─ tests/              → Unit tests (GoogleTest)
├─ build/              → Directorio de compilación (CMake)
└─ README.md
```

## 🔧 Requisitos
- Linux (probado en Debian/Ubuntu para Rock Pi 4).
- `g++`, `cmake`, `git`.
- Librerías: `libzmq3-dev`, `libczmq-dev`.

Instalación en Debian/Ubuntu:
```bash
sudo apt update
sudo apt -y install build-essential cmake pkg-config git libzmq3-dev libczmq-dev
```

## ▶️ Compilación
```bash
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

## ▶️ Ejecución
En una terminal:
```bash
./publisher tcp://0.0.0.0:5556
```

En otra terminal:
```bash
./consumer tcp://127.0.0.1:5556 events.temp
```

**Salida esperada:**
```
[SUB] events.temp | {"value":0}
[SUB] events.temp | {"value":1}
...
```

## ✅ Tests
Este proyecto usa [GoogleTest](https://github.com/google/googletest).

Compilar y ejecutar:
```bash
cd build
make unit_tests
ctest --output-on-failure
```

## 🧩 Ejercicios Bloque 1 (Fundamentos Linux + C/C++)

Este bloque incluye ejemplos de concurrencia e IPC en Linux, desarrollados como práctica antes de usar ZeroMQ:

- **Multithreading con `std::thread` + `std::mutex`**  
  Ejecutable: `thread_counter`  
  Tres hilos incrementan un contador compartido de forma segura.

- **Comunicación padre ↔ hijo con `fork()` y `pipe()`**  
  Ejecutable: `pipe_parent_child`  
  El proceso padre envía mensajes, el hijo responde en mayúsculas.

- **Sockets locales (AF_UNIX)**  
  - `unix_echo_server`: servidor que responde en mayúsculas.  
  - `unix_echo_client`: cliente que se conecta al servidor y recibe la respuesta.  

### Ejemplo de uso (socket UNIX)

En una terminal:
```bash
./unix_echo_server /tmp/zerobus_echo.sock
```

En otra terminal:
```bash
./unix_echo_client /tmp/zerobus_echo.sock $'hola\n'
```

**Salida esperada:**
```
[client] recv: HOLA
```
