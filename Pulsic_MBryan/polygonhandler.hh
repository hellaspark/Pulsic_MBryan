#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#ifdef DEBUG
#define debug_output(debugout) do { std::cerr << debugout << std::endl;} while (false)
#else
#define debug_output(debugout) do {} while (false)
#endif //DEBUG

/*Define explict states for line-point relationship. Limit value tracking matters
to avoid issues if the raycast passes through a vertex directly*/
enum class LineState { Left, On, Right, Rmax, Rmin };

/*Class of object for the lines of a polygon. Contains methods to check if a point is inside the limits of the line on the Y axis,
and to check if a point is to the right of the line or at its limits*/
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
			debug_output( Yp << " is inside Y line " << Ymin << " < " << Ymax );
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

	/*Print line information, only used for debugging*/
	void printLine()
	{
		std::cout << "(" << XminY << ", " << Ymin << ") (" << XmaxY << ", " << Ymax << ") ";
	}
};

/*Class of object for polygonal shapes. Contains methods to add lines to the polygon, and to check if a point is inside it*/
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
	/*Check if given coordinate is inside of the polygon object*/
	virtual bool isInside(const double Xp = 0.0f, const double Yp = 0.0f)
	{
		int lineCount = 0;
		/*Initialise variable to track if we've seen a line the point is directly on*/
		bool onLine = false;
		/*See if its worth checking this shape with a quick range check on Y axis*/
		if (Yp <= Ymax && Yp >= Ymin)
		{
			debug_output(Yp << " is greater than " << Ymin << " and less than " << Ymax);
			/*Create storage of first state and previous state*/
			LineState firstRelation = LineState::Left, prevRelation = LineState::Left;
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
					a local maxima or minima of an object, otherwise we only count the second
					as we're passing through an inflection of the line*/
					else if (currentRelation >= LineState::Rmax)
					{
						debug_output(Xp << " " << Yp << " is right of a line and at a limit: " << (int)currentRelation);
						/*previous relation was the same limit, so we're a maxima or minima*/
						if (prevRelation == currentRelation)
						{
							lineCount += 2;
						}
						/*previous relation was a limit but not the same limit, so we're inflected*/
						else if (prevRelation < LineState::Rmax)
						{
							lineCount++;
						}
					}
					/* Save the first relation to check with the last relation*/
					if (i == 0) firstRelation = currentRelation;
					/* Save to previous relation to check the connecting line's state*/
					prevRelation = currentRelation;
				}
			}
			/*If first relation is at a limit and last relation is at the opposite limit
			we need to increment the linecount to account for the inflection point*/
			if (firstRelation >= LineState::Rmax && firstRelation != prevRelation)
			{
				lineCount++;
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

/*Inherited class of polygon, the cutout. Modifies the method for checking a point is inside the cutout to exclude its edges,
as the requirement states that points on the edges of cutouts are inside the outline, not the cutout*/
class Cutout : public Polygon
{
public:
	/*Check if given coordinate is inside of the cutout object, edges count as outside*/
	virtual bool isInside(const double Xp = 0.0f, const double Yp = 0.0f)
	{
		int lineCount = 0;
		/*Initialise variable to track if we've seen a line the point is directly on*/
		bool onLine = false;
		/*See if its worth checking this shape with a quick range check on Y axis*/
		if (Yp <= Ymax && Yp >= Ymin)
		{
			debug_output(Yp << " is greater than " << Ymin << " and less than " << Ymax);
			/*Create storage of first state and previous state*/
			LineState firstRelation = LineState::Left, prevRelation = LineState::Left;
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
						/*previous relation was the same limit, so we're a maxima or minima*/
						if (prevRelation == currentRelation)
						{
							lineCount += 2;
						}
						/*previous relation was a limit but not the same limit, so we're inflected*/
						else if (prevRelation < LineState::Rmax)
						{
							lineCount++;
						}
					}
					/* Save the first relation to check with the last relation*/
					if (i == 0) firstRelation = currentRelation;
					/* Save to previous relation to check the connecting line's state*/
					prevRelation = currentRelation;
				}
			}
			/*If first relation is at a limit and last relation is at the opposite limit
			we need to increment the linecount to account for the inflection point*/
			if (firstRelation >= LineState::Rmax && firstRelation != prevRelation)
			{
				lineCount++;
			}
		}
		/*If we're on an edge, we're a cutout so we're outside the shape (so inside the outline),
		otherwise we count the lines we passed through and if it's odd, we're inside*/
		if (onLine == true || lineCount % 2 == 0) return false;
		else return true;
	}
};
