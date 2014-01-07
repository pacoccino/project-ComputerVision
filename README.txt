 Analyse et traitement d’images issues de robots
 développés au LaBRI dans le cadre de la RoboCup
=================================================

 Equipe
-------------------------------------------------

 Maxime  Bellier
 Pacien  Boisson
 Clément Brisset
 Cédric  Jolys


 Compilation
-------------------------------------------------

 Executer le Makefile permet de produire le
 binaire ComputerVisison :
 
	make

 Si nécessaire, il est possible de regénerer le
 Makefile avec qmake :

 	qmake ComputerVision.pro


 Execution
-------------------------------------------------

 Les paramètres du programme sont :

 <image directory>  dossier du type 
                    RhobanVisionLog/log1

 <start image>      Numéro de la première image

 <end image>        Numéro de la dernière image

 Paramètres facultatifs :

 <image prefix>     Prefix des images
 
 <image extension>  Extension des images 
 
 <delay (ms)>       Durée entre deux images

 
 Exemples
-------------------------------------------------

 Traiter les images '0.png' à '60.png' du 
 dossier log2 :
      
      ./RhobanVisionLog/log2/ 0 60 


 Traiter les images 'rgb0.png' à 'rgb60.png' du
 dossier log2, avec un delais de 1000 ms :

 ./RhobanVisionLog/log2/ 0 60 "rgb" ".png" 1000

 NB: Le programme ne fonctionne correctement 
 que sur les images HSV.



