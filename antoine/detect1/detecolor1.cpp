#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>

#define SOURCE_FILE "source"
#define OUTPUT_FILE "output"
#define FILTRE_FILE "filtre"

#define MAX_ERRORS 5
#define MIN_PIX 0
#define MAX_STEP 25

// C920 Logitech
#define FIELD_VIEW 78
#define FOCALE_LEN 3.67
#define SENSOR_DIAG 6.0
#define SENSOR_W 4.8
#define SENSOR_H 3.6

#define OBJECT_HEIGHT 4.4

#define DIST_SECURE_MIN 10
#define DIST_SECURE_MAX 20

using namespace cv;
using namespace std;


int pairiser (int nombre) {
    return (nombre % 2) ? nombre -1 : nombre;
}

float findRotation(int center, float angleView, int widthImage) {
    return ((center * angleView) / widthImage) - (angleView / 2); 
}


float findDistance(int rayon, int imageH) {
    return (FOCALE_LEN * OBJECT_HEIGHT * imageH) / (rayon * SENSOR_H) / 10;
}

void findVitesse(float distance, float angle, float & Rg, float & Rd){
    
    float diff = abs(angle) / 90;
    
    if (distance < DIST_SECURE_MIN and distance > 0) {
        if (angle == 0) {
            Rg = -1;
            Rd = -1;
        }
        if (angle > 0) {
            Rg = 0 - diff;
            Rd = -1;
        }
        if (angle < 0) {
            Rg = -1;
            Rd = 0 - diff;
        }
    }
    else if (distance > DIST_SECURE_MAX and distance < 1000) {
        if (angle == 0) {
            Rg = 1;
            Rd = 1;
        }
        if (angle > 0) {
            Rg = 1 - diff;
            Rd = 1;
        }
        if (angle < 0) {
            Rg = 1; Rd = 1 - diff; }
    }
    
    else {
        if (angle > 0) { Rg = 1; Rd = -1; }
        else if (angle < 0) { Rg = -1; Rd = 1; }
        else { Rg = 0; Rd = 0; }
    }
    
    
}

int explore(Point center, int stepX, int stepY, Mat image) {
    int x = center.x, 
        y = center.y, 
        r = 0, e = 0;
    while (e < 10 and x < image.cols and x > 0 and y < image.rows and y > 0) {
        x += stepX; y += stepY;
        Scalar test = image.at<uchar>(y, x);
        if (test.val[0]) { r ++; e = 0; }
        else { e ++; }
    }
    return r;
}

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
	namedWindow("base", CV_WINDOW_NORMAL);
	namedWindow("transformed", CV_WINDOW_NORMAL);
	namedWindow("panel", CV_WINDOW_NORMAL);
	
	resizeWindow("base", 500, 375);
	resizeWindow("panel", 400, 300);
	resizeWindow("transformed", 500, 375);
	
	moveWindow("base", 0, 0);
	moveWindow("transformed", 500, 550);
	moveWindow("panel", 0, 400);
	
	// Déclaration des variables utilisées
	int hTol = 54,
	    sTol = 57,
	    dSize = 2,
	    eSize = 7,
	    x, 
	    y, 
	    sumX = 0, 
	    sumY = 0, 
	    nbPix = 0,
	    norma = 0,
	    compteurErreurs = 0,
	    rayon = 0,
        key = 0;
	
	bool continuer = true,
	    neverEnd = false,
	    pause = false;
	   
	Mat frameO,
	    frame, 
	    frame2, 
        element;
	Point bary, 
	    center(0, 0);
	vector<Mat> channels;
	ofstream output;
	output.open(OUTPUT_FILE);

    // Création des barres de sélection	
    createTrackbar("hTolerance", "panel", &hTol, 100);
	createTrackbar("sTolerance", "panel", &sTol, 100);
	createTrackbar("dilateSize", "panel", &dSize, 30);
	createTrackbar("erodeSize", "panel", &eSize, 30);
	createTrackbar("normalize", "panel", &norma, 1);
	
	while (continuer) {
	
	    // Récupération d'une image
	    if (! pause) {
		    video >> frameO;
		}
		frame = frameO.clone();
		
		if (frame.empty()) {
		
		    // Gestion des erreurs si l'image est vide
			cout << "Problème frame\n";
			compteurErreurs ++;
			if (compteurErreurs > MAX_ERRORS) {
			    if (neverEnd) {
			        video.open(source);
			    }
			    else {
			        cout << "Arrêt : 6 frames erronées consécutives\n";
			        continuer = false;
			    }
			}
		}
		else {
		    compteurErreurs = 0;
		    
		    // Normalize (fonction de xiong & tong)
		    if (norma) {
	            cvtColor(frame, frame, CV_RGB2YCrCb);
	            split(frame, channels);
	            equalizeHist(channels[0], channels[0]);
	            merge(channels, frame);
	            cvtColor(frame, frame, CV_YCrCb2RGB);
		    }
		    
		    // BGR 2 HSV
			cvtColor(frame, frame2, CV_BGR2HSV);
			
			// Couleur ciblée en blanc et le reste en noir
			inRange(
			    frame2, 
                Scalar(175-hTol, 226-sTol, 0), 
                Scalar(175+hTol, 226+sTol, 255), 
                frame2
            );
            
            // Erode
            if (eSize > 0) {
                element = getStructuringElement(MORPH_ELLIPSE, Size(eSize, eSize));    
                erode(frame2, frame2, element);
            }
            
            // Dilate
            if (dSize > 0) {
                element = getStructuringElement(MORPH_ELLIPSE, Size(dSize, dSize));    
                dilate(frame2, frame2, element);
            }
           
            // Barycentre
            sumX = 0; sumY = 0; nbPix = 0;
            for (x = 0; x < pairiser(frame2.rows); x += 2) {
                for (y = 0; y < pairiser(frame2.cols); y += 2) {
                    Scalar intensity = frame2.at<uchar>(x, y);
                    if (intensity.val[0]) {
                        sumX += x; sumY += y; nbPix ++;
                    }
                }
            }
            
            // L'objet est trouvé
            if (nbPix > MIN_PIX) {
                bary = Point((int) (sumY / nbPix), (int) (sumX / nbPix));
                
                /*int diffX = min(abs(bary.x - center.x), MAX_STEP);
                if (bary.x < center.x) { diffX = 0 - diffX; }
                
                int diffY = min(abs(bary.y - center.y), MAX_STEP);
                if (bary.y < center.y) { diffY = 0 - diffY; }
                
                center = Point(center.x + diffX, center.y + diffY);*/
                
                center = Point(bary.x, bary.y);
                
                if (center.x > -1 and center.y > -1) {
                    circle(frame, center, 2, Scalar(0, 255, 0), -1);
                    
                    // Exploration dans plusieurs directions
                    int right = explore(center, 1, 0, frame2),
                        left = explore(center, -1, 0, frame2),
                        top = explore(center, 0, -1, frame2),
                        bot = explore(center, 0, 1, frame2),
                        topright = explore(center, 1, -1, frame2),
                        topleft = explore(center, -1, -1, frame2),
                        botright = explore(center, 1, 1, frame2),
                        botleft = explore(center, -1, 1, frame2);
                    
                    rayon = max(
                        max(
                            max(right, left), 
                            max(top, bot)
                        ),
                        max(
                            max(topright, topleft),
                            max(botright, botleft) 
                        )
                    );
                    
                    circle(frame, center, rayon, Scalar(0, 255, 0), 1);
                }
                
                float angle, distance, Rg, Rd;
                angle = findRotation(center.x, FIELD_VIEW, frame2.size().width);
                distance = findDistance(rayon, frame2.size().height);
                findVitesse(distance, angle, Rg, Rd);
                
                // Ecrire la rotation et la vitesse
                output 
                    << "A = " << angle << "°    "
                    << "D = " << distance << "cm    "
                    << "Rg = " << Rg << "   "
                    << "Rd = " << Rd << "\n";
            }
            
            // Affichage
			imshow("base", frame);
			imshow("transformed", frame2);
		}
		
		// Rafraîchissement
		key = waitKey(30);
		switch (key) {
		    case 32 :
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
	frameO.release();
	frame.release();
	frame2.release();
	output.close();
	
	cout << "Fin\n";
    
	return 0;
}

