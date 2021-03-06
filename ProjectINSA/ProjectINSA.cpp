#include "stdafx.h"
#include "opencvInclude.h"
#include "MatchingPattern.h"
#include "FindBox.h"
#include "MatchingSize.h"

using namespace std;

/*
Test de la reconnaissance de pattern
*/
void testPattern() {
	//chargement du matcher des pattern
	MatchingPattern matchP;
	//initialisation du matcher
	matchP.buildKeyPoint();

	MatchingSize macthS;
	//initialisation du matcher
	macthS.buildKeyPoint();

	vector<string> files;
	//base de test pour les pattern
	string pathpattern = Utils::GLOBAL_PATH + "/testpattern";
	Utils::read_directory(pathpattern, files);
	for (string &file : files)
	{
		string pattern = file;
		if (pattern != "." && pattern != "..") {
			vector<string> v{ Utils::explode(pattern, '.') };
			string pat = pathpattern + "/" + pattern.c_str();


			cout << "pattern " << file << endl;
			Mat temp = cvLoadImage(pat.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
			cout << "resultat " << matchP.matchWithMath(temp) << " "<< macthS.matchWithMath(temp) << endl;
			cout << "---------------" << endl;
		}
	}
	
	
}

/*
Main du projet
*/
int main()
{

	//testPattern();

	
	//chargement du matcher des pattern
	MatchingPattern matchP;
	//initialisation du matcher
	matchP.buildKeyPoint();

	//chargement du matcher des tailles
	MatchingSize macthS;
	//initialisation du matcher
	macthS.buildKeyPoint();

	vector<string> files;
	//images pour créeer les imagettes
	string pathpattern = Utils::GLOBAL_PATH + "/donnees";
	Utils::read_directory(pathpattern, files);
	int ind = 0;
	for (string &file : files)
	{
		string pattern = file;
		if (pattern != "." && pattern != "..") {
		
			string imName = pathpattern + "/" + pattern.c_str();
			//création d'un objet finBox lié au fichier
			FindBox f(imName, ind);
			//lancer la detection des box
			f.boxDetector();


			//récuperation des resultats de la detection

			//affichage des lignes trouvées
			f.diplayResult();
			//affichage des lignes qui entourent les affichettes
			f.displayAffichette();
			//enregistrer ces affichages dans un fichier
			f.saveLocalImage();

			//construire l'ensemble des fichiers
			f.buildAllLabelLine(matchP, macthS);


			ind++;
		}
	}


	

	//fin du programme
	std::cout << "fin " << endl;

	//termine le programme lorsqu'une touche est frappee
	waitKey(0);
	//system("pause");

	


	while (true);
    return 0;
}



