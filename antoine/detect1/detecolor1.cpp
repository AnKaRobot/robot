#include <iostream>
#include <opencv2/opencv.hpp>

#define MAX_ERRORS 5
#define MIN_PIX 100
#define MAX_STEP 25

using namespace cv;
using namespace std;


int pairiser (int nombre) {
    return (nombre % 2) ? nombre -1 : nombre;
}


int main (int argc, char **argv) {
    
	cout << "Début\n";
    
    // Capture de la source
    //string source("/home/robuntu/Videos/vid1.mp4");
	int source = 0;
    VideoCapture video(source);
    
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
	    sTol = 92,
	    dSize = 5,
	    eSize = 5,
	    x, 
	    y, 
	    sumX = 0, 
	    sumY = 0, 
	    nbPix = 0,
	    norma = 0,
	    compteurErreurs = 0;
	
	bool continuer = true,
	    neverEnd = true;
	   
	Mat frame, 
	    frame2, 
        element;
	Point bary, 
	    center(0, 0);
	vector<Mat> channels;

    // Création des barres de sélection	
    createTrackbar("hTolerance", "panel", &hTol, 100);
	createTrackbar("sTolerance", "panel", &sTol, 100);
	createTrackbar("dilateSize", "panel", &dSize, 30);
	createTrackbar("erodeSize", "panel", &eSize, 30);
	createTrackbar("normalize", "panel", &norma, 1);
	
	while (continuer) {
	
	    // Récupération d'une image
		video >> frame;
		
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
	            cvtColor(frame, frame, CV_BGR2YCrCb);
	            split(frame, channels);
	            equalizeHist(channels[0], channels[0]);
	            merge(channels, frame);
	            cvtColor(frame, frame, CV_YCrCb2BGR);
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
            if (nbPix > MIN_PIX) {
                bary = Point((int) (sumY / nbPix), (int) (sumX / nbPix));
                
                /*int diffX = min(abs(bary.x - center.x), MAX_STEP);
                if (bary.x < center.x) { diffX = 0 - diffX; }
                
                int diffY = min(abs(bary.y - center.y), MAX_STEP);
                if (bary.y < center.y) { diffY = 0 - diffY; }
                
                center = Point(center.x + diffX, center.y + diffY);*/
                
                center = Point(bary.x, bary.y);
                
                if (center.x > -1 and center.y > -1) {
                    circle(frame, center, 5, Scalar(0, 255, 0), -1);
                }
            }
            
            // Affichage
			imshow("base", frame);
			imshow("transformed", frame2);
		}
		
		// Rafraîchissement
		if (waitKey(30) != -1) { continuer = false; }
	}

    // Libération de mémoire
	destroyAllWindows();
	element.release();
	video.release();
	frame.release();
	frame2.release();
	
	cout << "Fin\n";
    
	return 0;
}

