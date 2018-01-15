#include "stdafx.h"
#include "FindBox.h"

/*
trait : contour d'une box
ligne  : correspond à une ligne du document qui contient une imagette pour connaitre le label de la ligne et 5 box
box : carré qui correspond à l'emplacement ou on doit dessiner
*/

/*
Sauvegarde les traits trouvés
deux coordonées : un point de départ et un point d'arrivé.
*/
void FindBox::saveBox(int i1, int j1, int i2, int j2) {
	auto pair = std::make_pair(std::make_pair(i1, j1), std::make_pair(i2, j2));
	findItsBrother(pair);
	resultsBox.push_back(pair);
}

/*
fonction qui permet d'incrémenter le compteur au niveau d'un pixel noir
*/
void FindBox::brushPointFound(int& j, vector<int>& compteur,  vector<int>& rotateOrigine) {
	int found = -1;//permet de savoir si il y a un autre compteur actif dans le champs
	int limitBrush = 4;//rayon de l'observation
	if (j + limitBrush < compteur.size() && j - limitBrush > -1) {//si on peut regarder autour
		for (int indBrush = 0; indBrush < (limitBrush*2+1); indBrush++) {//on regarde si autour il existe un compteur actif
			if (compteur[j - limitBrush + indBrush] > 0 && ((j - limitBrush + indBrush) != j)){//si un compteur actif
				found = (j - limitBrush + indBrush);//on enregistre le compteur
				break;
			}
		}
		if (found==-1) {//on demarre le compteur que si il n'y a rien autour
			compteur[j]++;
		}
		if(found!=-1 && compteur[j]>0) {//conflit entre brush (compteurs actifs) : eviter de gerer trop de brush
			compteurHandleMulti(j, found, compteur, rotateOrigine);
		}
	}
}

/*
quand un pixel noir n'est pas trouvé
*/
void FindBox::brushPointNotFound(int&  i,int& j, vector<int>& compteur, vector<int>& rotateOrigine) {
	int pointfound = false;//permet de savoir si autour de j il existe d'autre bon point
	int pixelTresh = 240;//longeur d'un trait
	int limitBrush = 4;//champs d'obervation
	unsigned int threshold = 1;
	//si le compteur etait actif ( compteur[j] > 0 ) alors on regarde si on peut le deplacer ou pas
	if (j + limitBrush < compteur.size() && j - limitBrush > -1 && compteur[j] > 0) {
		int newPointBrush = -1;//nouvelle coordonnée de la brush
		int sum = 0, nbPoint = 0;//pour calculer la nouvelle coordonnée de la brush
		for (int indBrush = 0; indBrush < limitBrush * 2 + 1; indBrush++) {//on peint sur des cases autour du point j pour verifier si il est possible de continuer le trait
			if (((unsigned int)image.at<unsigned char>(i, j - limitBrush + indBrush)) < threshold) {//un posible pixel disponible trouvé
				pointfound = true;
				nbPoint++;
				sum += (j - limitBrush + indBrush);
			}
			if (compteur[j - limitBrush + indBrush] > 0 && (j - limitBrush + indBrush) != j) {//si un autre compteur existe dans la zone on fusionne les deux
				int newI = (j - limitBrush + indBrush);
				compteurHandleMulti(newI,j, compteur, rotateOrigine);
				return;
			}
		}
		if (pointfound) {//pour une rotation on reequilibre le brush
			newPointBrush = sum / nbPoint;
		}
		if (!pointfound) {//si aucune nouvelle coordonnée possible 
			//plus personne ne peut peindre sur cette ligne donc on la supprime
			if (compteur[j] >pixelTresh) {//avant on verifie si c'est une ligne
				int temp = j;
				if (rotateOrigine[j] != 0)temp = rotateOrigine[j];
				saveBox(i - compteur[j], temp, i - 1, j);
			}
			compteur[j] = 0;
			rotateOrigine[j] = 0;
		}
		else {//decale vers un pixel qui peut etre une ligne
			compteur[newPointBrush] = compteur[j];
			if (rotateOrigine[j] == 0)rotateOrigine[newPointBrush] = j;//on sauvegarde le point d'origine pour prendre en consideration une rotation
			else rotateOrigine[newPointBrush] = rotateOrigine[j];
			rotateOrigine[j] = 0;
			compteur[j] = 0;
		}
	}
}

/*
Fusionne deux traits qui se trouve dans une même zone
*/
void FindBox::compteurHandleMulti(int& indexFirst, int& indexSecond, vector<int>& compteur, vector<int>& rotateOrigine) {
	if (compteur[indexFirst] > compteur[indexSecond]) {//si l'autre est plus grand on le sauvegarde
		compteur[indexSecond] = 0;
		rotateOrigine[indexSecond] = 0;
		rotateOrigine[indexFirst] = (rotateOrigine[indexFirst] == 0) ? indexSecond : rotateOrigine[indexFirst];
	}
	else {//sinon on sauvegarde l'actuel
		compteur[indexFirst] = 0;
		rotateOrigine[indexFirst] = 0;
	}
}

/*
Detecte l'ensemble des traits présents sur le document il faut que l'algo trouve les coordonnées d'un point et 
d'un second pour sauvegarder une trait
*/
void FindBox::boxDetector(){
	vector<int> compteur(image.cols);
	vector<int> rotateOrigine(image.cols);
	for (int i : compteur) {
		i = 0;
	}
	for (int i : rotateOrigine) {
		i = 0;
	}
	unsigned int threshold = 1;
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			if (((unsigned int)image.at<unsigned char>(i, j)) < threshold)
			{
				brushPointFound(j, compteur, rotateOrigine);
			}
			else {
				brushPointNotFound(i,j, compteur, rotateOrigine);
			}
		}
	}
	sortBox();
	setCoordLabel();
}

/*
Affiche l'ensemble des traits que le detecteur a trouvé
*/
void FindBox::diplayResult() {
	if (resultsBox.size() <2)std::cout << "vide" << std::endl;
	else {
		for (pair<pair<int, int>, pair<int, int>> p : resultsBox) {
			displayLineOnPicture(p);
		}
	}
}

/*
Affiche l'ensemble des traits en couple qui appartiennent à une même ligne de document
*/
void FindBox::diplayResultLine(int line) {
	vector<pair<int, int>> vectorLine = resultsFinalBox.at(line);
	for (pair<int, int> couple : vectorLine) {
		displayLineOnPicture(resultsBox.at(couple.first));
		if ((couple.second > -1)) {
			displayLineOnPicture(resultsBox.at(couple.first));
			displayLineOnPicture(resultsBox.at(couple.second));
		}
	}
}

/*
Affiche trait sur le document
*/
void FindBox::displayLineOnPicture(pair<pair<int, int>, pair<int, int>> p) {
	int x1 = p.first.second;
	int y1 = p.first.first;
	int x2 = p.second.second;
	int y2 = p.second.first;
	Point p1(x1, y1);
	Point p2(x2, y2);
	line(resultTestImage, p1, p2, Scalar(0, 0, 250), 20);
}

/*
Permet de trouver le frere d'un nouveau trait
*/
void FindBox::findItsBrother(pair<pair<int, int>, pair<int, int>> newLine) {
	bool found = false;
	int indiceLigne = findItsLine(newLine);//trouve sur quelle ligne du document il appartient
	std::cout << "nouvelle ligne : indice ligne " << indiceLigne << std::endl;
	if (indiceLigne > -1) {
		if (resultsFinalBox.at(indiceLigne).size() > 0) {
			for(int ind = 0;ind <resultsFinalBox.at(indiceLigne).size();ind++){//on parcourt les pairs
				pair<int, int> pointLine = resultsBox.at(resultsFinalBox.at(indiceLigne).at(ind).first).first;
				if (newLine.first.second < pointLine.second + 30//meme y
					&& newLine.first.second>pointLine.second - 30//meme y
					&& newLine.first.first< pointLine.first + 100//meme x
					&& newLine.first.first> pointLine.first - 100) {//meme trait trouvé donc pas besoin de le sauvegarder
					found = true;
					break;
				}
				if (abs(newLine.first.second - pointLine.second)< 300 
					&& abs(newLine.first.second - pointLine.second)> 225) {//la distance qui sépare deux traits pour former une box
					resultsFinalBox.at(indiceLigne).at(ind).second = resultsBox.size();
					found = true;
					break;
				}
			}
		}

		if (!found) {//si pas de frere detecté alors on crée un couple qui devra trouver un frere
			auto newPair = std::make_pair(resultsBox.size(), -1);
			resultsFinalBox.at(indiceLigne).push_back(newPair);
		}
	}
}

/*
Permet de trouver sur quelle ligne de document (0 à 6) le nouveau trait appartient
*/
int FindBox::findItsLine(pair<pair<int, int>, pair<int, int>> newLine) {
	int insertLine = -1, index = 0;;
	if (resultsFinalBox.size() != 0) {
		while (index < resultsFinalBox.size()) {
			int indexLine = resultsFinalBox.at(index).at(0).first;//recupere le premiere trait
			if (newLine.first.first< resultsBox.at(indexLine).first.first + 200 && newLine.first.first> resultsBox.at(indexLine).first.first - 200) {
				return index;//le x du nouveau trait est au même niveau que celui de la ligne du document. 
			}
			else if(newLine.first.first< resultsBox.at(indexLine).first.first ){
				insertLine = index;
			}
			index++;
		}
		if (insertLine == -1)insertLine = resultsFinalBox.size();
		std::vector<pair<int, int>> newVectorMatch;
		resultsFinalBox.insert(resultsFinalBox.begin() + index, newVectorMatch);
		return index;
	}
	else {
		std::vector<pair<int, int>> newVectorMatch;
		resultsFinalBox.insert(resultsFinalBox.begin() + index, newVectorMatch);
		return index;
	}

}

/*
Permet de trier les couples en fonction de leurs emplacements (gauche à droite)
Terminé : trie par ligne l'ensemble des couples 
*/
void FindBox::sortBox() {
	for (int ind = 0; ind < resultsFinalBox.size(); ind++) {
		sort(resultsFinalBox.at(ind).begin(), resultsFinalBox.at(ind).end(),
			[this](const pair<int, int> & a, const pair<int, int> & b) -> bool
		{
			return this->resultsBox.at(a.first).first.second <  this->resultsBox.at(b.first).first.second && a.second !=-1;
		});
	}
}

/*
¨Permet de recuperer les traits qui entourent les imagettes pour trouver le label de la ligne de document
*/
void FindBox::setCoordLabel() {
	for (int ind = 0; ind < resultsFinalBox.size(); ind++) {
		if (resultsFinalBox.at(ind).at(0).first > -1
			&& resultsFinalBox.at(ind).at(0).second > -1) {
			std::vector<pair<pair<int, int>, pair<int, int>>> labelBoxCoord;
			pair<pair<int, int>, pair<int, int>> pattern1 = resultsBox.at(resultsFinalBox.at(ind).at(0).first);
			pair<pair<int, int>, pair<int, int>> pattern2 = resultsBox.at(resultsFinalBox.at(ind).at(0).second);
			if (pattern1.first.second > pattern2.first.second) {
				pair<pair<int, int>, pair<int, int>> temp = pattern2;
				pattern2 = pattern1;
				pattern1 = temp;
			}
			float coefAHaut = (float)(pattern2.first.first - pattern1.first.first) / (float)(pattern2.first.second - pattern1.first.second);
			float coefBHaut = pattern2.first.first - coefAHaut * pattern2.first.second;
			int newIPattern1Haut = coefAHaut * (float)(pattern1.first.second - 400) + coefBHaut;
			int newIPattern2Haut = coefAHaut * (float)(pattern2.first.second - 350) + coefBHaut;

			float coefABas = (float)(pattern2.second.first - pattern1.second.first) / (float)(pattern2.second.second - pattern1.second.second);
			float coefBBas = pattern2.second.first - coefAHaut * pattern2.second.second;
			int newIPattern1Bas = coefABas * (float)(pattern1.second.second - 400) + coefBBas;
			int newIPattern2Bas = coefABas * (float)(pattern2.second.second - 350) + coefBBas;

			labelBoxCoord.push_back(std::make_pair(std::make_pair(newIPattern1Haut, pattern1.first.second - 400), std::make_pair(newIPattern1Bas, pattern1.second.second -400)));
			labelBoxCoord.push_back(std::make_pair(std::make_pair(newIPattern2Haut, pattern2.first.second -350), std::make_pair(newIPattern2Bas, pattern2.second.second -350)));
			labelBox.push_back(labelBoxCoord);
		}

	}
}

/*
affiche sur l'image d'origine les traits qui designent les imagettes
*/
void FindBox::displayAffichette() {
	for (int ind = 0; ind < labelBox.size(); ind++) {
		vector<pair<pair<int, int>, pair<int, int>>> affichette = labelBox.at(ind);
		displayLineOnPicture(affichette.at(0));
		displayLineOnPicture(affichette.at(1));
	}
}

/*
sauvegarde l'image
à faire apres un appel à un display
*/
void FindBox::saveLocalImage() {
	ostringstream name;
	name<< "image" << numImage << ".png";
	imwrite(name.str(), resultTestImage);
	ostringstream nametest;
	nametest << "image" << numImage << numImage  << ".png";
	imwrite(nametest.str(), this->image);
}

/*
permet de recuperer une sous partie de l'image d'origine à partir d'un couple de trait trouvé
*/
Mat FindBox::getSubImage(pair<pair<int, int>, pair<int, int>> p1, pair<pair<int, int>, pair<int, int>> p2, bool pattern) {
	if (p1.first.second > p2.first.second) {
		pair<pair<int, int>, pair<int, int>> temp = p2;
		p2 = p1;
		p1 = temp;
	}
	int correctionI = 0;
	double angeDeg = 0;

	if ( abs(p1.second.first - p2.second.first)>5) {
		pair<int, int> vecteur1, vecteur2;
		vecteur1 = make_pair(p1.first.second - p1.first.second, p2.first.first - p1.first.first);
		vecteur2 = make_pair(p2.first.second - p1.first.second, p2.first.first - p1.first.first);
		double  angleCos = ((vecteur1.first*vecteur2.first) + (vecteur1.second*vecteur2.second)) / (sqrt(pow(vecteur1.first, 2) + pow(vecteur1.second, 2))*sqrt(pow(vecteur2.first, 2) + pow(vecteur2.second, 2)));
		angeDeg = (isnan(angleCos)) ? 0 : angleCos * (180 / M_PI);
		correctionI = abs((p2.first.first - p1.first.first));
		if (p1.first.first>p2.first.first) {
			angeDeg *= -1;
			correctionI *= -1;
		}
		correctionI = correctionI / 2;
	}

	RotatedRect rect = RotatedRect(Point2f(p1.first.second - correctionI + 250 / 2, p1.first.first + correctionI +  250 / 2), Size2f(245, 245), angeDeg);
	Mat M, rotated, cropped;
	float angle = rect.angle;
	Size rect_size = rect.size;
	if (rect.angle < -45.) {
		angle += 90.0;
		swap(rect_size.width, rect_size.height);
	}
	M = getRotationMatrix2D(rect.center, angle, 1.0);
	Mat orig = originalImage;
	warpAffine(orig, rotated, M, orig.size(), INTER_CUBIC);//rotation du document en adequation avec l'angle de la box
	getRectSubPix(rotated, rect_size, rect.center, cropped);//recupere la matrice qui contient la zone decoupée
	

	return cropped;
}

/*
Construit tout les documents
*/
void FindBox::buildAllLabelLine( MatchingPattern matchP, MatchingSize matchS) {


	for (int ind = 0; ind < labelBox.size(); ind++) {
		buildLabelLine(ind, matchP, matchS);
	}
}


/*
Permet pour une ligne de construire l'ensemble des informations
*/
void FindBox::buildLabelLine(int line, MatchingPattern matchP, MatchingSize matchS) {
	//recuperation des elements du path
	vector<string> nameFileVector{ Utils::explode(imagePath, '/') };
	//recuperation du nom du fichier
	String nameFile = nameFileVector.back();
	//recuperation des lignes qui definissent l'imagette
	vector<pair<pair<int, int>, pair<int, int>>> affichette = labelBox.at(line);
	//recuperation de l'affichete
	Mat aff = getSubImage(affichette.at(0), affichette.at(1));
	//convertion en type GRAYSCALE pour le traitement
	Mat res;
	aff.convertTo(res , CV_LOAD_IMAGE_GRAYSCALE);
	//recuperation du label
	String label = matchP.matchWithMath(res);
	std::cout << "label find " << label  << std::endl;
	//construction des fichiers
	vector<pair<int, int>> lineElement = resultsFinalBox.at(line);
	ofstream myfile;

	String sizeLabel = matchS.matchWithMath(res);
	for (int ind = 0; ind < lineElement.size(); ind++) {
		if (lineElement.at(ind).second>-1) {//si =-1 alors la ligne n'a pas de frere
			String finalName = label + "_" + nameFile.substr(0, 3) + "_"+ nameFile.substr(3, 2)+"_"+ to_string(line+1)+"_"+ to_string(ind+1);
			ostringstream  contentTxt;
			contentTxt<<"#Comment " << "\n" << "label " << label.c_str() << "\n" << "form " <<nameFile.substr(0, 5) << "\n" << "scripter " << nameFile.substr(0, 3) + "\n" << "page " << nameFile.substr(3, 2) << "\n" << "row "<< to_string(line+1) << "\n" << "column " << to_string(ind+1);
			if (sizeLabel != "")contentTxt << "\n" << "size "<< sizeLabel;
			myfile.open(Utils::GLOBAL_PATH + "/results/"+ finalName +".txt");
			myfile << contentTxt.str();
			myfile.close();
			Mat imageSUB = getSubImage(resultsBox.at(lineElement.at(ind).first), resultsBox.at(lineElement.at(ind).second));
			imwrite(Utils::GLOBAL_PATH + "/results/" + finalName + ".png", imageSUB);

		}
	}
}


