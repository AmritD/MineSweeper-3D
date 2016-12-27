//============================================================================
// Name        : HighScores.h
// Author      : Amrit Dhakal
// Version     : 1.0
// Copyright   : © Amrit Dhakal 2015. All right reserved. :D
// Description : Class for managing high scores and high score files
//============================================================================

#ifndef HIGHSCORES_H_
#define HIGHSCORES_H_


using namespace std;
#include <iostream>
#include <vector>
#include <algorithm>

typedef pair<string, int> ScorePair;

/**
 * Class HighScores stores a list of high scores for a game.
 *
 */
class HighScores {
public:
	//Default values for title and max number of scores
	static const int DEFAULT_NUMBER_OF_SCORES;
	static const string DEFAULT_TITLE;

	/**
	 * sortByHighScore: comparator function to sort the scores by considering highest score to be the best
	 * 		parameter:
	 * 			a: the first pair of score
	 * 			b: the second pair of score
	 * 		return:
	 * 			true: if the first score is higher than the second
	 * 			false: if the first pair is not higher than the second
	 */
	static bool sortByHighScore(const ScorePair a, const ScorePair b);

	/**
	 * sortByLowScore: comparator function to sort the scores by considering lowest score to be the best
	 * 		parameter:
	 * 			a: the first pair of score
	 * 			b: the second pair of score
	 * 		return:
	 * 			true: if the first score is lower than the second
	 * 			false: if the first pair is not lower than the second
	 */
	static bool sortByLowScore(const ScorePair a, const ScorePair b);

	/**
	 * Default constructor: no arguments.
	 * sets default values for the instance variable
	 */
	HighScores();

	/**
	 * Constructor:
	 * parameters:
	 *              title: the title of this high score list.
	 *              number: the maximum number of high scores to be stored
	 *              highestBest: determines the order of the scores
	 *                      true: The best score will be the highest (e.g. pinball)
	 *                      false: The best score will be the lowest (e.g. golf)
	 */
	HighScores(string title, int number, bool highestBest);

	/**
	 * Destructor: clean up resources
	 */
	virtual ~HighScores();

	/**
	 * addScore: Add a player's name and score to the list if it is
	 *              good enough to merit being a high score.
	 *      parameters:
	 *              name: the name of the player
	 *              score: the player's score
	 *      return value:
	 *              true if the player and score are added to the list
	 *              false if the score was not good enough to make the list
	 */
	bool addScore(string name, int score);


	/*
	 * isHighScore: determine if a score is good enough to be on the list.
	 * Since there are a maximum number that can be stored, this may cause a
	 * poorer score to be removed.
	 * Note: "good enough" can be either higher or lower than others, depending
	 * on whether higher scores are better, or lower scores are better.
	 *
	 *      parameters:
	 *              score: the score to test
	 *      return value:
	 *              true: the score is good enough
	 *              false: it is not good enough.
	 */
	bool isHighScore(int score) const;

	/**
	 * clearScores: empty the high scores so there are no longer any stored.
	 */
	void clearScores();

	/**
	 * totalScores: the number of scores stored. Note: this will be less than
	 * or equal to the maximum.
	 *      return value: the total number of high scores that are stored.
	 */
	int totalScores();


	/**
	 * readFromFile: read the high score data from the file given
	 * by the filename.
	 *      parameter: filename the name of the file to be read
	 *      return value:
	 *              true: the file was successfully opened and read
	 *              false: the file open or read failed.
	 *
	 */
	bool readFromFile(const string &filename);

	/**
	 * writeToFile: write the high score data to a file given
	 * by the filename.
	 *      parameter: filename the name of the file to be read
	 */
	void writeToFile(const string &filename) const;

	/**
	 * print: print the contents of this high score table to cout
	 */
	void print() const;

	//bool sortByScore(const ScorePair a, const ScorePair b);

	/**
	 * isHighestBest: returns if the highest score is considered the best score
	 * 		return:
	 * 			true: if the highest score is considered the best score
	 * 			false: if the lowest score is considred the best score
	 */
	bool isHighestBest() const;

	/**
	 * getNumber: returns the maximum number of scores that can be stored
	 * 		return: the maximum number of scores that can be stored
	 */
	int getNumber() const;

	/**
	 * setNumber: sets the maximum number of scores to the given number.
	 * If the new number is less than the total number of scores already in the list,
	 * then the lowest scores are thrown away so that the maximum allowed scores is
	 * the new number given
	 * 		parameter: the new maximum number of scores to be stored
	 */
	void setNumber(const int number);

	/**
	 * getTitle: returns the title of the High Scores
	 * 		return: the title of the High scores
	 */
	const string& getTitle() const;

	/**
	 * setTitle: Sets the title of the High Scores to the given title
	 * 		parameter: the new title of the High Scores
	 */
	void setTitle(const string& title);

	/**
	 * getName: Returns the name of the player at the given rank (0 being the highest rank)
	 * 		parameter:
	 * 			i: the rank of the player
	 */
	string getName(const int i);

	/**
	 * getScore: Returns the score of the player at the given rank (0 being the highest rank)
	 * 		parameter:
	 * 			i: the rank of the player
	 */
	int getScore(const int i);

	/**
	 * getPlace: Returns the position of the score in the High score
	 *  list if it were to be added (0 being the top place)
	 * -1 is returned if the score is not eligible to be added to the high score list
	 * 		parameter
	 * 			score: the score whose position is to be decided
	 * 		return: the position of the score in the list if it were to be added
	 */
	int getPlace(const int score);

	/**
	 * updateScore:: updates the score at given index
	 * 		parameter
	 * 			name: new name of the player
	 * 			score: new score of the player
	 * 		return: new position of the score
	 *
	 */
	int updateScore(int i, string name, int score);

	/**
	 * incrementScores: increases the scores of every player by the given number
	 * if negative number is sent as parameter, it will decrease the score by that amount
	 * 		parameter:
	 * 			inc: number to increment the scores by (if negative, will decrement the scores)
	 */
	void incrementScores(const int inc);

	/**
	 * getHighestScore: returns the highest score
	 * return: highest score if there are any scores, 0 otherwise
	 */
	int getHighestScore(){
		if(m_scores.empty()){
			return 0;
		}
		return m_scores.at(0).second;
	}

protected:


	//Title of the high scores
	string m_title;

	//Number of scores to be stored
	int m_number;

	//Is highest score the best?
	bool m_highestBest;

	//Vector containing the ScorePair
	vector<ScorePair> m_scores;

	/**
	 * Sorts the scores based on whether the highest score is the best or the lowest
	 */
	void sortScores();
};



#endif /* HIGHSCORES_H_ */
