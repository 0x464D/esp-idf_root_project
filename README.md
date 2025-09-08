# Proyecto Básico ESP32

Este es un proyecto básico para ESP32 que parpadea un LED y muestra un mensaje en la consola.

## Estructura del Proyecto

- `main/main.c`: Contiene el código principal que controla el LED y muestra mensajes.
- `main/CMakeLists.txt`: Define cómo se compila el componente principal.
- `CMakeLists.txt`: Archivo principal del proyecto CMake.
- `sdkconfig.defaults`: Configuración predeterminada del proyecto.
- `partitions.csv`: Tabla de particiones de la flash.

## Cómo Compilar y Cargar el Programa

1. **Configurar el entorno ESP-IDF**:
   Asegúrate de tener instalado ESP-IDF y configuradas las variables de entorno. Si no lo has hecho, ejecuta:
   ```
   . $IDF_PATH/export.sh
   ```

2. **Permisos de acceso al puerto serie**:
   Para poder cargar el programa sin necesidad de usar `sudo`, necesitas agregar tu usuario al grupo `uucp`:
   ```
   sudo usermod -aG uucp $USER
   ```
   
   Luego, para que los cambios surtan efecto de inmediato sin reiniciar la sesión, ejecuta:
   ```
   newgrp uucp
   ```
   
   Alternativamente, puedes cerrar la sesión y volver a iniciarla.

3. **Compilar el proyecto**:
   En el directorio del proyecto, asegurate de que se cree el sdkconfig con las variables definidas en el archivo "sdkconfig_esp32_2MB.defaults" asi que ejecuta:
   ```bash
   rm sdkconfig
   idf.py build
   ```

   Otra forma de compilar es:
   ```bash
   idf.py -p /dev/ttyACM0 flash           # Compila y flashea
   idf.py -p /dev/ttyACM0 flash monitor   # Compila, flashea y abre el monitor
   ```

   Se puede definir en el sdkconfig el puerto (recuerda hacerlo en el sdkconfig.default y eliminar el sdkconfig para que se actualice el cambio al compilar) y solamente ejecutar **idf.py flash**:
   ```bash
   CONFIG_ESPTOOLPY_PORT="/dev/ttyACM0"
   ```

   O se puede definir en un export:
   ```bash
   export ESPPORT=/dev/ttyACM0
   idf.py flash
   ```

4. **Conectar tu ESP32**:
   Conecta tu placa ESP32 al ordenador mediante un cable USB.

   ```bash
   idf.py -p /dev/ttyUSB0 build	            Compila el proyecto
   idf.py -p /dev/ttyUSB0 flash	            Flashea el firmware al ESP32
   idf.py -p /dev/ttyUSB0 monitor            Abre el monitor serial (modo consola interactiva)
   idf.py -p /dev/ttyUSB0 flash monitor	   Flashea y luego abre el monitor serial
   esptool.py --port /dev/ttyACM0 chip_id       To know what board do you have (use dmesg to know the port)
   ```

5. **Cargar el programa**:
   Ejecuta el siguiente comando para cargar el programa en la ESP32:
   ```bash
   idf.py -p /dev/ttyACM0 flash
   ```
   Nota: El dispositivo está conectado al puerto `/dev/ttyACM0`.

6. **Ver la salida del programa**:
   Para ver los mensajes del programa, ejecuta:
   ```bash
   idf.py -p /dev/ttyACM0 monitor
   ```
   Puedes salir del monitor presionando `Ctrl+]`.

## Descripción del Programa

El programa realiza las siguientes acciones:
- Configura el pin GPIO2 como salida.
- En un bucle infinito:
  - Enciende el LED conectado al pin GPIO2.
  - Espera 1 segundo.
  - Apaga el LED.
  - Espera 1 segundo.
  - Muestra el mensaje "Hola desde ESP32!" en la consola.

## # Configuración del proyecto ESP32 básico

El archivo 'sdkconfig.defaults' es el que se debería modificar (DEFAULT)
   - El archivo sdkconfig se sobreescribe y modifica automáticamente por el 'idf.py menuconfig' y se genera un sdkconfig.old.
   - Se borra manualmente el sdkconfig cuando se hace alguna actualizacion de variables de configuración y luego se construye nuevamente'idf.py build'
   - Cuando se usa el menuconfig 

## Menuconfig opcional

Para crear un componente que se cargue y compile opcionalmente se debe hacer lo siguiente:

1. **Dentro del componente**:

   - Kconfig.projbuild crear el Flag para habilitar o deshabilitar el componente:

   ```bash
   menu "@FMA Component - LVGL"

      config USE_FMA_LVGL
         bool "Enable fma_lvgl"
         default n

   endmenu
   ```

   - CMakeLists.txt crear una expresión de CMake que permita incluir solamente si existe el Flag.
   ```c
   idf_component_register(SRCS
      $<$<BOOL:${CONFIG_USE_FMA_LVGL}>:src/fma_lvgl_lilygo.c>
      INCLUDE_DIRS "." "include"
   )
   ```

   - sdconfig.defaults crear el Flag (se antepone el prefijo CONFIG_) para habilitar 'y' o deshabilitar 'n':
   ```bash
   CONFIG_USE_FMA_LVGL=n
   ```

   - Dentro de cualquier código que use alguna funcion de este componente asegurarse de encapsularlo dentro de #ifdef
   ```c
   #ifdef CONFIG_USE_FMA_LVGL
      char lvgl_title[64];
      lvgl_init(lvgl_title);
      printf("lvgl_init() returned title: %s\n", lvgl_title);
   #endif
   ```

## Elegir el sdkconfig.defaults

   Se agrega esta configuración en el 'CMakeLists.txt' del proyecto:

   ```bash
   # Configura las opciones por default del sdkconfig para una placa en específico
   set(SDKCONFIG_DEFAULTS "sdkconfig_esp32_2MB.defaults")
   ```

   Otra alternativa es usarlo como parámetro:

   ```bash
   idf.py -D SDKCONFIG=/ruta/a/mi/sdkconfig.custom build
   ```

## Guardar la version en el firmware:

Para agregar una versión en el firmware se debe escribir la variable:

   ```bash
   file (STRINGS "BuildNumber" BUILD_NUMBER)
   set(PROJECT_VER "${BUILD_NUMBER}")
   ```

Y se puede consultar con los siguiente desde el código:

   ```c
   #include "esp_app_desc.h"

   void print_app_version(void) {
      const esp_app_desc_t* app_desc = esp_app_get_description();
      printf("App version: %s\n", app_desc->version);
   }
   ```

## Herramientas útiles

1. **Cute Com**:

   ```bash
   paru -S cutecom 
   ```