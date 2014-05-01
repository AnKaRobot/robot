#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>

#define SOURCE_FILE "source"
#define OUTPUT_FILE "output"

#define MAX_ERRORS 5
#define MIN_PIX 0

// C920 Logitech
#define FIELD_VIEW 78
#define FOCALE_LEN 3.67
#define SENSOR_DIAG 6.0
#define SENSOR_W 4.8
#define SENSOR_H 3.6

#define OBJECT_HEIGHT 4.4

#define DIST_SECURE_MIN 10
#define DIST_SECURE_MAX 20

#define TOP 0
#define TOP_RIGHT 1
#define RIGHT 2
#define BOT_RIGHT 3
#define BOT 4
#define BOT_LEFT 5
#define LEFT 6
#define TOP_LEFT 7

#define REFRESH 30
#define PAUSE_KEY 32

#define HUE_BASE 175
#define HUE_TOLERANCE 54
#define SATURATION_BASE 226
#define SATURATION_TOLERANCE 57
#define DILATE_SIZE 2
#define ERODE_SIZE 7

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
	    compteurErreurs = 0,
	    rayon = 0,
        key = 0;
        
    float angle = 0.0, 
        distance = 0.0, 
        Rg = 0.0, 
        Rd = 0.0;
	
	bool continuer = true,
	    pause = false;
	   
	Mat frameOrigine,
	    frame, 
	    frame2,
	    trace(
	        video.get(CV_CAP_PROP_FRAME_HEIGHT), 
	        video.get(CV_CAP_PROP_FRAME_WIDTH),
	        CV_8UC3
        ),
        element;
        
	Point center(0, 0);
	
	vector<Mat> channels;
	
	vector<vector<Point> > contours;
	
	Scalar intensity;
	
	ofstream output;
	output.open(OUTPUT_FILE);

    // Création des barres de sélection	
	createTrackbar("blur", "panel", &blur, 5);
	createTrackbar("trace", "panel", &tracer, 1);
	
	while (continuer) {
	
	    // Récupération d'une image
	    if (! pause) {
		    video >> frameOrigine;
		}
		frame = frameOrigine.clone();
		
		if (frame.empty()) {
		
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
		        GaussianBlur(frame, frame, Size(9, 9), blur, blur);
		    }
		    
		    // BGR 2 HSV
			cvtColor(frame, frame2, CV_BGR2HSV);
			
			// Couleur ciblée en blanc et le reste en noir
			inRange(
			    frame2, 
                Scalar(HUE_BASE - HUE_TOLERANCE, SATURATION_BASE - SATURATION_TOLERANCE, 0), 
                Scalar(HUE_BASE + HUE_TOLERANCE, SATURATION_BASE + HUE_TOLERANCE, 255), 
                frame2
            );
            
            // Erode
            element = getStructuringElement(MORPH_ELLIPSE, Size(ERODE_SIZE, ERODE_SIZE));    
            erode(frame2, frame2, element);
            
            // Dilate
            element = getStructuringElement(MORPH_ELLIPSE, Size(DILATE_SIZE, DILATE_SIZE));    
            dilate(frame2, frame2, element);           
            
            // Trouver l'objet
            findContours(frame2, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0)); 
            drawContours(frame, contours, -1, Scalar(255, 255, 0));
            findObject(contours, center, rayon);
            
            // Calculer angle distance et vitesses
            angle = findRotation(center.x, FIELD_VIEW, frame2.size().width);
            distance = findDistance(rayon, frame2.size().height);
            findVitesses(distance, angle, Rg, Rd);
                
            // Ecrire la rotation, la distance et les vitesses
            if(! pause) {
                output 
                    << "A = " << angle << "°            "
                    << "D = " << distance << "cm        "
                    << "Rg = " << Rg << "   "
                    << "Rd = " << Rd << "\n";
            }
            
            // Affichage
            if (tracer) {
            
                // Fenêtre Base
                circle(frame, center, 2, Scalar(0, 255, 0), -1); // centre
                circle(frame, center, rayon, Scalar(0, 255, 0), 1); // périmètre
                
                // Fenêtre Transformed
                cvtColor(frame2, frame2, CV_GRAY2BGR);
                circle(frame2, center, 2, Scalar(0, 0, 255), -1);
                circle(frame2, center, rayon, Scalar(0, 0, 255), 1); 
            }
            
            // Fenêtre Trace
            circle(trace, center, 2, Scalar(0, 0, 255), -1);
            circle(trace, center, rayon, Scalar(0, rayon * 5, 255), 1); 
            
			imshow("base", frame);
			imshow("transformed", frame2);
			imshow("trace", trace);
		}
		
		// Rafraîchissement
		key = waitKey(REFRESH);
		switch (key) {
		    case PAUSE_KEY :
		        pause = pause ? false : true;
	            break;
            case -1 :
                break;
            default :
                continuer = false;
                break;
		}
	}

    // Libération de mémoire
	destroyAllWindows();
	element.release();
	video.release();
	frameOrigine.release();
	frame.release();
	frame2.release();
	trace.release();
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

void findVitesses(float distance, float angle, float & Rg, float & Rd){
    
    float diff = abs(angle) / 90;
    
    if (distance < DIST_SECURE_MIN and distance > 0) {
        Rg = -1;
        Rd = -1;
        if (angle > 0) { Rg = 0 - diff; }
        else if (angle < 0) { Rd = 0 - diff; }
    }
    else if (distance > DIST_SECURE_MAX and distance < 1000) {
        Rg = 1;
        Rd = 1;
        if (angle > 0) { Rg = 1 - diff; }
        else if (angle < 0) { Rd = 1 - diff; }
    }
    else {
        if (angle > 0) { Rg = 1; Rd = -1; }
        else if (angle < 0) { Rg = -1; Rd = 1; }
        else { Rg = 0; Rd = 0; }
    }
    
    Rg *= 300;
    Rd *= 300;
}

