#include "interactions.hpp"
#include <iostream>

using namespace glm;
using namespace std;

Interactions::Interactions(glm::vec3 &eyeVec, 
	float &adjustMovementSpeedX, 
	float &adjustMovementSpeedZ, 
	float &maxJumpHeight,
	bool &popup,
	float &playerWidth, 
	float &playerHeight,
	std::vector<std::string> &inventory,
	std::vector<SceneNode *> &nodeVector,
	string &message
	) 
: 
	eyeVec(eyeVec), 
	adjustMovementSpeedX(adjustMovementSpeedX),
	adjustMovementSpeedZ(adjustMovementSpeedZ),
	maxJumpHeight(maxJumpHeight),
	popup(popup),
	playerWidth(playerWidth),
	playerHeight(playerHeight),
	inventory(inventory),
	nodeVector(nodeVector),
	message(message)
{

}


void Interactions::invokeAction( SceneNode *n ) {

	if ( n->getName() == "cup" ) {						// shrink
		eyeVec = vec3(eyeVec.x, eyeVec.y - 15.0, eyeVec.z);
		adjustMovementSpeedX = 0.6 * adjustMovementSpeedX;		// scale movement values
		adjustMovementSpeedZ = 0.5 * adjustMovementSpeedZ;		// scale movement values
		maxJumpHeight = 0.5 * maxJumpHeight;			// scale jump height
		n->interactive = false;							// can only interact once with cup
		playerHeight = playerHeight - 15.0;				// scale player height
		playerWidth = (playerHeight * 0.25);				// scale player width
		message = "Glug... gl-Ah! What's that beside you! Wait, was everything always this big?";
		enableStairVisibility();

	} else if ( n->getName() == "drinkMe" ) {
		popup = true;
		message = "I think it says... 'drink me'?";
		cout << "drinkMe" << endl;

	} else if ( n->getName() == "doorKnob" ) {
		if (checkForItem("key")) {
			message = "Great! The key fits!";
			cout << "unlocked!" << endl;
		} else {
			message = "Hmm... I think it's locked. Let's go look for a key.";
			cout << "locked" << endl;
		}

	} else if ( n->getName() == "key" ) {
		n->visibility = false;
		n->interactive = false;
		inventory.push_back(n->getName());
		message = "This key will probably come in handy. Guess I'll take it with me. ";
		cout << "grabbed key" << endl;

	} else if ( n->getName() == "queenOfHeartsPainting" ) {
		message = "I wonder who she is...?";
	} else if ( n->getName() == "case1" ) {
		message = "Cupcakes? I love cupcakes!";
	} else if ( n->getName() == "case2" ) {
		message = "What pretty cups! I feel like having some tea...";
	} else if ( n->getName() == "case3" ) {
		message = "Ah! Now THAT's a creepy photo to have hanging around.";
	} else if ( n->getName().substr(0, 5) == "stair" ) {
		message = "Were these stairs always here?";
	}
}

void Interactions::enableStairVisibility() {
	for (auto node : nodeVector) {
		if (node->m_name.substr(0, 5) == "stair") {
			node->visibility = true;
		}
	}
}

bool Interactions::checkForItem(string item) {
	for (auto i : inventory) {
		if (item == i) {
			return true;
		}
	}
	return false;
}