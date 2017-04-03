#pragma once
#include "PathNode.h"
#include "Player.h"

class AStar
{
public:
	void findPath(std::shared_ptr<Player> player)
	{
		openList.clear();
		closedList.clear();
		pathToWalk.clear();
		nextPath = nullptr;

		//Target: sixfour
		openList["start"] = player->checkNodes(aStarPointer);
		//addToList(openList, openList["start"]->connections);


		while (openList.size() > 0)
		{
			for (auto itr = openList.begin(); itr != openList.end(); itr++)
			{
				if (nextPath == nullptr && itr->second->dummy == false)
				{
					nextPath = itr->second;
				}
				if (itr->second != nullptr && nextPath != nullptr && (nextPath->distanceToTarget > itr->second->distanceToTarget) && itr->second->dummy == false)
				{
					nextPath = itr->second;
				}
			}

			addToList(openList, nextPath->connections);

			for (auto itr = openList.begin(); itr != openList.end(); itr++)
			{
				if (itr->second == nextPath)
				{
					if (openList.size() > 1)
					{
						openList.erase(itr);
						break;
					}
				}
			}
			nodeNum++;
			closedList[nodeName] = nextPath;
			nodeName = ("node" + std::to_string(nodeNum));

			if (nextPath->distanceToTarget == 0)
			{
				endNode = nextPath;
				openList.clear();
			}

			nextPath = nullptr;
		}

		nextPath = nullptr;

		//auto end = closedList.begin();
		//if (endNode == end->second)
		//{
		//	end++;
		//	nextPath = end->second;
		//}
		//else
		nextPath = closedList.begin()->second;

		nodeNum++;
		nodeName = ("node" + std::to_string(nodeNum));
		pathToWalk[nodeName] = nextPath;

		//nodeNum++;
		//nodeName = ("node" + std::to_string(nodeNum));
		//pathToWalk[nodeName] = endNode;

		while (nextPath->distanceToTarget != 0)
		{
			auto connect = nextPath->connections.begin();

			if (connect != nextPath->connections.end())
			{
				for (int i = 0; i < 4; i++)
				{
					for (auto closed = closedList.begin(); closed != closedList.end(); closed++)
					{
						if (connect->second == closed->second)
						{
							isconnected = true;
						}
						if (isconnected && (nextPath->distanceToTarget >= closed->second->distanceToTarget) && (nextPath->pos != closed->second->pos))
						{
							nextPath = closed->second;

							nodeNum++;
							nodeName = ("node" + std::to_string(nodeNum));
							pathToWalk[nodeName] = nextPath;
							isconnected = false;
							closedList.erase(closed);

							break;
						}
					}
					connect++;
				}
			}

		}

		nodeNum++;
	}

	void traversePath(std::shared_ptr<Player> player, float dT)
	{
		static auto pathItr = pathToWalk.begin();
		player->setAnim("walk");
		if (newTraverse)
		{
			pathItr = pathToWalk.begin();
			x = player->getWorldPosition().x;
			z = player->getWorldPosition().z;
			newTraverse = false;
			lerptime = 0;
		}

		//if (!traverse)
		//	pathItr = pathToWalk.begin();
		if (!newTraverse && pathItr != pathToWalk.end())
		{
			if (lerptime >= 1.0f && pathItr != pathToWalk.end())
			{
				x = pathItr->second->pos.x;
				z = pathItr->second->pos.z;
				pathItr++;
				lerptime = 0.0f;
			}

			// Player to start node
			if (pathItr != pathToWalk.end())
			{
				float posX = glm::mix(x, pathItr->second->pos.x, lerptime);
				float posZ = glm::mix(z, pathItr->second->pos.z, lerptime);

				player->setPosition(glm::vec3(posX, player->getWorldPosition().y, posZ));
				player->setRotationAngleY(glm::atan(posX / posZ));
				//player->setRotationAngleY(0.5f);
			}

			lerptime += dT;
		}
		else if (pathItr == pathToWalk.end())
		{
			traverse = false;
			newTraverse = true;
		}
	}

	void initializePathNodes()
	{
		//PathNode * end = new PathNode(glm::vec3(23.0f, 40.0f, 1.0f));

		//dummies
		PathNode * dummy1 = new PathNode(glm::vec3(500.0f, 40.0f, -300.0f), 5, true);
		PathNode * dummy2 = new PathNode(glm::vec3(500.0f, 40.0f, -300.0f), 4, true);
		PathNode * dummy3 = new PathNode(glm::vec3(500.0f, 40.0f, -300.0f), 5, true);
		PathNode * dummy4 = new PathNode(glm::vec3(500.0f, 40.0f, -300.0f), 5, true);
		PathNode * dummy5 = new PathNode(glm::vec3(500.0f, 40.0f, -300.0f), 1, true);
		PathNode * dummy6 = new PathNode(glm::vec3(500.0f, 40.0f, -300.0f), 3, true);
		PathNode * dummy7 = new PathNode(glm::vec3(500.0f, 40.0f, -300.0f), 2, true);

		//first row
		PathNode * onethree = new PathNode(glm::vec3(5.0f, 40.0f, -30.0f), 8);
		PathNode * onefour = new PathNode(glm::vec3(15.0f, 40.0f, -30.0f), 7);
		PathNode * onefive = new PathNode(glm::vec3(25.0f, 40.0f, -30.0f), 6);
		PathNode * onesix = new PathNode(glm::vec3(35.0f, 40.0f, -30.0f), 7);

		//second row
		PathNode * twoone = new PathNode(glm::vec3(-15.0f, 40.0f, -19.0f), 8);
		PathNode * twotwo = new PathNode(glm::vec3(-5.0f, 40.0f, -19.0f), 7);
		PathNode * twothree = new PathNode(glm::vec3(5.0f, 40.0f, -19.0f), 6);
		//dummy1
		//dummy2
		PathNode * twosix = new PathNode(glm::vec3(35.0f, 40.0f, -19.0f), 5);
		PathNode * twoseven = new PathNode(glm::vec3(45.0f, 40.0f, -19.0f), 6);
		PathNode * twoeight = new PathNode(glm::vec3(55.0f, 40.0f, -19.0f), 7);

		//third row
		PathNode * threeone = new PathNode(glm::vec3(-15.0f, 40.0f, -10.0f), 7);
		PathNode * threetwo = new PathNode(glm::vec3(-5.0f, 40.0f, -10.0f), 6);
		PathNode * threethree = new PathNode(glm::vec3(5.0f, 40.0f, -10.0f), 5);
		PathNode * threefour = new PathNode(glm::vec3(15.0f, 40.0f, -10.0f), 4);
		PathNode * threefive = new PathNode(glm::vec3(25.0f, 40.0f, -10.0f), 3);
		PathNode * threesix = new PathNode(glm::vec3(35.0f, 40.0f, -10.0f), 4);
		//dummy3
		PathNode * threeeight = new PathNode(glm::vec3(55.0f, 40.0f, -10.0f), 6);

		//fourth row
		PathNode * fourone = new PathNode(glm::vec3(-15.0f, 40.0f, 0.0f), 6);
		//dummy4
		PathNode * fourthree = new PathNode(glm::vec3(5.0f, 40.0f, 0.0f), 4);
		PathNode * fourfour = new PathNode(glm::vec3(15.0f, 40.0f, 0.0f), 3);
		PathNode * fourfive = new PathNode(glm::vec3(25.0f, 40.0f, 0.0f), 2);
		PathNode * foursix = new PathNode(glm::vec3(35.0f, 40.0f, 0.0f), 3);
		PathNode * fourseven = new PathNode(glm::vec3(45.0f, 40.0f, 0.0f), 4);
		PathNode * foureight = new PathNode(glm::vec3(55.0f, 40.0f, 0.0f), 5);

		//fifth row
		PathNode * fiveone = new PathNode(glm::vec3(-15.0f, 40.0f, 10.0f), 5);
		PathNode * fivetwo = new PathNode(glm::vec3(-5.0f, 40.0f, 10.0f), 4);
		PathNode * fivethree = new PathNode(glm::vec3(5.0f, 40.0f, 10.0f), 3);
		PathNode * fivefour = new PathNode(glm::vec3(15.0f, 40.0f, 10.0f), 2);
		//dummy5
		PathNode * fivesix = new PathNode(glm::vec3(35.0f, 40.0f, 10.0f), 2);
		PathNode * fiveseven = new PathNode(glm::vec3(45.0f, 40.0f, 10.0f), 3);
		PathNode * fiveeight = new PathNode(glm::vec3(55.0f, 40.0f, 10.0f), 4);

		//sixth row
		PathNode * sixone = new PathNode(glm::vec3(-15.0f, 40.0f, 20.0f), 5);
		//dummy6
		PathNode * sixthree = new PathNode(glm::vec3(5.0f, 40.0f, 20.0f), 2);
		PathNode * sixfour = new PathNode(glm::vec3(15.0f, 40.0f, 20.0f), 1);
		PathNode * sixfive = new PathNode(glm::vec3(20.0f, 43.0f, 26.0f), 0);
		PathNode * sixsix = new PathNode(glm::vec3(35.0f, 40.0f, 20.0f), 1);
		//dummy7
		PathNode * sixeight = new PathNode(glm::vec3(55.0f, 40.0f, 20.0f), 4);

		//seventh row
		PathNode * sevenone = new PathNode(glm::vec3(-15.0f, 40.0f, 30.0f), 5);
		PathNode * seventwo = new PathNode(glm::vec3(-5.0f, 40.0f, 30.0f), 4);
		PathNode * seventhree = new PathNode(glm::vec3(5.0f, 40.0f, 30.0f), 3);
		PathNode * sevenfour = new PathNode(glm::vec3(15.0f, 40.0f, 30.0f), 2);
		PathNode * sevenfive = new PathNode(glm::vec3(25.0f, 40.0f, 30.0f), 1);
		PathNode * sevensix = new PathNode(glm::vec3(35.0f, 40.0f, 30.0f), 2);
		PathNode * sevenseven = new PathNode(glm::vec3(45.0f, 40.0f, 30.0f), 3);
		PathNode * seveneight = new PathNode(glm::vec3(55.0f, 40.0f, 30.0f), 4);

		//aStar["EndNode"]		= end;

		//Dummies
		aStar["dummy1"] = dummy1;
		aStar["dummy2"] = dummy2;
		aStar["dummy3"] = dummy3;
		aStar["dummy4"] = dummy4;
		aStar["dummy5"] = dummy5;
		aStar["dummy6"] = dummy6;
		aStar["dummy7"] = dummy7;

		//first row
		aStar["onethree"] = onethree;
		aStar["onefour"] = onefour;
		aStar["onefive"] = onefive;
		aStar["onesix"] = onesix;

		//second row
		aStar["twoone"] = twoone;
		aStar["twotwo"] = twotwo;
		aStar["twothree"] = twothree;
		aStar["twosix"] = twosix;
		aStar["twoseven"] = twoseven;
		aStar["twoeight"] = twoeight;

		//third row
		aStar["threeone"] = threeone;
		aStar["threetwo"] = threetwo;
		aStar["threethree"] = threethree;
		aStar["threefour"] = threefour;
		aStar["threefive"] = threefive;
		aStar["threesix"] = threesix;
		aStar["threeeight"] = threeeight;

		//fourth row
		aStar["fourone"] = fourone;
		aStar["fourthree"] = fourthree;
		aStar["fourfour"] = fourfour;
		aStar["fourfive"] = fourfive;
		aStar["foursix"] = foursix;
		aStar["fourseven"] = fourseven;
		aStar["foureight"] = foureight;

		//fifth row
		aStar["fiveone"] = fiveone;
		aStar["fivetwo"] = fivetwo;
		aStar["fivethree"] = fivethree;
		aStar["fivefour"] = fivefour;
		aStar["fivesix"] = fivesix;
		aStar["fiveseven"] = fiveseven;
		aStar["fiveeight"] = fiveeight;

		//sixth row
		aStar["sixone"] = sixone;
		aStar["sixthree"] = sixthree;
		aStar["sixfour"] = sixfour;
		aStar["sixfive"] = sixfive;
		aStar["sixsix"] = sixsix;
		aStar["sixeight"] = sixeight;

		//seventh row
		aStar["sevenone"] = sevenone;
		aStar["seventwo"] = seventwo;
		aStar["seventhree"] = seventhree;
		aStar["sevenfour"] = sevenfour;
		aStar["sevenfive"] = sevenfive;
		aStar["sevensix"] = sevensix;
		aStar["sevenseven"] = sevenseven;
		aStar["seveneight"] = seveneight;

		// Init Connections
		// Row One
		aStar["onethree"]->addConnection(nullptr, aStar["onefour"], aStar["twothree"], nullptr);
		aStar["onefour"]->addConnection(nullptr, aStar["onefive"], aStar["dummy1"], aStar["onethree"]);
		aStar["onefive"]->addConnection(nullptr, aStar["onesix"], aStar["dummy2"], aStar["onefour"]);
		aStar["onesix"]->addConnection(nullptr, nullptr, aStar["twosix"], aStar["onefive"]);

		// Row Two
		aStar["twoone"]->addConnection(nullptr, aStar["twotwo"], aStar["threeone"], nullptr);
		aStar["twotwo"]->addConnection(nullptr, aStar["twothree"], aStar["threetwo"], aStar["twoone"]);
		aStar["twothree"]->addConnection(aStar["onethree"], aStar["dummy1"], aStar["threethree"], aStar["twotwo"]);
		aStar["dummy1"]->addConnection(aStar["onefour"], aStar["dummy2"], aStar["threefour"], aStar["twothree"]);
		aStar["dummy2"]->addConnection(aStar["onefive"], aStar["twosix"], aStar["threefive"], aStar["dummy1"]);
		aStar["twosix"]->addConnection(aStar["onesix"], aStar["twoseven"], aStar["threesix"], aStar["dummy2"]);
		aStar["twoseven"]->addConnection(nullptr, aStar["twoeight"], aStar["dummy3"], aStar["twosix"]);
		aStar["twoeight"]->addConnection(nullptr, nullptr, aStar["threeeight"], aStar["twoseven"]);

		// Row Three
		aStar["threeone"]->addConnection(aStar["twoone"], aStar["threetwo"], aStar["fourone"], nullptr);
		aStar["threetwo"]->addConnection(aStar["twotwo"], aStar["threethree"], aStar["dummy4"], aStar["threeone"]);
		aStar["threethree"]->addConnection(aStar["twothree"], aStar["threefour"], aStar["fourthree"], aStar["threetwo"]);
		aStar["threefour"]->addConnection(aStar["dummy1"], aStar["threefive"], aStar["fourfour"], aStar["threethree"]);
		aStar["threefive"]->addConnection(aStar["dummy2"], aStar["threesix"], aStar["fourfive"], aStar["threefour"]);
		aStar["threesix"]->addConnection(aStar["twosix"], aStar["dummy3"], aStar["foursix"], aStar["threefive"]);
		aStar["dummy3"]->addConnection(aStar["twoseven"], aStar["threeeight"], aStar["fourseven"], aStar["threesix"]);
		aStar["threeeight"]->addConnection(aStar["twoeight"], nullptr, aStar["foureight"], aStar["dummy3"]);

		// Row Four
		aStar["fourone"]->addConnection(aStar["threeone"], aStar["dummy4"], aStar["fiveone"], nullptr);
		aStar["dummy4"]->addConnection(aStar["threetwo"], aStar["fourthree"], aStar["fivetwo"], aStar["fourone"]);
		aStar["fourthree"]->addConnection(aStar["threethree"], aStar["fourfour"], aStar["fivethree"], aStar["dummy4"]);
		aStar["fourfour"]->addConnection(aStar["threefour"], aStar["fourfive"], aStar["fivefour"], aStar["fourthree"]);
		aStar["fourfive"]->addConnection(aStar["threefive"], aStar["foursix"], aStar["dummy5"], aStar["fourfour"]);
		aStar["foursix"]->addConnection(aStar["threesix"], aStar["fourseven"], aStar["fivesix"], aStar["fourfive"]);
		aStar["fourseven"]->addConnection(aStar["dummy3"], aStar["foureight"], aStar["fiveseven"], aStar["foursix"]);
		aStar["foureight"]->addConnection(aStar["threeeight"], nullptr, aStar["fiveeight"], aStar["fourseven"]);

		// Row Five
		aStar["fiveone"]->addConnection(aStar["fourone"], aStar["fivetwo"], aStar["sixone"], nullptr);
		aStar["fivetwo"]->addConnection(aStar["dummy4"], aStar["fivethree"], aStar["dummy6"], aStar["fiveone"]);
		aStar["fivethree"]->addConnection(aStar["fourthree"], aStar["fivefour"], aStar["sixthree"], aStar["fivetwo"]);
		aStar["fivefour"]->addConnection(aStar["fourfour"], aStar["dummy5"], aStar["sixfour"], aStar["fivethree"]);
		aStar["dummy5"]->addConnection(aStar["fourfive"], aStar["fivesix"], aStar["sixfive"], aStar["fivefour"]);
		aStar["fivesix"]->addConnection(aStar["foursix"], aStar["fiveseven"], aStar["sixsix"], aStar["dummy5"]);
		aStar["fiveseven"]->addConnection(aStar["fourseven"], aStar["fiveeight"], aStar["dummy7"], aStar["fivesix"]);
		aStar["fiveeight"]->addConnection(aStar["foureight"], nullptr, aStar["sixeight"], aStar["fiveseven"]);

		// Row Six
		aStar["sixone"]->addConnection(aStar["fiveone"], aStar["dummy6"], aStar["sevenone"], nullptr);
		aStar["dummy6"]->addConnection(aStar["fivetwo"], aStar["sixthree"], aStar["seventwo"], aStar["sixone"]);
		aStar["sixthree"]->addConnection(aStar["fivethree"], aStar["sixfour"], aStar["seventhree"], aStar["dummy6"]);
		aStar["sixfour"]->addConnection(aStar["fivefour"], aStar["sixfive"], aStar["sevenfour"], aStar["sixthree"]);
		aStar["sixfive"]->addConnection(aStar["dummy5"], aStar["sixsix"], aStar["sevenfive"], aStar["sixfour"]);
		aStar["sixsix"]->addConnection(aStar["fivesix"], aStar["dummy7"], aStar["sevensix"], aStar["sixfive"]);
		aStar["dummy7"]->addConnection(aStar["fiveseven"], aStar["sixeight"], aStar["sevenseven"], aStar["sixsix"]);
		aStar["sixeight"]->addConnection(aStar["fiveeight"], nullptr, aStar["seveneight"], aStar["dummy7"]);

		// Row seven
		aStar["sevenone"]->addConnection(aStar["sixone"], aStar["seventwo"], nullptr, nullptr);
		aStar["seventwo"]->addConnection(aStar["dummy6"], aStar["seventhree"], nullptr, aStar["sevenone"]);
		aStar["seventhree"]->addConnection(aStar["sixthree"], aStar["sevenfour"], nullptr, aStar["seventwo"]);
		aStar["sevenfour"]->addConnection(aStar["sixfour"], aStar["sevenfive"], nullptr, aStar["seventhree"]);
		aStar["sevenfive"]->addConnection(aStar["sixfive"], aStar["sevensix"], nullptr, aStar["sevenfour"]);
		aStar["sevensix"]->addConnection(aStar["sixsix"], aStar["sevenseven"], nullptr, aStar["sevenfive"]);
		aStar["sevenseven"]->addConnection(aStar["dummy7"], aStar["seveneight"], nullptr, aStar["sevensix"]);
		aStar["seveneight"]->addConnection(aStar["sixeight"], nullptr, nullptr, aStar["sevenseven"]);

		aStarPointer = &aStar;

		std::cout << "Nodes loaded and connected" << std::endl;
	}

	bool traverse;
	bool newTraverse = false;

private:
	std::string nodeName = "node";
	int nodeNum = 0;
	int addNum = 0;
	bool dupe = false;
	bool isconnected = false;
	
	float x;
	float z;

	// A* nodes
	std::map<std::string, PathNode *> aStar;
	std::map<std::string, PathNode *> openList;
	std::map<std::string, PathNode *> emptyList;
	std::map<std::string, PathNode *> closedList;
	std::map<std::string, PathNode *> pathToWalk;
	std::map<std::string, PathNode *> * aStarPointer;
	PathNode * nextPath;
	PathNode * endNode;

	template <typename T>
	T lerp(T d0, T d1, float t)
	{
		return d0 * (1 - t) + d1 * t;
	}

	float lerptime = 0.0f;

	void addToList(std::map<std::string, PathNode *> list, std::map<std::string, PathNode *> addingList)
		{
			for (auto itr = addingList.begin(); itr != addingList.end(); itr++)
			{

				for (auto closed = closedList.begin(); closed != closedList.end(); closed++)
				{
					if (itr->second == nullptr || itr->second == closed->second)
					{
						dupe = true;
					}
				}
				if (!dupe && itr->second != nullptr)
				{
					openList[nodeName] = itr->second;
				}
				nodeNum++;
				nodeName = ("node" + std::to_string(nodeNum));

				dupe = false;
			}
		}
};



