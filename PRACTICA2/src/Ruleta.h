/*
 * Ruleta.h
 *
 *  Created on: 6 oct. 2021
 *      Author: aikokujo,
 *      	Aitana
 */

#ifndef SRC_RULETA_H_
#define SRC_RULETA_H_

#include <stdlib.h>
#include <fstream>

#include "User.h"


class Ruleta {
private:
	string fraseResuelta_;
	string fraseOculta_;
	User u1_, u2_;

public:
	Ruleta();
	Ruleta(User &u1_, User &u2_);
	virtual ~Ruleta();
	Ruleta(const Ruleta &other);
	//Ruleta& operator=(const Ruleta &other);

	inline User getUser1() {
		return u1_;
	}
	inline User getUser2() {
		return u2_;
	}

	inline void setUser1(User u1) {
		u1_ = u1;
	}
	inline void setUser2(User u2) {
		u2_ = u2;
	}

	void getFraseAleatoria();
	User UserWTurn();
	User UserWoutTurn();
	void SetUserWTurn(User &u);
	void SetUserWoutTurn(User &u);
	void crearFraseOculta();
	string mostrarFraseOculta();

	inline string getFraseResuelta(){ return fraseResuelta_;}
	inline string getFraseOculta(){ return fraseOculta_;}

	inline void setFraseResuelta(string fraseResuelta) {
		fraseResuelta_ = fraseResuelta;
	}
	inline void setFraseOculta(string fraseOculta) {
		fraseOculta_ = fraseOculta;
	}


	int comprobarConsonante(char c);

	int comprobarVocal(char c);

	bool resolver(string frase);


	User getChampion();

	void setUser1State(int state) {
		u1_.setState(state);
	}

	void setUser2State(int state) {
		u2_.setState(state);
	}

	bool isFraseResuelta() {
		if (fraseResuelta_ == fraseOculta_) {
			return true;
		}
		return false;
	}

	bool existe(char c);
};

#endif /* SRC_RULETA_H_ */

