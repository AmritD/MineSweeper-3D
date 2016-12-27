//============================================================================
// Name        : HighScores.cpp
// Author      : Amrit Dhakal
// Version     : 1.0
// Copyright   : © Amrit Dhakal 2015. All right reserved. :D
// Description : Class for managing high scores and high score files
//============================================================================

#include "HighScores.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>
#include <stdio.h>

using namespace std;

typedef pair<string, int> ScorePair;
const int HighScores::DEFAULT_NUMBER_OF_SCORES = 10;
const string HighScores::DEFAULT_TITLE = "High Scores";


HighScores::HighScores():
					m_title(DEFAULT_TITLE),
					m_number(DEFAULT_NUMBER_OF_SCORES),
					m_highestBest(true)
{
}

HighScores::HighScores(string title, int number, bool highestBest):
					m_title(title),
					m_number(number),
					m_highestBest(highestBest)
{
}

HighScores::~HighScores() {
}


bool HighScores::sortByHighScore(const ScorePair a, const ScorePair b){
	return a.second > b.second;
}


bool HighScores::sortByLowScore(const ScorePair a, const ScorePair b){
	return a.second < b.second;
}

bool HighScores::addScore(string name, int score){
	if(isHighScore(score)){ 			//if the score is eligible to be added
		if(m_number <= m_scores.size())	//remove last score if the list is full
			m_scores.pop_back();
		m_scores.push_back(ScorePair(name, score));
		sortScores();
		return true;
	}
	else {
		return false;
	}

}


bool HighScores::isHighestBest() const {
	return m_highestBest;
}

int HighScores::getNumber() const {
	return m_number;
}

void HighScores::setNumber(int number) {
	int excessScore = m_scores.size() - number;
	//If there are more score than the new maximum, throw low ranked scores
	if(excessScore > 0){
		for(int i = 0; i < excessScore; i++){
			m_scores.pop_back();
		}
	}
	m_number = number;
}

const string& HighScores::getTitle() const {
	return m_title;
}

void HighScores::setTitle(const string& title) {
	m_title = title;
}

void HighScores::sortScores(){
	if(m_highestBest)
		sort(m_scores.begin(), m_scores.end(), sortByHighScore);
	else {

		sort(m_scores.begin(), m_scores.end(), sortByLowScore);
	}
}


bool HighScores::isHighScore(int score) const{
	if(m_number > m_scores.size()){
		return true;
	}
	else {
		if(m_highestBest){
			if(score > m_scores.at(m_number - 1).second){
				return true;
			}
			else {
				return false;
			}
		}
		else {
			if(score < m_scores.at(m_number - 1).second){
				return true;
			}
			else {
				return false;
			}
		}
	}

}

void HighScores::clearScores() {
	m_scores.clear();
}

int HighScores::totalScores(){
	return m_scores.size();
}

bool HighScores::readFromFile(const string &filename){
	ifstream in(filename.c_str(), ios::in);
	if(!in){
		cerr << "File " << filename << " could not be opened." << endl;
		return false;
	}
	getline(in, m_title);
	in >> m_number;
	in >> m_highestBest; //is highest the best? 1 for yes, 0 for no

	int numScores;
	in >> numScores;
	clearScores();
	string name;
	getline(in, name);
	int score;
	for(int i = 0; i < numScores; i++){
		getline(in, name);
		in >> score;
		m_scores.push_back(ScorePair(name, score));
		in.ignore();	//remove any new line character that might cause problem
	}
	in.close();
	sortScores();

	return true;
}

void HighScores::writeToFile(const string &filename) const{
	ofstream out(filename.c_str(), ios::out);

	if(!out){
		cerr << "File " << filename << " could not be opened for writing." << endl;
	}

	out << m_title << endl;
	out << m_number << endl;
	out << m_highestBest << endl;
	out << m_scores.size() << endl;

	for(int i = 0; i < m_scores.size(); i++){
		out << m_scores.at(i).first << endl;
		out << m_scores.at(i).second << endl;
	}

	out.close();
}

string HighScores::getName(const int i){
	return m_scores.at(i).first;
}

int HighScores::getScore(const int i){
	return m_scores.at(i).second;
}

int HighScores::updateScore(int i, string name, int score){
	m_scores.erase(m_scores.begin() + i);
	int scorePlace = getPlace(score);
	addScore(name, score);
	sortScores();
	return scorePlace;
}

int HighScores::getPlace(const int score){
	if(isHighScore(score)){

		int index = 0;
		if(m_highestBest){
			while(index < m_scores.size() && score <= m_scores.at(index).second){
				index++;
			}
		}

		else {
			while(index < m_scores.size() && score >= m_scores.at(index).second){
				index++;
			}
		}
		if(index < m_number){
			return index;
		}
		else {
			return -1;
		}
	}
	else{
		return -1;
	}
}

void HighScores::incrementScores(const int inc){
	for(int i = 0; i < m_scores.size(); i++){
		m_scores.at(i).second += inc ;
	}
}

void HighScores::print() const{
	cout << " ________________________________" << endl;
	fprintf(stdout, "| %-30s |\n",m_title.c_str());
	cout << "|--------------------------------|" << endl;
	if(m_scores.empty()){
		cout << "|No Scores to print              |" << endl;
	}
	else {
		cout << "|     Player          |  Score   |" << endl;
		cout << "|--------------------------------|" << endl;
		for (vector<ScorePair>::const_iterator iter = m_scores.begin();
				iter != m_scores.end(); ++iter) {
			fprintf(stdout, "| %-20s",iter->first.c_str());
			fprintf(stdout, "| %-8d |\n",iter->second);
		}
	}
	cout << "|________________________________|" << endl;
}




