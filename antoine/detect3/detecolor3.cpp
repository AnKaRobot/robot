#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <libconfig.h++>

#define CONFIG_FILE "config.cfg" // detectionFile

#define AVANCER 1 // 1
#define STOPPER 0 // 0
#define RECULER -1 // -1

using namespace cv;
using namespace std;
using namespace libconfig;

// Fonctions utilisées
void createWindows (Config & config);
float findRotation (int center, float angleView, int widthImage);
float findDistance (int rayon, int imageH, float focaleLength, float objectHeight, float sensorHeight);
void findVitesses (float distance, float angle, float & Rg, float & Rd,
	float tropPres, float tropLoin, float tropAGauche, float tropADroite, float vitesse);
void findObject (vector<vector<Point> > contours, Point & center, int & rayon);


int main (int argc, char **argv) {
    
	cout << "Début\n";

	// DECLARATIONS ===========[
	
	Config config;
	
	VideoCapture video;
	
	int blur,
	    normalisation,
	    erodeSize,
	    dilateSize,
	    
	    hueBase,
	    saturationBase,
	    hueTolerance,
	    saturationTolerance,
	    valueBase,
	    valueTolerance,
	    
	    useInRange2,
	    
	    hueBase2,
	    hueTolerance2,
	    saturationBase2,
	    saturationTolerance2,
	    valueBase2,
	    valueTolerance2,
	    
	    tracer,
	    compteurErreurs,
        key,
      	fontFaceText,
      	thicknessText,
	    rayon,
	    maximumStep,
	    
	    refreshValue;
	    
    float angle,
    	newAngle, 
        distance, 
        newDistance,
        Rg, 
        Rd,
	    vitesseRobot,
	    objectHeight,
	    
	    tropPres,
    	tropLoin,
    	tropAGauche,
    	tropADroite,

		focaleLengthValue,
    	sensorHeightValue,
    	fieldViewValue;
	
	bool continuer,
	    pause,
	    registerVideo,
	    registerFile,
	    
	    useWindowsValue;
	   
   double fontScaleText;
   
	Mat frameOrigine,
	    frameCouleurs,
	    frameHSV,
	    frameDetection,
	    frameDetectionInverseRed,
	    frameTrace,
        frameContours,
        frameOutput,
        element;
        
    ostringstream distanceText,
    	angleText,
    	vitesseGText,
    	vitesseDText;
    
	Point center,
		lineDistanceText,
		lineAngleText,
		lineVitesseGText,
		lineVitesseDText;
	
	vector<Mat> 
		channels, 
		outputFrames(3);
		
	Scalar colorText;
	
	vector<vector<Point> > contours;
	
	VideoWriter outputVideo;
	
	ofstream output;
	
	// FIN DECLARATIONS ==============]
	
	
	// AFFECTATIONS ===================[
	
	config.readFile(CONFIG_FILE);
    
    try {
		switch ((int) config.lookup("source.type")) {
			case 0 : // webcam
				video.open( (int) config.lookup("source.value"));
				break;
			case 1 : // file
				video.open( (const char*) config.lookup("source.value"));
				break;	
		}	
	}
   	catch (SettingException & e) {
   		cout << "Erreur Configuration\n";
		cout << e.getPath() << endl;    
	}
	// Gestion des erreurs si la capture est vide
	if (! video.isOpened()) {
		cout << "Problème source\n";
		return -1;
	}
    
    const int videoHeight = video.get(CV_CAP_PROP_FRAME_HEIGHT),
        videoWidth = video.get(CV_CAP_PROP_FRAME_WIDTH);
        
    try {
	    refreshValue = config.lookup("opencvBoucle.refresh");
	   	blur = config.lookup("detection.blur");
	    normalisation = config.lookup("detection.normalisation");
	    erodeSize = config.lookup("detection.erodeSize");
	    dilateSize = config.lookup("detection.dilateSize");
	    
	    hueBase = config.lookup("detection.inRange.hue.baseValue");
	    saturationBase = config.lookup("detection.inRange.saturation.baseValue");
	    hueTolerance = config.lookup("detection.inRange.hue.tolerance");
	    saturationTolerance = config.lookup("detection.inRange.saturation.tolerance");
	    valueBase = config.lookup("detection.inRange.value.baseValue");
	    valueTolerance = config.lookup("detection.inRange.value.tolerance");
	    
	    useInRange2 = config.lookup("detection.useInRange2");
	    
	    hueBase2 = config.lookup("detection.inRange2.hue.baseValue");
	    hueTolerance2 = config.lookup("detection.inRange2.hue.tolerance");
	    saturationBase2 = config.lookup("detection.inRange2.saturation.baseValue");
	    saturationTolerance2 = config.lookup("detection.inRange2.saturation.tolerance");
	    valueBase2 = config.lookup("detection.inRange2.value.baseValue");
	    valueTolerance2 = config.lookup("detection.inRange2.value.tolerance");
	    
	    tracer = 1;
	    compteurErreurs = 0;
        key = 0;
        fontFaceText = FONT_HERSHEY_SIMPLEX;
        thicknessText = 2;
	    rayon = max(videoWidth, videoHeight);
	    maximumStep = config.lookup("analyseObjet.distance.maximumStep");
	    
	    fontScaleText = 0.5;
	    
	    angle = 0.0;
    	newAngle = 0.0;
        distance = 0.0;
        newDistance = 0.0;
        Rg = 0.0;
        Rd = 0.0;
	    vitesseRobot = config.lookup("robot.vitesse");
	    objectHeight = config.lookup("objectHeight");
        
    	tropPres = config.lookup("analyseObjet.distance.tropPres");
    	tropLoin = config.lookup("analyseObjet.distance.tropLoin");
    	tropAGauche = config.lookup("analyseObjet.angle.tropAGauche");
    	tropADroite = config.lookup("analyseObjet.angle.tropADroite");
    	
		focaleLengthValue = config.lookup("camera.focaleLength"),
    	sensorHeightValue = config.lookup("camera.sensorHeight"),
    	fieldViewValue = config.lookup("camera.fieldView");
	
		continuer = true,
	    pause = false,
	    registerVideo = config.lookup("output.registerVideo"),
	    registerFile = config.lookup("output.registerFile");
	    
	    colorText = Scalar(0, 0, 255); // red
	    
		useWindowsValue = config.lookup("windows.useWindows");
	   
		frameTrace = Mat(videoHeight, videoWidth, CV_8UC3),
        frameOutput = Mat(videoHeight, videoWidth * 3, CV_8UC3);
        
		center = Point((int)(videoWidth / 2) , (int)(videoHeight / 2));
		lineDistanceText = Point(5, 20);
		lineAngleText = Point(5, 40);
		lineVitesseGText = Point(5, 60);
		lineVitesseDText = Point(5, 80);
	
		outputVideo.open(
			(const char*) config.lookup("output.video.file"),
			CV_FOURCC('M', 'J', 'P', 'G'), 
			config.lookup("output.video.fps"), 
			Size(videoWidth * 3, videoHeight), 
			true
		);
		
		output.open((const char*) config.lookup("output.file"));
	}
	catch (SettingException & e) {
		cout << "Erreur Configuration\n";
		cout << e.getPath() << endl;  
		return -1;  
    }
	
	const int refresh = refreshValue;
	
	const float focaleLength = focaleLengthValue,
		sensorHeight = sensorHeightValue,
		fieldView = fieldViewValue;
		
	const bool useWindows = useWindowsValue;
	
	// FIN AFFECTATIONS =======================]
	
	
	// Gestion des erreurs output
	if (! outputVideo.isOpened()) {
		cout << "Problème output video.\n";
		registerVideo = false;
	} 
	if (! output.is_open()) { registerFile = false; }
	
	
	// Creation des fenetres
	if (useWindows) { createWindows(config); }

    // Création des barres de sélection	
    if (useWindows) {
		createTrackbar("Valeur de flou", "panel", &blur, 30);
		createTrackbar("Appliquer l'égalisation d'histogramme", "panel", &normalisation, 1);
	
		createTrackbar("hue base", "panel", &hueBase, 180);
		createTrackbar("saturation base", "panel", &saturationBase, 255);
		createTrackbar("hue tolerance", "panel", &hueTolerance, 180);
		createTrackbar("saturation tolerance", "panel", &saturationTolerance, 255);
	
		createTrackbar("Appliquer la détection 2", "panel", &useInRange2, 1);
		
		createTrackbar("hue base 2", "panel", &hueBase2, 180);
		createTrackbar("saturation base 2", "panel", &saturationBase2, 255);
		createTrackbar("hue tolerance 2", "panel", &hueTolerance2, 180);
		createTrackbar("saturation tolerance 2", "panel", &saturationTolerance2, 255);
	
		createTrackbar("Taille Erode", "panel", &erodeSize, 30);
		createTrackbar("Taille Dilate", "panel", &dilateSize, 30);
	
		createTrackbar("Afficher la détection", "panel", &tracer, 1);
	}
	
	// Boucle OpenCV
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
		    if (normalisation) {
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
		        Scalar(
		        	hueBase - hueTolerance, 
		        	saturationBase - saturationTolerance, 
		        	valueBase - valueTolerance), 
		        Scalar(
		        	hueBase + hueTolerance, 
		        	saturationBase + saturationTolerance, 
		        	valueBase + valueTolerance), 
                frameDetection
            );
            
            // Autre rouge (il y a la teinte 180 et la 0)
            if (useInRange2) {
                inRange(
			        frameHSV, 
		            Scalar(
		            	hueBase2 - hueTolerance2, 
		            	saturationBase2 - saturationTolerance2, 
		            	valueBase2 - valueTolerance2
	            	), 
		            Scalar(
		            	hueBase2 + hueTolerance2, 
		            	saturationBase2 + saturationTolerance2, 
		            	valueBase2 + valueTolerance2
	            	), 
                    frameDetectionInverseRed
                );
                addWeighted(
                	frameDetection, 
                	1, 
                	frameDetectionInverseRed, 
                	1, 
                	0, 
                	frameDetection, 
                	frameDetection.type()
            	);
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
            newDistance = findDistance(
            	rayon, 
            	videoHeight,
            	focaleLength,
            	objectHeight,
            	sensorHeight
            );
	        newAngle = findRotation(center.x, fieldView, videoWidth);
	        // Amelioration de comportement quand detection defaillante
            if (abs(distance - newDistance) > maximumStep) {
            	Rg = STOPPER;
            	Rd = STOPPER;
            }
            else {
            	distance = newDistance;
            	angle = newAngle;
	        	findVitesses(
	        		distance, 
	        		angle, 
	        		Rg, 
	        		Rd,
	        		tropPres,
	        		tropLoin,
	        		tropADroite,
	        		tropAGauche,
	        		vitesseRobot
        		);
           	}
                
            
            if (rayon <= 0) { rayon = 1; }
            
            // Affichage
            if (tracer) {
            
                // Fenêtre Base
                circle(frameCouleurs, center, 2, Scalar(0, 255, 0), -1); // centre
                circle(frameCouleurs, center, rayon, Scalar(0, 255, 0), 1); // périmètre
                
                // Fenêtre Detection
                cvtColor(frameDetection, frameDetection, CV_GRAY2BGR);
                circle(frameDetection, center, 2, Scalar(0, 0, 255), -1);
                circle(frameDetection, center, rayon, Scalar(0, 0, 255), 1); 
                
                // Afficher le texte
                distanceText << "distance : " << distance;
                angleText << "angle : " << angle;
                vitesseGText << "vitesse G : " << Rg;
                vitesseDText << "vitesse D : " << Rd;
                
                putText(frameDetection, distanceText.str(), lineDistanceText, fontFaceText, fontScaleText, colorText, thicknessText);
                putText(frameDetection, angleText.str(), lineAngleText, fontFaceText, fontScaleText, colorText, thicknessText);
                putText(frameDetection, vitesseGText.str(), lineVitesseGText, fontFaceText, fontScaleText, colorText, thicknessText);
                putText(frameDetection, vitesseDText.str(), lineVitesseDText, fontFaceText, fontScaleText, colorText, thicknessText);
                
                distanceText.str("");
                distanceText.clear();
                angleText.str("");
                angleText.clear();
                vitesseGText.str("");
                vitesseGText.clear();
                vitesseDText.str("");
                vitesseDText.clear();
            }
            
            // Fenêtre Trace
            circle(frameTrace, center, 2, Scalar(0, 0, 255), -1);
            circle(frameTrace, center, rayon, Scalar(0, rayon * 5, 255), 1); 
            
			imshow("base", frameCouleurs);
			imshow("detection", frameDetection);
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
            	if (registerFile) {
		            output 
		                << "Rg = " << Rg << " "
		                << "Rd = " << Rd << " "
		                << "A = " << angle << "°            "
		                << "D = " << distance << "cm" << endl;
                }
            }
			
		}
		
		// Rafraîchissement
		key = waitKey(refresh);
		switch (key) {
		    case 32 : // Pause, spacebar
		        pause = pause ? false : true;
	            break;
            case 10 : // Exit, enter
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

void createWindows (Config & config) {
	try {
		namedWindow("base", CV_WINDOW_NORMAL);
		namedWindow("detection", CV_WINDOW_NORMAL);
		namedWindow("panel", CV_WINDOW_NORMAL);
		namedWindow("trace", CV_WINDOW_NORMAL);
	
	
		moveWindow("base", config.lookup("windows.base.x"), config.lookup("windows.base.y"));
		moveWindow(
			"detection", 
			config.lookup("windows.detection.x"), 
			config.lookup("windows.detection.y")
		);
		moveWindow("panel", config.lookup("windows.panel.x"), config.lookup("windows.panel.y"));
		moveWindow("trace", config.lookup("windows.trace.x"), config.lookup("windows.trace.y"));
	
		resizeWindow(
			"base", 
			config.lookup("windows.base.width"), 
			config.lookup("windows.base.height")
		);
		resizeWindow(
			"detection",
			config.lookup("windows.detection.width"),
			config.lookup("windows.detection.height")
		);
		resizeWindow(
			"panel",
			config.lookup("windows.panel.width"),
			config.lookup("windows.panel.height")
	);
		resizeWindow(
			"trace",
			config.lookup("windows.trace.width"),
			config.lookup("windows.trace.height")
		);
    }
    catch (SettingException & e) {
    	cout << "Erreur configuration windows";
		cout << e.getPath();    
    }
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


float findDistance(int rayon, int imageH, float focaleLength, float objectHeight, float sensorHeight) {
    if (rayon > 0) {
        return (focaleLength * objectHeight * imageH) / (rayon * sensorHeight) / 10;
    }
    else { return 0; }
}

void findVitesses(float distanceObjet, float angleObjet, float & gauche, float & droite,
	float tropPres, float tropLoin, float tropAGauche, float tropADroite, float vitesse){
    
    float diff = abs(angleObjet) / 90;
    
    // L'objet est trop loin
    if (distanceObjet >= tropLoin and distanceObjet < 1000) {
        gauche = AVANCER;
        droite = AVANCER;
        if (angleObjet > 0) { droite -= diff; }
        else if (angleObjet < 0) { gauche -= diff; }
    }
    // L'objet est trop près
    else if (distanceObjet <= tropPres and distanceObjet > 0) {
        gauche = RECULER;
        droite = RECULER;
        if (angleObjet > 0) { gauche += diff; }
        else if (angleObjet < 0) { droite += diff; }
    }
    else { // L'objet est à distance adéquate
        if (angleObjet >= tropADroite) { 
            gauche = diff; 
            droite = -diff; 
        }
        else if (angleObjet <= tropAGauche) { 
            gauche = -diff; 
            droite = diff; 
        }
        else { 
            gauche = STOPPER; 
            droite = STOPPER; 
        }
    }
    
    gauche *= vitesse;
    droite *= vitesse;
}

