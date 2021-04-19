#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>


class Line {
protected:
	double Ymax = 0.0, Ymin = 0.0, XmaxY = 0.0, XminY = 0.0; /*XmaxY is the X co-ordinate associated with Ymax, similar for XminY*/
public:

	Line(const double x1 = 0.0, const double y1 = 0.0, const double x2 = 0.0, const double y2 = 0.0)
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
	virtual bool isInsideY(const double Yp = 0.0f)
	{
		if (Yp >= Ymin && Yp <= Ymax) {
			std::cout << Yp << " is inside Y line " << Ymin << " < " << Ymax << std::endl;
			return true;
		}
		else return false;
	}
	/*Check if point is to the "right" (greater on the x axis) of the line diagonal*/
	virtual bool isRightofLine(const double Xp = 0.0f, const double Yp = 0.0f)
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

protected:
	double Ymax = 0.0, Ymin = DBL_MAX;
	std::string polygonName = "BLANK";
	std::vector< Line*> lines;
public:

	virtual void addLine(const double x1, const double y1, const double x2, const double y2)
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

	virtual bool isInside(const double Xp = 0.0f, const double Yp = 0.0f)
	{
		std::cout << "TEST_II " << Yp << " " << Ymax << " " << Ymin << std::endl;
		unsigned lineCount = 0;

		if (Yp <= Ymax && Yp >= Ymin)
		{
			std::cout << Yp << " is greater than " << Ymin << " and less than " << Ymax << std::endl;
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

	void setName(std::string name)
	{
		polygonName = name;
	}

	void printLines()
	{
		std::cout << polygonName << " has Ymin " << Ymin << " and Ymax " << Ymax << std::endl;
		for (unsigned i = 0; i < lines.size(); i++)
		{
			std::cout << "Line " << i << " points: ";
			lines[i]->printLine();
			std::cout << std::endl;
		}
	}

};

class CutLine : public Line
{
public:
	using Line::Line;

	virtual bool isInsideY(const double Yp = 0.0f) override
	{
		/*in the cutout, we want to exclude horizontal lines as they mark an edge where all points are
		outside the cutout */
		if (Yp > Ymin && Yp <= Ymax && Ymin != Ymax) {
			std::cout << Yp << " is inside Y line " << Ymin << " < " << Ymax << std::endl;
			return true;
		}
		else return false;
	}

	virtual bool isRightofLine(const double Xp = 0.0f, const double Yp = 0.0f)
	{

		/*Work out the x value of the line at the y value of the point*/
		double Sx = XminY + ((XmaxY - XminY) * ((Yp - Ymin) / (Ymax - Ymin)));
		/*detect vertex matching, if on vertex, we want to be outside the cutout*/
		if ((Xp == XminY && Yp == Ymin) || (Xp == XmaxY && Yp == Ymax))
		{
			return false; /*TODO fix this logic*/
		}
		else if (Xp >= Sx) return true;
		else return false;
	}
};

class Cutout : public Polygon
{
public:
	virtual void addLine(const double x1, const double y1, const double x2, const double y2) override
	{
		CutLine* newLine = new CutLine(x1, y1, x2, y2);
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

	virtual bool isInside(const double Xp = 0.0f, const double Yp = 0.0f) override
	{
		std::cout << "TEST_II " << Yp << " " << Ymax << " " << Ymin << std::endl;
		unsigned lineCount = 0;

		if (Yp < Ymax && Yp > Ymin)
		{
			std::cout << Yp << " is greater than " << Ymin << " and less than " << Ymax << std::endl;
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
	std::vector<Cutout> cutouts;
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
				outline.setName(polyName);
				outlineFlag = true;
				std::vector<std::pair<double, double>> coords;
				for (auto i = 0; i < vertices; i++) {
					std::string numbers;
					double x, y;
					std::getline(myFile, numbers);
					std::stringstream nss(numbers);
					nss >> x >> y;
					/*std::cout << x << ", " << y << std::endl;*/
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
				/*outline.printLines();*/
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
			std::cout << "TEST1" << std::endl;
			for (int i = 0; i < cutouts.size(); i++)
			{
				std::cout << "TEST2 " << i << std::endl;
				cutouts[i].printLines();
				if (cutouts[i].isInside(xpoint, ypoint) == true)
				{
					std::cout << "TEST3" << std::endl;
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
