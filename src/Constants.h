//============================================================================
// Name        : Constants.h
// Author      : Amrit Dhakal
// Version     : 1.0
// Copyright   : © Amrit Dhakal 2015. All right reserved. :D
//============================================================================
#ifndef CONSTANTS_H_
#define CONSTANTS_H_

//Game Constants
const int MAX_LEVEL = 10;

//SceneQueries
const Ogre::uint32 REMOVEABLE = 4;

// Field Dimension
const int BOARD_WIDTH = 500;
const int BOARD_HEIGHT = BOARD_WIDTH;

//Level Information

const int LEVEL_DIM[] = {0,10,10,12,12,14,14,16,16,20,20};
const int NUM_MINES[] = {0,15,20,21,26,30,35,39,46,80,100};
const int MAX_BONUS_TIME[] = {0,20,30,50,80,120,150,190,210,270,360};
const int POINTS_PER_REVEAL[] = {0,1,2,3,4,5,7,9,12,18,30};

//HighScore
const std::string HIGHSCORE_FILE = ".highScores";

// Mines Around Number Mesh
//const Ogre::String MINES_AROUND_MESH[] = {"","Mesh.mesh","", "Text.006_Text.011.mesh","Text.mesh","Text.003_Text.008.mesh","Text.004_Text.009.mesh", "Text.007_Text.012.mesh", "Mesh.007.mesh"};


//Mine Detector
const int DETECTOR_MAX_X = BOARD_WIDTH/2;
const int DETECTOR_MAX_Z = DETECTOR_MAX_X;
const int DETECTOR_Y[] = {0,-6,-6,-12,-12,-16,-16,-19,-19,-23,-23};
const Ogre::Vector3 DETECTOR_INITIAL_POSITION = Ogre::Vector3(0,DETECTOR_Y[1],200);
const Ogre::Vector3 DETECTOR_HEAD_POSITION = Ogre::Vector3(-7, -1, 1);


const Ogre::Vector3 GRAVITY = Ogre::Vector3(0, -9.81, 0);

const int CAMERA_SPEED = 200;


#endif /* CONSTANTS_H_ */
