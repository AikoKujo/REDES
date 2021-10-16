/*
 * Ruleta.cpp
 *
 *  Created on: 6 oct. 2021
 *      Author: aikokujo,
 *      	Aitana
 */

#include "Ruleta.h"

using namespace std;

Ruleta :: Ruleta(User &u1, User &u2){
  	u1_ = u1;
  	u2_ = u2;
  	getFraseAleatoria();
  	crearFraseOculta();
}

Ruleta::Ruleta() { // @suppress("Class members should be properly initialized")
	// TODO Auto-generated constructor stub

}

Ruleta::~Ruleta() {
	// TODO Auto-generated destructor stub
}

Ruleta::Ruleta(const Ruleta &other) { // @suppress("Class members should be properly initialized")
	// TODO Auto-generated constructor stub

}
/*
Ruleta& Ruleta::operator=(const Ruleta &other) { // @suppress("No return")
	// TODO Auto-generated method stub
	if (this->)
}
*/

User Ruleta :: UserWTurn(){

    	if(u1_.getState() == 3){

        	return u1_;
      }

      return u2_;
  	}


User Ruleta :: UserWoutTurn(){

    	if(u1_.getState() == 3){

        	return u2_;
      }

      return u1_;
  	}

void Ruleta :: SetUserWTurn(User &u) {
	if(u1_.getState() == 3){

		u1_ = u;
		return;
	}

	u2_ = u;
}


void Ruleta :: SetUserWoutTurn(User &u) {
	if(u1_.getState() == 3){

		u2_ = u;
		return;
	}

	u1_ = u;
}

 void Ruleta :: getFraseAleatoria(){

     int indice;
     string fraseResuelta;

     indice = rand()%20;

     ifstream f;

     f.open("Refranes.txt");

     for(int i=0; i<indice; i++){

       getline(f, fraseResuelta);
     }

	setFraseResuelta(fraseResuelta);

   }



void Ruleta :: crearFraseOculta () {
	string fraseOculta = "";

	for (size_t i = 0; i < getFraseResuelta().length(); i++) {
		if(getFraseResuelta()[i] == ' ') {
			fraseOculta += " ";

		}

		else if(getFraseResuelta()[i] == '.') { fraseOculta += "."; }

		else if(getFraseResuelta()[i] == ',') { fraseOculta += ","; }

		else if(getFraseResuelta()[i] == ';') { fraseOculta += ";"; }

		else if(getFraseResuelta()[i] == ':') { fraseOculta += ":"; }

		else{
			fraseOculta += "_";
		}
	}

	setFraseOculta(fraseOculta);
}


string Ruleta :: mostrarFraseOculta() {
	string frase = "";
	for (size_t i = 0; i < getFraseOculta().length(); i++) {
		frase += getFraseOculta()[i];
		frase += " ";
	}

	return frase;
}



int Ruleta :: comprobarConsonante(char c){

  int n = 0;
  int score = 0;
  User u = UserWTurn();

  for (size_t i = 0; i < getFraseResuelta().length(); i++){

    if(tolower(c) == getFraseResuelta()[i] || toupper(c) == getFraseResuelta()[i]){

      	fraseOculta_[i] = fraseResuelta_[i];
    	n++;

    }
  }

  if(n > 0)   {

	  u.setScore(u.getScore() + (50*n));

	  SetUserWTurn(u);


  }

  return n;
}



int Ruleta :: comprobarVocal(char c){

  int n = 0;
  int score = 0;
  User u = UserWTurn();

  for (size_t i = 0; i < getFraseResuelta().length(); i++){

	  if(tolower(c) == getFraseResuelta()[i] || toupper(c) == getFraseResuelta()[i]){
		  fraseOculta_[i] = fraseResuelta_[i];
		  n++;
	  }
		  if ((c == 'a' || c == 'A') && (getFraseResuelta()[i] == 'á' || getFraseResuelta()[i] == 'Á')) {
			  fraseOculta_[i] = fraseResuelta_[i];
			  		  n++;
		  }
		  else if ((c == 'e' || c == 'E') && (getFraseResuelta()[i] == 'é' || getFraseResuelta()[i] == 'É')) {
			  fraseOculta_[i] = fraseResuelta_[i];
			  		  n++;
		  }

		  else if ((c == 'i' || c == 'I') && (getFraseResuelta()[i] == 'í' || getFraseResuelta()[i] == 'Í')) {
			  fraseOculta_[i] = fraseResuelta_[i];
			  		  n++;
		  }

		  else if ((c == 'o' || c == 'O') && (getFraseResuelta()[i] == 'ó' || getFraseResuelta()[i] == 'Ó')) {
			  fraseOculta_[i] = fraseResuelta_[i];
			  		  n++;
		  }

		  else if ((c == 'u' || c == 'U') && (getFraseResuelta()[i] == 'ú' || getFraseResuelta()[i] == 'Ú')) {
			  fraseOculta_[i] = fraseResuelta_[i];
			  		  n++;
		  }

  }

u.setScore(u.getScore() - 50);
SetUserWTurn(u);

  return n;
}



bool Ruleta :: resolver(string frase){

	string fraseAux = "RESOLVER ";
	fraseAux += getFraseResuelta();
	fraseAux += "\n";

 	cout << "FraseResuelta: +" << getFraseResuelta() << "+" << endl;
 	cout << "frase con RESOLVER: +" << fraseAux << "+" << endl;
 	cout << "FraseOtorgada: +" << frase << "+" << endl;

 	if(fraseAux != frase){ return false;}

 	return true;
}


User Ruleta :: getChampion(){

  if(getUser1().getScore() > getUser2().getScore()){

    return getUser1();
  }

  return getUser2();
}



bool Ruleta :: existe(char c){
	for (size_t i = 0; i < getFraseOculta().length(); i++){


	    if(c == getFraseOculta()[i]){

	      return true;

	     }
     }

     return false;

}


