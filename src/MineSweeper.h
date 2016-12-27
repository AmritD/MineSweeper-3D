//============================================================================
// Name        : MineSweeper.h
// Author      : Amrit Dhakal
// Version     : 1.0
// Copyright   : © Amrit Dhakal 2015. All right reserved. :D
//============================================================================

#ifndef __MINESWEEPER_H_
#define __MINESWEEPER_H_

#include "BaseApplication.h"
#include "HighScores.h"
#include <vector>
#include "Cell.h"
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>
#include <OgreBulletDynamicsRigidBody.h>

//---------------------------------------------------------------------------

class MineSweeper : public BaseApplication
{
public:

	MineSweeper(void);
	virtual ~MineSweeper(void);
protected:

	/**
	 * Renderer for the scene
	 */
	CEGUI::OgreRenderer* mRenderer;

	/**
	 * Window manager for CEGUI
	 */
	CEGUI::Window *mGuiRoot;

	virtual void createScene(void);
	virtual void createFrameListener(void);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);


	void cellClicked(Ogre::String action);
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual void createViewports(void);
	void createField();
	void initialize(int cRow, int cCol);

	void setupGUI();
	std::vector<Cell*> mCells;
	Cell** mCellPointers;

	/**
	 * quit: Closes the game (called when Quit button is clicked)
	 */
	bool quit(const CEGUI::EventArgs &e);

	/**
	 * restart: Restarts the game (called when Restart button is clicked)
	 */
	bool restart(const CEGUI::EventArgs &e);

	/**
	 * Resume a paused game (called when Resume button is clicked)
	 */
	bool resume(const CEGUI::EventArgs &e);

	/**
	 * Set's the name of the current player
	 * (called when OK button of name prompt dialog box is clicked)
	 */
	bool nameEntered(const CEGUI::EventArgs &e);

	/**
	 * makes the high score list window visible and sets the high scores
	 */
	bool showHighScores(const CEGUI::EventArgs &e);

	/**
	 * Hides the high score list window
	 */
	bool hideHighScores(const CEGUI::EventArgs &e);

	/**
	 * Clears the high scores stored so far
	 */
	bool resetHighScores(const CEGUI::EventArgs &e);

	/**
	 * Changes the visibility of all the buttons in the scene
	 * Parameter:
	 * 		val: true if buttons are to be shown, false otherwise
	 */
	void showButtons(bool val);

	/**
	 * Updates the Highscores if the current score is eligible
	 * to be in the high score list (this method is called every time high score changes)
	 */
	void updateHighScores();

	/**
	 * Window manager for CEGUI
	 */

	void clearCells();

	/**
	 * Reveals all of the cells
	 */
	void revealAll();

	/**
	 * Sets up the physics objects for the game over animation at the end
	 */
	void setupPhysicsObjects();

	/**
	 * Counts the number of flagged cells
	 */
	int countFlags();

	/**
	 * Counts and returns the number of cells already revealed by the player
	 */
	int getNumRevealed(int prevReveal);

	/**
	 * Returns whether or not the player has completed the current level and needs to level up
	 */
	bool isLevelUp();

	/**
	 * Starts the next level
	 */
	void levelUp();

	/**
	 * End the game when the player hits a mine
	 */
	void gameOver();

	/**
	 * Counts the number of revealed cells
	 */
	int countRevealed();

	/**
	 * Updates the stats in the GUI as well as the state of GUI elements
	 */
	void updateGUI();


private:
	/**
	 * Constant for gravity (9.81)
	 */
	static const int g;

	/**
	 * Is the game being stopped?
	 */
	bool mStop = true;

	/**
	 * Time since last frame
	 */
	Ogre::Real mTimeSinceLastFrame;

	/**
	 * Direction of the camera
	 */
	Ogre::Vector3 mCameraDirection;

	/**
	 * Is the game paused?
	 */
	bool mPause = true;

	//TODO: Do I need this item
	bool mDeleted = false;

	/**
	 * Has the matrix been generated for the current level?
	 */
	bool mInitialized = false;

	/**
	 * Is the game over?
	 */
	bool mGameOver = false;

	/**
	 * Has the physics been initialized for the final flying animation?
	 */
	bool mPhysicsInitialized = false;

	/**
	 * World for the physics
	 */
	OgreBulletDynamics::DynamicsWorld *mWorld;

	/**
	 * Bounds for physics objects
	 */
	Ogre::AxisAlignedBox m_bounds;

	/**
	 * Time since the beginning of the simulation
	 */
	float mCurTime;

	/**
	 * Time when the game was over
	 */

	float mGameOverTime;

	/**
	 * High scores for the game
	 */
	HighScores* mHighScore;

	/**
	 * Multi column List for Dispalying high scores
	 */
	CEGUI::MultiColumnList* mScoreBox;

	/**
	 * Position of current score in the high score table
	 * -1 if the score is not yet in th high score list
	 */
	int mScorePosition = -1;

	/**
	 * Name of current player
	 */
	CEGUI::String mPlayerName;

	/**
	 * Current score of the player
	 */
	int mScore;

	/**
	 * Number of cells currently revealed
	 */
	int mCurNumRevealed;

	/**
	 * Dimension of the matrix. Note that the matrix is a square
	 */
	int mDim;

	/**
	 * Ray scene query used to detect the object under the metal detector
	 */
	Ogre::RaySceneQuery *mRaySceneQuery;

	/**
	 * Current level of the game
	 */
	int mLevel;

	/**
	 * Scene node for the metal detector
	 */
	Ogre::SceneNode* mDetector;

	/**
	 * Scene node for the metail detector head which is an invisible sphere
	 */
	Ogre::SceneNode* mDetectorHead;


	/**
	 * Viewport for the main game window.
	 */
	Ogre::Viewport* mViewport;

	/**
	 * List of  rigid body for the cells
	 */
	std::vector<OgreBulletDynamics::RigidBody *> mBodies;

	/**
	 * List of shapes for the cells
	 */
	std::vector<OgreBulletCollisions::CollisionShape *> mShapes;


};

//---------------------------------------------------------------------------

#endif // #ifndef __MINESWEEPER_H_

//---------------------------------------------------------------------------

