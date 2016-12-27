//============================================================================
// Name        : MineSweeper.cpp
// Author      : Amrit Dhakal
// Version     : 1.0
// Copyright   : © Amrit Dhakal 2015. All right reserved. :D
//============================================================================
#include "MineSweeper.h"
#include "Constants.h"
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include "Cell.h"
#include "tinyxml.h"
#include "HighScores.h"
#include <vector>
#include <Shapes/OgreBulletCollisionsBoxShape.h>
#include <Shapes/OgreBulletCollisionsSphereShape.h>

using namespace Ogre;


const int MineSweeper::g = 9.81;


/**
 * Converts OIS::MouseButtonID to CEGUI::MouseButton
 */
CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
{
	switch (buttonID)
	{
	case OIS::MB_Left:
		return CEGUI::LeftButton;

	case OIS::MB_Right:
		return CEGUI::RightButton;

	case OIS::MB_Middle:
		return CEGUI::MiddleButton;

	default:
		return CEGUI::LeftButton;
	}
}

//-------------------------------------------------------------------------------------
//---------------------------------------------------------------------------
MineSweeper::MineSweeper(void)
{
	mStop = true;
	mRaySceneQuery = 0;
	mCurTime = 0;
	mCurNumRevealed = 0;
	mLevel = 1;
	mScore = 0;
	mDetector = 0;
	mDim = LEVEL_DIM[mLevel];
	mGameOverTime = 0;
	mCameraDirection = Ogre::Vector3::ZERO;

}
//---------------------------------------------------------------------------
MineSweeper::~MineSweeper(void)
{

}
//-------------------------------------------------------------------------------------
void MineSweeper::showButtons(bool val){

	mGuiRoot->getChild("ResumeButton")->setVisible(val);
	mGuiRoot->getChild("RestartButton")->setVisible(val);
	mGuiRoot->getChild("HighScoresButton")->setVisible(val);

}
//-------------------------------------------------------------------------------------
bool MineSweeper::quit(const CEGUI::EventArgs &e)
{
	mShutDown = true;
	return true;
}
//-------------------------------------------------------------------------------------
bool MineSweeper::showHighScores(const CEGUI::EventArgs &e)
{
	mScoreBox->resetList();
	for(int i = 0; i < mHighScore->totalScores(); i++) {
		CEGUI::String name;
		CEGUI::String score;
		CEGUI::String rank;
		if(i % 2 == 0){
			rank = "[font='HighScoreFont-18.font'][colour='FFFF0000']" + std::to_string(i+1);
			name = "[font='HighScoreFont-18.font'][colour='FFFF0000']" + mHighScore->getName(i);
			score = "[font='HighScoreFont-18.font'][colour='FFFF0000']" + std::to_string(mHighScore->getScore(i));
		}
		else{
			rank = "[font='HighScoreFont-18.font'][colour='FF00FF00']" + std::to_string(i+1);
			name = "[font='HighScoreFont-18.font'][colour='FF00FF00']" + mHighScore->getName(i);
			score = "[font='HighScoreFont-18.font'][colour='FF00FF00']" + std::to_string(mHighScore->getScore(i));
		}
		int rowID = mScoreBox->addRow();	//Create new row and get row id
		CEGUI::ListboxTextItem* pRank= new CEGUI::ListboxTextItem(rank);
		CEGUI::ListboxTextItem* pName= new CEGUI::ListboxTextItem(name);
		pName->setSelectionBrushImage("GlossySerpentFHD/MultiListSelectionBrush");
		CEGUI::ListboxTextItem* pScore= new CEGUI::ListboxTextItem(score);
		// Add row to all the columns using row id received above
		mScoreBox->setItem(pRank, 0, rowID);
		mScoreBox->setItem(pName, 1, rowID);
		mScoreBox->setItem(pScore, 2, rowID);
	}
	mGuiRoot->getChild("MessageLabel")->setVisible(false);
	mGuiRoot->getChild("ScoreWindow")->setVisible(true);
	return true;
}
//-------------------------------------------------------------------------------------
bool MineSweeper::resetHighScores(const CEGUI::EventArgs &e)
{
	mScoreBox->resetList();
	mHighScore->clearScores();
	mHighScore->writeToFile(HIGHSCORE_FILE);
	mScorePosition = -1;
	if(mScore != 0){
		updateHighScores();
	}
	return true;
}
//-------------------------------------------------------------------------------------
bool MineSweeper::hideHighScores(const CEGUI::EventArgs &e)
{
	mGuiRoot->getChild("ScoreWindow")->setVisible(false);
	mGuiRoot->getChild("MessageLabel")->setVisible(true);
	return true;
}
//-------------------------------------------------------------------------------------
bool MineSweeper::restart(const CEGUI::EventArgs &e)
{
	if(!mGuiRoot->getChild("ScoreWindow")->isVisible()){
		mGameOver = false;
		mGameOverTime = 0;
		mCurTime = 0;
		mLevel =1;
		mDim = LEVEL_DIM[mLevel];
		mGuiRoot->getChild("GameOverWindow")->setVisible(false);
		showButtons(false);
		mStop = true;
		mPause = true;
		clearCells();
		createField();
		mInitialized = false;
		mScore = 0;
		mCurNumRevealed = 0;
		mScorePosition = -1;
		mGuiRoot->getChild("LevelUpWindow")->setVisible(false);
		mGuiRoot->getChild("ScoreValue")->setText(CEGUI::String(std::to_string(mScore)));
		mGuiRoot->getChild("LevelValue")->setText(std::to_string(mLevel));
		mGuiRoot->getChild("NameWindow")->setVisible(true);
		mGuiRoot->getChild("MessageLabel")->setText("Please Enter your name.");
		mGuiRoot->getChild("NameWindow")->getChild("NameBox")->activate();
		delete mWorld;
		mWorld = new OgreBulletDynamics::DynamicsWorld(mSceneMgr, m_bounds,
				GRAVITY);
	}
	return true;
}
//-------------------------------------------------------------------------------------
bool MineSweeper::resume(const CEGUI::EventArgs& e) {
	if(!mGuiRoot->getChild("ScoreWindow")->isVisible()){
		mPause = false;
		showButtons(false);
		mGuiRoot->getChild("MessageLabel")->setText("Press P to pause.");
	}
	return true;
}
//-------------------------------------------------------------------------------------
bool MineSweeper::nameEntered(const CEGUI::EventArgs& e) {
	Ogre::String pName(mGuiRoot->getChild("NameWindow")->getChild("NameBox")->getText().c_str());
	Ogre::StringUtil::trim(pName);	//trim white spaces
	if(pName != ""){	//Reject blank names or names with only whitespaces
		mPlayerName = pName;
		mGuiRoot->getChild("NameValue")->setText(pName);
		mGuiRoot->getChild("NameWindow")->setVisible(false);
		mGuiRoot->getChild("ResumeButton")->setVisible(false);
		mGuiRoot->getChild("MessageLabel")->setText("Click on a cell to start the game. Press P to Pause and access Buttons.");
		mStop = false;
		mPause = false;
	}
	return true;
}
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
void MineSweeper::updateHighScores(){
	if(mScorePosition == -1){
		int scorePos = mHighScore->getPlace(mScore);
		if(scorePos != -1){
			mScorePosition = mHighScore->getPlace(mScore);
			mHighScore->addScore(mPlayerName.c_str(), mScore);
		}
	}
	else{
		mScorePosition = mHighScore->updateScore(mScorePosition, mPlayerName.c_str(), mScore);
		mHighScore->writeToFile(HIGHSCORE_FILE);
	}

}
//-------------------------------------------------------------------------------------
void MineSweeper::clearCells(){
	for(int i = 0; i < mBodies.size(); i++){
		delete mBodies[i];
	}

	for(int i = 0; i < mBodies.size(); i++){
		mBodies.erase(mBodies.begin());
	}
	mBodies.clear();

	for(int i = 0; i < mShapes.size(); i++){
		delete mShapes[i];

	}
	for(int i = 0; i < mShapes.size(); i++){
		mShapes.erase(mShapes.begin());
	}


	mShapes.clear();

	for(int i = 0; i < mCells.size(); i++){
		mCells[i]->removeFromScene();
	}

	for(int i = 0; i < mCells.size(); i++){
		mCells.erase(mCells.begin());
	}
	mCells.clear();

}

int MineSweeper::countFlags(){
	int count = 0;
	for(int i = 0; i < mCells.size(); ++i){
		if(mCells[i]->isFlagged()){
			++count;
		}
	}
	return count;
}

int MineSweeper::getNumRevealed(int prevReveal){
	int count = 0;
	for(int i = 0; i < mCells.size(); ++i){
		if(mCells[i]->isRevelead()){
			++count;
		}
	}
	return count - prevReveal;
}

bool MineSweeper::frameRenderingQueued(const Ogre::FrameEvent& evt) {
	mTrayMgr->hideAll();
	updateGUI();
	mTimeSinceLastFrame = evt.timeSinceLastFrame;
	mCamera->setPosition(mCamera->getPosition() + CAMERA_SPEED*evt.timeSinceLastFrame*mCameraDirection);
	if(mInitialized){
		mCurTime += evt.timeSinceLastFrame;
	}
	bool result = BaseApplication::frameRenderingQueued(evt);

	if(mGameOver && ((mCurTime - mGameOverTime ) > 2)){
		mGuiRoot->getChild("GameOverWindow")->getChild("GameOverPrompt")->setText(
				"         Game Over !!! You clicked on a mine."  );
		mGuiRoot->getChild("MessageLabel")->setText(
				"         Game Over !!! You clicked on a mine.");
		mGuiRoot->getChild("GameOverWindow")->setVisible(true);
		setupPhysicsObjects();
		mWorld->stepSimulation(evt.timeSinceLastFrame);
	}
	return result;
}




int getIndex(int dim, int r, int c){
	return dim*c + r;
}

void MineSweeper::revealAll(){
	for (int i = 0; i < mCells.size(); ++i){
		if(mCells[i]->isMine()){
			mCells[i]->reveal(false, false);
		}
	}


}

void MineSweeper::setupPhysicsObjects(){

	if(mPhysicsInitialized){
		return;
	}

	for (int i = 0; i < mCells.size(); ++i){
		Cell* curCell = mCells[i];
		OgreBulletCollisions::BoxCollisionShape * cubeShape;
		OgreBulletDynamics::RigidBody * cubeBody;

		Vector3 size = curCell->getSceneNode()->_getWorldAABB().getSize()/2;
		Vector3 position = curCell->getPosition();
		cubeShape =	new OgreBulletCollisions::BoxCollisionShape(size);
		cubeBody =
				new OgreBulletDynamics::RigidBody(
						"cube" + Ogre::StringConverter::toString(time(0))
		+  Ogre::StringConverter::toString(i),
		mWorld
				);

		SceneNode* node = curCell->getSceneNode();



		cubeBody->setShape(node, //scene node
				cubeShape, //collision shape
				0.6f, //restitution
				0.6f, //friction
				1.0f, //mass
				position, //location of the shape
				Ogre::Quaternion(0, 0, 0, 1) //orientation
		);

		int x = Ogre::Math::RangeRandom(-1000, 1000);
		int y = Ogre::Math::RangeRandom(-1000, 1000);
		int z = Ogre::Math::RangeRandom(-1000, 1000);


		cubeBody->setLinearVelocity(Ogre::Vector3(x, y, z));
		if(curCell->getMineNode() != 0){
			size = curCell->getMineNode()->_getWorldAABB().getSize()/2;
			position = curCell->getMineNode()->getPosition();


			cubeShape =	new OgreBulletCollisions::BoxCollisionShape(size);

			cubeBody =
					new OgreBulletDynamics::RigidBody(
							"mine" + Ogre::StringConverter::toString(time(0))
			+  Ogre::StringConverter::toString(i),
			mWorld
					);

			node = curCell->getMineNode();



			cubeBody->setShape(node, //scene node
					cubeShape, //collision shape
					0.6f, //restitution
					0.6f, //friction
					1.0f, //mass
					position, //location of the shape
					Ogre::Quaternion(0, 0, 0, 1) //orientation
			);

			int x = Ogre::Math::RangeRandom(-1000, 1000);
			int y = Ogre::Math::RangeRandom(-1000, 1000);
			int z = Ogre::Math::RangeRandom(-1000, 1000);

			cubeBody->setLinearVelocity(Ogre::Vector3(x, y, z));
			cubeBody->applyImpulse(Ogre::Vector3(900, 900, 900),Ogre::Vector3(900, 900, 900));
			mBodies.push_back(cubeBody);
			mShapes.push_back(cubeShape);
		}

		if(curCell->getFlagNode() != 0){
			size = curCell->getFlagNode()->_getWorldAABB().getSize()/2;
			position = curCell->getFlagNode()->getPosition();


			cubeShape =	new OgreBulletCollisions::BoxCollisionShape(size);

			cubeBody =
					new OgreBulletDynamics::RigidBody(
							"flag" + Ogre::StringConverter::toString(time(0))
			+  Ogre::StringConverter::toString(i),
			mWorld
					);

			node = curCell->getFlagNode();



			cubeBody->setShape(node, //scene node
					cubeShape, //collision shape
					0.6f, //restitution
					0.6f, //friction
					1.0f, //mass
					position, //location of the shape
					Ogre::Quaternion(0, 0, 0, 1) //orientation
			);

			int x = Ogre::Math::RangeRandom(-1000, 1000);
			int y = Ogre::Math::RangeRandom(-1000, 1000);
			int z = Ogre::Math::RangeRandom(-1000, 1000);

			cubeBody->setLinearVelocity(Ogre::Vector3(x, y, z));
			mBodies.push_back(cubeBody);
			mShapes.push_back(cubeShape);

		}

		if(curCell->getNumberNode() != 0){
			size = curCell->getNumberNode()->_getWorldAABB().getSize()/2;
			position = curCell->getNumberNode()->getPosition();


			cubeShape =	new OgreBulletCollisions::BoxCollisionShape(size);

			cubeBody =
					new OgreBulletDynamics::RigidBody(
							"number" + Ogre::StringConverter::toString(time(0))
			+  Ogre::StringConverter::toString(i),
			mWorld
					);

			node = curCell->getNumberNode();



			cubeBody->setShape(node, //scene node
					cubeShape, //collision shape
					0.6f, //restitution
					0.6f, //friction
					1.0f, //mass
					position, //location of the shape
					Ogre::Quaternion(0, 0, 0, 1) //orientation
			);

			int x = Ogre::Math::RangeRandom(-1000, 1000);
			int y = Ogre::Math::RangeRandom(-1000, 1000);
			int z = Ogre::Math::RangeRandom(-1000, 1000);

			cubeBody->setLinearVelocity(Ogre::Vector3(x, y, z));
			mBodies.push_back(cubeBody);
			mShapes.push_back(cubeShape);
		}
	}
	mPhysicsInitialized = true;
}


void MineSweeper::createField(){
	for (int i = 0; i < mDim; i++){
		for (int j = 0; j < mDim; j++){

			Cell* cell = new Cell();
			cell->init(mSceneMgr);
			//scale to HEIGHT
			Ogre::AxisAlignedBox box = cell->getEntity()->getBoundingBox();
			Real boxSize = box.getSize().y;
			Real scaleAmt = ((BOARD_WIDTH)/mDim)/boxSize;
			cell->getSceneNode()->scale(scaleAmt, 0.2, scaleAmt);

			boxSize = boxSize*scaleAmt;
			cell->setPosition(Vector3(-BOARD_WIDTH/2 + boxSize/2 + i* boxSize, -50 + boxSize/2, -(BOARD_WIDTH/2) + boxSize/2 + j* boxSize));
			mCells.push_back(cell);
			cell->getEntity()->setQueryFlags(REMOVEABLE);
		}
	}

}


void MineSweeper::initialize(int cRow, int cCol){
	mInitialized = true;
	int row = mDim;
	int col = row;
	int mineLocation[row][col];
	for (int i = 0; i < row; i++){
		for (int j = 0; j < col; j++){
			mineLocation[i][j] = 0;
		}
	}
	int nMines = NUM_MINES[mLevel];
	int i = 0;
	while (i < nMines){
		int r = rand() % row;
		int c = rand() % col;

		if(mineLocation[r][c] != 1 && !(r == cRow && c == cCol)){
			mineLocation[r][c] = 1;
			i++;
		}


	}

	for (int i = 0; i < row; i++){
		for (int j = 0; j < col; j++){
			if(mineLocation[i][j]){
				mCells[getIndex(i, j, row)]->setMine();
			}
		}
	}

	int minesAround[row][col];
	for (int i = 0; i < row; i++){
		for (int j = 0; j < col; j++){
			int numMines = 0;
			std::vector<Cell*> neighbors;
			for(int r = i - 1; r<= i + 1; r++){
				for(int c = j - 1; c <= j + 1; c++){
					if((r >=0 && r <row) && (c >=0 && c< col)){
						if(!(r == i && c == j)){
							neighbors.push_back(mCells[getIndex(r, c, row)]);
						}
						if(mineLocation[r][c] == 1){
							numMines = numMines + 1;
						}
					}
				}

			}
			if (mineLocation[i][j] == 1){
				numMines = numMines - 1;
			}
			minesAround[i][j] = numMines;
			mCells[getIndex(i, j, row)]->setMinesAround(numMines);
			mCells[getIndex(i, j, row)]->setNeighbors(neighbors);
		}
	}
	mGuiRoot->getChild("MessageLabel")->setText("Press P to Pause.");



}

//---------------------------------------------------------------------------
void MineSweeper::createScene(void)
{
	std::srand(time(NULL));
	// Set the scene's ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
	Ogre::Light* light = mSceneMgr->createLight("MainLight");
	light->setPosition(200.0f, 800.0f, 500.0f);

	createField();

	Entity* detectorEnt = mSceneMgr->createEntity("Detector.mesh");
	Entity* headEnt = mSceneMgr->createEntity("sphere.mesh");

	mDetector = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	SceneNode* detectorNode = mDetector->createChildSceneNode();
	detectorNode->attachObject(detectorEnt);

	mDetectorHead = detectorNode->createChildSceneNode();
	mDetectorHead->attachObject(headEnt);
	Real s = 1/detectorEnt->getBoundingBox().getSize().x;
	mDetectorHead->scale(s,s,s);
	mDetectorHead->setPosition(DETECTOR_HEAD_POSITION);
	detectorNode->setPosition(0,0,0);

	mDetector->setPosition(DETECTOR_INITIAL_POSITION);

	Ogre::AxisAlignedBox box = detectorNode->getAttachedObject(0)->getBoundingBox();
	double boxSize = box.getSize().x * mDetector->getScale().x ;

	double length = mCells[0]->getEntity()->getBoundingBox().getSize().x * mCells[0]->getSceneNode()->getScale().x;
	double scaleAmt = (length)/boxSize;
	mDetector->yaw(Degree(-90));


	mDetector->scale(scaleAmt, scaleAmt, scaleAmt);
	mDetectorHead->setVisible(false);
	mCamera->setPosition(0, 600, 300);
	mCamera->pitch(Ogre::Degree(-65));

	mHighScore = new HighScores("High Scores", 20, true);

	if(!mHighScore->readFromFile(HIGHSCORE_FILE)){
		mHighScore->writeToFile(HIGHSCORE_FILE);
	}

	setupGUI();

	mGuiRoot->getChild("NameWindow")->getChild("NameBox")->activate();


	//Bullet physics
	mWorld = new OgreBulletDynamics::DynamicsWorld(mSceneMgr, m_bounds,
			GRAVITY);
}

void MineSweeper::createViewports(){

	mViewport = mWindow->addViewport(mCamera);
	mViewport->setDimensions(0, 0, 1, 1);
	mCamera->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));

}

void MineSweeper::setupGUI(){

	mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();

	CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
	CEGUI::SchemeManager::getSingleton().createFromFile("GlossySerpentFHD.scheme");
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("GlossySerpentFHDCursors/MouseArrow");
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	//	load our file
	mGuiRoot = wmgr.loadLayoutFromFile("MineSweeper.layout");
	//	//setup main window
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(mGuiRoot);
	//
	mGuiRoot->getChild("QuitButton")->subscribeEvent(
			CEGUI::PushButton::EventClicked,	//which event to call
			CEGUI::Event::Subscriber(&MineSweeper::quit, //method call
					this));	//object to call it on
	mGuiRoot->getChild("ResumeButton")->subscribeEvent(
			CEGUI::PushButton::EventClicked,	//which event to call
			CEGUI::Event::Subscriber(&MineSweeper::resume, //method call
					this));	//object to call it on
	//
	mGuiRoot->getChild("RestartButton")->subscribeEvent(
			CEGUI::PushButton::EventClicked,	//which event to call
			CEGUI::Event::Subscriber(&MineSweeper::restart, //method call
					this));	//object to call it on
	mGuiRoot->getChild("NameWindow")->getChild("NameButton")->subscribeEvent(
			CEGUI::PushButton::EventClicked,	//which event to call
			CEGUI::Event::Subscriber(&MineSweeper::nameEntered, //method call
					this));	//object to call it on
	mGuiRoot->getChild("HighScoresButton")->subscribeEvent(
			CEGUI::PushButton::EventClicked,	//which event to call
			CEGUI::Event::Subscriber(&MineSweeper::showHighScores, //method call
					this));	//object to call it on
	//
	//	//Setup score window
	mScoreBox = static_cast<CEGUI::MultiColumnList*>(mGuiRoot->getChild("ScoreWindow/ScoresList"));
	//mScoreBox->initialiseComponents();
	mScoreBox->setUserSortControlEnabled(false);
	mScoreBox->insertColumn("[font='HighScoreFont-12.font']Rank", 0, CEGUI::UDim(0.15,0), 0);
	mScoreBox->insertColumn("[font='HighScoreFont-12.font']Player Name", 1, CEGUI::UDim(0.485,0), 1);
	mScoreBox->insertColumn("[font='HighScoreFont-12.font']Score", 2, CEGUI::UDim(0.335,0), 2);
	mScoreBox->setUserColumnSizingEnabled(false);
	mGuiRoot->getChild("ScoreWindow")->getChild("ScoreClose")->subscribeEvent(
			CEGUI::PushButton::EventClicked,	//which event to call
			CEGUI::Event::Subscriber(&MineSweeper::hideHighScores, //method call
					this));	//object to call it on
	mGuiRoot->getChild("ScoreWindow")->getChild("ScoreReset")->subscribeEvent(
			CEGUI::PushButton::EventClicked,	//which event to call
			CEGUI::Event::Subscriber(&MineSweeper::resetHighScores, //method call
					this));	//object to call it on
	mGuiRoot->getChild("GameOverWindow")->getChild("RestartButton")->subscribeEvent(
			CEGUI::PushButton::EventClicked,	//which event to call
			CEGUI::Event::Subscriber(&MineSweeper::restart, //method call
					this));	//object to call it on
	mGuiRoot->getChild("GameOverWindow")->getChild("QuitButton")->subscribeEvent(
			CEGUI::PushButton::EventClicked,	//which event to call
			CEGUI::Event::Subscriber(&MineSweeper::quit, //method call
					this));	//object to call it on
	mGuiRoot->getChild("HighScoreValue")->setText(std::to_string(mHighScore->getHighestScore()));
	CEGUI::FrameWindow* scoreWindow = static_cast<CEGUI::FrameWindow*>(mGuiRoot->getChild("ScoreWindow"));
	scoreWindow->setCloseButtonEnabled(true);
	scoreWindow->getCloseButton()->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber(&MineSweeper::hideHighScores, this));
	//Setup HighScore Fonts
	CEGUI::FontManager::getSingleton().createFreeTypeFont("HighScoreFont-18.font", 18, true, "HighScoreFont.ttf");
	CEGUI::FontManager::getSingleton().createFreeTypeFont("HighScoreFont-12.font", 12, true, "HighScoreFont.ttf");
}






void MineSweeper::createFrameListener(void) {
	BaseApplication::createFrameListener();
	//init scene query
	mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray(), REMOVEABLE);
}



bool MineSweeper::keyPressed(const OIS::KeyEvent& arg) {
	//Check to see if Name prompt window is visible
	CEGUI::String isVisible = mGuiRoot->getChild("NameWindow")->getProperty("Visible");
	if(isVisible == "true") {	// Allow keyboard input only when name prompt window is visible
		CEGUI::System &sys = CEGUI::System::getSingleton();
		sys.getDefaultGUIContext().injectKeyDown((CEGUI::Key::Scan)arg.key);
		sys.getDefaultGUIContext().injectChar((CEGUI::Key::Scan)arg.text);
		return true;
	}
	else {
		if((arg.key == OIS::KC_SPACE && !mGuiRoot->getChild("ScoreWindow")->isVisible()
				&& ! mGuiRoot->getChild("GameOverWindow")->isVisible()) ){
			if(mStop & !mGameOver){
				mGuiRoot->getChild("LevelUpWindow")->setVisible(false);
				mPause = false;
				mStop = false;
				mGuiRoot->getChild("MessageLabel")->setText("Press P to pause.");
				showButtons(false);
			}
		}
		switch(arg.key){
		case OIS::KC_P:
			if(!mStop && !mGuiRoot->getChild("ScoreWindow")->isVisible()){
				if(mPause){
					resume(CEGUI::EventArgs());
				}
				else {
					mPause = true;
					showButtons(true);
					mGuiRoot->getChild("MessageLabel")->setText("Press P to resume.");
				}
			}
			break;
		case OIS::KC_V:
			//perform ray query
			cellClicked("Reveal");
			break;
		case OIS::KC_LEFT:
		case OIS::KC_A:
			mCameraDirection = Ogre::Vector3::UNIT_X;
			break;
		case OIS::KC_RIGHT:
		case OIS::KC_D:
			mCameraDirection = Ogre::Vector3::NEGATIVE_UNIT_X;
			break;
		case OIS::KC_UP:
		case OIS::KC_W:
			mCameraDirection = Ogre::Vector3::UNIT_Z;
			break;
		case OIS::KC_DOWN:
		case OIS::KC_S:
			mCameraDirection = Ogre::Vector3::NEGATIVE_UNIT_Z;
			break;
		case OIS::KC_X:
			mCameraDirection = Ogre::Vector3::UNIT_Y;
			break;
		case OIS::KC_Z:
			mCameraDirection = Ogre::Vector3::NEGATIVE_UNIT_Y;
			break;

		default:
			break;
		}
	}
	if (arg.key == OIS::KC_ESCAPE)
	{
		mShutDown = true;
	}
	return true;
}

bool MineSweeper::keyReleased(const OIS::KeyEvent& arg) {
	switch(arg.key){
	case OIS::KC_LEFT:
	case OIS::KC_W:
	case OIS::KC_A:
	case OIS::KC_S:
	case OIS::KC_D:
	case OIS::KC_RIGHT:
	case OIS::KC_UP:
	case OIS::KC_DOWN:
	case OIS::KC_Z:
	case OIS::KC_X:
		mCameraDirection = Ogre::Vector3::ZERO;
		break;
	default:
		break;
	}

	return BaseApplication::keyReleased(arg);
}

void MineSweeper::updateGUI(){
	if(!mGameOver){
		mGuiRoot->getChild("LevelValue")->setText(std::to_string(mLevel));
		int time = (int)mCurTime;
		int min = time / 60;
		int sec = time % 60;
		std::string t = std::to_string(min) + " : " + std::to_string(sec);
		mGuiRoot->getChild("TimeValue")->setText(t);
		mGuiRoot->getChild("ScoreValue")->setText(std::to_string(mScore));
		mGuiRoot->getChild("FlagsValue")->setText(std::to_string(countFlags()) + "/" + std::to_string(NUM_MINES[mLevel]));
		mGuiRoot->getChild("HighScoreValue")->setText(std::to_string(mHighScore->getHighestScore()));
	}
}

void MineSweeper::cellClicked(String action){
	Vector3 detectorPos = mDetectorHead->_getDerivedPosition();
	detectorPos.y = detectorPos.y + 100;
	mRaySceneQuery->setRay(Ray(mDetectorHead->_getDerivedPosition(),
			Ogre::Vector3::NEGATIVE_UNIT_Y));

	RaySceneQueryResult &qResult = mRaySceneQuery->execute();
	RaySceneQueryResult::iterator it = qResult.begin();
	//make sure there is something and it is an entity
	if(it != qResult.end() && it->movable){
		for(int i = 0; i < mCells.size(); i++){

			// If the first object is ball and second object is the block or vice versa
			if(	it->movable->getParentSceneNode() ==  mCells[i]->getSceneNode()){
				if(!mInitialized){
					initialize(i / mDim, i % mDim);
					mStop = false;
					mPause = false;
				}
				if(action == "Reveal"){
					if (!mCells[i]->reveal()){
						gameOver();
					}
					else {
						mScore += POINTS_PER_REVEAL[mLevel]*getNumRevealed(mCurNumRevealed);
						updateHighScores();
						mCurNumRevealed = countRevealed();
						if(isLevelUp()){
							levelUp();
						}
					}


				}
				else if(action == "Flag"){
					mCells[i]->flag();
					if(isLevelUp()){
						levelUp();
					}
				}
				mDeleted = true;
				break;
			}
		}
	}

}

void MineSweeper::gameOver(){
	revealAll();
	mGameOver = true;
	mGameOverTime = mCurTime;
	mStop = true;
	mPause = true;
	mPhysicsInitialized = false;
}
int MineSweeper::countRevealed(){
	int count = 0;
	for(Cell* c: mCells){
		if(c->isRevelead()){
			++count;
		}
	}
	return count;

}

void MineSweeper::levelUp(){
	mPause = true;
	mStop = true;
	for(int i = 0; i < mCells.size(); ++i){
		mCells[i]->reveal();
	}
	if(mLevel < MAX_LEVEL){

		if(mCurTime < MAX_BONUS_TIME[mLevel]){
			//award bonus points
			mScore += 2*(MAX_BONUS_TIME[mLevel] - mCurTime);
		}
		mLevel = mLevel + 1;
		mDim = LEVEL_DIM[mLevel];
		mGuiRoot->getChild("LevelValue")->setText(std::to_string(mLevel));
		mGuiRoot->getChild("LevelUpWindow")->getChild("LevelUpPrompt")->setText("Congratulations " + mPlayerName + " !!! You have reached Level "
				+ std::to_string(mLevel) + ". Please press Space to continue to next Level." );
		mGuiRoot->getChild("LevelUpWindow")->setVisible(true);
		mGuiRoot->getChild("MessageLabel")->setText("Congratulations " + mPlayerName + " !!! \n You have reached Level "
				+ std::to_string(mLevel) + ". Please press Space to continue to next Level.");
		showButtons(true);
		mGuiRoot->getChild("ResumeButton")->setVisible(false);

		clearCells();
		createField();
		mInitialized = false;
		mCurNumRevealed = 0;
		mCurTime = 0;
	}
	else {
		mGuiRoot->getChild("GameOverWindow")->getChild("GameOverPrompt")->setText("Congratulations " + mPlayerName + " !!! You have completed the Game."  );
		mGuiRoot->getChild("MessageLabel")->setText("Congratulations " + mPlayerName + " !!! You have completed the Game."  );
		mGuiRoot->getChild("GameOverWindow")->setVisible(true);
	}

}

bool MineSweeper::isLevelUp(){
	int countFlagged = 0;
	for (int i = 0; i < mCells.size(); ++i){
		Cell* cur = mCells[i];
		if(cur->isMine() & cur->isFlagged()){
			++countFlagged;
		}
	}
	int revealCells = mDim*mDim - NUM_MINES[mLevel];
	return countRevealed() == revealCells || countFlagged == NUM_MINES[mLevel];
}



bool MineSweeper::mouseMoved(const OIS::MouseEvent& arg) {
	//	return BaseApplication::mouseMoved(arg);
	if(!mPause && !mStop) {
		CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(70,70);
		CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().hide();
		Ogre::Real xPct = ((Ogre::Real)arg.state.X.abs)/mViewport->getActualWidth();
		Ogre::Real zPct = ((Ogre::Real)arg.state.Y.abs)/mViewport->getActualHeight();
		int newX = 2*DETECTOR_MAX_X*xPct -DETECTOR_MAX_X;
		int newZ = 2*DETECTOR_MAX_Z*(1-zPct) - DETECTOR_MAX_Z;
		mDetector->setPosition(Ogre::Vector3(newX, DETECTOR_Y[mLevel], -newZ));
	}
	else {
		CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().show();
		if (arg.state.buttonDown(OIS::MB_Right))
		{
			mCameraMan->injectMouseMove(arg);
		}
		CEGUI::System &sys = CEGUI::System::getSingleton();
		sys.getDefaultGUIContext().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
		// Scroll wheel.
		if (arg.state.Z.rel)
			sys.getDefaultGUIContext().injectMouseWheelChange(arg.state.Z.rel / 120.0f);
	}



	return true;
}


bool MineSweeper::mousePressed(const OIS::MouseEvent& arg,
		OIS::MouseButtonID id) {
	//	return true;
	if(!mPause){
		if(id == OIS::MB_Left){
			cellClicked("Reveal");
		}
		else if(id == OIS::MB_Right){
			cellClicked("Flag");
		}
	}
	else {
		CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertButton(id));
	}
	return true;
}

bool MineSweeper::mouseReleased(const OIS::MouseEvent& arg,
		OIS::MouseButtonID id) {
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertButton(id));
	for(int i = 0; i < mCells.size(); ++i){
		mCells[i]->light(false);
	}
	return true;
}

//---------------------------------------------------------------------------

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char *argv[])
#endif
{
	// Create application object
	MineSweeper app;

	try {
		app.go();
	} catch(Ogre::Exception& e)  {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		std::cerr << "An exception has occurred: " <<
				e.getFullDescription().c_str() << std::endl;
#endif
	}

	return 0;
}

#ifdef __cplusplus
}


#endif

//---------------------------------------------------------------------------

