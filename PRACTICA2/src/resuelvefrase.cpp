/*
 * resuelvefrase.cpp
 *
 *  Created on: 11 oct. 2021
 *      Author: Jose Manuel Flores Barranco,
 *      	Aitana Delgado Cabanillas
 */

#include <iostream>

using namespace std;

void updatePuntos(int &puntos, char letra, bool encontrada);

string crearfraseoculta (string fraseResuelta) {
	string fraseOculta = "";

	for (int i = 0; i < fraseResuelta.length(); i++) {
		if(fraseResuelta[i] != ' ') {
			fraseOculta += "_";

		}
		else{
			fraseOculta += " ";
		}
	}

	return fraseOculta;
}

void mostrarfraseoculta(string fraseOculta) {
	string frase = "";
	for (int i = 0; i < fraseOculta.length(); i++) {
		frase += fraseOculta[i];
		frase += " ";
	}

	cout << "frase: " << frase << endl;
}


char solicitarLetra(int &puntos){
  	char letra;

	cout << "Indique la letra"<<endl;
	cin >> letra;

	while(letra == 'a' || letra == 'e' || letra == 'i' || letra == 'o' || letra == 'u'){

	    //comprobarPuntos();

	    if(puntos < 50){
			cout<<"No puede comprar una vocal"<<endl;
			cout<<"Indique una consonante"<<endl;
			cin >> letra;
        }

        else{ return letra;}

	 }


  	return letra;

}

//bool comprobarPuntos(puntos);

void updatePuntos (int &puntos,char letra, bool encontrada) {
	if(letra == 'a' || letra == 'e' || letra == 'i' || letra == 'o' || letra == 'u'){

    puntos -= 50;
  	} else {
  		if (encontrada) {
  			puntos +=50;
  		}
  	}
}

bool comprobarLetra(string &fraseOculta, string &fraseResuelta, char letra, int &puntos) {
	bool encontrada = false;
	for (int i = 0; i < fraseResuelta.length(); i++) {
		if (fraseResuelta[i] == letra) {
			fraseOculta[i] = fraseResuelta[i];

			encontrada = true;
			cout << "Letra encontradaaa" << endl;
		}
	}
	updatePuntos(puntos, letra, encontrada);
	return encontrada;
}

bool isfraseResuelta(string fraseOculta, string fraseResuelta) {
	if (fraseOculta == fraseResuelta) {
		return true;
	}
	else {
		return false;
	}
}

int main(int argc, char **argv) {

	bool resuelta = false;
	int puntos = 5000;
	char aux;

	string fraseResuelta = "El sufrimiento es lo que hace madurar a las personas";
	string fraseOculta = crearfraseoculta(fraseResuelta);
	//cout << "frase: " << fraseResuelta;
	mostrarfraseoculta(fraseOculta);

	do {

	aux = solicitarLetra(puntos);
	if(comprobarLetra(fraseOculta, fraseResuelta, aux, puntos)) {
		cout << "Buena esaa" << endl;
	}
	else {
		cout << "Cagaste, cambio de turno" << endl;
	}
	cout << "Puntos actuales: " << puntos << endl;
	mostrarfraseoculta(fraseOculta);

	} while(isfraseResuelta(fraseOculta, fraseResuelta) == false);
}
