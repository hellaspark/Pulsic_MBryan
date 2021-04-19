#include <random>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <iostream>
#include <stdlib.h>

#include "polygonhandler.hh"


#define XLIM 100000
#define YLIM 100000

const double pi = 3.14159265358979323846;

std::vector<std::pair<double, double>> generateShape(const int vertices, const double scale, Polygon * outline)
{
	double totalTheta = ((double)vertices - 2) * 180;

	std::default_random_engine re;
	double lower_bound_angle = 0.0;
	double upper_bound_angle = 360.0;

	double lower_bound_length = 0.0;
	double upper_bound_length = scale;
	std::normal_distribution<double> lineLength(lower_bound_length, upper_bound_length);


	std::vector<std::pair<double, double>> coOrds;
	std::pair<double, double> firstPair;
	firstPair.first = rand() % XLIM;
	firstPair.second = rand() % YLIM;
	if (outline != nullptr) {
		for (int i = 0; i < 10000; i++)
		{
			if (outline->isInside(firstPair.first, firstPair.second) == true) break;
		}
	}
	coOrds.push_back(firstPair);
	for (int ii = 0; ii < (vertices - 1); ii++)
	{
		std::uniform_real_distribution<double> thetaAngle(0.0, totalTheta);
		double newAngle = fmod(thetaAngle(re), 360.0);
		std::pair<double, double> coordPair;
		coordPair.first = (coOrds.back().first + lineLength(re) * cos(newAngle * pi / 180));
		coordPair.second = (coOrds.back().second + lineLength(re) * sin(newAngle * pi / 180));
		coOrds.push_back(coordPair);
		totalTheta -= newAngle;
	}
	return coOrds;
}

void generateShapeFile(const unsigned numCutouts, const std::string fileName)
{
	std::ofstream Outfile(fileName, std::ofstream::out);
	std::string shapeType;
	Polygon outline;
	for (unsigned i = 0; i < numCutouts + 1; i++) 
	{
		if (i == 0) 
		{
			shapeType = "OUTLINE";
			int vertices = rand() % 10 + 3;
			std::vector<std::pair<double, double>>coordinates = generateShape(vertices, 40000, nullptr);
			Outfile << shapeType << " " << vertices << std::endl;
			std::cout << shapeType << " " << vertices << std::endl;
			for (int j = 0; j < coordinates.size(); j++)
			{
				Outfile << coordinates[j].first << " " << coordinates[j].second << std::endl;
				std::cout << coordinates[j].first << " " << coordinates[j].second << std::endl;
				if (j + 1  == coordinates.size() )
				{
					outline.addLine(coordinates[j].first, coordinates[j].second, coordinates[0].first, coordinates[0].second);
				}
				else
				{
					outline.addLine(coordinates[j].first, coordinates[j].second, coordinates[j + 1].first, coordinates[j + 1].second);
				}
			}
		}
		else 
		{
			shapeType = "CUT";
			int vertices = rand() % 10 + 3;
			std::vector<std::pair<double, double>>coordinates = generateShape(vertices, 3000, &outline);
			Outfile << shapeType << " " << vertices << std::endl;
			std::cout << shapeType << " " << vertices << std::endl;
			for (int j = 0; j < coordinates.size(); j++)
			{
				Outfile << coordinates[j].first << " " << coordinates[j].second << std::endl;
				std::cout << coordinates[j].first << " " << coordinates[j].second << std::endl;
			}
		}
	}
	Outfile.close();
}



void main(int argc, char* argv[])
{
	int numberOfFiles = atoi(argv[1]);

	for (int i = 0; i < numberOfFiles; i++)
	{
		int shapenum = rand() % 4 + 1;
		std::stringstream fn;
		fn << "shapes" << i << ".txt";
		generateShapeFile(shapenum, fn.str());
	}


}