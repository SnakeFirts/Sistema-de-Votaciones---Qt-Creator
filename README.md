# Sistema de Elecciones - Cliente/Servidor (Qt)

Proyecto académico desarrollado en **Qt/C++** que simula un sistema de votación electrónica mediante una arquitectura cliente-servidor con comunicación por sockets TCP.

## 📁 Estructura

```
.
├── Servidor/       # Aplicación servidor (gestiona conexiones y procesa votos)
│   ├── main.cpp
│   ├── principal.cpp / .h / .ui
│   ├── Resources.qrc
│   └── Alcaldes/   # Imágenes de candidatos usadas en la interfaz
│
└── Cliente/        # Aplicación cliente (interfaz de votación)
    ├── main.cpp
    ├── principal.cpp / .h / .ui      # Login
    ├── crearcuenta.cpp / .h / .ui    # Registro de usuario
    ├── dashboard.cpp / .h / .ui      # Panel principal
    ├── select.cpp / .h               # Selección de candidato
    ├── resources.qrc
    └── ELECCIONES/  # Imágenes de campaña y UI
```

## ⚙️ Tecnologías

- **Qt 5/6** (módulos `core`, `gui`, `widgets`, `network`)
- **C++11**
- Comunicación cliente-servidor vía `QTcpSocket` / `QTcpServer` (puerto `1234`)

## 🚀 Cómo compilar

1. Abrir `Servidor/Servidor.pro` en Qt Creator y compilar.
2. Abrir `Cliente/Cliente.pro` en Qt Creator y compilar.
3. Ejecutar primero el **Servidor**, luego una o más instancias del **Cliente**.

## 📝 Notas

Este proyecto fue desarrollado con fines educativos como parte de un curso universitario. Los archivos de datos generados en tiempo de ejecución (usuarios registrados, resultados de votación) **no se incluyen en este repositorio** por contener información personal de prueba.

## 👤 Autor

Proyecto desarrollado por [SnakeFirts](https://github.com/SnakeFirts) como trabajo de clase.
