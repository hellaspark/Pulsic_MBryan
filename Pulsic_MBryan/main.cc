#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>

#define DEBUG

#ifdef DEBUG
#define debug_output(debugout) do { std::cerr << debugout << std::endl;} while (false)
#else
#define debug_output(debugout) do {} while (false)
#endif //DEBUG

/*Define explict states for line-point relationship. Limit value tracking matters
to avoid issues if the raycast passes through a vertex directly*/
enum class LineState {Left, On, Right, Rmax, Rmin};

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
	virtual LineState isRightofLine(const double Xp = 0.0f, const double Yp = 0.0f)
	{
		/*Work out the x value of the line at the y value of the point*/
		double Sx = XminY + ((XmaxY - XminY) * ((Yp - Ymin) / (Ymax - Ymin)));
		if (Xp > Sx)
		{
			if (Yp == Ymax) return LineState::Rmax;
			else if (Yp == Ymin) return LineState::Rmin;
			else return LineState::Right;
		}
		else if (Xp == Sx) return LineState::On;
		else return LineState::Left;
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
		/*Add a new line to the Polygon*/
		Line* newLine = new Line(x1, y1, x2, y2);
		lines.push_back(newLine);

		/*Track the Y axis limits of the shape to easily exclude shape from checks*/
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

		unsigned lineCount = 0;
		bool onLine = false;
		/*See if its worth checking this shape with a quick range check on Y axis*/
		if (Yp <= Ymax && Yp >= Ymin)
		{
			debug_output( Yp << " is greater than " << Ymin << " and less than " << Ymax );
			LineState prevRelation = LineState::Left;
			for (unsigned i = 0; i < lines.size(); i++)
			{
				/*Determine if point is within the bounds of the Y co-ords of the line*/
				if (lines[i]->isInsideY(Yp) == true)
				{
					/*Determine if point is to the right of the line*/
					const LineState currentRelation = lines[i]->isRightofLine(Xp, Yp);
					if (currentRelation == LineState::On)
					{
						debug_output(Xp << " " << Yp << " is on a line");
						onLine = true;
					}
					else if (currentRelation == LineState::Right)
					{
						debug_output(Xp << " " << Yp << " is right of a line");
						lineCount++;
					}
					/*If our raycast passed through the limit of a line check with previous
					relation, if both are the same we count both, as we're passing through
					a local maxima or minima of an object, otherwise we only count the first
					as we're passing through an inflection of the line*/
					else if (currentRelation >= LineState::Rmax)
					{
						debug_output(Xp << " " << Yp << " is right of a line and at a limit: " << (int)currentRelation);
						if (prevRelation < LineState::Rmax || prevRelation == currentRelation)
						{
							lineCount++;
						}
					}
					/* Save to previous relation to check the connecting line's state*/
					prevRelation = currentRelation;
				}

			}
		}
		/*If we're on an edge, we're an outline so it's inside, otherwise we count the lines
		we passed through and if it's odd, we're inside*/
		if (onLine == true || lineCount % 2 != 0) return true;
		else return false;
	}

	void setName(std::string name)
	{
		/*Set name of polygon, only really used for debugging at the moment*/
		polygonName = name;
	}

	void printLines()
	{
		/*Print out the lines of the polygon for debugging*/
		std::cout << polygonName << " has Ymin " << Ymin << " and Ymax " << Ymax << std::endl;
		for (unsigned i = 0; i < lines.size(); i++)
		{
			std::cout << "Line " << i << " points: ";
			lines[i]->printLine();
			std::cout << std::endl;
		}
	}

};

class Cutout : public Polygon
{
public:

	virtual bool isInside(const double Xp = 0.0f, const double Yp = 0.0f)
	{

		unsigned lineCount = 0;
		bool onLine = false;
		/*See if its worth checking this shape with a quick range check on Y axis*/
		if (Yp <= Ymax && Yp >= Ymin)
		{
			debug_output(Yp << " is greater than " << Ymin << " and less than " << Ymax);
			LineState prevRelation = LineState::Left;
			for (unsigned i = 0; i < lines.size(); i++)
			{
				/*Determine if point is within the bounds of the Y co-ords of the line*/
				if (lines[i]->isInsideY(Yp) == true)
				{
					/*Determine if point is to the right of the line*/
					const LineState currentRelation = lines[i]->isRightofLine(Xp, Yp);
					if (currentRelation == LineState::On)
					{
						debug_output(Xp << " " << Yp << " is on a line");
						onLine = true;
					}
					else if (currentRelation == LineState::Right)
					{
						debug_output(Xp << " " << Yp << " is right of a line");
						lineCount++;
					}
					/*If our raycast passed through the limit of a line check with previous
					relation, if both are the same we count both, as we're passing through
					a local maxima or minima of an object, otherwise we only count the first
					as we're passing through an inflection of the line*/
					else if (currentRelation >= LineState::Rmax)
					{
						debug_output(Xp << " " << Yp << " is right of a line and at a limit: " << (int)currentRelation);
						if (prevRelation < LineState::Rmax || prevRelation == currentRelation)
						{
							lineCount++;
						}
					}
					/* Save to previous relation to check the connecting line's state*/
					prevRelation = currentRelation;
				}

			}
		}
		/*If we're on an edge, we're a cutout so we're outside the shape (so inside the outline), 
		otherwise we count the lines we passed through and if it's odd, we're inside*/
		if (onLine == true || lineCount % 2 == 0) return false;
		else return true;
	}
};

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
