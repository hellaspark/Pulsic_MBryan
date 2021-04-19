
#include <stdlib.h>

#include "polygonhandler.hh"

#undef DEBUG


int main(int argc, char* argv[])
{
	/*Helper output*/
	if (argc < 3)
	{
		std::cerr << "Call should be prog \"shapefile.txt\" xcoord ycoord" << std::endl;
	}
	double xpoint, ypoint;
	/*Parse arguments for X and Y*/
	xpoint = atof(argv[2]);
	ypoint = atof(argv[3]);
	/*Flag for checking our shape file has an outline*/
	bool outlineFlag = false;
	bool pointIsInside = false;
	std::ifstream myFile;
	std::string fileLine;
	/*Declare our outline and set of cutouts*/
	Polygon outline;
	std::vector<Cutout> cutouts;
	/*Open shape file*/
	myFile.open(argv[1]);
	if (myFile.is_open()) {
		while (std::getline(myFile, fileLine)) 
		{
			std::stringstream iss(fileLine);
			std::string polyName;
			int vertices;
			if (!(iss >> polyName >> vertices))
			{
				std::cerr << "Could not parse file" << std::endl;
			}
			if (polyName == "OUTLINE")
			{
				outline.setName(polyName);
				outlineFlag = true;
				std::vector<std::pair<double, double>> coords;
				for (auto i = 0; i < vertices; i++) {
					std::string numbers;
					double x, y;
					std::getline(myFile, numbers);
					std::stringstream nss(numbers);
					nss >> x >> y;
					coords.push_back(std::pair<double, double>(x, y));
				}
				for (auto i = 0; i < coords.size(); i++)
				{
					if (i + 1 == (coords.size()))
					{
						outline.addLine(coords[i].first, coords[i].second, coords[0].first, coords[0].second);
					}
					else
					{
						outline.addLine(coords[i].first, coords[i].second, coords[i + 1].first, coords[i + 1].second);
					}
				}
#ifdef DEBUG
				outline.printLines();
#endif // DEBUG
			}
			if (polyName == "CUT")
			{
				Cutout cutout;
				cutout.setName(polyName);
				std::vector<std::pair<double, double>> coords;
				for (auto i = 0; i < vertices; i++) {
					std::string numbers;
					double x, y;
					std::getline(myFile, numbers);
					std::stringstream nss(numbers);
					nss >> x >> y;
					coords.push_back(std::pair<double, double>(x, y));
				}
				for (auto i = 0; i < coords.size(); i++)
				{
					if (i + 1 == (coords.size()))
					{
						cutout.addLine(coords[i].first, coords[i].second, coords[0].first, coords[0].second);
					}
					else
					{
						cutout.addLine(coords[i].first, coords[i].second, coords[i + 1].first, coords[i + 1].second);
					}
				}
				cutouts.push_back(cutout);
			}

		}
	}
	else {
		std::cerr << "Shape file not found" << std::endl;
	}
	if (outlineFlag == true)
	{
		if (outline.isInside(xpoint, ypoint) == true)
		{
			pointIsInside = true;
			debug_output("Point is inside outline");
			for (int i = 0; i < cutouts.size(); i++)
			{
				debug_output("TP2 " << i);
				cutouts[i].printLines();
				if (cutouts[i].isInside(xpoint, ypoint) == true)
				{
					debug_output("TP3 " << i);
					pointIsInside = false;
				}
			}
		}

	}
	else
	{
		std::cerr << "No Outline found in file" << std::endl;
	}

	if (pointIsInside == true)
	{
		std::cout << "Point " << xpoint << ", " << ypoint << " is INSIDE the shape" << std::endl;
		return 1;
	}
	else
	{
		std::cout << "Point " << xpoint << ", " << ypoint << " is OUTSIDE the shape" << std::endl;
		return 0;
	}

}
