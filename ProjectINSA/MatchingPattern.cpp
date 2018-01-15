#include "stdafx.h"
#include "MatchingPattern.h"

/*
Construit les descriptor pour les pattern
*/
void MatchingPattern::buildKeyPoint() {
	vector<string> files;
	string pathpattern = Utils::GLOBAL_PATH +"/pattern";
	Utils::read_directory(pathpattern, files);
	int minHessian = 400;
	Mat patternMat;
	SurfFeatureDetector detector(minHessian);
	SurfDescriptorExtractor extractor;
	Mat descriptor;
	for (string &file : files)
	{
		string pattern = file;
		if (pattern != "." && pattern != "..") {
			vector<string> v{ Utils::explode(pattern, '.') };
			std::cout << pathpattern  + "/" + pattern.c_str() << endl;
			string pat = pathpattern + "/" + pattern.c_str();

			patternMat = cvLoadImage(pat.c_str() , CV_LOAD_IMAGE_GRAYSCALE);

			std::vector<KeyPoint> keypoint;
			detector.detect(patternMat, keypoint);
			extractor.compute(patternMat, keypoint, descriptor);
			pattern = v[0];
			descriptorPattern[pattern] = std::make_pair(descriptor, std::make_pair(patternMat, keypoint));
		}
	}
}

/*
Retourne le pattern reconnu au sein de l'image
*/
String MatchingPattern::matchWithMath(Mat match) {
	int minHessian = 400;
	SurfFeatureDetector detector(minHessian);
	std::vector<KeyPoint> keypoints;
	detector.detect(match, keypoints);
	SurfDescriptorExtractor extractor;
	Mat descriptors;
	extractor.compute(match, keypoints, descriptors);

	double minF =1500;
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	String nameLabelGoodmatch,nameLabelMinDist, nameLabel;
	int nbGoodMatch, nbGoodMatchF = 0;
	for (pair<string,pair<Mat, pair<Mat, std::vector<KeyPoint>>>> const &mapP : descriptorPattern) {
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

		for (int i = 0; i <  mapP.second.first.rows; i++)
		{
			if (matches[i].distance <=  0.1)
			{
				nbGoodMatch++;
			}
		}
		if ( nbGoodMatch>nbGoodMatchF) {
			nameLabelGoodmatch = mapP.first.c_str();
			nbGoodMatchF = nbGoodMatch;
		}
		if (min_dist<minF) {
			minF = min_dist;
			nameLabelMinDist = mapP.first.c_str();
		}
	}
	if (nameLabelGoodmatch == "") {
		nameLabel = nameLabelMinDist;
	}
	else nameLabel = nameLabelGoodmatch;
	//on retourne le label de l'image qui correspond au pattern
	return nameLabel;
}
