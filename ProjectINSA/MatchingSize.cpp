#include "stdafx.h"
#include "MatchingSize.h"

/*
Construit les descriptor pour les tailles
*/
void MatchingSize::buildKeyPoint() {
	vector<string> files;
	string pathpattern = Utils::GLOBAL_PATH + "/size";
	Utils::read_directory(pathpattern, files);
	int minHessian = 1200;
	Mat patternMat;
	SurfFeatureDetector detector(minHessian);
	SurfDescriptorExtractor extractor;
	Mat descriptor;
	for (string &file : files)
	{
		string pattern = file;
		if (pattern != "." && pattern != "..") {
			vector<string> v{ Utils::explode(pattern, '.') };
			std::cout << pathpattern + "/" + pattern.c_str() << endl;
			string pat = pathpattern + "/" + pattern.c_str();
			patternMat = cvLoadImage(pat.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
			std::vector<KeyPoint> keypoint;
			detector.detect(patternMat, keypoint);
			extractor.compute(patternMat, keypoint, descriptor);
			pattern = v[0];
			descriptorPattern[pattern] = std::make_pair(descriptor, std::make_pair(patternMat, keypoint));
		}
	}
}

/*
Retourne la taille reconnu au sein de l'image
*/
String MatchingSize::matchWithMath(Mat match) {
	int minHessian = 1200;
	SurfFeatureDetector detector(minHessian);
	std::vector<KeyPoint> keypoints;
	detector.detect(match, keypoints);
	SurfDescriptorExtractor extractor;
	Mat descriptors;
	extractor.compute(match, keypoints, descriptors);

	double minF = 100;
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	String nameLabel;
	int nbGoodMatch, nbGoodMatchF=0;
	for (pair<string, pair<Mat, pair<Mat, std::vector<KeyPoint>>>> const &mapP : descriptorPattern) {
		nbGoodMatch = 0;
		if (mapP.second.first.empty())
			cvError(0, "MatchFinder", "1st descriptor empty", __FILE__, __LINE__);
		if (descriptors.empty())
			cvError(0, "MatchFinder", "2nd descriptor empty", __FILE__, __LINE__);
		matcher.match(mapP.second.first, descriptors, matches);

		double min_dist = 100;

		for (int i = 0; i < mapP.second.first.rows; i++)
		{
			double dist = matches[i].distance;
			if (dist < min_dist) min_dist = dist;
		}

		for (int i = 0; i < mapP.second.first.rows; i++)
		{
			if (matches[i].distance <= 0.1)
			{
				nbGoodMatch++;
			}
		}

		if (min_dist < minF  && nbGoodMatch>nbGoodMatchF) {
			minF = min_dist;
			nbGoodMatchF = nbGoodMatch;
			nameLabel = mapP.first.c_str();
		}
	}

	if (minF > 0.1) {
		nameLabel = std::string();
	}

	return nameLabel;
}
