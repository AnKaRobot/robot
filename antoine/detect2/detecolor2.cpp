#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>

#define SOURCE_FILE "source"
#define OUTPUT_FILE "output"

// C920 Logitech
#define FIELD_VIEW 78
#define FOCALE_LEN 3.67
#define SENSOR_DIAG 6.0
#define SENSOR_W 4.8
#define SENSOR_H 3.6

#define OBJECT_HEIGHT 3

#define REFRESH 30
#define PAUSE_KEY 32
#define EXIT_KEY 10

#define HUE_BASE 180
#define HUE_TOLERANCE 20
#define SATURATION_BASE 255
#define SATURATION_TOLERANCE 39
#define DILATE_SIZE 2
#define ERODE_SIZE 7

#define MAX_ERRORS 5
#define MIN_PIX 0

#define VITESSE 100
#define TROP_PRES 9
#define TROP_LOIN 21
#define TROP_A_GAUCHE -11
#define TROP_A_DROITE 11

#define AVANCER 1
#define STOPPER 0
#define RECULER -1

using namespace cv;
using namespace std;

// Fonctions utilisées
float findRotation(int center, float angleView, int widthImage);
float findDistance(int rayon, int imageH);
void findVitesses(float distance, float angle, float & Rg, float & Rd);
void findObject(vector<vector<Point> > contours, Point & center, int & rayon);

int main (int argc, char **argv) {
    
	cout << "Début\n";
    
    // Capture de la source
    ifstream sourceFile;
    string source;
    sourceFile.open(SOURCE_FILE);
    getline(sourceFile, source);
    sourceFile.close();
    VideoCapture video;
    switch (source[0]) {
        case 'v':
            video.open(source.substr(2));
            break;
        default:
            video.open(0);
            break;
    }
    
    
    // Gestion des erreurs si la capture est vide
	if (! video.isOpened()) {
		cout << "Problème source\n";
		return -1;
	}

    // Création des fenêtres
	namedWindow("trace", CV_WINDOW_NORMAL);
	namedWindow("panel", CV_WINDOW_NORMAL);
	namedWindow("transformed", CV_WINDOW_NORMAL);
	namedWindow("base", CV_WINDOW_NORMAL);
	
	resizeWindow("base", 500, 375);
	resizeWindow("transformed", 500, 375);
	resizeWindow("panel", 450, 175);
	resizeWindow("trace", 500, 375);
	
	moveWindow("base", 0, 0);
	moveWindow("transformed", 520, 550);
	moveWindow("panel", 0, 410);
	moveWindow("trace", 520, 0);
	
	// Déclaration des variables utilisées
	int blur = 1,
	    tracer = 1,
	    norma = 1,
	    inverseRed = 1,
	    hue = HUE_BASE,
	    saturation = SATURATION_BASE,
	    hueCustom = HUE_TOLERANCE,
	    saturationCustom = SATURATION_TOLERANCE,
	    compteurErreurs = 0,
        key = 0,
        videoHeight = video.get(CV_CAP_PROP_FRAME_HEIGHT),
        videoWidth = video.get(CV_CAP_PROP_FRAME_WIDTH),
	    rayon = max(videoWidth, videoHeight);
        
    float angle = 0.0, 
        distance = 0.0, 
        Rg = 0.0, 
        Rd = 0.0;
	
	bool continuer = true,
	    pause = false;
	   
	Mat frameOrigine,
	    frameCouleurs,
	    frameHSV,
	    frameDetection,
	    frameDetectionInverseRed,
	    frameTrace(videoHeight, videoWidth, CV_8UC3),
        frameContours,
        element;
        
	Point center((int)(videoWidth / 2) , (int)(videoHeight / 2));
	
	vector<Mat> channels;
	
	vector<vector<Point> > contours;
	
	ofstream output;
	output.open(OUTPUT_FILE);

    // Création des barres de sélection	
	//createTrackbar("blur", "panel", &blur, 5);
	createTrackbar("trace", "panel", &tracer, 1);
	//createTrackbar("hue base", "panel", &hue, 180);
	//createTrackbar("saturation base", "panel", &saturation, 255);
	createTrackbar("hue tolerance", "panel", &hueCustom, 100);
	createTrackbar("saturation tolerance", "panel", &saturationCustom, 100);
	createTrackbar("norma", "panel", &norma, 1);
	createTrackbar("Inverse Red Range", "panel", &inverseRed, 1);
	
	while (continuer) {
	
	    // Récupération d'une image
	    if (! pause) {
		    video >> frameOrigine;
		}
		frameCouleurs = frameOrigine.clone();
		
		if (frameCouleurs.empty()) {
		
		    // Gestion des erreurs si l'image est vide
			cout << "Problème frame\n";
			compteurErreurs ++;
			if (compteurErreurs > MAX_ERRORS) {
	            cout << "Arrêt : 6 frames erronées consécutives\n";
	            continuer = false;
			}
		}
		else {
		    compteurErreurs = 0;
		    
		    // Blur / Flou
		    if (blur) {
		        //GaussianBlur(frame, frame, Size(9, 9), blur, blur);
		    }
		    
		    // Normalize
		    if (norma) {
		    	cvtColor(frameCouleurs, frameCouleurs, CV_BGR2YCrCb); 
		    	split(frameCouleurs, channels); 
		    	equalizeHist(channels[0], channels[0]);
		    	merge(channels,frameCouleurs); 
			    cvtColor(frameCouleurs, frameCouleurs, CV_YCrCb2BGR); 
		    }
		    
		    // BGR 2 HSV
			cvtColor(frameCouleurs, frameHSV, CV_BGR2HSV);
			
			// Couleur ciblée en blanc et le reste en noir
            
			inRange(
			    frameHSV, 
		        Scalar(hue - hueCustom, saturation - saturationCustom, 0), 
		        Scalar(hue + hueCustom, saturation + saturationCustom, 255), 
                frameDetection
            );
            
            // Autre rouge (il y a la teinte 180 et la 0)
            if (inverseRed) {
                inRange(
			        frameHSV, 
		            Scalar(0, saturation - saturationCustom, 0), 
		            Scalar(5, saturation + saturationCustom, 255), 
                    frameDetectionInverseRed
                );
                addWeighted(frameDetection, 1, frameDetectionInverseRed, 1, 0, frameDetection, frameDetection.type());
            }
            
            // Erode
            element = getStructuringElement(MORPH_ELLIPSE, Size(ERODE_SIZE, ERODE_SIZE));    
            erode(frameDetection, frameDetection, element);
            
            // Dilate
            element = getStructuringElement(MORPH_ELLIPSE, Size(DILATE_SIZE, DILATE_SIZE));    
            dilate(frameDetection, frameDetection, element);           
            
            // Trouver l'objet
            frameContours = frameDetection.clone();
            findContours(frameContours, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0)); 
            drawContours(frameDetection, contours, -1, Scalar(255, 255, 0));
            findObject(contours, center, rayon);
            
            // Calculer angle distance et vitesses
            angle = findRotation(center.x, FIELD_VIEW, videoWidth);
            distance = findDistance(rayon, videoHeight);
            findVitesses(distance, angle, Rg, Rd);
                
            // Ecrire la rotation, la distance et les vitesses
            if(! pause) {
                output 
                    << "Rg = " << Rg << " "
                    << "Rd = " << Rd << " "
                    << "A = " << angle << "°            "
                    << "D = " << distance << "cm" << endl;
            }
            
            if (rayon <= 0) { rayon = 1; }
            
            // Affichage
            if (tracer) {
            
                // Fenêtre Base
                circle(frameCouleurs, center, 2, Scalar(0, 255, 0), -1); // centre
                circle(frameCouleurs, center, rayon, Scalar(0, 255, 0), 1); // périmètre
                
                // Fenêtre Transformed
                cvtColor(frameDetection, frameDetection, CV_GRAY2BGR);
                circle(frameDetection, center, 2, Scalar(0, 0, 255), -1);
                circle(frameDetection, center, rayon, Scalar(0, 0, 255), 1); 
            }
            
            // Fenêtre Trace
            circle(frameTrace, center, 2, Scalar(0, 0, 255), -1);
            circle(frameTrace, center, rayon, Scalar(0, rayon * 5, 255), 1); 
            
			imshow("base", frameCouleurs);
			imshow("transformed", frameDetection);
			imshow("trace", frameTrace);
		}
		
		// Rafraîchissement
		key = waitKey(REFRESH);
		switch (key) {
		    case PAUSE_KEY :
		        pause = pause ? false : true;
	            break;
            case EXIT_KEY :
                continuer = false;
                break;
		}
	}

    // Libération de mémoire
	destroyAllWindows();
	element.release();
	video.release();
	frameOrigine.release();
	frameCouleurs.release();
	frameHSV.release();
	frameDetection.release();
	frameDetectionInverseRed.release();
	frameTrace.release();
	frameContours.release();
	output.close();
	
	cout << "Fin\n";
    
	return 0;
}

void findObject(vector<vector<Point> > contours, Point & center, int & rayon) {
    
    int newRayon = 0, 
        newNewRayon = 0, 
        minDifference = -1, 
        newDifference = 0,
        minX = -1, 
        maxX = -1, 
        minY = -1, 
        maxY = -1,
        x = 0,
        y = 0;
                
        Point newCentre, 
            newNewCentre;
            
    // Parcourir tous les contours découverts
    for (vector<vector<Point> >::iterator vecPt = contours.begin(); vecPt != contours.end(); ++vecPt)   {
            minX = -1; maxX = -1;
            minY = -1; maxY = -1;
        
        // Récupérer les min, max de X, Y pour les points du contour
        for (vector<Point>::iterator pt = vecPt->begin(); pt != vecPt->end(); ++pt) {
            
            x = (*pt).x;
            y = (*pt).y;
            
            if (x <= minX or minX == -1) { 
                minX = x; }
            else if (x >= maxX or maxX == -1) { 
                maxX = x; }
            if (y <= minY or minY == -1) { 
                minY = y; }
            else if (y >= maxY or maxY == -1) { 
                maxY = y; }
            
        }
        // Calculer le centre à partir des minimum et maximum
        newNewCentre = Point(maxX - ((maxX-minX)/2), maxY - ((maxY-minY)/2) );
        // Calculer le rayon à partir de la longueur
        newNewRayon = max((maxX - minX), (maxY - minY)) / 2;
        
        newDifference = 
            abs(center.x - newNewCentre.x) 
            + abs(center.y - newNewCentre.y)
            + abs(rayon - newNewRayon);
        
        // Choisir le contour le plus pertinent par rapport au centre de l'image précédente
        if (newDifference < minDifference or minDifference == -1) {
            minDifference = newDifference;
            newCentre = newNewCentre;
            newRayon = newNewRayon;
        }    
    }
    
    // Mettre à jour le centre et le rayon
    center = newCentre;
    rayon = newRayon;  
}

float findRotation(int center, float angleView, int widthImage) {
    if (widthImage > 0) {
        return ((center * angleView) / widthImage) - (angleView / 2); 
    }
    else { return 0; }
}


float findDistance(int rayon, int imageH) {
    if (rayon > 0) {
        return (FOCALE_LEN * OBJECT_HEIGHT * imageH) / (rayon * SENSOR_H) / 10;
    }
    else { return 0; }
}

void findVitesses(float distanceObjet, float angleObjet, float & gauche, float & droite){
    
    float diff = abs(angleObjet) / 90;
    
    // L'objet est trop loin
    if (distanceObjet >= TROP_LOIN and distanceObjet < 1000) {
        gauche = AVANCER;
        droite = AVANCER;
        if (angleObjet > 0) { droite -= diff; }
        else if (angleObjet < 0) { gauche -= diff; }
    }
    // L'objet est trop près
    else if (distanceObjet <= TROP_PRES and distanceObjet > 0) {
        gauche = RECULER;
        droite = RECULER;
        if (angleObjet > 0) { gauche += diff; }
        else if (angleObjet < 0) { droite += diff; }
    }
    else { // L'objet est à distance adéquate
        if (angleObjet >= TROP_A_DROITE) { 
            gauche = diff; 
            droite = -diff; 
        }
        else if (angleObjet <= TROP_A_GAUCHE) { 
            gauche = -diff; 
            droite = diff; 
        }
        else { 
            gauche = STOPPER; 
            droite = STOPPER; 
        }
    }
    
    gauche *= VITESSE;
    droite *= VITESSE;
}

