PAV - P2: detección de actividad vocal (VAD)
============================================
A continuación se resolverán los ejercicios propuestos en el fichero README.md original

Ejercicios
----------

### Etiquetado manual de los segmentos de voz y silencio

- Etiquete manualmente los segmentos de voz y silencio del fichero grabado al efecto. Inserte, a 
  continuación, una captura de `wavesurfer` en la que se vea con claridad la señal temporal, el contorno de
  potencia y la tasa de cruces por cero, junto con el etiquetado manual de los segmentos.

![Etiquetado_ZRC_Waveform](https://user-images.githubusercontent.com/61736138/76706049-f8256b80-66e4-11ea-8003-953c51f08faa.png)

- A la vista de la gráfica, indique qué valores considera adecuados para las magnitudes siguientes:

	* Incremento del nivel potencia en dB, respecto al nivel correspondiente al silencio inicial, para estar
      seguros de que un segmento de señal se corresponde con voz.
      
      Hemos considerado que el nivel de poténcia inicial es de unos 8 a 10 dB y, como sabemos que las letras que tienen una poténcia menor son aquellas del tipo fricativas, hemos tomado su valor (35.5dB) como el mínimo al cual correspondería voz. Por lo tanto, el incremento del nivel de poténcia en dB para estar seguros que un segmento de señal corresponde a voz es de 25dB.

	* Duración mínima razonable de los segmentos de voz y silencio.
	
		La duración mínima razonable de los segmentos de voz es de 20ms, mientras que para los segmentos de silencio es de 180ms ya que no queremos detectar una pausa en el habla como un silencio.

	* ¿Es capaz de sacar alguna conclusión a partir de la evolución de la tasa de cruces por cero?
	
		La tasa de cruces por zero nos sirve como alternativa a la poténcia en el caso de las fricativas sordas y otras consonantes que tienen niveles de poténcia bajos y, en ocasiones, se pueden llegar a confundir con silencio o ruido ambiente. Esto lo hacemos teniendo en cuenta que las fricativas sordas tienen una tasa de cruces por cero más elevada que el resto. A pesar de todo, en archivos de audio con poco ruido, únciamente comparando los niveles de poténcia ya se detecta correctamente y, en audios con mucho ruido, este puede distorsionar la detección al tener trozos de ruido con tasas de cruze por cero elevadas. Por lo tanto, la vía principal de detección de actividad vocal es la poténcia.


### Desarrollo del detector de actividad vocal

- Complete el código de los ficheros de la práctica para implementar un detector de actividad vocal tan
  exacto como sea posible. Tome como objetivo la maximización de la puntuación-F `TOTAL`.
  
  	Cómo se puede observar en la siguiente imagen, una vez completado en código de los ficheros de la práctica para implementar el VAD, hemos conseguido una puntuación FTOTAL máxima de 98.62%.
	
	![AudioEvaluation](https://user-images.githubusercontent.com/61736138/76706053-007da680-66e5-11ea-87d9-ac9f2d092fdd.png)


- Inserte una gráfica en la que se vea con claridad la señal temporal, el etiquetado manual y la detección
  automática conseguida para el fichero grabado al efecto. 

![Comparacion_LAB_VAD](https://user-images.githubusercontent.com/61736138/76706131-96193600-66e5-11ea-8cd2-319f534c3ff6.png)

- Explique, si existen. las discrepancias entre el etiquetado manual y la detección automática.

	Hemos conseguido un archivo .vad pràcticamente idéntico al .lab que habiamos generado manualmente; por lo tanto tendriamos un FTOTAL muy próxima al 100%. De hecho, no habiamos etiquetado de forma manual el silencio final, cosa que si nos ha detectado el VAD y ha marcado en el archivo .vad. Ahora bien, como hemos configurado nuestro detector de actividad vocal para clavar los periodos de habla y de silencio de nuestro archivo de audio, el FTOTAL en otros archivos de audio puede no llegar a ser tan elevado. Esto lo comprovaremos en el siguiente punto.

- Evalúe los resultados sobre la base de datos `db.v4` con el script `vad_evaluation.pl` e inserte a 
  continuación las tasas de sensibilidad (*recall*) y precisión para el conjunto de la base de datos (sólo
  el resumen).
  
 	Mediante el script `vad_evaluation.pl` nos perminte evaluar nuestro detector de actividad vocal sobre archivos de audio que no sean el nuestro y comprovar su funcionamiento real. Al ejecutar el comando `scripts/run_vad.sh db.v4/*/*wav` se calcula y muestra por el terminal los parámetros Racall, Precision y F-Score de cada uno de los archivos y, dinalmente, realiza una media que es lo que nos interesa. Los parámetros acabados de mecionar se pueden observar para el caso de fragmentos de silencio y de voz en la siguiente imagen: 
	
	![DatabaseEvaluation](https://user-images.githubusercontent.com/61736138/76706161-b6e18b80-66e5-11ea-83b4-3c9fa2a28e1a.png)


### Trabajos de ampliación

#### Cancelación del ruido en los segmentos de silencio

- Si ha desarrollado el algoritmo para la cancelación de los segmentos de silencio, inserte una gráfica en
  la que se vea con claridad la señal antes y después de la cancelación (puede que `wavesurfer` no sea la
  mejor opción para esto, ya que no es capaz de visualizar varias señales al mismo tiempo).
  
  	Para observar la cancelación de los segmentos de silencio hemos mostrado los dos archivos de audio a la vez con el Wavesurfer y, para verlo de forma más precisa, en vez de comparar sus formas de onda (Waveform) compararemos sus graficas de poténcia. En la parte superior se puede ver la poténcia del señal de audio pav_2381.wav (archivo original), y en la parte inferior, la potencia del archivo modificado junto a su forma de onda. Podemos observar que solo hay niveles de poténcia distintos de 0 en los momentos en que se habla, en caso contario, esta es nula.

![WhatsApp Image 2020-03-14 at 18 25 33](https://user-images.githubusercontent.com/61736138/76687801-acf95300-6627-11ea-8dbc-1c82207b87e3.jpeg)


#### Gestión de las opciones del programa usando `docopt_c`

- Si ha usado `docopt_c` para realizar la gestión de las opciones y argumentos del programa `vad`, inserte
  una captura de pantalla en la que se vea el mensaje de ayuda del programa.
  
  Mediante el comando `bin/vad -h` o `bin/vad --help` podemos mostrar por terminar el mensaje de ayuda del programa con todos los comandos que podemos insertar:
  
  	![VadOptions](https://user-images.githubusercontent.com/61736138/76706241-4edf7500-66e6-11ea-9feb-b0b5271a9841.png)

	A continuación observamos el correcto funcionamiento de esta herramienta, ya que al establecer valores distintos a los parámetros `a1, a2, mvs...`, el FTOTAL se reduce como era de esperar al no tener los parámetros idóneos.

	![VadParam](https://user-images.githubusercontent.com/61736138/76706274-84845e00-66e6-11ea-9fd4-1038332d6a59.png)

### Contribuciones adicionales y/o comentarios acerca de la práctica

- Indique a continuación si ha realizado algún tipo de aportación suplementaria (algoritmos de detección o 
  parámetros alternativos, etc.).

- Si lo desea, puede realizar también algún comentario acerca de la realización de la práctica que considere
  de interés de cara a su evaluación.

