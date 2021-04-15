#include <iostream>
#include <vector>

class Line {
private:
	double Ymax, Ymin, XmaxY, XminY; /*XmaxY is the X co-ordinate associated with Ymax, similar for XminY*/
public:

	Line(const double x1 = 0.0f, const double y1 = 0.0f, const double x2 = 0.0f, const double y2= 0.0f) {
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
	bool isInsideY(const double Yp = 0.0f) {
		if (Yp >= Ymin || Yp <= Ymax) return true;
		else return false;
	}
	bool isLeftofLine(const double Xp = 0.0f, const double Yp = 0.0f) {
		Sx = XminY + ((XmaxY - XminY) * ((Yp - Ymin) / (Ymax - Ymin)));
		if (Xp >= Sx) return true;
		else return false;
	}
};

class Polygon {
private:
	std::vector< Line*> lines;
	double Ymax, Ymin;
public:

	void addLine(const double x1, const double y1, const double x2, const double y2) {
		Line* newLine = new Line(x1, y1, x2, y2);
		lines.push_back(newLine);
	}

	Polygon() {

	}
};