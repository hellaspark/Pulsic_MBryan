#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

class Line {
private:
	double Ymax, Ymin, XmaxY, XminY; /*XmaxY is the X co-ordinate associated with Ymax, similar for XminY*/
public:

	Line(const double x1 = 0.0f, const double y1 = 0.0f, const double x2 = 0.0f, const double y2= 0.0f) 
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
};

class Polygon {
private:
	std::vector< Line*> lines;
	double Ymax, Ymin;
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
		for (unsigned i = 0; i < lines.size(); i++) 
		{
			if (lines[i]->isInsideY(Yp)) 
			{
				if (lines[i]->isRightofLine(Xp, Yp)) 
				{
					lineCount++;
				}
			}
		}
		if (lineCount % 2 == 0) return false;
		else return true;
	}

};

int main(int argc, char* argv[]) 
{
	if (argc < 3) 
	{
		std::cerr << "Call should be ./prog \"shapefile.txt\" xcoord ycoord" << std::endl;
	}
	bool outlineFlag = false;
	std::ifstream myFile;
	std::string fileLine;
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
				for (unsigned i; i < vertices; i++) {

				}
			}
		}
	}
	else {
		std::cout << "Shape file not found" << std::endl;
	}



	return 0;
}