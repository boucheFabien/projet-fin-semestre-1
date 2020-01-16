#include<iostream>
#include<fstream>
#include<SDL.h>
#include<SDL_ttf.h>
#include <SDL_image.h>
#include <string.h>
#include"structures.h"
#include"config_sdl.h"
using namespace std;

Article* tabarticle;
int NBARTICLE, CURRENT_ARTICLE = 0, ACTUAL_SAVE, nb_chemin = 1;
int* chemin;
int* chemin_save;
SDL_Rect Jouer, Editer, Quitter, Save01, Save02, Save03, Save04, Slot01, Slot02, Slot03, Slot04, New, Load, MenuB, GraphB;
SDL_Rect Fond, RetourJeu;
SDL_Surface* graphimg = NULL, * menuimg = NULL, * fondimg = NULL, * returnimg = NULL;

int initSDL() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		cout << "Echec à l’ouverture";
		return 1;
	}

	TTF_Init();

	font = TTF_OpenFont("C:\\Windows\\Fonts\\calibri.ttf", 100);
	bouton_menu_font = TTF_OpenFont("C:\\Windows\\Fonts\\calibri.ttf", 50);
	petite_font = TTF_OpenFont("C:\\Windows\\Fonts\\calibri.ttf", 25);

	win = SDL_CreateWindow("LDVELH",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		LARGEUR,
		HAUTEUR,
		SDL_WINDOW_SHOWN
	);
	if (win == NULL)
		cout << "erreur ouverture fenetre";

	rendu = SDL_CreateRenderer(
		win,
		-1,
		SDL_RENDERER_ACCELERATED);
}

void destroySDL() {
	SDL_DestroyRenderer(rendu);
	SDL_DestroyWindow(win);
	TTF_CloseFont(font);
	TTF_CloseFont(bouton_menu_font);
	TTF_CloseFont(petite_font);
	TTF_Quit();
	SDL_Quit();
}

void fond_ecran() {
	fondimg = IMG_Load("background.jpg");
	SDL_Texture* texture_fond = SDL_CreateTextureFromSurface(rendu, fondimg);
	SDL_QueryTexture(texture_fond, NULL, NULL, &Fond.w, &Fond.h);
	Fond.x = 0;
	Fond.y = 0;
	SDL_RenderCopy(rendu, texture_fond, NULL, &Fond);
	SDL_DestroyTexture(texture_fond);
	SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
	SDL_RenderDrawRect(rendu, &Fond);
	SDL_RenderPresent(rendu);
}

void Bmenu() {
	menuimg = IMG_Load("menu.png");
	SDL_Texture* texture_menu = SDL_CreateTextureFromSurface(rendu, menuimg);
	SDL_QueryTexture(texture_menu, NULL, NULL, &MenuB.w, &MenuB.h);
	MenuB.x = LARGEUR - MenuB.w - 20;
	MenuB.y = HAUTEUR - MenuB.h - 30;
	SDL_RenderCopy(rendu, texture_menu, NULL, &MenuB);
	SDL_DestroyTexture(texture_menu);
}

void Bgraph() {
	graphimg = IMG_Load("graph1.png");
	SDL_Texture* texture_graph = SDL_CreateTextureFromSurface(rendu, graphimg);
	SDL_QueryTexture(texture_graph, NULL, NULL, &GraphB.w, &GraphB.h);
	GraphB.x = LARGEUR - GraphB.w - 20;
	GraphB.y = HAUTEUR - GraphB.h - MenuB.h - 40;
	SDL_RenderCopy(rendu, texture_graph, NULL, &GraphB);
	SDL_DestroyTexture(texture_graph);
	SDL_RenderPresent(rendu);
}

void Bretour() {
	returnimg = IMG_Load("retour.png");
	SDL_Texture* texture_retour = SDL_CreateTextureFromSurface(rendu, returnimg);
	SDL_QueryTexture(texture_retour, NULL, NULL, &RetourJeu.w, &RetourJeu.h);
	RetourJeu.x = 0;
	RetourJeu.y = 0;
	SDL_RenderCopy(rendu, texture_retour, NULL, &RetourJeu);
	SDL_DestroyTexture(texture_retour);
}

void afficherMenu() {
	fond_ecran();

	SDL_Color noir = { 0, 0, 0 };
	SDL_Texture* texture_jouer = loadText(rendu, "Jouer", noir, font);
	SDL_QueryTexture(texture_jouer, NULL, NULL, &Jouer.w, &Jouer.h);
	Jouer.x = LARGEUR / 2 - Jouer.w / 2;
	Jouer.y = HAUTEUR / 4 - Jouer.h / 2;
	SDL_RenderCopy(rendu, texture_jouer, NULL, &Jouer);
	SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
	SDL_RenderDrawRect(rendu, &Jouer);
	SDL_DestroyTexture(texture_jouer);

	SDL_Texture* texture_editer = loadText(rendu, "Editer", noir, font);
	SDL_QueryTexture(texture_editer, NULL, NULL, &Editer.w, &Editer.h);
	Editer.x = LARGEUR / 2 - Editer.w / 2;
	Editer.y = 2 * (HAUTEUR / 4) - Editer.h / 2;
	SDL_RenderCopy(rendu, texture_editer, NULL, &Editer);
	SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
	SDL_RenderDrawRect(rendu, &Editer);
	SDL_DestroyTexture(texture_editer);

	SDL_Texture* texture_quitter = loadText(rendu, "Quitter", noir, font);
	SDL_QueryTexture(texture_quitter, NULL, NULL, &Quitter.w, &Quitter.h);
	Quitter.x = LARGEUR / 2 - Quitter.w / 2;
	Quitter.y = 3 * (HAUTEUR / 4) - Quitter.h / 2;
	SDL_RenderCopy(rendu, texture_quitter, NULL, &Quitter);
	SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
	SDL_RenderDrawRect(rendu, &Quitter);
	SDL_DestroyTexture(texture_quitter);
	SDL_RenderPresent(rendu);
}

void importer() {
	int nb_article, nb_liens;
	char tmp[5000];

	ifstream entree("testExport.lb", ios::in);
	if (!entree)
		cout << "Probleme d'ouverture" << endl;

	entree.getline(tmp, 4);			//nb article
	NBARTICLE = atoi(tmp);

	//création tableau
	tabarticle = new Article[NBARTICLE];

	for (int i = 0; i < NBARTICLE; i++)
	{
		entree.getline(tmp, 5, ',');		//id article
		tabarticle[i].id = atoi(tmp);		//conversion char / entier

		entree.getline(tmp, 5000);	//id texte article
		tabarticle[i].texte = new char[5000];
		strcpy_s(tabarticle[i].texte, 5000, tmp);

		entree.getline(tmp, 4);		//nb liens
		nb_liens = atoi(tmp);
		tabarticle[i].nbLiens = nb_liens;

		tabarticle[i].tLiens = new Lien[nb_liens];

		for (int j = 0; j < nb_liens; j++)
		{
			entree.getline(tmp, 4, ',');				//id destination
			tabarticle[i].tLiens[j].destination = atoi(tmp);

			entree.getline(tmp, 1000);	//texte destination
			tabarticle[i].tLiens[j].texte = new char[1000];
			strcpy_s(tabarticle[i].tLiens[j].texte, 1000, tmp);
		}
	}

	entree.close();

	chemin = new int[nb_chemin];
}

void save_chemin(Article a) {
	chemin_save = chemin;
	chemin[nb_chemin] = a.id;
	nb_chemin++;
	chemin = new int[nb_chemin];
	for (int i = 1; i < nb_chemin; i++) {
		chemin[i] = chemin_save[i];
	}
	//delete chemin_save;
	chemin[0] = 1;
}

int import_save(int num_partie) {
	Article save;
	char s[5];
	char n[5];
	char c[5];
	if (num_partie == 1) {
		ifstream entree("save01.txt", ios::in);
		if (!entree) {
			save.id = 0;
			entree.close();
			return 0;
		}
		else {
			entree.getline(s, '\n');
			save.id = atoi(s);
			entree.getline(n, '\n');
			nb_chemin = atoi(n);
			entree.getline(n, '\n');
			chemin = new int[nb_chemin];
			for (int i = 0; i < nb_chemin; i++) {
				entree.getline(c, '\n');
				chemin[i] = atoi(c);
			}
			entree.close();
			return save.id;
		}
	}
	if (num_partie == 2) {
		ifstream entree("save02.txt", ios::in);
		if (!entree) {
			save.id = 0;
			entree.close();
			return 0;
		}
		else {
			entree.getline(s, '\n');
			save.id = atoi(s);
			entree.getline(n, '\n');
			nb_chemin = atoi(n);
			entree.getline(n, '\n');
			chemin = new int[nb_chemin];
			for (int i = 0; i < nb_chemin; i++) {
				entree.getline(c, '\n');
				chemin[i] = atoi(c);
			}
			entree.close();
			return save.id;
		}
	}
	if (num_partie == 3) {
		ifstream entree("save03.txt", ios::in);
		if (!entree) {
			save.id = 0;
			entree.close();
			return 0;
		}
		else {
			entree.getline(s, '\n');
			save.id = atoi(s);
			entree.getline(n, '\n');
			nb_chemin = atoi(n);
			entree.getline(n, '\n');
			chemin = new int[nb_chemin];
			for (int i = 0; i < nb_chemin; i++) {
				entree.getline(c, '\n');
				chemin[i] = atoi(c);
			}
			entree.close();
			return save.id;
		}
	}
	if (num_partie == 4) {
		ifstream entree("save04.txt", ios::in);
		if (!entree) {
			save.id = 0;
			entree.close();
			return 0;
		}
		else {
			entree.getline(s, '\n');
			save.id = atoi(s);
			entree.getline(n, '\n');
			nb_chemin = atoi(n);
			entree.getline(n, '\n');
			chemin = new int[nb_chemin];
			for (int i = 0; i < nb_chemin; i++) {
				entree.getline(c, '\n');
				chemin[i] = atoi(c);
			}
			entree.close();
			return save.id;
		}
	}
}

int change_texte(Article& texte) {
	int longeur = strlen(texte.texte);

	int limite = 150;

	int curseur = limite;

	while (curseur < longeur) {
		while (texte.texte[curseur] != ' ' && (curseur < longeur)) {
			curseur++;
		}

		if (texte.texte[curseur] == ' ') {
			texte.texte[curseur] = '#';
			curseur = curseur + limite - 1;
		}
	}

	texte.texte[curseur + 1] = '#';

	return longeur;
}

void enlèvenul(Article& texte) {
	int longeur = strlen(texte.texte);

	for (int i = 0; i < longeur; i++) {
		if (texte.texte[i] == '#')
			texte.texte[i] = ' ';
	}
}

void affiche_texte(int id) {
	fond_ecran();

	int longeur = change_texte(tabarticle[id]);

	int emplacement = 30;

	int curseur = 0;

	int j = 0;

	char afficher[1000];

	for (int i = 0; i < 1000; i++) {
		afficher[i] = ' ';
	}

	while (curseur < longeur) {
		for (int i = 0; i < 1000; i++) {
			afficher[i] = ' ';
		}

		j = 0;

		int i = curseur;

		while (tabarticle[id].texte[i] != '#') {
			afficher[j] = tabarticle[id].texte[i];

			j++;

			curseur++;

			i++;
		}
		afficher[j] = '\0';
		j = 0;

		curseur++;

		SDL_Color noir = { 0,0,0 };

		SDL_Rect positionTexte;

		positionTexte.x = 30;
		positionTexte.y = emplacement;

		TTF_Font* font_temp = TTF_OpenFont("C:\\Windows\\Fonts\\FREESCPT.TTF", 38);
		SDL_Texture* texture = loadText(rendu, afficher, noir, font_temp);

		SDL_QueryTexture(texture, NULL, NULL, &positionTexte.w, &positionTexte.h);
		//si on veut modifier le cadre du texte

		SDL_RenderCopy(rendu, texture, NULL, &positionTexte);

		emplacement = emplacement + 40;
		SDL_DestroyTexture(texture);
		TTF_CloseFont(font_temp);
	}

	//on met à jour le rendu
	SDL_RenderPresent(rendu);

	enlèvenul(tabarticle[id]);
}

void affiche_liens(Article Choix) {
	TTF_Init();
	TTF_Font* font_temp = TTF_OpenFont("C:\\Windows\\Fonts\\calibri.ttf", 19);

	for (int i = 0; i < Choix.nbLiens; i++) {
		char tmp[1000];
		_itoa_s(Choix.tLiens[i].destination, tmp, 10);
		strcat_s(tmp, " --> ");
		strcat_s(tmp, Choix.tLiens[i].texte);
		SDL_Rect Choix1;
		Choix1.x = 50;
		Choix1.y = (HAUTEUR - 100) + i * 20;
		SDL_Color Blanc = { 255,255,255 };
		SDL_Texture* texture = loadText(rendu, tmp, Blanc, font_temp);	//Choix.tLiens[i].texte
		SDL_QueryTexture(texture, NULL, NULL, &Choix1.w, &Choix1.h);
		SDL_RenderCopy(rendu, texture, NULL, &Choix1);
	}
	SDL_RenderPresent(rendu);
	TTF_CloseFont(font_temp);
}

void menu_graph() {
	SDL_Rect Historique;
	SDL_Color noir = { 0, 0, 0 };
	SDL_Texture* texture_histo = loadText(rendu, "Historique", noir, font);
	SDL_QueryTexture(texture_histo, NULL, NULL, &Historique.w, &Historique.h);
	Historique.x = LARGEUR / 2 - Historique.w / 2;
	Historique.y = HAUTEUR / 3 - Historique.h / 2;
	SDL_RenderCopy(rendu, texture_histo, NULL, &Historique);
	SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
	SDL_RenderDrawRect(rendu, &Historique);
	SDL_DestroyTexture(texture_histo);

	SDL_Rect Complet;
	SDL_Texture* texture_complet = loadText(rendu, "Graph Complet", noir, font);
	SDL_QueryTexture(texture_complet, NULL, NULL, &Complet.w, &Complet.h);
	Complet.x = LARGEUR / 2 - Complet.w / 2;
	Complet.y = 2 * (HAUTEUR / 3) - Complet.h / 2;
	SDL_RenderCopy(rendu, texture_complet, NULL, &Complet);
	SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
	SDL_RenderDrawRect(rendu, &Complet);
	SDL_DestroyTexture(texture_complet);

	Bretour();

	SDL_RenderPresent(rendu);

	bool continuer_menu = true;
	while (continuer_menu) {
		SDL_Event event_menu;
		SDL_WaitEvent(&event_menu);

		switch (event_menu.type)
		{
		case SDL_QUIT:
			for (int i = 0; i < NBARTICLE; i++) {
				for (int j = 0; j < tabarticle[i].nbLiens; j++) {
					delete[] tabarticle[i].tLiens[j].texte;
				}
				delete[] tabarticle[i].tLiens;
			}
			delete[] tabarticle;
			destroySDL();
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (event_menu.button.button == SDL_BUTTON_LEFT) {
				if ((event_menu.button.x > Historique.x&& event_menu.button.x < Historique.x + Historique.w) && (event_menu.button.y > Historique.y&& event_menu.button.y < Historique.y + Historique.h)) {
					fond_ecran();
					continuer_menu = false;
					dessine_graph();
					break;
				}
				if ((event_menu.button.x > Complet.x&& event_menu.button.x < Complet.x + Complet.w) && (event_menu.button.y > Complet.y&& event_menu.button.y < Complet.y + Complet.h)) {
					fond_ecran();
					continuer_menu = false;
					dessine_graph_complet();
					break;
				}
				if ((event_menu.button.x > RetourJeu.x&& event_menu.button.x < RetourJeu.x + RetourJeu.w) && (event_menu.button.y > RetourJeu.y&& event_menu.button.y < RetourJeu.y + RetourJeu.h)) {
					fond_ecran();
					continuer_menu = false;
					jouer();
					break;
				}
			}
		}
	}
}

void jouer() {
	while (CURRENT_ARTICLE >= 0) {
		affiche_texte(CURRENT_ARTICLE);
		affiche_liens(tabarticle[CURRENT_ARTICLE]);

		Bmenu();
		Bgraph();

		if (tabarticle[CURRENT_ARTICLE].nbLiens != 0) {
			bool continuer = true;
			SDL_Event event;

			while (continuer) {
				SDL_WaitEvent(&event);

				switch (event.type)
				{
				case SDL_QUIT:
					for (int i = 0; i < NBARTICLE; i++) {
						for (int j = 0; j < tabarticle[i].nbLiens; j++) {
							delete[] tabarticle[i].tLiens[j].texte;
						}
						delete[] tabarticle[i].tLiens;
					}
					delete[] tabarticle;
					delete[] chemin;
					continuer = false;
					destroySDL();
					break;

				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT) {
						if (event.button.x > MenuB.x&& event.button.x<MenuB.x + MenuB.w && event.button.y>MenuB.y&& event.button.y < MenuB.y + MenuB.h)
						{
							fond_ecran();
							afficherMenu();
							menu();
							break;
						}
						if (event.button.x > GraphB.x&& event.button.x<GraphB.x + GraphB.w && event.button.y>GraphB.y&& event.button.y < GraphB.y + GraphB.h)
						{
							fond_ecran();
							menu_graph();
							break;
						}
						for (int i = 0; i < tabarticle[CURRENT_ARTICLE].nbLiens; i++) {
							if (event.button.x > 50 && event.button.x < LARGEUR - 200 && event.button.y > HAUTEUR - 100 && event.button.y < (HAUTEUR - 100) + 20 * i + 20) {
								CURRENT_ARTICLE = tabarticle[CURRENT_ARTICLE].tLiens[i].destination - 1;
								save_chemin(tabarticle[CURRENT_ARTICLE]);
								export_save(CURRENT_ARTICLE, ACTUAL_SAVE);
								continuer = false;
								break;
							}
						}
					}
				}
			}
		}
		else {
			fond_ecran();
			SDL_Rect Perdu;
			Perdu.x = 0;
			Perdu.y = 0;
			SDL_Color Rouge = { 255, 0, 0 };
			SDL_Texture* texture_perdu = loadText(rendu, "Perdu !", Rouge, font);
			SDL_QueryTexture(texture_perdu, NULL, NULL, &Perdu.w, &Perdu.h);
			SDL_RenderCopy(rendu, texture_perdu, NULL, &Perdu);
			SDL_DestroyTexture(texture_perdu);
			SDL_RenderPresent(rendu);
			CURRENT_ARTICLE = -1;
			SDL_Delay(2000);
		}
	}
}

void export_save(int partie, int num_partie) {
	int i = 0;
	if (num_partie == 1) {
		ofstream sortie("save01.txt", ios::out);
		sortie << partie << "\n";
		sortie << nb_chemin << "\n" << "\n";
		for (i = 0; i < nb_chemin - 1; i++) {
			sortie << chemin[i] << "\n";
		}
		chemin[i] = tabarticle[CURRENT_ARTICLE].id;
		sortie << tabarticle[CURRENT_ARTICLE].id << "\n";
		sortie.close();
	}
	if (num_partie == 2) {
		ofstream sortie("save02.txt", ios::out);
		sortie << partie << "\n";
		sortie << nb_chemin << "\n" << "\n";
		for (i = 0; i < nb_chemin - 1; i++) {
			sortie << chemin[i] << "\n";
		}
		chemin[i] = tabarticle[CURRENT_ARTICLE].id;
		sortie << tabarticle[CURRENT_ARTICLE].id << "\n";
		sortie.close();
	}
	if (num_partie == 3) {
		ofstream sortie("save03.txt", ios::out);
		sortie << partie << "\n";
		sortie << nb_chemin << "\n" << "\n";
		for (i = 0; i < nb_chemin - 1; i++) {
			sortie << chemin[i] << "\n";
		}
		chemin[i] = tabarticle[CURRENT_ARTICLE].id;
		sortie << tabarticle[CURRENT_ARTICLE].id << "\n";
		sortie.close();
	}
	if (num_partie == 4) {
		ofstream sortie("save04.txt", ios::out);
		sortie << partie << "\n";
		sortie << nb_chemin << "\n" << "\n";
		for (i = 0; i < nb_chemin - 1; i++) {
			sortie << chemin[i] << "\n";
		}
		chemin[i] = tabarticle[CURRENT_ARTICLE].id;
		sortie << tabarticle[CURRENT_ARTICLE].id << "\n";
		sortie.close();
	}
}

void menu_load(SDL_Renderer* rendu) {
	SDL_Color c = { 0,0,0 };
	TTF_Font* font_tmp = TTF_OpenFont("C:\\Windows\\Fonts\\calibri.ttf", 60);
	SDL_Texture* texture1 = loadText(rendu, "Save01", c, font_tmp);
	SDL_Texture* texture2 = loadText(rendu, "Save02", c, font_tmp);
	SDL_Texture* texture3 = loadText(rendu, "Save03", c, font_tmp);
	SDL_Texture* texture4 = loadText(rendu, "Save04", c, font_tmp);
	Save01.h = Save02.h = Save03.h = Save04.h = 25;
	Save01.w = 165;
	Save02.w = 60;
	Save03.w = 60;
	Save04.w = 60;
	Save01.x = Save02.x = Save03.x = Save04.x = 715;
	Save01.y = 150;
	Save02.y = 300;
	Save03.y = 450;
	Save04.y = 600;
	SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
	SDL_QueryTexture(texture1, NULL, NULL, &Save01.w, &Save01.h);
	SDL_QueryTexture(texture2, NULL, NULL, &Save02.w, &Save02.h);
	SDL_QueryTexture(texture3, NULL, NULL, &Save03.w, &Save03.h);
	SDL_QueryTexture(texture4, NULL, NULL, &Save04.w, &Save04.h);
	SDL_RenderCopy(rendu, texture1, NULL, &Save01);
	SDL_RenderCopy(rendu, texture2, NULL, &Save02);
	SDL_RenderCopy(rendu, texture3, NULL, &Save03);
	SDL_RenderCopy(rendu, texture4, NULL, &Save04);
	SDL_RenderDrawRect(rendu, &Save01);
	SDL_RenderDrawRect(rendu, &Save02);
	SDL_RenderDrawRect(rendu, &Save03);
	SDL_RenderDrawRect(rendu, &Save04);
	Bmenu();
	SDL_RenderPresent(rendu);
	SDL_DestroyTexture(texture1);
	SDL_DestroyTexture(texture2);
	SDL_DestroyTexture(texture3);
	SDL_DestroyTexture(texture4);

	bool continuer = true;
	SDL_Event event;
	while (continuer)
	{
		SDL_WaitEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			continuer = false;
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT) {
				if (event.button.x > Save01.x&& event.button.x<Save01.x + Save01.w && event.button.y>Save01.y&& event.button.y < Save01.y + Save01.h)
				{
					ACTUAL_SAVE = 1;
					CURRENT_ARTICLE = import_save(ACTUAL_SAVE);
					fond_ecran();
					jouer();
					break;
				}
				if (event.button.x > Save02.x&& event.button.x<Save02.x + Save02.w && event.button.y>Save02.y&& event.button.y < Save02.y + Save02.h)
				{
					ACTUAL_SAVE = 2;
					CURRENT_ARTICLE = import_save(ACTUAL_SAVE);
					fond_ecran();
					jouer();
					break;
				}
				if (event.button.x > Save03.x&& event.button.x<Save03.x + Save03.w && event.button.y>Save03.y&& event.button.y < Save03.y + Save03.h)
				{
					ACTUAL_SAVE = 3;
					CURRENT_ARTICLE = import_save(ACTUAL_SAVE);
					fond_ecran();
					jouer();
					break;
				}
				if (event.button.x > Save04.x&& event.button.x<Save04.x + Save04.w && event.button.y>Save04.y&& event.button.y < Save04.y + Save04.h)
				{
					ACTUAL_SAVE = 4;
					CURRENT_ARTICLE = import_save(ACTUAL_SAVE);
					fond_ecran();
					jouer();
					break;
				}
				if (event.button.x > MenuB.x&& event.button.x<MenuB.x + MenuB.w && event.button.y>MenuB.y&& event.button.y < MenuB.y + MenuB.h)
				{
					fond_ecran();
					afficherMenu();
					menu();
					break;
				}
			}
		}
	}
}

void menu_New(SDL_Renderer* rendu) {
	SDL_Color c = { 0,0,0 };
	TTF_Font* font_tmp = TTF_OpenFont("C:\\Windows\\Fonts\\calibri.ttf", 60);
	SDL_Texture* texture1 = loadText(rendu, "Slot01", c, font_tmp);
	SDL_Texture* texture2 = loadText(rendu, "Slot02", c, font_tmp);
	SDL_Texture* texture3 = loadText(rendu, "Slot03", c, font_tmp);
	SDL_Texture* texture4 = loadText(rendu, "Slot04", c, font_tmp);
	Slot01.h = Slot02.h = Slot03.h = Slot04.h = 25;
	Slot01.w = 165;
	Slot02.w = 60;
	Slot03.w = 60;
	Slot04.w = 60;
	Slot01.x = Slot02.x = Slot03.x = Slot04.x = 715;
	Slot01.y = 150;
	Slot02.y = 300;
	Slot03.y = 450;
	Slot04.y = 600;
	SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
	SDL_QueryTexture(texture1, NULL, NULL, &Slot01.w, &Slot01.h);
	SDL_QueryTexture(texture2, NULL, NULL, &Slot02.w, &Slot02.h);
	SDL_QueryTexture(texture3, NULL, NULL, &Slot03.w, &Slot03.h);
	SDL_QueryTexture(texture4, NULL, NULL, &Slot04.w, &Slot04.h);
	SDL_RenderCopy(rendu, texture1, NULL, &Slot01);
	SDL_RenderCopy(rendu, texture2, NULL, &Slot02);
	SDL_RenderCopy(rendu, texture3, NULL, &Slot03);
	SDL_RenderCopy(rendu, texture4, NULL, &Slot04);
	SDL_RenderDrawRect(rendu, &Slot01);
	SDL_RenderDrawRect(rendu, &Slot02);
	SDL_RenderDrawRect(rendu, &Slot03);
	SDL_RenderDrawRect(rendu, &Slot04);
	Bmenu();
	SDL_RenderPresent(rendu);
	SDL_DestroyTexture(texture1);
	SDL_DestroyTexture(texture2);
	SDL_DestroyTexture(texture3);
	SDL_DestroyTexture(texture4);

	bool continuer = true;
	SDL_Event event;
	while (continuer)
	{
		SDL_WaitEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			continuer = false;
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT) {
				if (event.button.x > Slot01.x&& event.button.x<Slot01.x + Slot01.w && event.button.y>Slot01.y&& event.button.y < Slot01.y + Slot01.h)
				{
					ACTUAL_SAVE = 1;
					fond_ecran();
					jouer();
					break;
				}
				if (event.button.x > Slot02.x&& event.button.x<Slot02.x + Slot02.w && event.button.y>Slot02.y&& event.button.y < Slot02.y + Slot02.h)
				{
					ACTUAL_SAVE = 2;
					fond_ecran();
					jouer();
					break;
				}
				if (event.button.x > Slot03.x&& event.button.x<Slot03.x + Slot03.w && event.button.y>Slot03.y&& event.button.y < Slot03.y + Slot03.h)
				{
					ACTUAL_SAVE = 3;
					fond_ecran();
					jouer();
					break;
				}
				if (event.button.x > Slot04.x&& event.button.x<Slot04.x + Slot04.w && event.button.y>Slot04.y&& event.button.y < Slot04.y + Slot04.h)
				{
					ACTUAL_SAVE = 4;
					fond_ecran();
					jouer();
					break;
				}
				if (event.button.x > MenuB.x&& event.button.x<MenuB.x + MenuB.w && event.button.y>MenuB.y&& event.button.y < MenuB.y + MenuB.h)
				{
					fond_ecran();
					afficherMenu();
					menu();
					break;
				}
			}
		}
	}
}

void menu_choose_LoadorNew(SDL_Renderer* rendu) {
	SDL_Color c = { 0,0,0 };
	TTF_Font* font_tmp = TTF_OpenFont("C:\\Windows\\Fonts\\calibri.ttf", 60);
	SDL_Texture* texture1 = loadText(rendu, "New Game", c, font_tmp);
	SDL_Texture* texture2 = loadText(rendu, "Load Game", c, font_tmp);
	New.h = Load.h = 25;
	New.w = 165;
	Load.w = 60;
	New.x = Load.x = 715;
	New.y = 150;
	Load.y = 300;
	SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
	SDL_QueryTexture(texture1, NULL, NULL, &New.w, &New.h);
	SDL_QueryTexture(texture2, NULL, NULL, &Load.w, &Load.h);
	SDL_RenderCopy(rendu, texture1, NULL, &New);
	SDL_RenderCopy(rendu, texture2, NULL, &Load);
	SDL_RenderDrawRect(rendu, &New);
	SDL_RenderDrawRect(rendu, &Load);
	SDL_DestroyTexture(texture1);
	SDL_DestroyTexture(texture2);
	Bmenu();
	SDL_RenderPresent(rendu);

	bool continuer = true;
	SDL_Event event;
	while (continuer)
	{
		SDL_WaitEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			continuer = false;
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT) {
				if (event.button.x > New.x&& event.button.x<New.x + New.w && event.button.y>New.y&& event.button.y < New.y + New.h)
				{
					fondimg = IMG_Load("background.jpg");
					SDL_Texture* texture_fond = SDL_CreateTextureFromSurface(rendu, fondimg);
					SDL_QueryTexture(texture_fond, NULL, NULL, &Fond.w, &Fond.h);
					Fond.x = 0;
					Fond.y = 0;
					SDL_RenderCopy(rendu, texture_fond, NULL, &Fond);
					SDL_DestroyTexture(texture_fond);
					SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
					SDL_RenderDrawRect(rendu, &Fond);
					SDL_RenderPresent(rendu);
					menu_New(rendu);
					break;
				}
				if (event.button.x > Load.x&& event.button.x<Load.x + Load.w && event.button.y>Load.y&& event.button.y < Load.y + Load.h)
				{
					fondimg = IMG_Load("background.jpg");
					SDL_Texture* texture_fond = SDL_CreateTextureFromSurface(rendu, fondimg);
					SDL_QueryTexture(texture_fond, NULL, NULL, &Fond.w, &Fond.h);
					Fond.x = 0;
					Fond.y = 0;
					SDL_RenderCopy(rendu, texture_fond, NULL, &Fond);
					SDL_DestroyTexture(texture_fond);
					SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
					SDL_RenderDrawRect(rendu, &Fond);
					SDL_RenderPresent(rendu);
					menu_load(rendu);
					break;
				}
				if (event.button.x > MenuB.x&& event.button.x<MenuB.x + MenuB.w && event.button.y>MenuB.y&& event.button.y < MenuB.y + MenuB.h)
				{
					fondimg = IMG_Load("background.jpg");
					SDL_Texture* texture_fond = SDL_CreateTextureFromSurface(rendu, fondimg);
					SDL_QueryTexture(texture_fond, NULL, NULL, &Fond.w, &Fond.h);
					Fond.x = 0;
					Fond.y = 0;
					SDL_RenderCopy(rendu, texture_fond, NULL, &Fond);
					SDL_DestroyTexture(texture_fond);
					SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
					SDL_RenderDrawRect(rendu, &Fond);
					SDL_RenderPresent(rendu);
					afficherMenu();
					menu();
					break;
				}
			}
		}
	}
	TTF_CloseFont(font_tmp);
}

void menu() {
	bool continuer_menu = true;
	while (continuer_menu) {
		SDL_Event event_menu;
		SDL_WaitEvent(&event_menu);

		switch (event_menu.type)
		{
		case SDL_QUIT:
			for (int i = 0; i < NBARTICLE; i++) {
				for (int j = 0; j < tabarticle[i].nbLiens; j++) {
					delete[] tabarticle[i].tLiens[j].texte;
				}
				delete[] tabarticle[i].tLiens;
			}
			delete[] tabarticle;
			destroySDL();
			break;

			break;

		case SDL_MOUSEBUTTONDOWN:
			if (event_menu.button.button == SDL_BUTTON_LEFT) {
				if ((event_menu.button.x > Jouer.x&& event_menu.button.x < Jouer.x + Jouer.w) && (event_menu.button.y > Jouer.y&& event_menu.button.y < Jouer.y + Jouer.h)) {
					fond_ecran();
					continuer_menu = false;
					menu_choose_LoadorNew(rendu);
					break;
				}
				if ((event_menu.button.x > Editer.x&& event_menu.button.x < Editer.x + Editer.w) && (event_menu.button.y > Editer.y&& event_menu.button.y < Editer.y + Editer.h)) {
					fond_ecran();
					continuer_menu = false;
					menu_edition();
					break;
				}
				if ((event_menu.button.x > Quitter.x&& event_menu.button.x < Quitter.x + Quitter.w) && (event_menu.button.y > Quitter.y&& event_menu.button.y < Quitter.y + Quitter.h)) {
					continuer_menu = false;
					for (int i = 0; i < NBARTICLE; i++) {
						for (int j = 0; j < tabarticle[i].nbLiens; j++) {
							delete[] tabarticle[i].tLiens[j].texte;
						}
						delete[] tabarticle[i].tLiens;
					}
					delete[] tabarticle;
					destroySDL();
					break;
				}
			}
		}
	}
}

Noeud faire_noeud(Article* a, int x, int y) {
	Noeud N;
	N.article = a;
	N.x = /*en fonction de la distance au début*/x;
	N.y = /*en fonction de nb de noeud ayant le  meme x*/y;

	return N;
}

Arete faire_Arrete(Noeud Origine, Noeud Destination) {
	Arete A;
	A.destination = &Destination;
	A.origine = &Origine;

	return A;
}

void dessine_noeud(Noeud N) {
	TTF_Init();
	TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\calibri.ttf", 20);

	SDL_Rect Rect;
	Rect.x = N.x;
	Rect.y = N.y;
	Rect.h = H;
	Rect.w = L;

	SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
	SDL_RenderFillRect(rendu, &Rect);
	SDL_Color Blanc = { 255,255,255 };
	char nb[10];
	_itoa_s((N.article->id), nb, 10);
	SDL_Texture* texture = loadText(rendu, nb, Blanc, font);
	SDL_QueryTexture(texture, NULL, NULL, &Rect.w, &Rect.h);
	SDL_RenderCopy(rendu, texture, NULL, &Rect);

	SDL_RenderPresent(rendu);
}

void dessine_arrete(Arete a) {
	dessine_noeud(*(a.destination));
	dessine_noeud(*(a.origine)); //peut être supprimé
	SDL_RenderDrawLine(rendu, a.destination->x, a.destination->y + (H / 2), a.origine->x + L, (a.origine)->y + (H / 2));
	SDL_RenderPresent(rendu);
}

void dessine_graph() {
	int cmp = 0;
	int cmp2 = 0;

	for (int i = 0; i < nb_chemin - 1; i++) {
		if (cmp < 15) {
			dessine_noeud(faire_noeud(&(tabarticle[chemin[i] - 1]), (100 * (cmp + 1)), (400 + (cmp2 * 40))));
			dessine_arrete(faire_Arrete(faire_noeud(&(tabarticle[chemin[i] - 1]), (100 * (cmp + 1)), (400 + (cmp2 * 40))), faire_noeud(&(tabarticle[chemin[i + 1] - 1]), (100 * (cmp + 2)), (400 + (cmp2 * 40)))));
		}
		else {
			cmp = 0;
			cmp2++;
		}

		cmp++;
	}

	Bretour();

	SDL_RenderPresent(rendu);

	bool continuer_menu = true;
	while (continuer_menu) {
		SDL_Event event_menu;
		SDL_WaitEvent(&event_menu);

		switch (event_menu.type)
		{
		case SDL_QUIT:
			for (int i = 0; i < NBARTICLE; i++) {
				for (int j = 0; j < tabarticle[i].nbLiens; j++) {
					delete[] tabarticle[i].tLiens[j].texte;
				}
				delete[] tabarticle[i].tLiens;
			}
			delete[] tabarticle;
			destroySDL();
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (event_menu.button.button == SDL_BUTTON_LEFT) {
				if ((event_menu.button.x > RetourJeu.x&& event_menu.button.x < RetourJeu.x + RetourJeu.w) && (event_menu.button.y > RetourJeu.y&& event_menu.button.y < RetourJeu.y + RetourJeu.h)) {
					SDL_SetRenderDrawColor(rendu, 198, 138, 94, 255);
					SDL_RenderClear(rendu);
					continuer_menu = false;
					jouer();
					break;
				}
			}
		}
	}
}

void dessine_graph_complet() {
	Noeud tab[350];

	for (int h = 0; h < 350; h++) {
		tab[h] = faire_noeud(tabarticle + h, 0, 0);
	}

	int compt = 0;

	for (int j = 0; j < 14; j++) {
		for (int i = 0; i < 25; i++) {
			tab[compt].x = 68 * i;
			tab[compt].y = 68 * j;

			//tab[compt].article->id = tabarticle[compt].id;
			compt++;
		}
	}

	for (int k = 0; k < 350; k++) {
		for (int q = 0; q < tabarticle[k].nbLiens; q++) {
			dessine_arrete(faire_Arrete(tab[k], tab[(tabarticle[k].tLiens[q].destination) - 1]));
			//dessine_arrete(faire_Arrete(faire_noeud(tabarticle[chemin[i]], 100 * (i + 1), HAUTEUR / 2), faire_noeud(tabarticle[chemin[i + 1]], 100 * (i + 2), HAUTEUR / 2)));
		}
	}

	compt = 0;

	for (int j = 0; j < 14; j++) {
		for (int i = 0; i < 25; i++) {
			dessine_noeud(tab[compt]);

			//tab[compt].article->id = tabarticle[compt].id;
			compt++;
		}
	}
}

void exportHTML() {
	char tmp[21];

	//system("mkdir \\iutbg-smbetu.univ-lyon1.fr\homes\Documents\Init algo\Semaine spé\LDVELH\LDVELH\Article"); //création dossier Article

	for (int i = 1; i <= NBARTICLE; i++)						//création et remplissage des articles dans le dossier article
	{
		snprintf(tmp, sizeof(tmp), "Article/page%d", i);		//concatenation Article/pagei
		strcat_s(tmp, ".html");									//concatenation de Article/pagei.html

		ofstream pages(tmp);									//création de i pages (pagei.html)

		//Début HTML sans CSS(pour l'instant) Article
		pages << "<!Doctype html>" << endl;
		pages << "<html lang=\"fr\">" << endl;
		pages << "<meta http-equiv=\"Content-Type\" content=\"text/html\" charset=\"utf-8\">" << endl;
		pages << "<head>" << endl;
		pages << "<title>" << tabarticle[i - 1].id << "</title>" << endl;
		pages << "</head>" << endl;
		pages << "<style>A{text-decoration:none;} </style>" << endl;
		pages << "<body bgcolor=#C68A5E text=black style=\"texte_align:center \"> " << endl;
		pages << "<div  style = \"border-color: black; border-style:solid; border-width: 3px ; border-radius: 10px; width:300px; margin:auto; text-align:center; \" >" << endl;
		pages << "<h1 style =\"text-shadow: 2px 5px 5px #C68A5E ; cursor:default ;\"><b>" << tabarticle[i - 1].id << "</b></h1>" << endl;
		pages << "</div>" << endl;
		pages << "<br>" << endl;
		pages << "<p style = \"cursor:default ; font-size: 125 %; margin:auto; padding:23px ; \" ><b>" << tabarticle[i - 1].texte << "</b></p>" << endl;
		pages << "<br>" << endl;
		pages << "<br>" << endl;
		pages << "<br>" << endl;

		//Début HTML sans CSS Liens
		for (int j = 0; j < tabarticle[i - 1].nbLiens; j++)
		{
			pages << "<div style=\"border-color: black; border-style:solid; border-width: 3px; border-radius: 10px;  margin:auto; padding-top:10px; text-align:center; \">" << endl;
			pages << "<a href=\"page" << tabarticle[i - 1].tLiens[j].destination << ".html\"><h2 style = \"font-size: 115 %; text=black;\" ><b>" << tabarticle[i - 1].tLiens[j].destination << "</b></h2></a>" << endl;
			pages << "<p><b>" << tabarticle[i - 1].tLiens[j].texte << "</b></p>" << endl;
			pages << "<br>" << endl;
			pages << "</div>" << endl;
			pages << "</body>" << endl;
			pages << "</html>" << endl;
		}

		pages.close();  //fermeture de l'écriture

		cout << tmp << " a ete creer" << endl;
	}
}

void saisie_texte(SDL_Rect rectangle) {
	SDL_Rect affiche_saisie;
	affiche_saisie.x = rectangle.x + 5;
	affiche_saisie.y = rectangle.y + 3;
	SDL_Color blanc = { 255, 255, 255 };
	int taille = 1;
	char* ecriture;
	ecriture = new char[taille];
	bool continuer_menu = true, majuscule = false;
	while (continuer_menu) {
		SDL_Event event;
		SDL_WaitEvent(&event);

		switch (event.type)
		{
		case SDL_QUIT:
			for (int i = 0; i < NBARTICLE; i++) {
				for (int j = 0; j < tabarticle[i].nbLiens; j++) {
					delete[] tabarticle[i].tLiens[j].texte;
				}
				delete[] tabarticle[i].tLiens;
			}
			delete[] tabarticle;
			destroySDL();
			break;

			break;

		case SDL_KEYDOWN:
			if (event.key.keysym.sym == 1073741881) {
				if (!majuscule) {
					majuscule = true;
				}
				else {
					majuscule = false;
				}
				break;
			}
			else if (event.key.keysym.sym == 8) {
				taille--;
				char* redim_ecriture = ecriture;
				ecriture = new char[taille];
				for (int i = 0; i < taille - 1; i++) {
					ecriture[i] = redim_ecriture[i];
				}
				delete redim_ecriture;
				ecriture[taille - 1] = '\0';
				cout << ecriture;
				SDL_RenderFillRect(rendu, &rectangle);
				SDL_Texture* texture_affiche_saisie = loadText(rendu, ecriture, blanc, petite_font);
				SDL_QueryTexture(texture_affiche_saisie, NULL, NULL, &affiche_saisie.w, &affiche_saisie.h);
				SDL_RenderCopy(rendu, texture_affiche_saisie, NULL, &affiche_saisie);
				SDL_RenderPresent(rendu);
			}
			else {
				taille++;
				char* redim_ecriture = ecriture;
				ecriture = new char[taille];
				for (int i = 0; i < taille - 2; i++) {
					ecriture[i] = redim_ecriture[i];
				}
				delete redim_ecriture;
				if (majuscule) {
					ecriture[taille - 2] = event.key.keysym.sym - 32;
				}
				else {
					ecriture[taille - 2] = event.key.keysym.sym;
				}
			}
			ecriture[taille - 1] = '\0';
			cout << ecriture;
			SDL_Texture* texture_affiche_saisie = loadText(rendu, ecriture, blanc, petite_font);
			SDL_QueryTexture(texture_affiche_saisie, NULL, NULL, &affiche_saisie.w, &affiche_saisie.h);
			SDL_RenderCopy(rendu, texture_affiche_saisie, NULL, &affiche_saisie);
			SDL_RenderPresent(rendu);
			break;
		}
	}
}

void menu_edition() {
	TTF_Font* font_temp = TTF_OpenFont("C:\\Windows\\Fonts\\calibri.ttf", 25);

	SDL_Rect saisir_titre;
	SDL_Color noir = { 0, 0, 0 };
	SDL_Texture* texture_saisir_titre = loadText(rendu, "Saisissez le titre de votre livre :", noir, font_temp);
	SDL_QueryTexture(texture_saisir_titre, NULL, NULL, &saisir_titre.w, &saisir_titre.h);
	saisir_titre.x = LARGEUR / 2 - saisir_titre.w / 2;
	saisir_titre.y = HAUTEUR / 4 - saisir_titre.h / 2;
	SDL_RenderCopy(rendu, texture_saisir_titre, NULL, &saisir_titre);
	SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
	SDL_RenderDrawRect(rendu, &saisir_titre);
	SDL_DestroyTexture(texture_saisir_titre);

	SDL_Rect champ_saisir_titre;
	champ_saisir_titre.x = saisir_titre.x;
	champ_saisir_titre.y = saisir_titre.y + saisir_titre.h + 10;
	champ_saisir_titre.w = saisir_titre.w;
	champ_saisir_titre.h = 25 + 6;

	SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
	SDL_RenderFillRect(rendu, &champ_saisir_titre);

	SDL_RenderPresent(rendu);
	TTF_CloseFont(font_temp);

	bool continuer_menu = true, majuscule = false;
	while (continuer_menu) {
		SDL_Event event;
		SDL_WaitEvent(&event);

		switch (event.type)
		{
		case SDL_QUIT:
			for (int i = 0; i < NBARTICLE; i++) {
				for (int j = 0; j < tabarticle[i].nbLiens; j++) {
					delete[] tabarticle[i].tLiens[j].texte;
				}
				delete[] tabarticle[i].tLiens;
			}
			delete[] tabarticle;
			destroySDL();
			break;

			break;

		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT) {
				if ((event.button.x > champ_saisir_titre.x&& event.button.x < champ_saisir_titre.x + champ_saisir_titre.w) && (event.button.y > champ_saisir_titre.y&& event.button.y < champ_saisir_titre.y + champ_saisir_titre.h)) {
					continuer_menu = false;
					saisie_texte(champ_saisir_titre);
					break;
				}
			}
		}
	}
}

//void saisie_titre() {
//
//}
//
//void saisie_article() {
//
//}

int main(int argn, char* argv[]) {
	importer();
	//exportHTML();

	initSDL();
	//edition();
	afficherMenu();
	menu();

	/*char* a = new char[6];
	for (int i = 0; i < 5; i++) {
		a[i] = 'b';
	}
	a[5] = '\0';
	cout << a << endl;*/

	return 0;
}