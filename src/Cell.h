//============================================================================
// Name        : Cell.h
// Author      : Amrit Dhakal
// Version     : 1.0
// Copyright   : © Amrit Dhakal 2015. All right reserved. :D
//============================================================================
#ifndef CELL_H_
#define CELL_H_

#include "BaseApplication.h"
#include <vector>

class Cell{

public:
	Cell(void);

	virtual ~Cell(void);

	void init(Ogre::SceneManager*  & mSceneMgr);

	/**
	 * getPosition: returns the position of the cell in the scene
	 *
	 * return: the position of the cell
	 */
	Ogre::Vector3 getPosition(void);

	/**
	 * removeFromScene: remove the cell from the scene
	 */
	void removeFromScene();

	/**
	 * getSceneNode: returns the scenenode associated with the cell
	 *
	 * return: the sceneNode associated with the cell
	 */
	Ogre::SceneNode* getSceneNode(void){
		return mSceneNode;
	}

	/**
	 * getEntity: returns the entity associated with the cell
	 *
	 * return: the entity associated with the cell
	 */
	Ogre::Entity* getEntity(){
		return mEntity;
	}

	/**
	 * scale: Scales the cell to the given scale vector
	 *
	 * Parameter:
	 * 		scale: the scale vector to scale the cell by
	 */
	void scale(Ogre::Vector3 scale){
		mSceneNode->scale(scale);
	}

	/**
	 * setPosition: sets the position of the scene node
	 *
	 * Parameter:
	 * 		pos: the new position of the scene node
	 */
	void setPosition(Ogre::Vector3 pos){
		mPosition = pos;
		mSceneNode->setPosition(pos);
	}

	void setReward(){
		mIsReward = true;
	}
	void setMinesAround(int mines){
		mMinesAround = mines;
	}

	void setMine(){
		mIsMine = true;
	}

	bool isReward(){
		return mIsReward;
	}
	bool isMine(){
		return mIsMine;
	}
	bool isRevelead(){
		return mRevealed;
	}
	bool isFlagged(){
		return mIsFlagged;
	}
	void flag();

	int getNumMinesAround(){
		return mMinesAround;
	}
	int countFlagsAround();

	void setNeighbors(std::vector<Cell*> n){
		mNeighbors = n;

	}

	Ogre::SceneNode* getNumberNode(){
		return mNumberNode;
	}

	Ogre::SceneNode* getMineNode(){
		return mMineNode;
	}

	Ogre::SceneNode* getFlagNode(){
		return mFlagNode;
	}

	int getMinesAround(){
		return mMinesAround;
	}

	void light(bool isLighted = true);

	bool reveal(bool revealOnly = false, bool light = true);

	static const Ogre::uint32 INTERSECTABLE;

protected:
	/**
	 * Entity assocaited with the cell
	 */
	Ogre::Entity* mEntity;

	Ogre::SceneNode* mFlagNode;

	Ogre::SceneNode* mMineNode;

	Ogre::SceneNode* mNumberNode;

	/**
	 * Scenemanager managing the scene
	 */
	Ogre::SceneManager* mSceneMgr;

	/**
	 * SceneNode assocaited with the cell
	 */
	Ogre::SceneNode* mSceneNode;

	/**
	 * current Position of the cell in the scene.
	 */
	Ogre::Vector3 mPosition;

	bool mIsMine;
	bool mRevealed = false;
	bool mIsReward = false;
	int mMinesAround;
	bool mIsFlagged;
	std::vector<Cell*> mNeighbors;

};




#endif /* CELL_H_ */
