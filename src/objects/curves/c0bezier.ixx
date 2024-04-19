export module c0bezier;

import <vector>;

import curve;
import point;
import shader;

export class C0Bezier : public Curve
{
public:
	C0Bezier(const std::vector<Point*>& points) : Curve(getCurveName(), points)
	{}

	C0Bezier(std::initializer_list<Point*> points) : Curve(getCurveName(), points)
	{}

	virtual void update() override
	{
		if (!scheduledToUpdate)
			return;

		fillPositions();
		uploadPositions();
		
		scheduledToUpdate = false;
	}

private:
	inline virtual std::string getCurveName() const override
	{
		return "C0 Bezier";
	}

	virtual void fillPositions() override
	{
		positions.clear();
		positionsQuadratic.clear();
		positionsLine.clear();
		auto inputSize = points.size();

		switch (inputSize)
		{
		case 0:
		case 1:
			break;
		case 2:
			for (int i = 0; i < inputSize; i++)
			{
				positionsLine.push_back(points[i]->getPosition());
			}
			break;
		case 3:
			for (int i = 0; i < inputSize; i++)
			{
				positionsQuadratic.push_back(points[i]->getPosition());
			}
			break;
		default:
			// First points
			for (int i = 0; i < degree + 1; i++)
			{
				positions.push_back(points[i]->getPosition());
			}
			positions.push_back(points[degree]->getPosition());

			int pointsProcessed = degree + 1;
			// full segments
			for (int i = 0; i < (inputSize - degree - 1) / degree; i++)
			{
				for (int j = 0; j < degree; j++)
				{
					positions.push_back(points[degree + 1 + degree * i + j]->getPosition());
					pointsProcessed++;
				}
				positions.push_back(points[degree + 1 + degree * i + degree - 1]->getPosition());
			}
			int rest = inputSize - pointsProcessed;
			switch (rest)
			{
			case 1:
				positionsLine.push_back(points[pointsProcessed - 1]->getPosition());
				positionsLine.push_back(points[pointsProcessed]->getPosition());
				break;
			case 2:
				positionsQuadratic.push_back(points[pointsProcessed - 1]->getPosition());
				positionsQuadratic.push_back(points[pointsProcessed]->getPosition());
				positionsQuadratic.push_back(points[pointsProcessed + 1]->getPosition());

				// for polygon rendering
				positions.push_back(points[pointsProcessed]->getPosition());
				positions.push_back(points[pointsProcessed + 1]->getPosition());
				break;
			}
		}
	}

};