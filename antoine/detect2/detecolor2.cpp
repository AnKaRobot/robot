#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>

// SOURCE ET OUTPUT
#define SOURCE_FILE "source" // source
#define OUTPUT_FILE "output" // output
#define OUTPUT_VIDEO_FILE "videoutput.avi" // videoutput.avi
#define DETECTION_FILE "detectionFile" // detectionFile

#define OUTPUT_VALEURS true // true
#define OUTPUT_VIDEO true // true
// ================||

// VALEURS CAMERA (default : logitech C92)
#define FIELD_VIEW 78 // 78
#define FOCALE_LEN 3.67 // 3.67
#define SENSOR_DIAG 6.0 // 6.0
#define SENSOR_W 4.8 // 4.8
#define SENSOR_H 3.6 // 3.6
// ==============||

// CARACTERISTIQUES OBJET
#define OBJECT_HEIGHT 8 // 10
// ======================||

// BOUCLE OPENCV
#define REFRESH 30 // 30
#define PAUSE_KEY 32 // 32
#define EXIT_KEY 10 // 10
// =============||

// VALEURS PANEL DETECTION
#define BLUR 5

#define HUE_BASE 180 // 180
#define HUE_TOLERANCE 5 // 20
#define SATURATION_BASE 255 // 186
#define SATURATION_TOLERANCE 165 // 60

#define HUE_BASE_INVERSE 0 // 0
#define HUE_TOLERANCE_INVERSE 0 // 5
#define SATURATION_BASE_INVERSE 255 // 184
#define SATURATION_TOLERANCE_INVERSE 165 // 60

#define ERODE_SIZE 10 // 7
#define DILATE_SIZE 0 // 2
// =====================||

// CHOIX DES VITESSES DES ROUES
#define VITESSE 50 // 50
#define TROP_PRES 30 // 30
#define TROP_LOIN 100 // 100
#define TROP_A_GAUCHE -11 // -11
#define TROP_A_DROITE 11 // 11

#define AVANCER 1 // 1
#define STOPPER 0 // 0
#define RECULER -1 // -1
// =============================||

// COMPORTEMENT DU ROBOT
#define MAXIMUM_DEPLACEMENT_POSSIBLE 20 // 20
// =====================||


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
    
    if(sourceFile.is_open()) {
		getline(sourceFile, source);
		sourceFile.close();
	}
	else { source = "0"; }
	
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
	resizeWindow("panel", 500, 750);
	resizeWindow("trace", 500, 375);
	
	moveWindow("base", 0, 0);
	moveWindow("transformed", 0, 375);
	moveWindow("panel", 500, 0);
	moveWindow("trace", 1000, 0);
	
	// Déclaration des variables utilisées
	int blur = BLUR,
	    tracer = 1,
	    norma = 1,
	    inverseRed = 1,
	    hue = HUE_BASE,
	    saturation = SATURATION_BASE,
	    hueCustom = HUE_TOLERANCE,
	    saturationCustom = SATURATION_TOLERANCE,
	    hueInverse = HUE_BASE_INVERSE,
	    hueToleranceInverse = HUE_TOLERANCE_INVERSE,
	    saturationInverse = SATURATION_BASE_INVERSE,
	    saturationToleranceInverse = SATURATION_TOLERANCE_INVERSE,
	    compteurErreurs = 0,
        key = 0,
        videoHeight = video.get(CV_CAP_PROP_FRAME_HEIGHT),
        videoWidth = video.get(CV_CAP_PROP_FRAME_WIDTH),
	    rayon = max(videoWidth, videoHeight),
	    erodeSize = ERODE_SIZE,
	    dilateSize = DILATE_SIZE;
        
    float angle = 0.0,
    	newAngle = 0.0, 
        distance = 0.0, 
        newDistance = 0.0,
        Rg = 0.0, 
        Rd = 0.0;
	
	bool continuer = true,
	    pause = false,
	    registerVideo = OUTPUT_VIDEO,
	    registerValeurs = OUTPUT_VALEURS;
	   
	Mat frameOrigine,
	    frameCouleurs,
	    frameHSV,
	    frameDetection,
	    frameDetectionInverseRed,
	    frameTrace(videoHeight, videoWidth, CV_8UC3),
        frameContours,
        frameOutput(videoHeight, videoWidth * 3, CV_8UC3),
        element;
        
	Point center((int)(videoWidth / 2) , (int)(videoHeight / 2));
	
	vector<Mat> channels, outputFrames(3);
	
	vector<vector<Point> > contours;
	
	VideoWriter outputVideo;
	outputVideo.open(
		OUTPUT_VIDEO_FILE, 
		CV_FOURCC('M', 'J', 'P', 'G'), 
		10, 
		Size(videoWidth * 3, videoHeight), 
		true
	);
	if (! outputVideo.isOpened()) {
		cout << "Problème output video.\n";
		registerVideo = false;
	} 
	
	ofstream output;
	output.open(OUTPUT_FILE);
	if (! output.is_open()) { registerValeurs = false; }

    // Création des barres de sélection	
	createTrackbar("Valeur de flou", "panel", &blur, 30);
	createTrackbar("Appliquer l'égalisation d'histogramme", "panel", &norma, 1);
	
	createTrackbar("hue base", "panel", &hue, 180);
	createTrackbar("saturation base", "panel", &saturation, 255);
	createTrackbar("hue tolerance", "panel", &hueCustom, 180);
	createTrackbar("saturation tolerance", "panel", &saturationCustom, 255);
	
	createTrackbar("hue base Inverse", "panel", &hueInverse, 180);
	createTrackbar("saturation base Inverse", "panel", &saturationInverse, 255);
	createTrackbar("hue tolerance Inverse", "panel", &hueToleranceInverse, 180);
	createTrackbar("saturation tolerance Inverse", "panel", &saturationToleranceInverse, 255);
	
	createTrackbar("Taille Erode", "panel", &erodeSize, 30);
	createTrackbar("Taille Dilate", "panel", &dilateSize, 30);
	
	createTrackbar("Appliquer la détection inverse", "panel", &inverseRed, 1);
	
	createTrackbar("Afficher la détection", "panel", &tracer, 1);
	
	
	
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
			if (compteurErreurs > 5) {
	            cout << "Arrêt : 6 frames erronées consécutives\n";
	            continuer = false;
			}
		}
		else {
		    compteurErreurs = 0;
		    
		    // Blur / Flou
		    if (blur) {
		        GaussianBlur(frameCouleurs, frameCouleurs, Size(9, 9), blur, blur);
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
		            Scalar(hueInverse - hueToleranceInverse, saturationInverse - saturationToleranceInverse, 0), 
		            Scalar(hueInverse + hueToleranceInverse, saturationInverse + saturationToleranceInverse, 255), 
                    frameDetectionInverseRed
                );
                addWeighted(frameDetection, 1, frameDetectionInverseRed, 1, 0, frameDetection, frameDetection.type());
            }
            
            // Erode
            if (erodeSize) {
		        element = getStructuringElement(
		        	MORPH_ELLIPSE, 
		        	Size(erodeSize, erodeSize)
	        	);    
		        erode(frameDetection, frameDetection, element);
	        }
	        
	        // Dilate
	        if (dilateSize) {
		        element = getStructuringElement(
		        	MORPH_ELLIPSE, 
		        	Size(dilateSize, dilateSize)
	        	);    
		        dilate(frameDetection, frameDetection, element);           
            }
            
            // Trouver l'objet
            frameContours = frameDetection.clone();
            findContours(frameContours, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0)); 
            drawContours(frameDetection, contours, -1, Scalar(255, 255, 0));
            findObject(contours, center, rayon);
            
            // Calculer angle distance et vitesses
            newDistance = findDistance(rayon, videoHeight);
	        newAngle = findRotation(center.x, FIELD_VIEW, videoWidth);
	        // Amelioration de comportement quand detection defaillante
            if (abs(distance - newDistance) > MAXIMUM_DEPLACEMENT_POSSIBLE) {
            	Rg = STOPPER;
            	Rd = STOPPER;
            }
            else {
            	distance = newDistance;
            	angle = newAngle;
	        	findVitesses(distance, angle, Rg, Rd);
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
			
			// Output
            if (! pause) {
            	if (registerVideo) {
				    outputFrames[0] = frameCouleurs;
					outputFrames[1] = frameDetection;
					outputFrames[2] = frameTrace;
		        	hconcat(outputFrames, frameOutput);
		        	outputVideo << frameOutput;
            	}
            	if (registerValeurs) {
		            output 
		                << "Rg = " << Rg << " "
		                << "Rd = " << Rd << " "
		                << "A = " << angle << "°            "
		                << "D = " << distance << "cm" << endl;
                }
            }
			
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
	frameOutput.release();
	output.close();
	outputVideo.release();
	
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

