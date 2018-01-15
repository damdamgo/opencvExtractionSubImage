#pragma once
#include <iostream>
#include <stdio.h>
#include <vector>
#include <windows.h>
#include "opencvInclude.h"
#include "MatchingPattern.h"
#include "MatchingSize.h"
#include "Utils.h"
#include <fstream>
#include <math.h>  

#define M_PI 3.141592653589793238462643383279502884L /* pi */

using namespace std;

class FindBox {
	String imagePath;
	Mat image;
	Mat originalImage;
	int numImage;
	Mat resultTestImage;
	std::vector<pair<pair<int, int>, pair<int, int>>> resultsBox;//permet de sauvegarder les lignes trouvées sur le document
	std::vector<std::vector<pair<int, int>>> resultsFinalBox;//sauvegarde de pair de d'index qui font reference sur des index de la resultsBox. Cela forme des pairs qui entourent les images.
	std::vector<std::vector<pair<pair<int, int>, pair<int, int>>>>labelBox;//sauvegarde des listes qui contienent des lignes qui entourent affichette pour en deduire le label de la ligne de document.
public :
	FindBox(string imPath,int numImage) {
		this->numImage = numImage;
		this->imagePath = imPath;
		Mat tmp = cvLoadImage(imPath.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
		Mat tmp2;
		threshold(tmp, tmp2, 200, 255, THRESH_BINARY);
		int erosion_size = 2;
		Mat element = getStructuringElement(MORPH_RECT,
			Size(2 * erosion_size + 1, 2 * erosion_size + 2),
			Point(erosion_size, erosion_size));
		erode(tmp2, this->image, element);
		/// Apply the dilation operation


		this->originalImage = cvLoadImage(imPath.c_str());
		this->resultTestImage = cvLoadImage(imPath.c_str());
		//std::cout << "type " << image.type() << endl;
		//std::cout << "ee" << (unsigned int)image.at<unsigned char>(2, 2) << "ee" << (unsigned int)image.at<unsigned char>(2, 2) << std::endl;
	}
	void saveBox(int i1, int j1, int i2, int j2);
	void boxDetector();
	void diplayResult();
	void diplayResultLine(int line);
	void displayAffichette();
	void saveLocalImage();
	void buildLabelLine(int line, MatchingPattern matchP, MatchingSize matchS);
	void buildAllLabelLine(MatchingPattern matchP,MatchingSize matchS);
private :
	void findItsBrother(pair<pair<int, int>, pair<int, int>> newLine);
	int findItsLine(pair<pair<int, int>, pair<int, int>> newLine);
	void displayLineOnPicture(pair<pair<int, int>, pair<int, int>> p);
	void sortBox();
	void setCoordLabel();
	Mat getSubImage(pair<pair<int, int>, pair<int, int>> p1, pair<pair<int, int>, pair<int, int>> p2,bool pattern = false);
	void brushPointFound(int& j, vector<int>& compteur, vector<int>& rotateOrigine);
	void brushPointNotFound(int&  i,int& j, vector<int>& compteur, vector<int>& rotateOrigine);
	void compteurHandleMulti(int& indexFirst, int& indexSecond, vector<int>& compteur, vector<int>& rotateOrigine);
};