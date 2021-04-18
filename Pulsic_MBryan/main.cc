#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>


class Line {
private:
	double Ymax = 0.0, Ymin = 0.0, XmaxY = 0.0, XminY = 0.0; /*XmaxY is the X co-ordinate associated with Ymax, similar for XminY*/
public:

	Line(const double x1 = 0.0, const double y1 = 0.0, const double x2 = 0.0, const double y2= 0.0) 
	{
		/*Sort the points by y axis position (makes the equations a bit simpler)*/
		if (y1 < y2) {
			Ymin = y1;
			XminY = x1;
			Ymax = y2;
			XmaxY = x2;
		}
		else {
			Ymin = y2;
			XminY = x2;
			Ymax = y1;
			XmaxY = x1;
		}
	}
	/*Check if point's y axis is inside the line's*/
	bool isInsideY(const double Yp = 0.0f) 
	{
		if (Yp >= Ymin || Yp <= Ymax) return true;
		else return false;
	}
	/*Check if point is to the "right" (greater on the x axis) of the line diagonal*/
	bool isRightofLine(const double Xp = 0.0f, const double Yp = 0.0f) 
	{
		/*Work out the x value of the line at the y value of the point*/
		double Sx = XminY + ((XmaxY - XminY) * ((Yp - Ymin) / (Ymax - Ymin)));
		if (Xp >= Sx) return true;
		else return false;
	}

	void printLine()
	{
		std::cout << "(" << XminY << ", " << Ymin << ") (" << XmaxY << ", " << Ymax << ") "; 
	}
};

class Polygon {
private:
	std::vector< Line*> lines;
	double Ymax = 0.0, Ymin = 0.0;
public:

	void addLine(const double x1, const double y1, const double x2, const double y2) 
	{
		Line* newLine = new Line(x1, y1, x2, y2);
		lines.push_back(newLine);
		if (y1 > Ymax) 
		{
			Ymax = y1;
		}
		if (y2 > Ymax) 
		{
			Ymax = y2;
		}
		if (y1 < Ymin) 
		{
			Ymin = y1;
		}
		if (y2 < Ymin) 
		{
			Ymin = y2;
		}
	}

	bool isInside(const double Xp = 0.0f, const double Yp = 0.0f) 
	{
		unsigned lineCount = 0;
		if (Yp < Ymax && Yp > Ymin)
		{
			for (unsigned i = 0; i < lines.size(); i++)
			{
				if (lines[i]->isInsideY(Yp) == true)
				{
					if (lines[i]->isRightofLine(Xp, Yp) == true)
					{
						lineCount++;
					}
				}
			}
		}
		if (lineCount % 2 == 0) return false;
		else return true;
	}

	void printLines()
	{
		for (unsigned i = 0; i < lines.size(); i++)
		{
			std::cout << "Line " << i << " points: ";
			lines[i]->printLine();
			std::cout << std::endl;
		}
	}

};

int main(int argc, char* argv[]) 
{
	if (argc < 3) 
	{
		std::cerr << "Call should be ./prog \"shapefile.txt\" xcoord ycoord" << std::endl;
	}
	double xpoint, ypoint;
	xpoint = atof(argv[2]);
	ypoint = atof(argv[3]);
	bool outlineFlag = false;
	bool pointIsInside = false;
	std::ifstream myFile;
	std::string fileLine;
	Polygon outline;
	std::vector<Polygon*> cutouts;
	myFile.open(argv[1]);
	if (myFile.is_open()) {
		while (std::getline(myFile, fileLine)) /*rewrite this bit*/
		{
			std::stringstream iss(fileLine);
			std::string polyName; 
			unsigned vertices;
			if (!(iss >> polyName >> vertices))
			{
				std::cerr << "Could not parse file" << std::endl;
			}
			if (polyName == "OUTLINE")
			{
				outlineFlag = true;
				std::vector<std::pair<double, double>> coords;
				for (unsigned i = 0; i < vertices; i++) {
					std::string numbers;
					double x, y;
					std::getline(myFile, numbers);
					std::stringstream nss(numbers);
					nss >> x >> y;
					/*std::cout << x << ", " << y << std::endl;*/
					coords.push_back(std::pair<double, double>(x, y));					
				}
				for (long int i = 0; i < coords.size(); i++)
				{
					if (i+1l == (coords.size()))
					{
						outline.addLine(coords[i].first, coords[i].second, coords[0].first, coords[0].second);
					}
					else
					{
						outline.addLine(coords[i].first, coords[i].second, coords[i + 1l].first, coords[i + 1l].second);
					}
				}
				std::cout << "Outline" << std::endl;
				outline.printLines();
			}
			if (polyName == "CUT")
			{
				Polygon cutout;
				std::vector<std::pair<double, double>> coords;
				for (unsigned i = 0; i < vertices; i++) {
					std::string numbers;
					double x, y;
					std::getline(myFile, numbers);
					std::stringstream nss(numbers);
					nss >> x >> y;
					coords.push_back(std::pair<double, double>(x, y));
				}
				for (long int i = 0; i < coords.size(); i++)
				{
					if (i + 1l == (coords.size()))
					{
						cutout.addLine(coords[i].first, coords[i].second, coords[0].first, coords[0].second);
					}
					else
					{
						cutout.addLine(coords[i].first, coords[i].second, coords[i + 1l].first, coords[i + 1l].second);
					}
				}
				std::cout << "Cut" << std::endl;
				cutout.printLines();
				cutouts.push_back(&cutout);
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
			for (int i = 0; i < cutouts.size(); i++)
			{
				if (cutouts[i]->isInside(xpoint, ypoint) == true)
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
