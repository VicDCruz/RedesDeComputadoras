# RedesDeComputadoras
Proyectos sobre NS3 de la materia Redes de Computadora Ago-Dic 2018

# Instrucciones de uso
* Instalar Docker en su computadora [LINK](https://docs.docker.com/install/linux/docker-ce/ubuntu/), ver el tutorial por el SO que se quiera usar.
* ```console
    docker pull vicdcruz/ns3:first
  ```
* Ejecutar
  ```console
    chmod +x ns3run.sh
    ./ns3run.sh
  ```
* Para usar el programa de NS3
  ```console
    docker exec -it ns3 bash
  ```
* Compilar un programa, dentro de **ns3**
  ```console
    ./waf
  ```
* Usar un programa, dentro de **ns3**
  ```console
    ./waf --run scratch/proyects/[NOMBRE DEL ARCHIVO, SIN LA EXTENSION CC]
  ```

# Referencias
* [Tutorial 1](https://www.nsnam.org/docs/release/3.29/tutorial/html/conceptual-overview.html#a-first-ns-3-script)
* [Copiar dinamicamente archivos para docker](https://stackoverflow.com/questions/18878216/docker-how-to-live-sync-host-folder-with-container-folder)
* [NS3 en Docker](https://hub.docker.com/r/ryankurte/docker-ns3/~/dockerfile/)