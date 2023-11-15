# ESP32ServerClientUDP
server ESP32 
cliente01 Belly
cliente02 Chest

el server va enchufado a la compu. tiene dos botones
cuando toco, el cliente se resetea*

Version1 mando reset 
Version2 mando una orden para poner en sensor en modo calibracion

el sensor tiene 2 modos: 
_Calibrated manda datos continuos mientras hace la calibracion 
_Crude cambios de estado para (1 y 2) de la senal ya calibrada

para la calibracion uso esta libreria Plaquette.h
https://github.com/SofaPirate/Plaquette/tree/master

En Unity, uso este asset https://github.com/dwilches/Ardity/tree/master/UnityProject/Assets/Ardity/Scripts
el script "SerialController"
y otro que hize yo llamado Inputs (en la carpeta Unity)
con esta funcion activo todas las respuestas audio-visuales del juego

![image](https://github.com/YESI360/ESP32ServerClientUDP/assets/41321821/79df5078-6c7f-4571-ac49-eb41573f0ce9)

#demo del sensor en el pecho
https://drive.google.com/file/d/11XEk1AfwvsFiewQ2JZujupUTQfr0nQug/view?usp=drive_link

#demo de la instalacion
https://drive.google.com/file/d/1VkUtWb_ikpXN3ZJYNjZLXKw96-gHU1DK/view?usp=drive_link

#demo del funcionamiento server/client
[https://drive.google.com/file/d/12OC1lFa4HGMUCTZ6yY3kOXbRDUAy4mEZ/view?usp=sharing](https://drive.google.com/file/d/1OIRZ3unpIvKRhJJr_Llv9eHtv2xBvs_L/view?usp=drive_link)


![server client + inside](https://user-images.githubusercontent.com/41321821/205462976-2d1d24ca-24ab-4deb-bcf2-380d6776a7ac.jpg)

![sensorv4](https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExMG91Z2Z0Y2N1cHYxZG03b3Zsajh4b3VkeDFmc3R2NzFwa3J6OHM1eiZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/hp0cSpJAWCZ881lewp/giphy.mp4)

