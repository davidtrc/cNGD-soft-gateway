cognitiveNGD
============

CognitiveNewGenerationDevice

*añadir librerias: 
- Botón derecho sobre el proyecto.
- Properties
- En la sección xc32-gcc -> Include Directories: todos los includes de las librerias implicadas.
  (En algunas versiones más recientes de MPLABX es posible que la opción Include Directories se encuentre dentro de
	xc32-gcc pero no en la categoría general... buscad entre las distintas categorías del menu desplegable)

Para el firmware del nodo (NODE_FW), las librerias a añadir (el orden estricto) son:

/NODE FW.X/CWSN LSI Node/include
/libs/Microchip/include

FIN.
