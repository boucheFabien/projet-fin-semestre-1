#include<iostream>

const int LARGEUR = 1700;
const int HAUTEUR = 1000;
const int H = 30;
const int L = 50;


SDL_Renderer* rendu;
SDL_Window* win;
TTF_Font* font;
TTF_Font* bouton_menu_font;
TTF_Font* petite_font;

typedef struct {
	char* texte = NULL; //texte possible sur le lien
	int destination; //contient l'id de l'article destination
}Lien;

typedef struct {
	int id; //identifiant de l'article
	char* texte = NULL; //texte de l'article
	Lien* tLiens = NULL; //tableau dynamique de liens vers les autres articles
	int nbLiens;
}Article;

typedef struct {
	Article* article = NULL; //l'article lié au noeud du graphe
	int x, y; //position du noeud dans la fenêtre
}Noeud;

typedef struct {
	Noeud* origine = NULL; //pointeur sur le noeud origine du lien
	Noeud* destination = NULL; //pointeur sur le noeud destination du lien
}Arete;

//FONCTIONS

int initSDL();
void destroySDL();
void afficherMenu();
void importer();
void save_chemin(Article a);
int import_save(int num_partie);
int change_texte(Article& texte);
void enlèvenul(Article& texte);
void affiche_texte(int id);
void affiche_liens(Article Choix);
void jouer();
void export_save(int partie, int num_partie);
void menu_load(SDL_Renderer* rendu);
void menu_New(SDL_Renderer* rendu);
void menu_choose_LoadorNew(SDL_Renderer* rendu);
void menu();
Noeud faire_noeud(Article* a, int x, int y);
Arete faire_Arrete(Noeud Origine, Noeud Destination);
void dessine_noeud(Noeud N);
void dessine_arrete(Arete a);
void dessine_graph();
void menu_graph();
void dessine_graph_complet();
void exportHTML();
void saisie_texte(SDL_Rect rectangle);
void menu_edition();