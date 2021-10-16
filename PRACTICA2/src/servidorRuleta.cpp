/*
 * servidorRuleta.cpp
 *
 *  Created on: 12 oct. 2021
 *      Author: Jose Manuel Flores Barranco
 				Aitana Delgado Cabanillas
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#include <vector>
#include <queue>
#include <ctime>
#include <ctype.h>

#include "User.h"
#include "Ruleta.h"

#define MSG_SIZE 250
#define MAX_CLIENTS 30

using namespace std;


/*
 * El servidor ofrece el servicio de una ruleta de la suerte
 */

/**
 * Devuelve el indice del vector que coincide con el descriptor pasado como parametro
 * @return -1 si falla
 * 		    n siendo n el indice del vector
 */
int getRuletaByUser(User &u, vector <Ruleta> &ruletas);

int getUserBySD(const int sd, vector <User> &usuarios);

bool esVocal(char letra);
bool esConsonante(char letra);

bool usernameFree(vector <User> &usuarios, string nombre);

void manejador(int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]);

int main ( ) {

  	srand(time(NULL));		//Inicializamos semilla

	queue <User> ready;
	char aux[100];
	char aux1[100]; //Para almacenar variables en sscanf()
	char char_array[50];
	char char_array2[50];
	char c;
	int index, indexR;	//Indices de los vectores de usuarios y ruletas

	/*----------------------------------------------------
		Descriptor del socket y buffer de datos
	-----------------------------------------------------*/
	int sd, new_sd;
	struct sockaddr_in sockname, from;
	char buffer[MSG_SIZE];
	socklen_t from_len;
    fd_set readfds, auxfds;
   	int salida;
   	int arrayClientes[MAX_CLIENTS];
    int numClientes = 0;
	int recibidos;



    int on, ret;

	/*----------------------------------------------------
		USUARIOS
	-----------------------------------------------------*/

	User u, u2;
	vector <User> usuarios;


	/*----------------------------------------------------
		RULETAS
	-----------------------------------------------------*/
	Ruleta r;
	vector <Ruleta> ruletas;

	/* --------------------------------------------------
		Se abre el socket
	---------------------------------------------------*/
  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
    		exit (1);
	}

    	// Activaremos una propiedad del socket para permitir· que otros
    	// sockets puedan reutilizar cualquier puerto al que nos enlacemos.
    	// Esto permite· en protocolos como el TCP, poder ejecutar un
    	// mismo programa varias veces seguidas y enlazarlo siempre al
   	 	// mismo puerto. De lo contrario habrÌa que esperar a que el puerto
    	// quedase disponible (TIME_WAIT en el caso de TCP)
    	on=1;
    	ret = setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));



	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2050);
	sockname.sin_addr.s_addr =  INADDR_ANY;

	if (bind (sd, (struct sockaddr *) &sockname, sizeof (sockname)) == -1)
	{
		perror("Error en la operación bind");
		exit(1);
	}


   	/*---------------------------------------------------------------------
		De las peticiones que vamos a aceptar sólo necesitamos el
		tamaño de su estructura, el resto de información (familia, puerto,
		ip), nos la proporcionará el método que recibe las peticiones.
   	----------------------------------------------------------------------*/
	from_len = sizeof (from);


	if(listen(sd,1) == -1){
		perror("Error en la operación de listen");
		exit(1);
	}

	//Inicializar los conjuntos fd_set
    	FD_ZERO(&readfds);
    	FD_ZERO(&auxfds);
    	FD_SET(sd,&readfds);
    	FD_SET(0,&readfds);


    	//Capturamos la señal SIGINT (Ctrl+c)
    	signal(SIGINT,manejador);

	/*-----------------------------------------------------------------------
		El servidor acepta una petición
	------------------------------------------------------------------------ */
		while(1){

            //Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya conectados)

            auxfds = readfds;

            salida = select(FD_SETSIZE,&auxfds,NULL,NULL,NULL);


            if(salida > 0){


                for(int i=0; i<FD_SETSIZE; i++){
                    //Buscamos el socket por el que se ha establecido la comunicación
                    if(FD_ISSET(i, &auxfds)) {
                    	// ES UN SOCKET NUEVO
                        if( i == sd){
                        	//COMPROBAMOS SI LA LECTURA DEL SOCKET ES VALIDA
                            if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1){
                                perror("Error aceptando peticiones");
                            }
                            else
                            {//COMPROBAMOS SI SE PUEDE ACEPTAR LA PETICION DE CONEXION DEL NUEVO CLIENTE
                                if(numClientes < MAX_CLIENTS){
                                    arrayClientes[numClientes] = new_sd;
                                    numClientes++;
                                    FD_SET(new_sd,&readfds);

                                    strcpy(buffer, "Bienvenido a la RULETA DE LA SUERTE\n");
                                    send(new_sd,buffer,sizeof(buffer),0);

                                    bzero(buffer,sizeof(buffer));

                                    strcpy(buffer, "+Ok. Usuario conectado\n");
                                    send(new_sd,buffer,sizeof(buffer),0);

                                    u = User(new_sd);
                                    u.setSD(new_sd);
                                    u.setState(0);
                                    if(usuarios.size() <= 30){usuarios.push_back(u);}
                                }
                                else
                                {
                                    bzero(buffer,sizeof(buffer)); //Limpiar buffer
                                    strcpy(buffer,"Demasiados clientes conectados\n");
                                    send(new_sd,buffer,sizeof(buffer),0);
                                    close(new_sd);
                                }

                            }


                        }
                        else if (i == 0){ //EL SOCKET ES EL PROPIO SERVIDOR
                            //Se ha introducido información de teclado
                            bzero(buffer, sizeof(buffer));
                            fgets(buffer, sizeof(buffer),stdin);

                            //Controlar si se ha introducido "SALIR", cerrando todos los sockets y finalmente saliendo del servidor. (implementar)
                            if(strcmp(buffer,"SALIR\n") == 0){

                                for (int j = 0; j < numClientes; j++){
						  			 bzero(buffer, sizeof(buffer));
						  			strcpy(buffer,"Desconexión servidor\n");
                                    send(arrayClientes[j],buffer , sizeof(buffer),0);

                                    bzero(buffer, sizeof(buffer));
									strcpy(buffer,"DECONNECTION_SERVICE: 2574847E32P");
									send(arrayClientes[j],buffer , sizeof(buffer),0);

                                    close(arrayClientes[j]);
                                    FD_CLR(arrayClientes[j],&readfds);
                                }
                                    close(sd);
                                    exit(-1);


                            }
                            //Mensajes que se quieran mandar a los clientes (implementar)

                        }
                        else{ // ES UN MENSAJE DE UN CLIENTE
                        	index = getUserBySD(i, usuarios);
                            bzero(buffer,sizeof(buffer));

                            recibidos = recv(i,&buffer,sizeof(buffer),0);

                            if(recibidos > 0){
								//QUITAR CUANDO ACABEMOS PRUEBAS
                                /*if(strcmp(buffer,"SALIR\n") == 0){

                                    salirCliente(i,&readfds,&numClientes,arrayClientes);

                                }
                                else{*/
                            	cout << "USUARIO ESTADO: " << usuarios[index].getState() << endl;
                                	cout << "EN EL ELSE\n";
                                	cout << "USUARIO ACTIVO: " << usuarios[index].getUsername() << endl;
                                	if (usuarios[index].getState() == 0) {
                                    	cout << "VALOR USUARIO: "<< buffer << "\n";
                                    	cout << "ES EL = " << usuarios[index].getSD() << "\n";


                                		if((sscanf(buffer, "USUARIO %s", buffer) == 1) && !usuarios[index].hasUsername()){
                                        	cout << "VALOR USUARIO: "<< buffer << "\n";
                                			if(usuarios[index].validateUsername(buffer) && usernameFree(usuarios, buffer)){
												usuarios[index].setUsername(buffer);

												bzero(buffer,sizeof(buffer));
												strcpy(buffer,"+OK. Usuario correcto\n");
												send(usuarios[index].getSD(), buffer,sizeof(buffer),0);
                                			}



                                			else{
                                				cout << "VALOR USUARIO: \n";
	                                			bzero(buffer,sizeof(buffer));
	                                			strcpy(buffer,"-Err. Ususario incorrecto\n");
	                                			send(usuarios[index].getSD(), buffer,sizeof(buffer),0);

                                			}
                                		}

                                		else if (usuarios[index].hasUsername() && sscanf(buffer, "PASSWORD %s", buffer) == 1) {
                                			if (usuarios[index].validatePassword(buffer)) {

												usuarios[index].setPassword(buffer);

												bzero(buffer,sizeof(buffer));
												strcpy(buffer,"+OK. Usuario validado\n");
												send(usuarios[index].getSD(), buffer,sizeof(buffer),0);

												usuarios[index].setState(1);
												bzero(buffer,sizeof(buffer));
                                			}
                                			else {
	                                			bzero(buffer,sizeof(buffer));
	                                			strcpy(buffer,"-Err. Error en la validación \n");
	                                			send(usuarios[index].getSD(), buffer,sizeof(buffer),0);
                                			}
                                		}

                                		else if ((sscanf(buffer, "REGISTER -u %s -p %s", aux, aux1) == 2) ){

											if(!usuarios[index].hasUsername() && !usuarios[index].hasPassword() && !usuarios[index].validateUsername(aux)){

													usuarios[index].writeUser(aux, aux1);


													bzero(buffer,sizeof(buffer));
													strcpy(buffer,"+OK. Usuario registrado\n");
													send(usuarios[index].getSD(), buffer,sizeof(buffer),0);
		                      				}
		                      				else if(usuarios[index].hasUsername()){
                                    			bzero(buffer,sizeof(buffer));
												strcpy(buffer,"-Err. No puede realizar un registro mientras se logea\n");
												send(usuarios[index].getSD(), buffer,sizeof(buffer),0);
                                  			}

		                      				else{

                                    			bzero(buffer,sizeof(buffer));
												strcpy(buffer,"-Err. Credenciales existentes\n");
												send(usuarios[index].getSD(), buffer,sizeof(buffer),0);
                                  			}

                                    	}

                                		else if(strcmp(buffer,"SALIR\n") == 0){

											bzero(buffer,sizeof(buffer));
											sprintf(buffer,"+Ok. 1Desconexión procesada\n");
											send(usuarios[index].getSD(), buffer,sizeof(buffer),0);


											bzero(buffer,sizeof(buffer));
											sprintf(buffer,"DECONNECTION_SERVICE: 2574847E32P");
											send(usuarios[index].getSD(), buffer,sizeof(buffer),0);

											auto elem_to_remove = usuarios.begin() + index;
											if (elem_to_remove != usuarios.end()) {
												usuarios.erase(elem_to_remove);
											}
											else {
												usuarios.pop_back();
											}
											salirCliente(i,&readfds,&numClientes,arrayClientes);

                                		}

                                    	else if(sscanf(buffer, "%s", aux) == 1) {

                                   			bzero(buffer,sizeof(buffer));
											sprintf(buffer,"-Err.1Comando no valido\n");
											send(usuarios[index].getSD(), buffer,sizeof(buffer),0);
											}

                                	}

                                	if (usuarios[index].getState() == 1) {
                                		if(strcmp(buffer,"INICIAR-PARTIDA\n") == 0) {

											if(ready.empty()){
                        						ready.push(usuarios[index]);

                        						bzero(buffer, sizeof(buffer));
												strcpy(buffer,"+Ok. Petición Recibida. Quedamos a la espera de más jugadores\n");
												send(usuarios[index].getSD(),buffer , sizeof(buffer),0);

                        						bzero(buffer,sizeof(buffer));
                        						strcpy(buffer,"+OK. Emparejando...\n");
                        						send(usuarios[index].getSD(), buffer,sizeof(buffer),0);
                        						usuarios[index].setState(2);
                     						}

											else {
												int index_2 = getUserBySD(ready.front().getSD(), usuarios);

												r = Ruleta(usuarios[index],usuarios[index_2]);
												ready.pop();

												r.setUser1State(4); //El jugador que sale de la cola empieza el turno
												r.setUser2State(3); //El otro jugador espera turno

												r.getUser1().setUsername(usuarios[index].getUsername());
												r.getUser2().setUsername(usuarios[index_2].getUsername());

												usuarios[index] = r.getUser1();
												usuarios[index_2] = r.getUser2();

												if(ruletas.size() <= 10){
													ruletas.push_back(r);

													ruletas[ruletas.size()-1] = r;
												}


												    // copying the contents of the
												    // string to char array
												strcpy(char_array, r.getUser1().getUsername().c_str());

												bzero(buffer,sizeof(buffer));
												sprintf(buffer, "+OK. Rival encontrado: %s\n Iniciando partida...", char_array);
												send(r.getUser2().getSD(), buffer,sizeof(buffer),0);

												strcpy(char_array, r.getUser2().getUsername().c_str());

												bzero(buffer,sizeof(buffer));
												sprintf(buffer, "+OK. Rival encontrado: %s\n Iniciando partida...", char_array);
												send(r.getUser1().getSD(), buffer,sizeof(buffer),0);

												strcpy(char_array, r.mostrarFraseOculta().c_str());

												bzero(buffer,sizeof(buffer));
												sprintf(buffer,"+OK.La frase a resolver es: %s\n",char_array);
												send(r.getUser2().getSD(), buffer,sizeof(buffer),0);

												bzero(buffer,sizeof(buffer));
												sprintf(buffer,"+OK.La frase a resolver es: %s\n",char_array);
												send(r.getUser1().getSD(), buffer,sizeof(buffer),0);


												strcpy(char_array, r.getUser2().getUsername().c_str());

												bzero(buffer,sizeof(buffer));
												sprintf(buffer,"+OK.Es tu turno %s\n",char_array);
												send(r.getUser2().getSD(), buffer,sizeof(buffer),0);

												bzero(buffer,sizeof(buffer));
												sprintf(buffer,"+OK.El turno es de: %s\n",char_array);
												send(r.getUser1().getSD(), buffer,sizeof(buffer),0);

											}
                                		}

                                		else if(strcmp(buffer,"SALIR\n") == 0){

											bzero(buffer,sizeof(buffer));
											sprintf(buffer,"Saliendo del juego. Usuario %s...\n", usuarios[index].getUsername());
											send(usuarios[index].getSD(), buffer,sizeof(buffer),0);
											bzero(buffer,sizeof(buffer));
											sprintf(buffer,"+Ok. 2Desconexión procesada\n");
											send(usuarios[index].getSD(), buffer,sizeof(buffer),0);

											bzero(buffer,sizeof(buffer));
											sprintf(buffer,"DECONNECTION_SERVICE: 2574847E32P");
											send(usuarios[index].getSD(), buffer,sizeof(buffer),0);

											auto elem_to_remove = usuarios.begin() + index;
											if (elem_to_remove != usuarios.end()) {
												usuarios.erase(elem_to_remove);
											}
											else {
												usuarios.pop_back();
											}

											salirCliente(i,&readfds,&numClientes,arrayClientes);
                                		}

                                		else if(sscanf(buffer, "%s", aux) == 1){
                                			cout << "AUX: " << aux << endl;
                                   			bzero(buffer,sizeof(buffer));
											sprintf(buffer,"-Err. 2CHOTAComando no valido\n");
											send(usuarios[index].getSD(), buffer,sizeof(buffer),0);
											}
                                	}

                                	else if (usuarios[index].getState() == 2) {

                                		//SI TIENE ESTADO 2 ESTÁ ESPERANDO EMPAREJAMIENTO Y NO PUEDE EJECUTAR ACCIONES
                                		if(strcmp(buffer,"SALIR\n") == 0){
                                			bzero(buffer,sizeof(buffer));
											sprintf(buffer,"Saliendo del juego usuario %s...\n", usuarios[index].getUsername());
											send(usuarios[index].getSD(), buffer,sizeof(buffer),0);

											ready.pop();
											usuarios[index].setState(1);
                                		}

                                		else if(sscanf(buffer, "%s", aux) == 1){

                                   			bzero(buffer,sizeof(buffer));
											sprintf(buffer,"-Err.3Comando no valido\n");
											send(usuarios[index].getSD(), buffer,sizeof(buffer),0);
											}
                                	}

                                	else if (usuarios[index].getState() == 3) {

										int nVeces= 0;
										cout << "INDEX: " << index << endl;
										indexR = getRuletaByUser(usuarios[index], ruletas);
										cout << "INDEXR: " << indexR << endl;
                                		int index_2 = getUserBySD(ruletas[indexR].UserWoutTurn().getSD(), usuarios);
                                		cout << "INDEX_2: " << index_2 << endl;


										cout<<"Entro mariconchi\n";

                                		if(sscanf(buffer, "CONSONANTE %c", &c) == 1) {
                                			cout<<"Entiendo la consonante\n";

                                			if(ruletas[indexR].existe(c)){
													bzero(buffer,sizeof(buffer));
			                                		strcpy(buffer,"-Err. Consonante ya encontrada. Intentelo de nuevo\n");
			                                		send(r.UserWTurn().getSD(), buffer,sizeof(buffer),0);
                       					 	}

                                			if (esConsonante(c)) {
                                				cout<<"Es consonante\n";
												if (nVeces = ruletas[indexR].comprobarConsonante(c) > 0) { // @suppress("Assignment in condition")
														cout << " n veces "<< nVeces<<endl;
																									    // copying the contents of the
																									    // string to char array
														strcpy(char_array, ruletas[indexR].mostrarFraseOculta().c_str());

														bzero(buffer,sizeof(buffer));
														cout << "CHOTAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n"<<endl;
														sprintf(buffer,"+OK.**%c** aparece %i veces. Frase: %s\n", c, nVeces, char_array);
														cout << "CHOTAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA2\n"<<endl;
														send(ruletas[indexR].UserWTurn().getSD(), buffer,sizeof(buffer),0);
														cout << "CHOTAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA3\n"<<endl;

														bzero(buffer,sizeof(buffer));
														sprintf(buffer,"+OK.**%c** aparece %i veces. Frase: %s\n", c, nVeces, char_array);
														send(ruletas[indexR].UserWoutTurn().getSD(), buffer,sizeof(buffer),0);

														if (ruletas[indexR].isFraseResuelta()) {

															strcpy(char_array, ruletas[indexR].getFraseResuelta().c_str());

															strcpy(char_array2, ruletas[indexR].getChampion().getUsername().c_str());

															bzero(buffer,sizeof(buffer));
															sprintf(buffer,"+Ok. Partida finalizada. frase: %s.Ha ganado el jugador %s, con puntuación %d\n",char_array, char_array2, ruletas[indexR].getChampion().getScore());
															send(ruletas[indexR].UserWoutTurn().getSD(), buffer,sizeof(buffer),0);

															u = ruletas[indexR].getUser1();
															u.setState(1);
															ruletas[indexR].setUser1(u);
															u = ruletas[indexR].getUser2();
															u.setState(1);
															ruletas[indexR].setUser2(u);
															usuarios[index] = ruletas[indexR].getUser2();
															usuarios[index_2] = ruletas[indexR].getUser1();

															auto elem_to_remove = ruletas.begin() + indexR;
															if (elem_to_remove != ruletas.end()) {
																ruletas.erase(elem_to_remove);
															}
															else {
																ruletas.pop_back();
															}

														}

                                				}

                                				else{
                                					cout<<"No SE ENCONTROe\n";
													bzero(buffer,sizeof(buffer));
													cout << "AQUI\n";
													strcpy(buffer,"-Err. Consonante no encontrada. Pierde turno\n");
													cout << "A\n";
													cout << "VALOR A: " << ruletas[indexR].UserWTurn().getUsername() << endl;
													send(r.UserWTurn().getSD(), buffer,sizeof(buffer),0);

													strcpy(char_array, ruletas[indexR].UserWoutTurn().getUsername().c_str());

													bzero(buffer,sizeof(buffer));
													sprintf(buffer,"+OK.Es tu turno %s\n",char_array);
													send(ruletas[indexR].UserWoutTurn().getSD(), buffer,sizeof(buffer),0);

													bzero(buffer,sizeof(buffer));
													sprintf(buffer,"+OK.El turno es de: %s\n",char_array);
													send(ruletas[indexR].UserWTurn().getSD(), buffer,sizeof(buffer),0);

													if (ruletas[indexR].getUser1() == ruletas[indexR].UserWTurn()) {
														u = ruletas[indexR].getUser1();
														u.setState(4);
														ruletas[indexR].setUser1(u);
														u = ruletas[indexR].getUser2();
														u.setState(3);
														ruletas[indexR].setUser2(u);
														usuarios[index] = ruletas[indexR].getUser1();
														usuarios[index_2] = ruletas[indexR].getUser2();
													}

													else if (ruletas[indexR].getUser2() == ruletas[indexR].UserWTurn()) {
														u = ruletas[indexR].getUser1();
														u.setState(3);
														ruletas[indexR].setUser1(u);
														u = ruletas[indexR].getUser2();
														u.setState(4);
														ruletas[indexR].setUser2(u);
														usuarios[index] = ruletas[indexR].getUser2();
														usuarios[index_2] = ruletas[indexR].getUser1();
													}

                                       	 		}

                                			}
                                			else {
                                				cout<<"NO consonante\n";
                                				bzero(buffer,sizeof(buffer));
												strcpy(buffer,"-Err. Debe introducir una consonante, intentelo de nuevo\n");
												send(ruletas[indexR].UserWTurn().getSD(), buffer,sizeof(buffer),0);
                                			}
                                		}

                                		else if(sscanf(buffer, "VOCAL %c", &c) == 1){

                                			if (esVocal(c)){
												cout <<"Holo putorro, soy una vocal"<<endl;
												if(ruletas[indexR].existe(c)){
													bzero(buffer,sizeof(buffer));
			                                		strcpy(buffer,"-Err. Vocal ya encontrada. Intentelo de nuevo\n");
			                                		send(r.UserWTurn().getSD(), buffer,sizeof(buffer),0);
                       					 		}

												else if(nVeces = ruletas[indexR].comprobarVocal(c) > 0){ // @suppress("Assignment in condition")
													cout <<"Holo putorro, soy una vocal"<<endl;

													// copying the contents of the
													// string to char array
                                					strcpy(char_array, ruletas[indexR].mostrarFraseOculta().c_str());

                                            		bzero(buffer,sizeof(buffer));
                                            		cout <<"Holo putorro, soy una vocal1"<<endl;
			                                		sprintf(buffer,"+OK.**%c** aparece %i veces. Frase: %s\n", c, nVeces, char_array);
			                                		cout <<"Holo putorro, soy una vocal 2"<<endl;
			                                		send(ruletas[indexR].UserWTurn().getSD(), buffer,sizeof(buffer),0);
			                                		cout <<"Holo putorro, soy una vocal 3"<<endl;

			                                		bzero(buffer,sizeof(buffer));
			                                		sprintf(buffer,"+OK.**%c** aparece %i veces. Frase: %s\n", c, nVeces, char_array);
			                                		send(ruletas[indexR].UserWoutTurn().getSD(), buffer,sizeof(buffer),0);
			                                		cout <<"Holo putorro, soy una vocal 4"<<endl;

			                                		if (ruletas[indexR].isFraseResuelta()) {

														strcpy(char_array, ruletas[indexR].getFraseResuelta().c_str());

														strcpy(char_array2, ruletas[indexR].getChampion().getUsername().c_str());

														bzero(buffer,sizeof(buffer));
														sprintf(buffer,"+Ok. Partida finalizada. frase: %s.Ha ganado el jugador %s, con puntuación %d\n",char_array, char_array2, ruletas[indexR].getChampion().getScore());
														send(ruletas[indexR].UserWoutTurn().getSD(), buffer,sizeof(buffer),0);

														bzero(buffer,sizeof(buffer));
														sprintf(buffer,"+Ok. Partida finalizada. frase: %s.Ha ganado el jugador %s, con puntuación %d\n",char_array, char_array2, ruletas[indexR].getChampion().getScore());
														send(ruletas[indexR].UserWTurn().getSD(), buffer,sizeof(buffer),0);

														u = ruletas[indexR].getUser1();
														u.setState(1);
														ruletas[indexR].setUser1(u);
														u = ruletas[indexR].getUser2();
														u.setState(1);
														ruletas[indexR].setUser2(u);
														usuarios[index] = ruletas[indexR].getUser2();
														usuarios[index_2] = ruletas[indexR].getUser1();

														auto elem_to_remove = ruletas.begin() + indexR;
														if (elem_to_remove != ruletas.end()) {
															ruletas.erase(elem_to_remove);
														}
														else {
															ruletas.pop_back();
														}

													}

                                          		}
												else if (ruletas[indexR].UserWTurn().getScore() < 50) {
													bzero(buffer,sizeof(buffer));
			                                		strcpy(buffer,"-OK. No tiene puntuacion suficiente\n. Intentelo de nuevo\n");
			                                		send(r.UserWTurn().getSD(), buffer,sizeof(buffer),0);
												}

                                          		else{
                                                	bzero(buffer,sizeof(buffer));
			                                		strcpy(buffer,"-Err. Vocal no encontrada. Pierde turno\n");
			                                		send(r.UserWTurn().getSD(), buffer,sizeof(buffer),0);


			                                		strcpy(char_array, ruletas[indexR].UserWoutTurn().getUsername().c_str());

													bzero(buffer,sizeof(buffer));
													sprintf(buffer,"+OK.Es tu turno %s\n",char_array);
													send(ruletas[indexR].UserWoutTurn().getSD(), buffer,sizeof(buffer),0);

													bzero(buffer,sizeof(buffer));
													sprintf(buffer,"+OK.El turno es de: %s\n",char_array);
													send(ruletas[indexR].UserWTurn().getSD(), buffer,sizeof(buffer),0);

													if (ruletas[indexR].getUser1() == ruletas[indexR].UserWTurn()) {
														cout << "EL USER 1 TIENE EL TURNO\n";
														u = ruletas[indexR].getUser1();
														u.setState(4);
														ruletas[indexR].setUser1(u);
														u = ruletas[indexR].getUser2();
														u.setState(3);
														ruletas[indexR].setUser2(u);
														usuarios[index] = ruletas[indexR].getUser1();
														usuarios[index_2] = ruletas[indexR].getUser2();
													}

													else if (ruletas[indexR].getUser2() == ruletas[indexR].UserWTurn()) {
														u = ruletas[indexR].getUser1();
														u.setState(3);
														ruletas[indexR].setUser1(u);
														u = ruletas[indexR].getUser2();
														u.setState(4);
														ruletas[indexR].setUser2(u);
														usuarios[index] = ruletas[indexR].getUser2();
														usuarios[index_2] = ruletas[indexR].getUser1();
													}

                                              	}

                                			} else {
                                				bzero(buffer,sizeof(buffer));
												strcpy(buffer,"-Err. Debe introducir una vocal, intentelo de nuevo\n");
												send(ruletas[indexR].UserWTurn().getSD(), buffer,sizeof(buffer),0);
                                			}
                                		}
                                		else if(sscanf(buffer, "RESOLVER %s", aux) == 1) {
                                			cout << "ELO\n";

                                			if (ruletas[indexR].resolver(buffer)) {
                                        			bzero(buffer,sizeof(buffer));
													strcpy(buffer,"+OK. Resolviste la frase. Has ganado\n");
													send(ruletas[indexR].UserWTurn().getSD(), buffer,sizeof(buffer),0);


													strcpy(char_array, ruletas[indexR].UserWTurn().getUsername().c_str());
													strcpy(char_array2, ruletas[index].getFraseResuelta().c_str());

													bzero(buffer,sizeof(buffer));
			                                		sprintf(buffer,"+Ok. Partida finalizada. FRASE: %s. Ha ganado el jugador %s con %d puntos.\n", char_array2, char_array, ruletas[index].UserWTurn().getScore());
			                                		send(ruletas[indexR].UserWoutTurn().getSD(), buffer,sizeof(buffer),0);

			                                		bzero(buffer,sizeof(buffer));
													strcpy(buffer,"La solucion era :\n");
													send(ruletas[indexR].UserWoutTurn().getSD(), buffer,sizeof(buffer),0);

													strcpy(char_array, ruletas[index].getFraseResuelta().c_str());

													bzero(buffer,sizeof(buffer));
													sprintf(buffer,"%s\n", char_array);
													send(ruletas[indexR].UserWoutTurn().getSD(), buffer,sizeof(buffer),0);


													u = ruletas[indexR].getUser1();
													u.setState(1);
													ruletas[indexR].setUser1(u);
													u = ruletas[indexR].getUser2();
													u.setState(1);
													ruletas[indexR].setUser2(u);
													usuarios[index] = ruletas[indexR].getUser2();
													usuarios[index_2] = ruletas[indexR].getUser1();

													auto elem_to_remove = ruletas.begin() + indexR;
													if (elem_to_remove != ruletas.end()) {
														ruletas.erase(elem_to_remove);
													}
													else {
														ruletas.pop_back();
													}

                                			}

                                			else {
                                          			bzero(buffer,sizeof(buffer));

                                          			strcpy(char_array, ruletas[index].getFraseResuelta().c_str());

													bzero(buffer,sizeof(buffer));
			                                		sprintf(buffer,"+Ok. Partida finalizada. FRASE: <refrán>. No se ha acertado la frase.”\n", char_array);
			                                		send(ruletas[indexR].UserWTurn().getSD(), buffer,sizeof(buffer),0);

			                                		strcpy(char_array, ruletas[indexR].getChampion().getUsername().c_str());

			                                		bzero(buffer,sizeof(buffer));
			                                		sprintf(buffer,"Ha ganado el jugador %s, con puntuación %d\n", char_array, ruletas[indexR].getChampion().getScore());
			                                		send(ruletas[indexR].UserWoutTurn().getSD(), buffer,sizeof(buffer),0);

			                                		u = ruletas[indexR].getUser1();
													u.setState(1);
													ruletas[indexR].setUser1(u);
													u = ruletas[indexR].getUser2();
													u.setState(1);
													ruletas[indexR].setUser2(u);
													usuarios[index] = ruletas[indexR].getUser2();
													usuarios[index_2] = ruletas[indexR].getUser1();

													auto elem_to_remove = ruletas.begin() + indexR;
													if (elem_to_remove != ruletas.end()) {
														ruletas.erase(elem_to_remove);
													}
													else {
														ruletas.pop_back();
													}

                                			}

                                		}
                                		else if(strcmp(buffer,"PUNTUACION\n") == 0) {
                                      		cout << "Score"<< ruletas[indexR].UserWTurn().getScore()<< endl;
                                			bzero(buffer,sizeof(buffer));
											sprintf(buffer, "+OK. Puntuacion: %i\n", ruletas[indexR].UserWTurn().getScore());
											send(ruletas[indexR].UserWTurn().getSD(), buffer,sizeof(buffer),0);
                                		}

                                		else if(strcmp(buffer,"SALIR\n") == 0){
                                			strcpy(char_array, usuarios[index].getUsername().c_str());

											bzero(buffer,sizeof(buffer));
											sprintf(buffer,"Saliendo del juego usuario %s...\n", char_array);
											send(ruletas[indexR].UserWTurn().getSD(), buffer,sizeof(buffer),0);

											//+Ok. Ha salido el otro jugador. Finaliza la partida.

											bzero(buffer,sizeof(buffer));
											sprintf(buffer,"+Ok. Ha salido el otro jugador. Finaliza la partida.\n");
											send(ruletas[indexR].UserWTurn().getSD(), buffer,sizeof(buffer),0);

											u = ruletas[indexR].getUser1();
											u.setState(1);
											ruletas[indexR].setUser1(u);
											u = ruletas[indexR].getUser2();
											u.setState(1);
											ruletas[indexR].setUser2(u);
											usuarios[index] = ruletas[indexR].getUser2();
											usuarios[index_2] = ruletas[indexR].getUser1();

											auto elem_to_remove = ruletas.begin() + indexR;
											if (elem_to_remove != ruletas.end()) {
												ruletas.erase(elem_to_remove);
											}
											else {
												ruletas.pop_back();
											}
                                		}

                                		else{
                                      		bzero(buffer,sizeof(buffer));
											sprintf(buffer,"-Err.4Comando no valido\n");
											send(usuarios[index].getSD(), buffer,sizeof(buffer),0);



                                    	}


                                	}


                                	else if (usuarios[index].getState() == 4){

                                		indexR = getRuletaByUser(usuarios[index], ruletas);
                                		int index_2 = getUserBySD(ruletas[indexR].UserWTurn().getSD(), usuarios);

										if(strcmp(buffer,"PUNTUACION\n") == 0) {
                                			bzero(buffer,sizeof(buffer));
											sprintf(buffer, "+OK. Puntuacion: %i\n", ruletas[indexR].UserWoutTurn().getScore());
											send(ruletas[indexR].UserWoutTurn().getSD(), buffer,sizeof(buffer),0);
                                		}

										else if(strcmp(buffer,"SALIR\n") == 0){
											strcpy(char_array, usuarios[index].getUsername().c_str());
											bzero(buffer,sizeof(buffer));
											sprintf(buffer,"Saliendo del juego usuario %s...\n", char_array);
											send(usuarios[index].getSD(), buffer,sizeof(buffer),0);

											//+Ok. Ha salido el otro jugador. Finaliza la partida.

											bzero(buffer,sizeof(buffer));
											sprintf(buffer,"+Ok. Ha salido el otro jugador. Finaliza la partida.\n");
											send(usuarios[index_2].getSD(), buffer,sizeof(buffer),0);

											if (ruletas[indexR].getUser1() == ruletas[indexR].UserWTurn()) {
												u = ruletas[indexR].getUser1();
												u.setState(1);
												ruletas[indexR].setUser1(u);
												u = ruletas[indexR].getUser2();
												u.setState(1);
												ruletas[indexR].setUser2(u);
												usuarios[index] = ruletas[indexR].getUser1();
												usuarios[index_2] = ruletas[indexR].getUser2();
											}

											else if (ruletas[indexR].getUser2() == ruletas[indexR].UserWTurn()) {
												u = ruletas[indexR].getUser1();
												u.setState(1);
												ruletas[indexR].setUser1(u);
												u = ruletas[indexR].getUser2();
												u.setState(1);
												ruletas[indexR].setUser2(u);
												usuarios[index] = ruletas[indexR].getUser2();
												usuarios[index_2] = ruletas[indexR].getUser1();
											}

											auto elem_to_remove = ruletas.begin() + indexR;
											if (elem_to_remove != ruletas.end()) {
												ruletas.erase(elem_to_remove);
											}
											else {
												ruletas.pop_back();
											}
										}

                                		else{
                                			cout << "UUUUU: " << buffer << endl;
											bzero(buffer,sizeof(buffer));
											sprintf(buffer, "-Err.No es su turno\n");
											send(ruletas[indexR].UserWoutTurn().getSD(), buffer,sizeof(buffer),0);

											bzero(buffer,sizeof(buffer));
											sprintf(buffer, "-Err. Unicamente puede consultar su puntuación indicando PUNTUACION\n");
											send(ruletas[indexR].UserWoutTurn().getSD(), buffer,sizeof(buffer),0);

                                    	}
                                  }
                                //}
                            }
                            //Si el cliente introdujo ctrl+c
                            if(recibidos== 0)
                            {
                                printf("El socket %d, ha introducido ctrl+c\n", i);
                                //Eliminar ese socket
                                salirCliente(i,&readfds,&numClientes,arrayClientes);
                            }
                        }
                    }
                }
            }
		}
	close(sd);
	return 0;

}

int getRuletaByUser(User &u, vector <Ruleta> &ruletas) {
	int indice = -1;
	User u1, u2;

	for (size_t i = 0; i < ruletas.size(); i++) {
		if (ruletas[i].getUser1().getUsername() == u.getUsername()) {
			indice = i;
			cout << "\n //// RULETA U1: " << ruletas[i].getUser1().getUsername() << " Y " << u.getUsername() << endl;
		}
		else if (ruletas[i].getUser2().getUsername() == u.getUsername()) {
			indice = i;
			cout << "\n //// RULETA U2: " << ruletas[i].getUser2().getUsername() << " Y " << u.getUsername() << endl;
		}
		cout << "\n //// RULETA U1: " << ruletas[i].getUser1().getUsername() << " Y " << u.getUsername() << endl;
	}
	return indice;
}

int getUserBySD(const int sd, vector <User> &usuarios) {
	int indice = -1;

	for (size_t i = 0; i<usuarios.size(); i++) {
		if (usuarios[i].getSD() == sd) {
			indice = i;
			return indice;
		}
	}

	return indice;
}

void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]){

    char buffer[250];

    close(socket);
    FD_CLR(socket,readfds);

    //Re-estructurar el array de clientes
    for (int j = 0; j < (*numClientes) - 1; j++)
        if (arrayClientes[j] == socket)
            break;
    for (int j = 0; j < (*numClientes) - 1; j++)
        (arrayClientes[j] = arrayClientes[j+1]);

    (*numClientes)--;

    bzero(buffer,sizeof(buffer));
    sprintf(buffer,"Desconexión del cliente: %d\n",socket);

    for(int j=0; j<(*numClientes); j++)
        if(arrayClientes[j] != socket)
            send(arrayClientes[j],buffer,sizeof(buffer),0);


}

bool esVocal(char letra)
{
    // Convertir para simplificar comparaciones
    char letraMinuscula = tolower(letra);
    char vocales[] = "aeiou";
    // Recorrer las vocales y comparar si la letra es una de ellas
    int i;
    for (i = 0; vocales[i]; i++)
    {
        if (letraMinuscula == vocales[i])
        {
            return true;
        }
    }
    // Si terminamos de recorrer y no regresamos true dentro del for, entonces no es vocal
    return false;
}

bool esConsonante(char letra)
{
    return isalpha(letra) && !esVocal(letra);
}

bool usernameFree(vector <User> &usuarios, string nombre) {
	for (size_t i = 0; i < usuarios.size(); i++) {
		if (usuarios[i].getUsername() == nombre) {
			return false;
		}
	}
	return true;
}

void manejador (int signum){
    printf("\nSe ha recibido la señal sigint\n");
    signal(SIGINT,manejador);

    //Implementar lo que se desee realizar cuando ocurra la excepción de ctrl+c en el servidor
}
