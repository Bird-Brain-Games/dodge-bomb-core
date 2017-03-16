#pragma once

#include <vector>
#include "GameMath.h"

template <typename T>
struct SpeedControlTableRow
{
	int segment;
	float tValue;
	float arcLength;
	float arcLengthNormalized;
	T sampleValue;
};

template <typename T>
class KeyframeController
{
private:
	unsigned int m_pCurrentKeyframe;
	unsigned int m_pNextKeyframe;
	std::vector<T> m_pKeys;
	std::vector<bool> connected;
	float m_pKeyLocalTime;

	std::vector< SpeedControlTableRow<T> > m_pSpeedControlLookUpTable;

public:
	KeyframeController()
		: m_pCurrentKeyframe(0),
		m_pNextKeyframe(1),
		m_pKeyLocalTime(0.0f),
		paused(false),
		loops(true),
		connectedEndpoints(false),
		doesSpeedControl(false),
		curvePlaySpeed(1.0f)
	{

	}

	void calculateLookupTable(int numSamples)
	{
		if (m_pKeys.size() > 1)
		{
			m_pSpeedControlLookUpTable.clear(); // clear existing table

			float timeStep = 1.0f / numSamples;

			// Create table and compute segment, t value and sample columns of table
			for (unsigned int i = 0; i < m_pKeys.size() - 1; i++) // loop through each segment
			{
				for (float j = 0.0f; j <= 1.0f; j += timeStep) // iterate through each sample on the current segment
				{
					SpeedControlTableRow<T> row;
					row.segment = i;
					row.tValue = j;
					row.sampleValue = Math::lerp(m_pKeys[i], m_pKeys[i + 1], j); // or catmull or bezier

					m_pSpeedControlLookUpTable.push_back(row);
				}
			}

			// Calculate arc length column of table
			int numEntries = m_pSpeedControlLookUpTable.size();

			if (numEntries == 0)
			{
				std::cout << "Failed to create look up table. " << std::endl;
				return;
			}

			// Initialize first row of table
			// Remember the struct has no ctor so we need to make sure to set everything
			m_pSpeedControlLookUpTable[0].arcLength = 0.0f;
			m_pSpeedControlLookUpTable[0].arcLengthNormalized = 0.0f;

			for (int i = 1; i < numEntries; i++) // actually compute magnitude between segments
			{
				T difference = m_pSpeedControlLookUpTable[i].sampleValue - m_pSpeedControlLookUpTable[i - 1].sampleValue;
				float distance = glm::length(difference);

				m_pSpeedControlLookUpTable[i].arcLength = distance + m_pSpeedControlLookUpTable[i - 1].arcLength;
			}

			// Normalize arc lengths
			float totalCurveLength = m_pSpeedControlLookUpTable[numEntries - 1].arcLength;

			for (int i = 1; i < numEntries; i++)
				m_pSpeedControlLookUpTable[i].arcLengthNormalized = m_pSpeedControlLookUpTable[i].arcLength / totalCurveLength;
		}
	}

	T speedControlledUpdate(float dt)
	{
		// key local time is now from start of curve to end of curve, instead of per segment

		m_pKeyLocalTime += (dt / curvePlaySpeed); // control playback speed

		updateSegmentIndices();

		// look up values from table
		for (unsigned int i = 1; i < m_pSpeedControlLookUpTable.size(); i++)
		{
			if (m_pSpeedControlLookUpTable[i].arcLengthNormalized >= m_pKeyLocalTime)
			{
				// calculate t value
				float arc0 = m_pSpeedControlLookUpTable[i - 1].arcLengthNormalized;
				float arc1 = m_pSpeedControlLookUpTable[i].arcLengthNormalized;
				float tVal = Math::invLerp(m_pKeyLocalTime, arc0, arc1);

				// calculate intermediate table
				T sample0 = m_pSpeedControlLookUpTable[i - 1].sampleValue;
				T sample1 = m_pSpeedControlLookUpTable[i].sampleValue;

				return Math::lerp(sample0, sample1, tVal);
			}
		}

		return T(); // if lookup table is set up correctly, this should never trigger
	}

	T update(float dt, lineMode mode = lineMode::LERP)
	{
		if (m_pKeys.size() < 2)
			return T();

		if (doesSpeedControl)
			return speedControlledUpdate(dt);
		else
			return noSpeedControlUpdate(dt, mode);
	}

	std::vector<T> getCurrentPoints()
	{
		std::vector<T> currentPoints;

		if (!connectedEndpoints)
		{
			while (m_pNextKeyframe + 1 >= m_pKeys.size() ||
				m_pCurrentKeyframe == 0.0f)
			{
				m_pKeyLocalTime = 1.0f;
				updateSegmentIndices();
			}

			currentPoints.push_back(m_pKeys[m_pCurrentKeyframe - 1]);
			currentPoints.push_back(m_pKeys[m_pCurrentKeyframe]);
			currentPoints.push_back(m_pKeys[m_pNextKeyframe]);
			currentPoints.push_back(m_pKeys[m_pNextKeyframe + 1]);
		}

		else
		{
			if (m_pCurrentKeyframe == 0)
			{
				currentPoints.push_back(m_pKeys[m_pKeys.size() - 1]);
			}
			else
			{
				currentPoints.push_back(m_pKeys[m_pCurrentKeyframe - 1]);
			}
			currentPoints.push_back(m_pKeys[m_pCurrentKeyframe]);


			if (m_pNextKeyframe >= m_pKeys.size())
			{
				currentPoints.push_back(m_pKeys[0]);
				currentPoints.push_back(m_pKeys[1]);
			}
			else if (m_pNextKeyframe + 1 >= m_pKeys.size())
			{
				currentPoints.push_back(m_pKeys[m_pNextKeyframe]);
				currentPoints.push_back(m_pKeys[0]);
			}
			else
			{
				currentPoints.push_back(m_pKeys[m_pNextKeyframe]);
				currentPoints.push_back(m_pKeys[m_pNextKeyframe + 1]);
			}
		}

		return currentPoints;
	}

	void updateSegmentIndices()
	{
		if (m_pKeyLocalTime >= 1.0f || !connected.at(m_pCurrentKeyframe))
		{
			m_pKeyLocalTime = 0.0f;
			m_pCurrentKeyframe++;
			m_pNextKeyframe++;

			// If we're at the end of the animation, jump back to beginning
			// Note: you can add additional logic here to handle end of animation behaviour
			// such as: ping-ponging (playing in reverse back to beginning), closed loop, etc.
			if (m_pNextKeyframe >= m_pKeys.size())
			{
				if (!connectedEndpoints)
				{
					m_pCurrentKeyframe = 0;
					m_pNextKeyframe = 1;
				}
				else
				{
					m_pNextKeyframe = 0;
				}
			}
			if (m_pCurrentKeyframe >= m_pKeys.size())
				m_pCurrentKeyframe = 0;
		}
	}

	T noSpeedControlUpdate(float dt, lineMode mode)
	{
		m_pKeyLocalTime += dt;

		updateSegmentIndices();

		T p0 = m_pKeys[m_pCurrentKeyframe];
		T p1 = m_pKeys[m_pNextKeyframe];
		T p2;
		T p3;
		std::vector<T> currentPoints;

		switch (mode)
		{
		case LERP:
			return Math::lerp<T>(p0, p1, m_pKeyLocalTime);
			break;
		case CATMULLROM:
			if (m_pKeys.size() < 3) break;
			if (m_pKeys.size() <= 3 && !connectedEndpoints) break;
			currentPoints = getCurrentPoints();

			p0 = currentPoints.at(0);
			p1 = currentPoints.at(1);
			p2 = currentPoints.at(2);
			p3 = currentPoints.at(3);
			return Math::catmull(p0, p1, p2, p3, m_pKeyLocalTime);
			
			break;
		case BEZIER:
			if (m_pKeys.size() < 3) break;
			if (m_pKeys.size() <= 3 && !connectedEndpoints) break;

			// If not divisible by 3, make it so
			while ((m_pNextKeyframe + 1) % 3 != 0)
			{
				m_pKeyLocalTime = 1.0f;
				updateSegmentIndices();
			}

			currentPoints = getCurrentPoints();

			p0 = currentPoints.at(0);
			p1 = currentPoints.at(1);
			p2 = currentPoints.at(2);
			p3 = currentPoints.at(3);

			return Math::bezier(p0, p1, p2, p3, m_pKeyLocalTime);
			
			break;
		default:
			break;
		}

		return T();
	}

	void addKey(T key)
	{
		m_pKeys.push_back(key);
		connected.push_back(true);
	}

	void setKey(unsigned int idx, T key)
	{
		if (idx >= m_pKeys.size())
			return;
		m_pKeys[idx] = key;
	}

	void setConnectedKey(unsigned int idx, bool isConnected)
	{
		if (idx >= m_pKeys.size())
			return;
		connected[idx] = isConnected;
	}

	float curvePlaySpeed;
	bool paused;
	bool loops;
	bool doesSpeedControl;
	bool connectedEndpoints;
};