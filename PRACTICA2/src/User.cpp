/*
 * User.cpp
 *
 *  Created on: 6 oct. 2021
 *      Author: aikokujo,
 *      	Aitana
 */

#include "User.h"

User::User() {
	setUsername("NO_INTRODUCIDO");
	setPassword("NO_INTRODUCIDO");
	setScore(0);
	setState(-1);
	setSD(-1);
}

User::User(int sd) {
	setUsername("NO_INTRODUCIDO");
	setPassword("NO_INTRODUCIDO");
	setScore(0);
	setState(-1);
	setSD(sd);
}

User::User(string username, string password, int score, int state, int sd) {
	// TODO Auto-generated constructor stub
	setUsername(username);
	setPassword(password);
	setScore(score);
	setState(state);
	setSD(sd);
}

User::~User() {
	// TODO Auto-generated destructor stub
}
/*
User::User(const User &other) {
	// TODO Auto-generated constructor stub
	setUsername(other.username_);
	setPassword(other.password_);
	setScore(other.score_);
	setState(other.state_);
}
*/
User& User::operator=(const User &other) {
	// TODO Auto-generated method stub
	setUsername(other.username_);
	setPassword(other.password_);
	setScore(other.score_);
	setState(other.state_);
	setSD(other.sd_);

	return *this;
}

bool User::operator==(const User &other) {
	if (getUsername() == other.username_) {
		return true;
	}
	return false;
}

bool User::isUsername(string username) {
	if (getUsername() == username) {
		return true;
	}
	return false;
}

bool User::isPassword(string password) {
	if (getPassword() == password) {
		return true;
	}
	return false;
}

bool User::isScore(int score) {
	if (getScore() == score) {
		return true;
	}
	return false;
}

bool User::isState(int state) {
	if (getState() == state) {
		return true;
	}
	return false;
}

bool User::hasUsername() {
	if (getUsername() == "NO_INTRODUCIDO") {
		return false;
	}
	return true;
}

bool User::hasPassword() {
	if (getPassword() == "NO_INTRODUCIDO") {
		return false;
	}
	return true;
}

bool User::canSpend(int cost) {
	if (getScore() >= cost) {
		return true;
	}
	return false;
}


void User::writeUser(string username, string password) {
	 ofstream f;

	 f.open("Credenciales.txt", ios::app);
		cout<<"Antes de escribir"<<endl;
	 f << username << " " << password << endl;
	 	cout << "Escrito"<<endl;

	 f.close();
}

bool User::validateUsername(string username) {

	string aux;

	ifstream f;

	f.open("Credenciales.txt");

	while(getline(f, aux, ' ')){
		if (aux == username) return true;
		getline(f, aux, '\n');
	}

	f.close();

	return false;
}

bool User::validatePassword(string password) {
	string aux;

	ifstream f;

	f.open("Credenciales.txt");

	while(getline(f, aux, ' ')){

    	if(getUsername() == aux){
        	getline(f, aux, '\n');
			cout << "VALOR CONTRASEÑA: " << aux <<endl;
			if (aux == password) return true;
     	 }

     	 else{ getline(f, aux, '\n');}

	}

	f.close();

	return false;
}


