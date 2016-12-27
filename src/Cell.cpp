//============================================================================
// Name        : Cell.cpp
// Author      : Amrit Dhakal
// Version     : 1.0
// Copyright   : © Amrit Dhakal 2015. All right reserved. :D
//============================================================================

#include "Cell.h"
#include "BaseApplication.h"
#include <iostream>
#include "Constants.h"

#include <stdlib.h>

using namespace Ogre;



const Ogre::uint32 Cell::INTERSECTABLE = 2;
Cell::Cell(void)
{
	mSceneMgr = 0;
	mEntity =  0;
	mSceneNode = 0;
	mPosition = 0;
	mFlagNode = 0;
	mNumberNode = 0;
	mMineNode = 0;

}

Cell::~Cell(void){

}

void Cell::init(Ogre::SceneManager*  & sceneMgr){
	mSceneMgr = sceneMgr;
	mEntity =  mSceneMgr->createEntity("cube.mesh");
	mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mSceneNode->attachObject(mEntity);
	mSceneNode->setPosition(mPosition);
	mEntity->setMaterialName("Picture/Cell");
	mEntity->setQueryFlags(INTERSECTABLE);
	mRevealed = false;
	mIsReward = false;
	mMinesAround = 0;
	mIsFlagged = false;
	mIsMine = false;
}
Vector3 Cell::getPosition(){
	return mSceneNode->getPosition();
}

void Cell::removeFromScene(){
	mSceneMgr->destroySceneNode(mSceneNode);
	if (mFlagNode) {
		mSceneMgr->destroySceneNode(mFlagNode);
	}
	if (mNumberNode) {
		mSceneMgr->destroySceneNode(mNumberNode);
	}
	if(mMineNode) {
		mSceneMgr->destroySceneNode(mMineNode);
	}
}

void Cell::flag(){
	if(!mRevealed){
		mIsFlagged = !mIsFlagged;
		if(!mFlagNode){
			Entity* flag = mSceneMgr->createEntity("Flag.mesh");
			mFlagNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
			mFlagNode->attachObject(flag);

			double boxSize = flag->getBoundingBox().getSize().x * mFlagNode->getScale().x ;

			double length = mEntity->getBoundingBox().getSize().z * mSceneNode->getScale().z;

			double scaleAmt = (length/1.5)/boxSize;

			mFlagNode->_setDerivedPosition(mSceneNode->_getDerivedPosition());
			mFlagNode->scale(scaleAmt, scaleAmt, scaleAmt);
			mFlagNode->translate(0,10,0);
			mFlagNode->setVisible(false);
		}
		mFlagNode->flipVisibility();
	}
}

void Cell::light(bool isLighted){
	if(isLighted){
		mEntity->setMaterialName("Picture/Lighted");
	}
	else{
		mEntity->setMaterialName("Picture/Cell");
	}
}

int Cell::countFlagsAround(){
	int count = 0;
	for(int i = 0; i < mNeighbors.size(); i++){
		if(mNeighbors[i]->isFlagged()){
			count++;
		}
	}
	return count;
}



bool Cell::reveal(bool revealOnly, bool light){
	if(!mIsFlagged){
		if(!mRevealed){
			mRevealed = true;
			if(mIsMine){

				mSceneNode->setVisible(false);
				Ogre::Entity* revealEntity = mSceneMgr->createEntity("Mine.mesh");
				mMineNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
				mMineNode->attachObject(revealEntity);

				double boxSize = revealEntity->getBoundingBox().getSize().z * mMineNode->getScale().z ;

				double length = mEntity->getBoundingBox().getSize().z * mSceneNode->getScale().z;

				double scaleAmt = (length)/boxSize;

				mMineNode->_setDerivedPosition(mSceneNode->_getDerivedPosition());
				mMineNode->scale(scaleAmt, scaleAmt, scaleAmt);
				return false;
			}
			mSceneNode->setVisible(false);
			if(mMinesAround != 0){
				Ogre::Entity* revealEntity = mSceneMgr->createEntity(std::to_string(mMinesAround) + ".mesh");
				mNumberNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
				mNumberNode->attachObject(revealEntity);

				double boxSize = revealEntity->getBoundingBox().getSize().z * mNumberNode->getScale().z ;

				double length = mEntity->getBoundingBox().getSize().z * mSceneNode->getScale().z;

				double scaleAmt = (length/2)/boxSize;

				mNumberNode->_setDerivedPosition(mSceneNode->_getDerivedPosition());
				mNumberNode->scale(scaleAmt, scaleAmt, scaleAmt);
				mNumberNode->translate(-length/4, 0, length/4);
				mNumberNode->pitch(Ogre::Degree(30));
			}
			if(mMinesAround == 0){
				mSceneNode->showBoundingBox(true);
				for(int i = 0; i < mNeighbors.size(); i++){
					if(!mNeighbors[i]->reveal(true, false))
						return false;
				}
			}
			else {
				return true;
			}

		}
		else {

			if(countFlagsAround() == mMinesAround && mMinesAround != 0 && !revealOnly){
				for(int i = 0; i < mNeighbors.size(); i++){
					if(!mNeighbors[i]->reveal(true, false))
						return false;
				}
			}
			else if(!revealOnly && light) {
				for(int i = 0; i < mNeighbors.size(); i++){
					if(!mNeighbors[i]->isFlagged())
						mNeighbors[i]->light();
				}
			}

		}
	}

	return true;
}




