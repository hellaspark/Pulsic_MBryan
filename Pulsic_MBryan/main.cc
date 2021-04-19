
#include <stdlib.h>

#include "polygonhandler.hh"


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
		/*While not at end of file*/
		while (std::getline(myFile, fileLine)) 
		{
			std::stringstream iss(fileLine);
			std::string polyName;
			int vertices;
			if (!(iss >> polyName >> vertices))
			{
				std::cerr << "Could not parse file, line format incorrect" << std::endl;
				break;
			}
			if (polyName == "OUTLINE")
			{
				/*Shape is the outline, fill the created outline object*/
				outline.setName(polyName);
				outlineFlag = true;
				/*Load a vector with the list of co-ordinates*/
				std::vector<std::pair<double, double>> coords;
				for (int i = 0; i < vertices; i++) {
					std::string numbers;
					double x, y;
					/*Pop the next line off the file into the list of co-ords*/
					std::getline(myFile, numbers);
					std::stringstream nss(numbers);
					nss >> x >> y;
					coords.push_back(std::pair<double, double>(x, y));
				}
				/*With list of co-ordinates, add a series of lines beginning and ending at those co-ordinates
				wrap around last co-ordinates to complete the shape*/
				for (int i = 0; i < coords.size(); i++)
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
				/*Shape is a cutout, create a new cutout object, fill it, and add it to the list of cutouts*/
				Cutout cutout;
				cutout.setName(polyName);
				std::vector<std::pair<double, double>> coords;
				/*Load a vector with the list of co-ordinates*/
				for (int i = 0; i < vertices; i++) {
					std::string numbers;
					double x, y;
					/*Pop the next line off the file into the list of co-ords*/
					std::getline(myFile, numbers);
					std::stringstream nss(numbers);
					nss >> x >> y;
					coords.push_back(std::pair<double, double>(x, y));
				}
				/*With list of co-ordinates, add a series of lines beginning and ending at those co-ordinates
				wrap around last co-ordinates to complete the shape*/
				for (int i = 0; i < coords.size(); i++)
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
	/*File is valid input if we have an outline object*/
	if (outlineFlag == true)
	{
		/*Check if point is inside the outline, if it's not, leave flag as false and stop execution
		otherwise, set flag as true for now*/
		if (outline.isInside(xpoint, ypoint) == true)
		{
			pointIsInside = true;
			debug_output("Point is inside outline");
			/*Loop over all cutouts and check if point is inside any of them, if it is, set flag to false again*/
			for (int i = 0; i < cutouts.size(); i++)
			{
#ifdef DEBUG
				cutouts[i].printLines();
#endif // DEBUG
				if (cutouts[i].isInside(xpoint, ypoint) == true)
				{
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
