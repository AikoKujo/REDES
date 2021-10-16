/*
 * User.h
 *
 *  Created on: 6 oct. 2021
 *      Author: aikokujo,
 *      	Aitana
 */

#ifndef SRC_USER_H_
#define SRC_USER_H_

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

class User {
private:
	string username_, password_;
	int score_, state_, sd_;


public:
	User();
	User(int sd_);
	User(string username_, string password_, int score_ = 0, int state_ = -1, int sd_=-1);
	virtual ~User();
	//User(const User &other);
	User& operator=(const User &other);
	bool operator==(const User &other);

	inline string getUsername() {
		return username_;
	}
	inline string getPassword() {
		return password_;
	}
	inline int getScore() {
		return score_;
	}
	inline int getState() {
		return state_;
	}
	inline int getSD() {
		return sd_;
	}

	inline bool setUsername(string username){
		//Podriamos comprobar en un fichero si está disponible
		if (username == ""){
			return false;
		}
		username_ = username;
		return true;
	}
	inline bool setPassword(string password) {
		if (password == ""){
			return false;
		}
		password_ = password;
		return true;
	}
	inline bool setScore(int score) {
		if (score < 0){
			return false;
		}
		score_ = score;
		return true;
	}
	inline bool setState(int state) {
		/*
		if (state < -1 || state > 4 ) {
			return false;
		}
		*/
		state_ = state;
		return true;
	}

	inline void setSD(int sd) {
		sd_ = sd;
	}

	bool isUsername(string username);
	bool isPassword(string password);
	bool isScore(int score);
	bool isState(int state);
	bool hasUsername();
	bool hasPassword();

	bool isItMe(User u) {
		if (getUsername() == u.getUsername()) return true;

		return false;
	}

	void writeUser(string username, string password);
	bool validateUsername(string username);
	bool validatePassword(string password);

	bool canSpend(int cost);
};

#endif /* SRC_USER_H_ */
