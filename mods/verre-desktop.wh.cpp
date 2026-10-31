// ==WindhawkMod==
// @id              verre-desktop
// @name            Verre Desktop
// @description     Widgets en verre pour le bureau, chacun attaché à son bureau virtuel
// @version         1.0.1
// @author          David WATEBA
// @github          https://github.com/Drawm07
// @homepage        https://github.com/Drawm07/verre-desktop
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ldwrite -lgdi32 -lole32 -lshell32 -lpdh -liphlpapi -luuid -lpropsys -lwindowscodecs -lversion -lpsapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Verre Desktop

Transforme le bureau de Windows 11 en tableau de bord en verre : horloge,
état du système, recherche, raccourcis, minuteur de concentration, temps
d'écran, et un écran d'accueil sur les autres bureaux virtuels. Six fonds
d'écran sont faits pour aller avec.

![Bureau 1 sur le fond Orbite](https://raw.githubusercontent.com/Drawm07/verre-desktop/main/captures/07-orbite-bureau1.png)

![Écran d'accueil sur le fond Soie](https://raw.githubusercontent.com/Drawm07/verre-desktop/main/captures/08-soie-accueil.png)

Code source, fonds d'écran et captures : https://github.com/Drawm07/verre-desktop

## Ce que vous obtenez

**Bureau 1**
- **Horloge** et date en français.
- **Carte Système** : processeur (avec courbe), mémoire, carte graphique,
  disque C:, réseau. Bouton **Libérer** : vide la mémoire de travail des
  apps, sans en fermer aucune. Bouton **Nettoyer** : analyse les fichiers
  temporaires, les rapports de plantage et la corbeille, puis demande
  confirmation avant de supprimer.
- **Barre de recherche** : apps installées, dossiers, recherche web (dans
  Chrome) et recherche de fichiers. Entrée lance la ligne choisie.
- **Poste de pilotage** : minuteur de concentration (travail / pause, cycle
  de 4 sessions) et temps d'écran du jour par app (rien n'est compté après
  5 minutes sans activité).
- **Accès rapide** : 6 dossiers réglables et l'explorateur.
- **Mes sites** : aperçus de vos sites, ouverts dans Chrome avec le compte
  de votre choix.

**Autres bureaux virtuels** : écran d'accueil avec salutation, pastilles
CPU / RAM / Disque, grille de vos applications et barre Dossiers.

Chaque groupe n'apparaît que sur son bureau virtuel.

## Les fonds d'écran

À télécharger dans le dossier `fonds` du dépôt GitHub, puis à choisir dans
les Paramètres (ou en diaporama).

**Éclipse**

![Éclipse](https://raw.githubusercontent.com/Drawm07/verre-desktop/main/captures/06-eclipse-bureau1.png)

**Orbite**

![Orbite](https://raw.githubusercontent.com/Drawm07/verre-desktop/main/captures/07-orbite-bureau1.png)

**Soie**

![Soie](https://raw.githubusercontent.com/Drawm07/verre-desktop/main/captures/08-soie-bureau1.png)

**Dunes**

![Dunes](https://raw.githubusercontent.com/Drawm07/verre-desktop/main/captures/09-dunes-bureau1.png)

**Capsules de lumière**

![Capsules de lumière](https://raw.githubusercontent.com/Drawm07/verre-desktop/main/captures/10-capsules-lumiere-bureau1.png)

**Aurore boréale**

![Aurore boréale](https://raw.githubusercontent.com/Drawm07/verre-desktop/main/captures/11-aurore-boreale-bureau1.png)

## Pour commencer (onglet Paramètres)

- **Prénom** : pour la salutation de l'écran d'accueil.
- **Dossier des applications** : les raccourcis (`.lnk`) de la grille,
  triés par nom. Un préfixe numérique (« 01 Spotify.lnk ») fixe l'ordre et
  n'est pas affiché. La tuile « Ajouter » ouvre ce dossier.
- **Dossiers** : un chemin complet (`C:\...`) ou un dossier spécial
  (`shell:Desktop`, `shell:Personal`, `shell:Downloads`,
  `shell:My Pictures`...).
- **Sites** : nom, adresse, et compte Chrome (l'adresse du compte, ou
  `choisir` pour choisir le profil à chaque clic, ou vide).
- **Fond d'écran** et **Dossier des fonds** : « Ne pas changer le fond »
  par défaut, le mod ne touche à rien tant que vous n'en choisissez pas.
- **Apparence** : opacité du verre, couleur d'accent, position de chaque
  widget.

## Bon à savoir

- Le mod tourne dans son propre processus `windhawk.exe` (« mod outil ») :
  s'il plante, la barre des tâches et le bureau ne sont pas touchés.
- Aucune connexion réseau dans le mod. Exceptions : ouvrir le navigateur,
  et faire photographier un site une seule fois par Chrome pour son aperçu.
- Avec le diaporama, chaque bureau garde son propre fond (Windows ne permet
  pas de préparer le fond d'un autre bureau).
- Testé sur Windows 11, 1920 × 1080 à 150 %. Pas encore testé : mise en
  veille, plusieurs écrans, autres échelles d'affichage.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- opaciteVerre: 37
  $name: Opacité du verre (%)
  $description: 0 = verre invisible, 100 = opaque. 37 par défaut
- couleurAccent: "#66BCFF"
  $name: Couleur d'accent
  $description: Code couleur hexadécimal, par exemple #66BCFF
- fond: aucun
  $name: Fond d'écran
  $description: Un des fonds Verre (dans le dossier des fonds). Il est appliqué à chaque bureau virtuel
  $options:
  - aucun: Ne pas changer le fond
  - 06-eclipse: Éclipse
  - 07-orbite: Orbite
  - 08-soie: Soie
  - 09-dunes: Dunes
  - 10-capsules-lumiere: Capsules de lumière
  - 11-aurore-boreale: Aurore boréale
  - 12-gemini-anneaux: Anneaux
- dossierFonds: "%USERPROFILE%\\Pictures\\Verre"
  $name: Dossier des fonds
  $description: Dossier qui contient les images (06-eclipse.png, 07-orbite.png...)
- diaporamaMinutes: 0
  $name: Diaporama (minutes)
  $description: Change le fond du bureau affiché toutes les N minutes, dans l'ordre ; chaque bureau garde le sien. 0 = pas de diaporama
- horloge:
  - x: 28
  - y: 40
  $name: Position de l'horloge
  $description: En pixels depuis le coin haut gauche de l'écran (mesurés à l'échelle 150 %)
- systeme:
  - x: 24
  - y: 250
  $name: Position de la carte Système
  $description: En pixels depuis le coin haut gauche de l'écran (mesurés à l'échelle 150 %)
- accesRapide:
  - marge: 24
  - y: 64
  $name: Position de l'Accès rapide
  $description: Marge = distance au bord droit de l'écran ; y depuis le haut (pixels à 150 %)
- recherche:
  - y: 64
  $name: Position de la barre de recherche
  $description: Distance au haut de l'écran (pixels à 150 %) ; la barre reste centrée
- accueil:
  - y: 100
  $name: Position de l'écran d'accueil
  $description: Distance au haut de l'écran (pixels à 150 %) ; l'accueil reste centré
- sites:
  - x: 24
  - y: 680
  $name: Position du bouton Sites
  $description: En pixels depuis le coin haut gauche de l'écran (mesurés à l'échelle 150 %)
- pilotage:
  - y: 250
  $name: Position du poste de pilotage
  $description: Distance au haut de l'écran (pixels à 150 %) ; le poste reste centré
- minutesConcentration: 25
  $name: Minuteur - durée de concentration (minutes)
- minutesPause: 5
  $name: Minuteur - durée de la pause (minutes)
- site1Nom: "YouTube"
  $name: Site 1 - nom
- site1Adresse: "https://www.youtube.com"
  $name: Site 1 - adresse
  $description: Adresse complète (https://...). Vide = site masqué. L'aperçu est photographié une seule fois par Chrome
- site1Compte: ""
  $name: Site 1 - compte Chrome
  $description: Compte du profil Chrome à utiliser (nom@gmail.com), « choisir » pour choisir à chaque clic, vide = profil habituel
- site2Nom: "Gmail"
  $name: Site 2 - nom
- site2Adresse: "https://mail.google.com"
  $name: Site 2 - adresse
- site2Compte: ""
  $name: Site 2 - compte Chrome
- site3Nom: "GitHub"
  $name: Site 3 - nom
- site3Adresse: "https://github.com"
  $name: Site 3 - adresse
- site3Compte: ""
  $name: Site 3 - compte Chrome
- site4Nom: "ChatGPT"
  $name: Site 4 - nom
- site4Adresse: "https://chatgpt.com"
  $name: Site 4 - adresse
- site4Compte: ""
  $name: Site 4 - compte Chrome
- site5Nom: "Wikipédia"
  $name: Site 5 - nom
- site5Adresse: "https://fr.wikipedia.org"
  $name: Site 5 - adresse
- site5Compte: ""
  $name: Site 5 - compte Chrome
- site6Nom: "Google Drive"
  $name: Site 6 - nom
- site6Adresse: "https://drive.google.com"
  $name: Site 6 - adresse
- site6Compte: ""
  $name: Site 6 - compte Chrome
- site7Nom: ""
  $name: Site 7 - nom
- site7Adresse: ""
  $name: Site 7 - adresse
- site7Compte: ""
  $name: Site 7 - compte Chrome
- site8Nom: ""
  $name: Site 8 - nom
- site8Adresse: ""
  $name: Site 8 - adresse
- site8Compte: ""
  $name: Site 8 - compte Chrome
- prenom: ""
  $name: Prénom
  $description: Affiché dans la salutation de l'écran d'accueil (« Bonjour ... »)
- dossierApps: "%USERPROFILE%\\Documents\\Verre Desktop\\Apps"
  $name: Dossier des applications
  $description: Raccourcis affichés dans la grille de l'écran d'accueil (26 au maximum)
- renommages: "Samsung Notes=Notes; ChatGPT Classic=ChatGPT; Windows PowerShell=PowerShell"
  $name: Noms raccourcis
  $description: Nom du raccourci=Nom affiché, séparés par des points-virgules
- moteurRecherche: https://www.google.com/search?q=%s
  $name: Moteur de recherche web
  $description: Adresse de recherche de la barre, %s y est remplacé par le texte tapé
- dossier1Nom: Bureau
  $name: Dossier 1 - nom
- dossier1Chemin: shell:Desktop
  $name: Dossier 1 - chemin
- dossier2Nom: Documents
  $name: Dossier 2 - nom
- dossier2Chemin: shell:Personal
  $name: Dossier 2 - chemin
- dossier3Nom: Téléchargements
  $name: Dossier 3 - nom
- dossier3Chemin: shell:Downloads
  $name: Dossier 3 - chemin
- dossier4Nom: Musique
  $name: Dossier 4 - nom
- dossier4Chemin: shell:My Music
  $name: Dossier 4 - chemin
- dossier5Nom: Vidéos
  $name: Dossier 5 - nom
- dossier5Chemin: shell:My Video
  $name: Dossier 5 - chemin
- dossier6Nom: Images
  $name: Dossier 6 - nom
- dossier6Chemin: shell:My Pictures
  $name: Dossier 6 - chemin
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
// GetIfTable2 (réseau) a besoin de winsock2.h et ws2ipdef.h avant iphlpapi.h.
// Windhawk inclut déjà windows.h avant ce fichier : MinGW gère ce cas et
// prévient seulement par un avertissement, qu'on masque ici.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-W#warnings"
#include <winsock2.h>
#pragma clang diagnostic pop
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <pdh.h>
#include <pdhmsg.h>  // PDH_MORE_DATA
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <knownfolders.h>  // FOLDERID_AppsFolder
#include <appmodel.h>  // paquets des apps du Store
#include <propkey.h>   // PKEY_AppUserModel_ID
#include <wincodec.h>  // WIC : décodage des logos PNG
#include <math.h>            // sinf, cosf (anneau du minuteur)
#include <psapi.h>           // EnumProcesses, EmptyWorkingSet
#include <stdio.h>  // swprintf_s

// ---------------------------------------------------------------------------
// Apparence : valeurs validées sous Rainmeter (Variables.inc et skins Verre)
// ---------------------------------------------------------------------------

// Toutes les mesures sont celles de l'écran à 150 %, c'est-à-dire 144 ppp.
// À une autre échelle, on multiplie tout par (ppp de l'écran / 144).
const float PPP_REFERENCE = 144.0f;

// Une couleur comme dans Variables.inc : rouge, vert, bleu, opacité (0 à 255).
struct Couleur {
    BYTE r, g, b, a;
};

// Verre par défaut : haut à 37 % (95/255), bas un peu plus clair (70/95 du
// haut). L'opacité réglée change les deux dans la même proportion.
const Couleur VERRE_HAUT = {28, 32, 44, 95};
const Couleur VERRE_BAS = {18, 20, 28, 70};
const Couleur CONTOUR_HAUT = {255, 255, 255, 55};
const Couleur CONTOUR_BAS = {255, 255, 255, 14};
const Couleur TEXTE = {244, 245, 247, 255};
const Couleur TEXTE2 = {199, 202, 208, 255};
const Couleur TEXTE3 = {169, 173, 181, 255};
const Couleur ACCENT_PAR_DEFAUT = {102, 188, 255, 255};

// Couleurs réellement utilisées, recalculées à chaque lecture des réglages.
Couleur g_verreHaut = VERRE_HAUT;
Couleur g_verreBas = VERRE_BAS;
Couleur g_accent = ACCENT_PAR_DEFAUT;
Couleur g_accentDoux = {102, 188, 255, 60};  // l'accent à 24 %

// L'accent avec une autre opacité (0 à 255).
Couleur AccentTransparent(BYTE opacite) {
    return {g_accent.r, g_accent.g, g_accent.b, opacite};
}
const Couleur PISTE = {255, 255, 255, 26};
const Couleur BOUTON = {255, 255, 255, 16};
const Couleur BOUTON_SURVOL = {255, 255, 255, 36};
const Couleur BOUTON_CONTOUR = {255, 255, 255, 24};
const Couleur TUILE_SURVOL = {255, 255, 255, 40};
const Couleur TUILE_SURVOL_CONTOUR = {255, 255, 255, 90};
const Couleur OMBRE = {0, 0, 0, 70};

const WCHAR POLICE_TITRE[] = L"Book Antiqua";
const WCHAR POLICE[] = L"Segoe UI";
// Police d'icônes fournie avec Windows 10 et 11 (lecture, volume...).
const WCHAR POLICE_ICONES[] = L"Segoe MDL2 Assets";

const float RAYON_CARTE = 21.0f;
const float RAYON_BOUTON = 15.0f;
const float RAYON_TUILE = 20.0f;

const WCHAR NOM_CLASSE[] = L"VerreDesktop_Widget";
const UINT WM_APP_REGLAGES = WM_APP + 1;  // les réglages ont changé
const UINT WM_APP_APPS = WM_APP + 2;      // la liste des apps est prête
const UINT WM_APP_CATALOGUE = WM_APP + 3; // le catalogue de la recherche est prêt
const UINT WM_APP_INDEX = WM_APP + 4;     // l'index des dossiers est prêt
const UINT WM_APP_APERCUS = WM_APP + 5;   // des aperçus de sites sont prêts
const UINT WM_APP_ENTRETIEN = WM_APP + 6; // « Libérer » ou « Nettoyer » a fini
const UINT_PTR MINUTERIE_SECONDE = 1;
const UINT_PTR MINUTERIE_CURSEUR = 2;     // clignotement du curseur

const int AUCUNE_ZONE = -1;

// ---------------------------------------------------------------------------
// Les widgets
// ---------------------------------------------------------------------------

// Un widget = une fenêtre transparente posée sur le bureau, qui se dessine
// elle-même. Position et taille en pixels de référence (écran à 150 %).
struct Widget {
    HWND fenetre;
    int x, y, largeur, hauteur;
    bool centre;          // centré horizontalement (x ignoré)
    bool depuisLaDroite;  // x compté depuis le bord droit de l'écran
    bool accueil;         // groupe « autres bureaux » (sinon : bureau 1)
    bool chaqueSeconde;   // redessiné à chaque seconde
    void (*dessiner)(ID2D1RenderTarget* cible, float echelle);
    // Zones cliquables (boutons, tuiles) : facultatif.
    int (*zoneSous)(float x, float y);  // en pixels de référence
    void (*cliquer)(int zone);
    int survol;                   // zone sous la souris
    bool sourisSuivie;            // TrackMouseEvent déjà demandé
    ID2D1DCRenderTarget* cible;   // gardée d'un dessin à l'autre
};

void DessinerHorloge(ID2D1RenderTarget* cible, float echelle);
void DessinerSysteme(ID2D1RenderTarget* cible, float echelle);
void DessinerAccesRapide(ID2D1RenderTarget* cible, float echelle);
void DessinerAccueil(ID2D1RenderTarget* cible, float echelle);
int ZoneAccesRapide(float x, float y);
void CliquerAccesRapide(int zone);
int ZoneAccueil(float x, float y);
void CliquerAccueil(int zone);
void RecreerWidgetsPerdus();
int ZoneSysteme(float x, float y);
void CliquerSysteme(int zone);
void DessinerBoutonsEntretien(ID2D1RenderTarget* cible, float echelle);
void CopierReglage(PCWSTR nom, WCHAR* destination, int taille);
void DessinerRecherche(ID2D1RenderTarget* cible, float echelle);
int ZoneRecherche(float x, float y);
void CliquerRecherche(int zone);
void DessinerSites(ID2D1RenderTarget* cible, float echelle);
int ZoneSites(float x, float y);
void CliquerSites(int zone);
void DessinerPilotage(ID2D1RenderTarget* cible, float echelle);
int ZonePilotage(float x, float y);
void CliquerPilotage(int zone);

Widget g_horloge = {nullptr, 28, 40, 620, 230, false, false, false, true,
                    DessinerHorloge, nullptr, nullptr, AUCUNE_ZONE};
Widget g_systeme = {nullptr, 24, 250, 480, 410, false, false, false, true,
                    DessinerSysteme, ZoneSysteme, CliquerSysteme,
                    AUCUNE_ZONE};
Widget g_accesRapide = {nullptr, 24, 64, 450, 405, false, true, false, false,
                        DessinerAccesRapide, ZoneAccesRapide,
                        CliquerAccesRapide, AUCUNE_ZONE};
Widget g_accueil = {nullptr, 0, 100, 1016, 714, true, false, true, true,
                    DessinerAccueil, ZoneAccueil, CliquerAccueil, AUCUNE_ZONE};
// La barre de recherche, centrée en haut ; sa fenêtre est assez haute pour
// la liste de résultats (transparente quand la liste est fermée).
Widget g_recherche = {nullptr, 0, 64, 640, 520, true, false, false, false,
                      DessinerRecherche, ZoneRecherche, CliquerRecherche,
                      AUCUNE_ZONE};
// Le bouton Sites sous la carte Système ; sa fenêtre est assez haute pour
// le panneau d'aperçus (transparente quand il est replié).
Widget g_sites = {nullptr, 24, 680, 480, 260, false, false, false, false,
                  DessinerSites, ZoneSites, CliquerSites, AUCUNE_ZONE};

// Le poste de pilotage, centré, redessiné chaque seconde (minuteur).
Widget g_pilotage = {nullptr, 0, 250, 720, 410, true, false, false, true,
                     DessinerPilotage, ZonePilotage, CliquerPilotage,
                     AUCUNE_ZONE};

// Chaque widget est posé juste au-dessus du bureau, dans cet ordre : le
// dernier posé est le plus bas. La recherche est donc en tête, pour que sa
// liste de résultats passe par-dessus les autres cartes.
Widget* const g_widgets[] = {&g_recherche, &g_horloge,   &g_systeme,
                             &g_accesRapide, &g_sites,  &g_pilotage,
                             &g_accueil};

bool AfficherWidget(Widget* widget);
void PlacerWidget(Widget* widget);

// ---------------------------------------------------------------------------
// État du mod
// ---------------------------------------------------------------------------

HMODULE g_module;          // notre DLL, propriétaire de la classe de fenêtre
HANDLE g_fil;              // le thread qui possède et dessine les fenêtres
DWORD g_idFil;
HANDLE g_filPret;          // signalé quand le thread a fini de démarrer
bool g_demarrageReussi;

// Tout ce qui suit n'est utilisé que par le thread des fenêtres.
ID2D1Factory* g_fabriqueD2D;
IDWriteFactory* g_fabriqueTexte;
ID2D1StrokeStyle* g_traitRond;       // bouts et angles arrondis
ID2D1StrokeStyle* g_traitPointille;  // contour de la tuile « Ajouter »

// Réglages.
struct Dossier {
    WCHAR nom[64];
    WCHAR chemin[MAX_PATH];
};
Dossier g_dossiers[6];
WCHAR g_prenom[64];
WCHAR g_dossierApps[MAX_PATH];
WCHAR g_renommages[512];

// Bureaux virtuels : le registre de Windows dit sur quel bureau on est.
const WCHAR CLE_BUREAUX[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops";
HKEY g_cleBureaux;
HANDLE g_changementBureaux;  // signalé quand la clé du registre change
bool g_surPremierBureau = true;

// Vrai pendant que le mod lui-même montre, cache ou range ses fenêtres.
// Le reste du temps, personne d'autre n'a le droit de le faire.
bool g_changementVoulu;

// Fenêtre invisible, sans rapport avec le bureau : elle porte la minuterie et
// reçoit les messages du mod. Les widgets, eux, sont rattachés au bureau et
// disparaissent s'il est détruit (redémarrage de l'Explorateur).
HWND g_controleur;

// ---------------------------------------------------------------------------
// Petits outils
// ---------------------------------------------------------------------------

// Libère un objet COM (Direct2D, DirectWrite) et remet le pointeur à zéro.
template <typename T>
void Liberer(T*& objet) {
    if (objet) {
        objet->Release();
        objet = nullptr;
    }
}

D2D1_COLOR_F VersD2D(Couleur c) {
    return D2D1::ColorF(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f,
                        c.a / 255.0f);
}

int Arrondir(float valeur) {
    return (int)(valeur + 0.5f);
}

float Echelle(HWND fenetre) {
    return GetDpiForWindow(fenetre) / PPP_REFERENCE;
}

ULONGLONG VersEntier(FILETIME temps) {
    return ((ULONGLONG)temps.dwHighDateTime << 32) | temps.dwLowDateTime;
}

bool DansRectangle(float x, float y, D2D1_RECT_F r) {
    return x >= r.left && x < r.right && y >= r.top && y < r.bottom;
}

// 12.5 devient 12,5 : écriture française des nombres.
void VirguleFrancaise(WCHAR* texte) {
    for (; *texte; texte++) {
        if (*texte == L'.') {
            *texte = L',';
        }
    }
}

ID2D1SolidColorBrush* Pinceau(ID2D1RenderTarget* cible, Couleur couleur) {
    D2D1_COLOR_F couleurD2D = VersD2D(couleur);
    ID2D1SolidColorBrush* pinceau = nullptr;
    if (FAILED(cible->CreateSolidColorBrush(&couleurD2D, nullptr, &pinceau))) {
        return nullptr;
    }
    return pinceau;
}

// Pinceau en dégradé vertical : « haut » en y0, « bas » en y1 (en pixels).
ID2D1LinearGradientBrush* CreerDegrade(ID2D1RenderTarget* cible,
                                       Couleur haut,
                                       Couleur bas,
                                       float y0,
                                       float y1) {
    D2D1_GRADIENT_STOP arrets[2] = {{0.0f, VersD2D(haut)},
                                    {1.0f, VersD2D(bas)}};
    ID2D1GradientStopCollection* arretsD2D = nullptr;
    if (FAILED(cible->CreateGradientStopCollection(
            arrets, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &arretsD2D))) {
        return nullptr;
    }

    D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES direction = {
        D2D1::Point2F(0.0f, y0), D2D1::Point2F(0.0f, y1)};
    ID2D1LinearGradientBrush* pinceau = nullptr;
    if (FAILED(cible->CreateLinearGradientBrush(&direction, nullptr,
                                                arretsD2D, &pinceau))) {
        pinceau = nullptr;
    }
    arretsD2D->Release();
    return pinceau;
}

// Lance un fichier, un dossier ou une commande dans un petit thread à part :
// un raccourci posé sur OneDrive peut mettre du temps à s'ouvrir, et le
// thread des fenêtres ne doit jamais attendre.
struct Lancement {
    WCHAR fichier[2048];  // assez long pour une adresse web
    WCHAR parametres[2048];
};

DWORD WINAPI FilLancement(LPVOID parametre) {
    Lancement* lancement = (Lancement*)parametre;
    HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    SHELLEXECUTEINFO infos = {};
    infos.cbSize = sizeof(infos);
    // NOASYNC : attendre la fin du lancement avant que ce thread se termine.
    infos.fMask = SEE_MASK_NOASYNC;
    infos.lpVerb = L"open";
    infos.lpFile = lancement->fichier;
    infos.lpParameters =
        lancement->parametres[0] ? lancement->parametres : nullptr;
    infos.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteEx(&infos)) {
        Wh_Log(L"Lancement impossible (%u)", GetLastError());
    }

    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    HeapFree(GetProcessHeap(), 0, lancement);
    return 0;
}

void Lancer(const WCHAR* fichier, const WCHAR* parametres) {
    Lancement* lancement = (Lancement*)HeapAlloc(
        GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Lancement));
    if (!lancement) {
        return;
    }
    lstrcpyn(lancement->fichier, fichier, ARRAYSIZE(lancement->fichier));
    if (parametres) {
        lstrcpyn(lancement->parametres, parametres,
                 ARRAYSIZE(lancement->parametres));
    }
    HANDLE fil = CreateThread(nullptr, 0, FilLancement, lancement, 0, nullptr);
    if (fil) {
        CloseHandle(fil);
    } else {
        HeapFree(GetProcessHeap(), 0, lancement);
    }
}

// Ouvre un dossier dans l'Explorateur : « C:\... » ou « shell:Desktop ».
// Sans dossier, ouvre simplement l'Explorateur.
void OuvrirDossier(const WCHAR* chemin) {
    Lancer(L"explorer.exe", chemin);
}

// Colle « dossier\nom » dans « sortie ». Faux si le résultat ne tient pas :
// on ignore alors ce chemin, au lieu de planter (ce que ferait swprintf_s).
bool Joindre(WCHAR* sortie, size_t taille, const WCHAR* dossier,
             const WCHAR* nom) {
    size_t longueurDossier = wcslen(dossier), longueurNom = wcslen(nom);
    if (longueurDossier + 1 + longueurNom + 1 > taille) {
        return false;
    }
    memcpy(sortie, dossier, longueurDossier * sizeof(WCHAR));
    sortie[longueurDossier] = L'\\';
    memcpy(sortie + longueurDossier + 1, nom, longueurNom * sizeof(WCHAR));
    sortie[longueurDossier + 1 + longueurNom] = L'\0';
    return true;
}

// Chemin de Chrome, d'après la liste « App Paths » du registre, où Windows
// range les programmes installés (vue 64 bits : le mod tourne en 32 bits).
// Faux si Chrome n'est pas installé.
bool CheminChrome(WCHAR* chemin, DWORD taille) {
    const WCHAR CLE[] =
        L"Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe";
    const HKEY RACINES[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};
    for (HKEY racine : RACINES) {
        DWORD octets = taille * sizeof(WCHAR);
        if (RegGetValue(racine, CLE, nullptr,
                        RRF_RT_REG_SZ | RRF_SUBKEY_WOW6464KEY, nullptr, chemin,
                        &octets) == ERROR_SUCCESS) {
            return true;
        }
    }
    return false;
}

// Ouvre une adresse web dans Chrome (ou le navigateur par défaut si Chrome
// n'est pas installé), dans un profil précis si « profil » est donné
// (« Profile 1 »...).
void OuvrirDansChrome(const WCHAR* adresse, const WCHAR* profil = nullptr) {
    WCHAR chrome[MAX_PATH], parametre[2200];
    if (CheminChrome(chrome, MAX_PATH)) {
        if (profil && profil[0]) {
            swprintf_s(parametre, L"--profile-directory=\"%s\" \"%s\"",
                       profil, adresse);
        } else {
            swprintf_s(parametre, L"\"%s\"", adresse);
        }
        Lancer(chrome, parametre);
    } else {
        Lancer(adresse, nullptr);
    }
}

// ---------------------------------------------------------------------------
// Mesures du système (mises à jour une fois par seconde)
// ---------------------------------------------------------------------------

const int NB_POINTS_COURBE = 420;  // un point par pixel de la courbe

struct Mesures {
    float cpu;  // de 0 à 1
    float courbeCpu[NB_POINTS_COURBE];  // le plus récent à la fin
    float ramUtiliseeGo, ramTotaleGo;
    float gpu;  // de 0 à 1, ou -1 si indisponible
    float disqueUtiliseGo, disqueTotalGo;
    double descenteOctetsParS, monteeOctetsParS;
};
Mesures g_mesures;

// Valeurs précédentes, pour calculer des écarts d'une seconde à l'autre.
ULONGLONG g_cpuInactifAvant, g_cpuTotalAvant;
ULONG64 g_recusAvant, g_envoyesAvant;
ULONGLONG g_instantReseauAvant;
int g_secondesAvantDisque;

PDH_HQUERY g_requeteGpu;
PDH_HCOUNTER g_compteurGpu;

void DemarrerMesures() {
    for (float& point : g_mesures.courbeCpu) {
        point = 0.0f;
    }
    g_mesures.gpu = -1.0f;

    // Carte graphique : compteurs « GPU Engine » du moteur 3D. On utilise le
    // nom anglais du compteur : Windows est en français, le nom traduit
    // changerait selon la langue.
    if (PdhOpenQuery(nullptr, 0, &g_requeteGpu) == ERROR_SUCCESS) {
        if (PdhAddEnglishCounter(
                g_requeteGpu,
                L"\\GPU Engine(*engtype_3D)\\Utilization Percentage", 0,
                &g_compteurGpu) == ERROR_SUCCESS) {
            PdhCollectQueryData(g_requeteGpu);  // première prise, sans valeur
        } else {
            Wh_Log(L"Compteur GPU indisponible");
            PdhCloseQuery(g_requeteGpu);
            g_requeteGpu = nullptr;
        }
    }
}

void ArreterMesures() {
    if (g_requeteGpu) {
        PdhCloseQuery(g_requeteGpu);
        g_requeteGpu = nullptr;
    }
}

void MesurerCpu() {
    FILETIME inactif, noyau, utilisateur;
    if (!GetSystemTimes(&inactif, &noyau, &utilisateur)) {
        return;
    }
    // Le temps « noyau » contient déjà le temps inactif.
    ULONGLONG tempsInactif = VersEntier(inactif);
    ULONGLONG tempsTotal = VersEntier(noyau) + VersEntier(utilisateur);
    ULONGLONG ecartTotal = tempsTotal - g_cpuTotalAvant;
    if (g_cpuTotalAvant && ecartTotal) {
        g_mesures.cpu =
            1.0f - (float)(tempsInactif - g_cpuInactifAvant) / ecartTotal;
    }
    g_cpuInactifAvant = tempsInactif;
    g_cpuTotalAvant = tempsTotal;

    // La courbe avance d'un point par seconde.
    for (int i = 0; i < NB_POINTS_COURBE - 1; i++) {
        g_mesures.courbeCpu[i] = g_mesures.courbeCpu[i + 1];
    }
    g_mesures.courbeCpu[NB_POINTS_COURBE - 1] = g_mesures.cpu;
}

void MesurerMemoire() {
    MEMORYSTATUSEX etat = {};
    etat.dwLength = sizeof(etat);
    if (GlobalMemoryStatusEx(&etat)) {
        const float GO = 1073741824.0f;
        g_mesures.ramTotaleGo = etat.ullTotalPhys / GO;
        g_mesures.ramUtiliseeGo = (etat.ullTotalPhys - etat.ullAvailPhys) / GO;
    }
}

void MesurerGpu() {
    if (!g_requeteGpu || PdhCollectQueryData(g_requeteGpu) != ERROR_SUCCESS) {
        return;
    }

    // Un compteur par processus qui utilise la carte : on les additionne.
    DWORD tailleOctets = 0, nombre = 0;
    if (PdhGetFormattedCounterArray(g_compteurGpu, PDH_FMT_DOUBLE,
                                    &tailleOctets, &nombre,
                                    nullptr) != (PDH_STATUS)PDH_MORE_DATA) {
        return;
    }
    auto valeurs = (PDH_FMT_COUNTERVALUE_ITEM*)HeapAlloc(GetProcessHeap(), 0,
                                                         tailleOctets);
    if (!valeurs) {
        return;
    }
    if (PdhGetFormattedCounterArray(g_compteurGpu, PDH_FMT_DOUBLE,
                                    &tailleOctets, &nombre,
                                    valeurs) == ERROR_SUCCESS) {
        double total = 0.0;
        for (DWORD i = 0; i < nombre; i++) {
            if (valeurs[i].FmtValue.CStatus == PDH_CSTATUS_VALID_DATA) {
                total += valeurs[i].FmtValue.doubleValue;
            }
        }
        g_mesures.gpu = (float)(total > 100.0 ? 1.0 : total / 100.0);
    }
    HeapFree(GetProcessHeap(), 0, valeurs);
}

void MesurerDisque() {
    ULARGE_INTEGER libre, total;
    if (GetDiskFreeSpaceEx(L"C:\\", nullptr, &total, &libre)) {
        const float GO = 1073741824.0f;
        g_mesures.disqueTotalGo = total.QuadPart / GO;
        g_mesures.disqueUtiliseGo = (total.QuadPart - libre.QuadPart) / GO;
    }
}

void MesurerReseau() {
    MIB_IF_TABLE2* table = nullptr;
    if (GetIfTable2(&table) != NO_ERROR) {
        return;
    }
    // Seules les vraies cartes réseau connectées comptent : pas la boucle
    // locale, ni les cartes virtuelles, ni les « filtres » que Windows pose
    // sur une carte (ils compteraient les mêmes octets une deuxième fois).
    ULONG64 recus = 0, envoyes = 0;
    for (ULONG i = 0; i < table->NumEntries; i++) {
        const MIB_IF_ROW2& carte = table->Table[i];
        if (carte.InterfaceAndOperStatusFlags.HardwareInterface &&
            !carte.InterfaceAndOperStatusFlags.FilterInterface &&
            carte.OperStatus == IfOperStatusUp &&
            carte.Type != IF_TYPE_SOFTWARE_LOOPBACK) {
            recus += carte.InOctets;
            envoyes += carte.OutOctets;
        }
    }
    FreeMibTable(table);

    ULONGLONG maintenant = GetTickCount64();
    if (g_instantReseauAvant && maintenant > g_instantReseauAvant &&
        recus >= g_recusAvant && envoyes >= g_envoyesAvant) {
        double secondes = (maintenant - g_instantReseauAvant) / 1000.0;
        g_mesures.descenteOctetsParS = (recus - g_recusAvant) / secondes;
        g_mesures.monteeOctetsParS = (envoyes - g_envoyesAvant) / secondes;
    }
    g_recusAvant = recus;
    g_envoyesAvant = envoyes;
    g_instantReseauAvant = maintenant;
}

void MettreAJourMesures() {
    MesurerCpu();
    MesurerMemoire();
    MesurerGpu();
    MesurerReseau();
    if (g_secondesAvantDisque-- <= 0) {
        MesurerDisque();  // le disque change lentement : toutes les 30 s
        g_secondesAvantDisque = 30;
    }
}

float PartRam() {
    return g_mesures.ramTotaleGo > 0
               ? g_mesures.ramUtiliseeGo / g_mesures.ramTotaleGo
               : 0.0f;
}

float PartDisque() {
    return g_mesures.disqueTotalGo > 0
               ? g_mesures.disqueUtiliseGo / g_mesures.disqueTotalGo
               : 0.0f;
}

// ---------------------------------------------------------------------------
// Applications de la grille (lues dans un thread à part)
// ---------------------------------------------------------------------------

const int NB_TUILES = 26;  // 9 × 3, moins la place de « Ajouter »

struct App {
    WCHAR nom[64];
    WCHAR chemin[MAX_PATH];
    int largeurIcone, hauteurIcone;
    BYTE* pixels;           // icône : bleu, vert, rouge, opacité prémultipliée
    ID2D1Bitmap* bitmap;    // la même, prête pour Direct2D (thread fenêtres)
};

struct ListeApps {
    int nombre;
    App apps[NB_TUILES];
};

ListeApps* g_apps;              // la liste affichée (thread fenêtres)
LONG g_generationApps;          // numéro de la dernière lecture demandée
ULONGLONG g_instantLectureApps; // pour relire au plus toutes les 10 s

struct DemandeApps {
    LONG generation;
    int tailleIcone;  // en pixels réels
    WCHAR dossier[MAX_PATH];
    WCHAR renommages[512];
};

void LibererApps(ListeApps* liste) {
    if (!liste) {
        return;
    }
    for (int i = 0; i < liste->nombre; i++) {
        Liberer(liste->apps[i].bitmap);
        if (liste->apps[i].pixels) {
            HeapFree(GetProcessHeap(), 0, liste->apps[i].pixels);
        }
    }
    HeapFree(GetProcessHeap(), 0, liste);
}

// « 01 Microsoft Teams.lnk » devient « Teams » : on enlève l'extension, le
// préfixe numérique, « Microsoft », puis on applique les renommages réglés
// (« Samsung Notes=Notes; ChatGPT Classic=ChatGPT »).
void NomAffiche(const WCHAR* fichier, const WCHAR* renommages, WCHAR* nom,
                int taille) {
    const WCHAR* debut = fichier;
    while (*debut >= L'0' && *debut <= L'9') {
        debut++;
    }
    if (debut != fichier) {
        while (*debut == L' ') {
            debut++;
        }
    }
    if (wcsncmp(debut, L"Microsoft ", 10) == 0) {
        debut += 10;
    }
    lstrcpyn(nom, debut, taille);
    WCHAR* point = wcsrchr(nom, L'.');
    if (point) {
        *point = L'\0';
    }

    // Parcourt « avant=après; avant=après ».
    const WCHAR* p = renommages;
    while (*p) {
        while (*p == L' ' || *p == L';') {
            p++;
        }
        const WCHAR* egal = wcschr(p, L'=');
        const WCHAR* fin = wcschr(p, L';');
        if (!fin) {
            fin = p + wcslen(p);
        }
        if (!egal || egal > fin) {
            p = fin;
            continue;
        }
        const WCHAR* finAvant = egal;
        while (finAvant > p && finAvant[-1] == L' ') {
            finAvant--;
        }
        const WCHAR* apres = egal + 1;
        while (*apres == L' ') {
            apres++;
        }
        const WCHAR* finApres = fin;
        while (finApres > apres && finApres[-1] == L' ') {
            finApres--;
        }
        int longueurAvant = (int)(finAvant - p);
        if ((int)wcslen(nom) == longueurAvant &&
            wcsncmp(nom, p, longueurAvant) == 0) {
            int longueurApres = (int)(finApres - apres);
            if (longueurApres < taille) {
                wcsncpy(nom, apres, longueurApres);
                nom[longueurApres] = L'\0';
            }
            return;
        }
        p = fin;
    }
}

// Range une icône dans l'app, en pixels prémultipliés (le format attendu par
// Direct2D). Une app qui n'a qu'une petite icône (Rhapsody : 64 px) arrive
// posée dans un coin d'une grande image vide : on recadre alors sur la
// partie visible pour qu'elle soit agrandie au lieu de rester minuscule.
void RangerIcone(App* app, BYTE* pixels, int largeur, int hauteur) {
    // Si une couleur dépasse l'opacité, l'image n'est pas prémultipliée :
    // on la convertit.
    bool premultipliee = true;
    for (int i = 0; i < largeur * hauteur && premultipliee; i++) {
        BYTE* p = pixels + 4 * i;
        if (p[0] > p[3] || p[1] > p[3] || p[2] > p[3]) {
            premultipliee = false;
        }
    }
    if (!premultipliee) {
        for (int i = 0; i < largeur * hauteur; i++) {
            BYTE* p = pixels + 4 * i;
            for (int c = 0; c < 3; c++) {
                p[c] = (BYTE)(p[c] * p[3] / 255);
            }
        }
    }

    int gauche = largeur, haut = hauteur, droite = -1, bas = -1;
    for (int y = 0; y < hauteur; y++) {
        for (int x = 0; x < largeur; x++) {
            if (pixels[4 * (y * largeur + x) + 3] > 8) {
                if (x < gauche) gauche = x;
                if (x > droite) droite = x;
                if (y < haut) haut = y;
                if (y > bas) bas = y;
            }
        }
    }
    int largeurVisible = droite - gauche + 1, hauteurVisible = bas - haut + 1;
    if (droite >= 0 && largeurVisible * 10 < largeur * 6 &&
        hauteurVisible * 10 < hauteur * 6) {
        int cote = largeurVisible > hauteurVisible ? largeurVisible
                                                   : hauteurVisible;
        BYTE* recadre =
            (BYTE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cote * cote * 4);
        if (recadre) {
            int decalageX = (cote - largeurVisible) / 2;
            int decalageY = (cote - hauteurVisible) / 2;
            for (int y = 0; y < hauteurVisible; y++) {
                memcpy(recadre + 4 * ((y + decalageY) * cote + decalageX),
                       pixels + 4 * ((y + haut) * largeur + gauche),
                       largeurVisible * 4);
            }
            HeapFree(GetProcessHeap(), 0, pixels);
            pixels = recadre;
            largeur = hauteur = cote;
        }
    }

    app->pixels = pixels;
    app->largeurIcone = largeur;
    app->hauteurIcone = hauteur;
}

// Icône d'un raccourci telle que Windows l'affiche (y compris une icône
// personnalisée), sans la flèche de raccourci. Marche pour les apps
// classiques. Pour une app du Microsoft Store, un programme 32 bits comme
// windhawk.exe n'obtient qu'une page blanche (testé) : voir LireLogoPaquet.
bool LireIconeShell(App* app, int taille) {
    IShellItem* element = nullptr;
    IShellItemImageFactory* fabrique = nullptr;
    HBITMAP image = nullptr;
    bool obtenue =
        SUCCEEDED(SHCreateItemFromParsingName(app->chemin, nullptr,
                                              IID_IShellItem,
                                              (void**)&element)) &&
        SUCCEEDED(element->QueryInterface(IID_IShellItemImageFactory,
                                          (void**)&fabrique)) &&
        SUCCEEDED(fabrique->GetImage({taille, taille},
                                     SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK,
                                     &image));
    Liberer(fabrique);
    Liberer(element);
    if (!obtenue) {
        return false;
    }

    BITMAP infosImage;
    if (!GetObject(image, sizeof(infosImage), &infosImage) ||
        infosImage.bmWidth <= 0 || infosImage.bmHeight <= 0) {
        DeleteObject(image);
        return false;
    }
    int largeur = infosImage.bmWidth, hauteur = infosImage.bmHeight;
    BYTE* pixels = (BYTE*)HeapAlloc(GetProcessHeap(), 0, largeur * hauteur * 4);
    if (!pixels) {
        DeleteObject(image);
        return false;
    }

    BITMAPINFO format = {};
    format.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    format.bmiHeader.biWidth = largeur;
    format.bmiHeader.biHeight = -hauteur;  // ligne 0 en haut
    format.bmiHeader.biPlanes = 1;
    format.bmiHeader.biBitCount = 32;
    format.bmiHeader.biCompression = BI_RGB;
    HDC dc = GetDC(nullptr);
    int lignes = dc ? GetDIBits(dc, image, 0, hauteur, pixels, &format,
                                DIB_RGB_COLORS)
                    : 0;
    if (dc) {
        ReleaseDC(nullptr, dc);
    }
    DeleteObject(image);
    if (lignes != hauteur) {
        HeapFree(GetProcessHeap(), 0, pixels);
        return false;
    }
    RangerIcone(app, pixels, largeur, hauteur);
    return true;
}

// Identifiant d'app du Store vers lequel pointe le raccourci, par exemple
// « SpotifyAB.SpotifyMusic_zpdnekdrzrea0!Spotify » (famille du paquet, « ! »,
// nom de l'app dans le paquet). Faux pour une app classique.
bool IdentifiantAppStore(const WCHAR* raccourci, WCHAR* identifiant,
                         int taille) {
    identifiant[0] = L'\0';
    IShellLink* lien = nullptr;
    IPersistFile* fichier = nullptr;
    PIDLIST_ABSOLUTE cible = nullptr;
    IShellItem2* element = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr,
                                   CLSCTX_INPROC_SERVER, IID_IShellLink,
                                   (void**)&lien)) &&
        SUCCEEDED(lien->QueryInterface(IID_IPersistFile, (void**)&fichier)) &&
        SUCCEEDED(fichier->Load(raccourci, STGM_READ)) &&
        SUCCEEDED(lien->GetIDList(&cible)) &&
        SUCCEEDED(SHCreateItemFromIDList(cible, IID_IShellItem2,
                                         (void**)&element))) {
        LPWSTR valeur = nullptr;
        if (SUCCEEDED(element->GetString(PKEY_AppUserModel_ID, &valeur))) {
            lstrcpyn(identifiant, valeur, taille);
            CoTaskMemFree(valeur);
        }
    }
    Liberer(element);
    if (cible) {
        CoTaskMemFree(cible);
    }
    Liberer(fichier);
    Liberer(lien);
    return wcschr(identifiant, L'!') != nullptr;
}

// Lit un fichier texte UTF-8 (le manifeste d'un paquet) en texte Windows.
WCHAR* LireFichierTexte(const WCHAR* chemin) {
    HANDLE fichier = CreateFile(
        chemin, GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, 0, nullptr);
    if (fichier == INVALID_HANDLE_VALUE) {
        return nullptr;
    }
    WCHAR* texte = nullptr;
    DWORD taille = GetFileSize(fichier, nullptr);
    char* octets = taille && taille < 4 * 1024 * 1024
                       ? (char*)HeapAlloc(GetProcessHeap(), 0, taille)
                       : nullptr;
    DWORD lus = 0;
    if (octets && ReadFile(fichier, octets, taille, &lus, nullptr) &&
        lus == taille) {
        int longueur =
            MultiByteToWideChar(CP_UTF8, 0, octets, taille, nullptr, 0);
        texte = (WCHAR*)HeapAlloc(GetProcessHeap(), 0,
                                  (longueur + 1) * sizeof(WCHAR));
        if (texte) {
            MultiByteToWideChar(CP_UTF8, 0, octets, taille, texte, longueur);
            texte[longueur] = L'\0';
        }
    }
    if (octets) {
        HeapFree(GetProcessHeap(), 0, octets);
    }
    CloseHandle(fichier);
    return texte;
}

// Dans le manifeste (AppxManifest.xml), le logo de l'app est l'attribut
// Square44x44Logo de son <Application Id="...">. Exemple :
// « Images\TeamsForWorkNewAppList.png ».
bool LogoDuManifeste(const WCHAR* dossierPaquet, const WCHAR* idApp,
                     WCHAR* logo, int taille) {
    WCHAR chemin[MAX_PATH];
    swprintf_s(chemin, L"%s\\AppxManifest.xml", dossierPaquet);
    WCHAR* manifeste = LireFichierTexte(chemin);
    if (!manifeste) {
        return false;
    }

    WCHAR recherche[300];
    swprintf_s(recherche, L"Id=\"%s\"", idApp);
    bool trouve = false;
    const WCHAR* application = wcsstr(manifeste, recherche);
    const WCHAR* attribut =
        application ? wcsstr(application, L"Square44x44Logo=\"") : nullptr;
    if (attribut) {
        attribut += wcslen(L"Square44x44Logo=\"");
        const WCHAR* fin = wcschr(attribut, L'"');
        int longueur = fin ? (int)(fin - attribut) : 0;
        if (longueur > 0 && longueur < taille) {
            wcsncpy(logo, attribut, longueur);
            logo[longueur] = L'\0';
            trouve = true;
        }
    }
    HeapFree(GetProcessHeap(), 0, manifeste);
    return trouve;
}

// Le paquet contient plusieurs versions du logo : « .targetsize-48 »,
// « .scale-200 », « _altform-unplated »... On prend la plus grande, de
// préférence « unplated » (sans fond de couleur, comme dans la barre des
// tâches), mais jamais « lightunplated » ni « contrast » (thèmes clairs et
// contrastés).
bool MeilleurFichierLogo(const WCHAR* dossierPaquet, const WCHAR* logo,
                         WCHAR* fichier) {
    WCHAR base[MAX_PATH];
    swprintf_s(base, L"%s\\%s", dossierPaquet, logo);
    WCHAR* extension = wcsrchr(base, L'.');
    if (!extension || wcsrchr(base, L'\\') > extension) {
        return false;
    }
    *extension = L'\0';
    WCHAR dossier[MAX_PATH];
    lstrcpyn(dossier, base, MAX_PATH);
    *wcsrchr(dossier, L'\\') = L'\0';

    WCHAR motif[MAX_PATH];
    swprintf_s(motif, L"%s*.png", base);
    WIN32_FIND_DATA trouve;
    HANDLE recherche = FindFirstFile(motif, &trouve);
    if (recherche == INVALID_HANDLE_VALUE) {
        return false;
    }
    int meilleurScore = -1;
    do {
        const WCHAR* nom = trouve.cFileName;
        if (wcsstr(nom, L"lightunplated") || wcsstr(nom, L"contrast")) {
            continue;
        }
        int taille = 44;  // fichier sans précision : taille de base
        const WCHAR* cible = wcsstr(nom, L"targetsize-");
        const WCHAR* echelle = wcsstr(nom, L"scale-");
        if (cible) {
            taille = _wtoi(cible + wcslen(L"targetsize-"));
        } else if (echelle) {
            taille = 44 * _wtoi(echelle + wcslen(L"scale-")) / 100;
        }
        if (taille > 512) {
            taille = 512;
        }
        int score = taille + (wcsstr(nom, L"altform-unplated") ? 1000 : 0);
        if (score > meilleurScore) {
            meilleurScore = score;
            swprintf_s(fichier, MAX_PATH, L"%s\\%s", dossier, nom);
        }
    } while (FindNextFile(recherche, &trouve));
    FindClose(recherche);
    return meilleurScore >= 0;
}

// Décode une image (PNG...) avec WIC, le décodeur d'images de Windows, à la
// taille demandée, en pixels prémultipliés. Les pixels sont à libérer avec
// HeapFree.
bool DecoderImage(const WCHAR* fichier, int largeur, int hauteur,
                  BYTE** pixelsSortie) {
    IWICImagingFactory* fabrique = nullptr;
    IWICBitmapDecoder* decodeur = nullptr;
    IWICBitmapFrameDecode* image = nullptr;
    IWICBitmapScaler* reduction = nullptr;
    IWICFormatConverter* conversion = nullptr;
    BYTE* pixels = nullptr;
    bool reussi = false;

    if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                   CLSCTX_INPROC_SERVER,
                                   IID_IWICImagingFactory,
                                   (void**)&fabrique)) &&
        SUCCEEDED(fabrique->CreateDecoderFromFilename(
            fichier, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand,
            &decodeur)) &&
        SUCCEEDED(decodeur->GetFrame(0, &image)) &&
        SUCCEEDED(fabrique->CreateBitmapScaler(&reduction)) &&
        SUCCEEDED(reduction->Initialize(image, largeur, hauteur,
                                        WICBitmapInterpolationModeFant)) &&
        SUCCEEDED(fabrique->CreateFormatConverter(&conversion)) &&
        SUCCEEDED(conversion->Initialize(
            reduction, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
            nullptr, 0.0, WICBitmapPaletteTypeCustom))) {
        pixels = (BYTE*)HeapAlloc(GetProcessHeap(), 0, largeur * hauteur * 4);
        if (pixels &&
            SUCCEEDED(conversion->CopyPixels(nullptr, largeur * 4,
                                             largeur * hauteur * 4, pixels))) {
            *pixelsSortie = pixels;
            reussi = true;
        } else if (pixels) {
            HeapFree(GetProcessHeap(), 0, pixels);
        }
    }
    Liberer(conversion);
    Liberer(reduction);
    Liberer(image);
    Liberer(decodeur);
    Liberer(fabrique);
    return reussi;
}

bool DecoderPng(const WCHAR* fichier, int taille, App* app) {
    BYTE* pixels = nullptr;
    if (!DecoderImage(fichier, taille, taille, &pixels)) {
        return false;
    }
    RangerIcone(app, pixels, taille, taille);
    return true;
}

// Logo d'une app du Microsoft Store, lu directement dans son paquet.
bool LireLogoPaquet(App* app, int taille) {
    WCHAR identifiant[256];
    if (!IdentifiantAppStore(app->chemin, identifiant,
                             ARRAYSIZE(identifiant))) {
        return false;
    }
    WCHAR* point = wcschr(identifiant, L'!');
    *point = L'\0';
    const WCHAR* famille = identifiant;
    const WCHAR* idApp = point + 1;

    // Famille du paquet → nom complet du paquet installé → son dossier.
    UINT32 nombre = 0, longueurNoms = 0;
    GetPackagesByPackageFamily(famille, &nombre, nullptr, &longueurNoms,
                               nullptr);
    if (nombre == 0 || nombre > 16 || longueurNoms > 4096) {
        return false;
    }
    PWSTR noms[16];
    WCHAR tampon[4096];
    if (GetPackagesByPackageFamily(famille, &nombre, noms, &longueurNoms,
                                   tampon) != ERROR_SUCCESS) {
        return false;
    }
    WCHAR dossierPaquet[MAX_PATH];
    UINT32 longueurDossier = MAX_PATH;
    if (GetPackagePathByFullName(noms[0], &longueurDossier, dossierPaquet) !=
        ERROR_SUCCESS) {
        return false;
    }

    WCHAR logo[MAX_PATH], fichier[MAX_PATH];
    return LogoDuManifeste(dossierPaquet, idApp, logo, ARRAYSIZE(logo)) &&
           MeilleurFichierLogo(dossierPaquet, logo, fichier) &&
           DecoderPng(fichier, taille, app);
}

// Icône d'une app de la grille : logo du paquet pour une app du Store, sinon
// icône donnée par Windows. Si rien ne marche, la tuile reste sans icône.
void LireIcone(App* app, int taille) {
    if (!LireLogoPaquet(app, taille) && !LireIconeShell(app, taille)) {
        Wh_Log(L"Pas d'icône pour %s", app->nom);
    }
}

// Lit le dossier des apps : noms triés, chemins et icônes.
ListeApps* LireApps(const DemandeApps* demande) {
    ListeApps* liste = (ListeApps*)HeapAlloc(GetProcessHeap(),
                                             HEAP_ZERO_MEMORY,
                                             sizeof(ListeApps));
    if (!liste) {
        return nullptr;
    }

    // Le dossier est créé s'il n'existe pas encore (premier lancement).
    if (GetFileAttributes(demande->dossier) == INVALID_FILE_ATTRIBUTES) {
        SHCreateDirectoryEx(nullptr, demande->dossier, nullptr);
    }

    // On garde les 26 premiers fichiers par ordre alphabétique (le préfixe
    // numérique « 01 », « 02 »... fixe donc l'ordre).
    WCHAR motif[MAX_PATH];
    WIN32_FIND_DATA trouve;
    HANDLE recherche = Joindre(motif, MAX_PATH, demande->dossier, L"*")
                           ? FindFirstFile(motif, &trouve)
                           : INVALID_HANDLE_VALUE;
    if (recherche != INVALID_HANDLE_VALUE) {
        do {
            if (trouve.dwFileAttributes &
                (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN)) {
                continue;
            }
            // Insertion à sa place dans la liste triée.
            int place = liste->nombre;
            while (place > 0 &&
                   lstrcmpi(trouve.cFileName,
                            wcsrchr(liste->apps[place - 1].chemin, L'\\') +
                                1) < 0) {
                place--;
            }
            WCHAR chemin[MAX_PATH];
            if (place >= NB_TUILES ||
                !Joindre(chemin, MAX_PATH, demande->dossier,
                         trouve.cFileName)) {
                continue;
            }
            int fin = liste->nombre < NB_TUILES ? liste->nombre
                                                : NB_TUILES - 1;
            for (int i = fin; i > place; i--) {
                liste->apps[i] = liste->apps[i - 1];
            }
            App& app = liste->apps[place];
            app = {};
            lstrcpyn(app.chemin, chemin, MAX_PATH);
            if (liste->nombre < NB_TUILES) {
                liste->nombre++;
            }
        } while (FindNextFile(recherche, &trouve));
        FindClose(recherche);
    }

    for (int i = 0; i < liste->nombre; i++) {
        App& app = liste->apps[i];
        NomAffiche(wcsrchr(app.chemin, L'\\') + 1, demande->renommages,
                   app.nom, ARRAYSIZE(app.nom));
        LireIcone(&app, demande->tailleIcone);
    }
    return liste;
}

DWORD WINAPI FilLectureApps(LPVOID parametre) {
    DemandeApps* demande = (DemandeApps*)parametre;
    HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    ListeApps* liste = LireApps(demande);
    if (liste && !(g_controleur &&
                   PostMessage(g_controleur, WM_APP_APPS,
                               demande->generation, (LPARAM)liste))) {
        LibererApps(liste);  // le mod s'arrête
    }

    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    HeapFree(GetProcessHeap(), 0, demande);
    return 0;
}

// Demande une nouvelle lecture du dossier des apps (dans un thread à part,
// car le dossier peut être sur OneDrive).
void DemanderApps() {
    if (!g_accueil.fenetre) {
        return;
    }
    DemandeApps* demande = (DemandeApps*)HeapAlloc(
        GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(DemandeApps));
    if (!demande) {
        return;
    }
    demande->generation = ++g_generationApps;
    demande->tailleIcone = Arrondir(52 * Echelle(g_accueil.fenetre));
    lstrcpyn(demande->dossier, g_dossierApps, MAX_PATH);
    lstrcpyn(demande->renommages, g_renommages, ARRAYSIZE(demande->renommages));
    g_instantLectureApps = GetTickCount64();

    HANDLE fil =
        CreateThread(nullptr, 0, FilLectureApps, demande, 0, nullptr);
    if (fil) {
        CloseHandle(fil);
    } else {
        HeapFree(GetProcessHeap(), 0, demande);
    }
}

// Les icônes prêtes pour Direct2D dépendent de la « cible » de dessin : si
// elle est recréée, il faut les recréer aussi.
void OublierBitmapsApps() {
    if (g_apps) {
        for (int i = 0; i < g_apps->nombre; i++) {
            Liberer(g_apps->apps[i].bitmap);
        }
    }
}

// ---------------------------------------------------------------------------
// Briques de dessin (coordonnées en pixels de référence, × echelle)
// ---------------------------------------------------------------------------

// Rectangle arrondi plein.
void RemplirArrondi(ID2D1RenderTarget* cible,
                    float x, float y, float largeur, float hauteur,
                    float rayon, Couleur couleur, float echelle) {
    ID2D1SolidColorBrush* pinceau = Pinceau(cible, couleur);
    if (!pinceau) {
        return;
    }
    D2D1_ROUNDED_RECT forme = {
        D2D1::RectF(x * echelle, y * echelle, (x + largeur) * echelle,
                    (y + hauteur) * echelle),
        rayon * echelle, rayon * echelle};
    cible->FillRoundedRectangle(&forme, pinceau);
    pinceau->Release();
}

// Contour de 1 px d'un rectangle arrondi, décalé d'un demi-pixel pour tomber
// pile sur une rangée de pixels (sinon il est flou, étalé sur deux).
void ContourArrondi(ID2D1RenderTarget* cible,
                    float x, float y, float largeur, float hauteur,
                    float rayon, ID2D1Brush* pinceau, float echelle,
                    ID2D1StrokeStyle* style = nullptr) {
    D2D1_ROUNDED_RECT forme = {
        D2D1::RectF(Arrondir(x * echelle) + 0.5f, Arrondir(y * echelle) + 0.5f,
                    Arrondir((x + largeur) * echelle) - 0.5f,
                    Arrondir((y + hauteur) * echelle) - 0.5f),
        rayon * echelle, rayon * echelle};
    cible->DrawRoundedRectangle(&forme, pinceau, 1.0f, style);
}

// Verre bleu nuit en dégradé + contour qui s'efface, sur une zone donnée.
void DessinerVerreZone(ID2D1RenderTarget* cible,
                       float x, float y, float largeur, float hauteur,
                       float rayon, float echelle) {
    ID2D1LinearGradientBrush* verre =
        CreerDegrade(cible, g_verreHaut, g_verreBas, y * echelle,
                     (y + hauteur) * echelle);
    ID2D1LinearGradientBrush* contour =
        CreerDegrade(cible, CONTOUR_HAUT, CONTOUR_BAS, y * echelle,
                     (y + hauteur) * echelle);
    if (verre && contour) {
        D2D1_ROUNDED_RECT forme = {
            D2D1::RectF(x * echelle, y * echelle, (x + largeur) * echelle,
                        (y + hauteur) * echelle),
            rayon * echelle, rayon * echelle};
        cible->FillRoundedRectangle(&forme, verre);
        ContourArrondi(cible, x, y, largeur, hauteur, rayon, contour, echelle);
    }
    Liberer(contour);
    Liberer(verre);
}

// Le fond d'une carte qui occupe toute la fenêtre.
void DessinerVerre(ID2D1RenderTarget* cible, float echelle) {
    D2D1_SIZE_F taille = cible->GetSize();
    DessinerVerreZone(cible, 0, 0, taille.width / echelle,
                      taille.height / echelle, RAYON_CARTE, echelle);
}

enum Alignement { A_GAUCHE, A_DROITE, A_CENTRE };

// Écrit un texte avec DirectWrite. (x, y) : coin haut gauche du texte, ou son
// bord droit / son centre selon l'alignement. taillePt : la FontSize de
// Rainmeter, qui ne tient pas compte de l'échelle de Windows (1 pt = 4/3 px).
// largeurMax > 0 : le texte trop long est coupé avec « … ».
// Renvoie la hauteur du texte, en pixels de référence.
float Ecrire(ID2D1RenderTarget* cible,
             const WCHAR* texte,
             const WCHAR* police,
             float taillePt,
             DWRITE_FONT_WEIGHT graisse,
             Couleur couleur,
             float x,
             float y,
             Alignement alignement,
             float echelle,
             float largeurMax = 0.0f,
             float* largeurTexte = nullptr) {
    IDWriteTextFormat* format = nullptr;
    if (FAILED(g_fabriqueTexte->CreateTextFormat(
            police, nullptr, graisse, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, taillePt * 4.0f / 3.0f * echelle,
            L"fr-FR", &format))) {
        return 0.0f;
    }
    format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    float largeurBoite = largeurMax > 0.0f ? largeurMax * echelle : 3000.0f;
    float gauche = x * echelle;
    if (alignement == A_DROITE) {
        format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
        gauche -= largeurBoite;
    } else if (alignement == A_CENTRE) {
        format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        gauche -= largeurBoite / 2.0f;
    }

    if (largeurMax > 0.0f) {
        IDWriteInlineObject* troisPoints = nullptr;
        if (SUCCEEDED(g_fabriqueTexte->CreateEllipsisTrimmingSign(
                format, &troisPoints))) {
            DWRITE_TRIMMING coupe = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0,
                                     0};
            format->SetTrimming(&coupe, troisPoints);
            troisPoints->Release();
        }
    }

    float hauteur = 0.0f;
    IDWriteTextLayout* miseEnPage = nullptr;
    if (SUCCEEDED(g_fabriqueTexte->CreateTextLayout(
            texte, (UINT32)wcslen(texte), format, largeurBoite, 2000.0f,
            &miseEnPage))) {
        ID2D1SolidColorBrush* pinceau = Pinceau(cible, couleur);
        if (pinceau) {
            cible->DrawTextLayout(D2D1::Point2F(gauche, y * echelle),
                                  miseEnPage, pinceau,
                                  D2D1_DRAW_TEXT_OPTIONS_NONE);
            pinceau->Release();
        }
        DWRITE_TEXT_METRICS metriques;
        if (SUCCEEDED(miseEnPage->GetMetrics(&metriques))) {
            hauteur = metriques.height / echelle;
            if (largeurTexte) {
                *largeurTexte =
                    metriques.widthIncludingTrailingWhitespace / echelle;
            }
        }
        miseEnPage->Release();
    }
    format->Release();
    return hauteur;
}

// Même texte précédé d'une ombre noire décalée d'un pixel (StringEffect=Shadow).
float EcrireAvecOmbre(ID2D1RenderTarget* cible, const WCHAR* texte,
                      const WCHAR* police, float taillePt, Couleur couleur,
                      Couleur ombre, float x, float y, Alignement alignement,
                      float echelle, float largeurMax = 0.0f) {
    float pixel = 1.0f / echelle;
    Ecrire(cible, texte, police, taillePt, DWRITE_FONT_WEIGHT_NORMAL, ombre,
           x + pixel, y + pixel, alignement, echelle, largeurMax);
    return Ecrire(cible, texte, police, taillePt, DWRITE_FONT_WEIGHT_NORMAL,
                  couleur, x, y, alignement, echelle, largeurMax);
}

// Barre de progression arrondie : piste grise + partie remplie en bleu.
void DessinerBarre(ID2D1RenderTarget* cible, float x, float y, float largeur,
                   float hauteur, float valeur, float echelle) {
    if (valeur < 0.0f) valeur = 0.0f;
    if (valeur > 1.0f) valeur = 1.0f;
    float remplie = largeur * valeur;
    if (remplie < hauteur) {
        remplie = hauteur;  // jamais plus petite qu'un rond
    }
    RemplirArrondi(cible, x, y, largeur, hauteur, hauteur / 2, PISTE, echelle);
    RemplirArrondi(cible, x, y, remplie, hauteur, hauteur / 2, g_accent, echelle);
}

// Trace une ligne brisée (ouverte ou fermée) avec des angles arrondis.
void DessinerLigne(ID2D1RenderTarget* cible, const D2D1_POINT_2F* points,
                   int nombre, bool fermee, float epaisseur, Couleur couleur) {
    ID2D1PathGeometry* trace = nullptr;
    ID2D1GeometrySink* crayon = nullptr;
    ID2D1SolidColorBrush* pinceau = Pinceau(cible, couleur);
    if (pinceau && nombre > 1 &&
        SUCCEEDED(g_fabriqueD2D->CreatePathGeometry(&trace)) &&
        SUCCEEDED(trace->Open(&crayon))) {
        crayon->BeginFigure(points[0], D2D1_FIGURE_BEGIN_HOLLOW);
        for (int i = 1; i < nombre; i++) {
            crayon->AddLine(points[i]);
        }
        crayon->EndFigure(fermee ? D2D1_FIGURE_END_CLOSED
                                 : D2D1_FIGURE_END_OPEN);
        if (SUCCEEDED(crayon->Close())) {
            cible->DrawGeometry(trace, pinceau, epaisseur, g_traitRond);
        }
    }
    Liberer(crayon);
    Liberer(trace);
    Liberer(pinceau);
}

// L'icône de dossier en trait bleu, comme le « Path Dossier » de Rainmeter.
// taille : 1 pour l'Accès rapide, plus petite pour la barre Dossiers.
void DessinerIconeDossier(ID2D1RenderTarget* cible, float x, float y,
                          const D2D1_POINT_2F (&contour)[6], float epaisseur,
                          float echelle) {
    D2D1_POINT_2F points[6];
    for (int i = 0; i < 6; i++) {
        points[i] = D2D1::Point2F((x + contour[i].x) * echelle,
                                  (y + contour[i].y) * echelle);
    }
    DessinerLigne(cible, points, 6, true, epaisseur * echelle, g_accent);
}

const D2D1_POINT_2F DOSSIER_GRAND[6] = {{18, 22.5f},    {25.5f, 22.5f},
                                        {27.75f, 25.2f}, {40.5f, 25.2f},
                                        {40.5f, 42},     {18, 42}};
const D2D1_POINT_2F DOSSIER_PETIT[6] = {{16, 17}, {23, 17}, {25.5f, 20},
                                        {38, 20}, {38, 35}, {16, 35}};

// Un bouton de dossier : fond (plus clair au survol), contour, icône, nom.
void DessinerBoutonDossier(ID2D1RenderTarget* cible, D2D1_RECT_F r,
                           float rayon, bool survole,
                           const D2D1_POINT_2F (&icone)[6], float epaisseurIcone,
                           const WCHAR* nom, float taillePt, float xTexte,
                           float yTexte, float largeurTexte, float echelle) {
    RemplirArrondi(cible, r.left, r.top, r.right - r.left, r.bottom - r.top,
                   rayon, survole ? BOUTON_SURVOL : BOUTON, echelle);
    ID2D1SolidColorBrush* contour = Pinceau(cible, BOUTON_CONTOUR);
    if (contour) {
        ContourArrondi(cible, r.left, r.top, r.right - r.left,
                       r.bottom - r.top, rayon, contour, echelle);
        contour->Release();
    }
    DessinerIconeDossier(cible, r.left, r.top, icone, epaisseurIcone, echelle);
    Ecrire(cible, nom, POLICE, taillePt, DWRITE_FONT_WEIGHT_NORMAL, TEXTE,
           r.left + xTexte, r.top + yTexte, A_GAUCHE, echelle, largeurTexte);
}

// ---------------------------------------------------------------------------
// Bureau 1 : horloge
// ---------------------------------------------------------------------------

// « vendredi 30 octobre », écrit par Windows en français.
void DateEnFrancais(const SYSTEMTIME& moment, WCHAR* date, int taille) {
    if (!GetDateFormatEx(L"fr-FR", 0, &moment, L"dddd d MMMM", date, taille,
                         nullptr)) {
        date[0] = L'\0';
    }
}

void DessinerHorloge(ID2D1RenderTarget* cible, float echelle) {
    SYSTEMTIME maintenant;
    GetLocalTime(&maintenant);

    WCHAR heure[8];
    swprintf_s(heure, L"%02d:%02d", maintenant.wHour, maintenant.wMinute);
    WCHAR date[64];
    DateEnFrancais(maintenant, date, ARRAYSIZE(date));

    float hauteurHeure = EcrireAvecOmbre(cible, heure, POLICE_TITRE, 94.5f,
                                         TEXTE, OMBRE, 0, 0, A_GAUCHE, echelle);
    Ecrire(cible, date, POLICE, 19.5f, DWRITE_FONT_WEIGHT_NORMAL, TEXTE2, 9,
           hauteurHeure - 9, A_GAUCHE, echelle);
}

// ---------------------------------------------------------------------------
// Bureau 1 : carte Système
// ---------------------------------------------------------------------------

void DessinerCourbeCpu(ID2D1RenderTarget* cible, float echelle) {
    const float X = 30, Y = 111, LARGEUR = 420, HAUTEUR = 51;
    static D2D1_POINT_2F points[NB_POINTS_COURBE];
    for (int i = 0; i < NB_POINTS_COURBE; i++) {
        points[i] = D2D1::Point2F(
            (X + LARGEUR * i / (NB_POINTS_COURBE - 1)) * echelle,
            (Y + HAUTEUR * (1.0f - g_mesures.courbeCpu[i])) * echelle);
    }
    DessinerLigne(cible, points, NB_POINTS_COURBE, false, 3.0f * echelle,
                  g_accent);
}

// Écrit un débit en octets par seconde : « 512 o/s », « 12,3 ko/s »...
void FormaterDebit(WCHAR* texte, size_t taille, double octetsParS) {
    if (octetsParS < 1024.0) {
        swprintf_s(texte, taille, L"%.0f o/s", octetsParS);
    } else if (octetsParS < 1024.0 * 1024.0) {
        swprintf_s(texte, taille, L"%.1f ko/s", octetsParS / 1024.0);
    } else {
        swprintf_s(texte, taille, L"%.1f Mo/s", octetsParS / 1048576.0);
    }
}

void DessinerSysteme(ID2D1RenderTarget* cible, float echelle) {
    const DWRITE_FONT_WEIGHT NORMAL = DWRITE_FONT_WEIGHT_NORMAL;
    const DWRITE_FONT_WEIGHT GRAS = DWRITE_FONT_WEIGHT_SEMI_BOLD;
    WCHAR valeur[64];

    DessinerVerre(cible, echelle);
    Ecrire(cible, L"Système", POLICE, 16.5f, GRAS, TEXTE, 30, 27, A_GAUCHE,
           echelle);
    Ecrire(cible, L"mis à jour à l'instant", POLICE, 13.5f, NORMAL, TEXTE3,
           450, 31.5f, A_DROITE, echelle);

    // Processeur et sa courbe
    Ecrire(cible, L"Processeur", POLICE, 15, NORMAL, TEXTE, 30, 78, A_GAUCHE,
           echelle);
    swprintf_s(valeur, L"%d %%", Arrondir(g_mesures.cpu * 100));
    Ecrire(cible, valeur, POLICE, 15, GRAS, TEXTE, 450, 78, A_DROITE, echelle);
    DessinerCourbeCpu(cible, echelle);

    // Mémoire
    Ecrire(cible, L"Mémoire", POLICE, 15, NORMAL, TEXTE, 30, 177, A_GAUCHE,
           echelle);
    swprintf_s(valeur, L"%.1f / %.1f Go", g_mesures.ramUtiliseeGo,
               g_mesures.ramTotaleGo);
    VirguleFrancaise(valeur);
    Ecrire(cible, valeur, POLICE, 15, NORMAL, TEXTE2, 450, 177, A_DROITE,
           echelle);
    DessinerBarre(cible, 30, 210, 420, 9, PartRam(), echelle);

    // Carte graphique (« — » si les compteurs sont indisponibles)
    Ecrire(cible, L"Carte graphique", POLICE, 15, NORMAL, TEXTE, 30, 231,
           A_GAUCHE, echelle);
    if (g_mesures.gpu >= 0.0f) {
        swprintf_s(valeur, L"%d %%", Arrondir(g_mesures.gpu * 100));
    } else {
        swprintf_s(valeur, L"—");
    }
    Ecrire(cible, valeur, POLICE, 15, NORMAL, TEXTE2, 450, 231, A_DROITE,
           echelle);
    DessinerBarre(cible, 30, 264, 420, 9, g_mesures.gpu, echelle);

    // Disque C:
    Ecrire(cible, L"Disque C:", POLICE, 15, NORMAL, TEXTE, 30, 285, A_GAUCHE,
           echelle);
    swprintf_s(valeur, L"%d / %d Go", Arrondir(g_mesures.disqueUtiliseGo),
               Arrondir(g_mesures.disqueTotalGo));
    Ecrire(cible, valeur, POLICE, 15, NORMAL, TEXTE2, 450, 285, A_DROITE,
           echelle);
    DessinerBarre(cible, 30, 318, 420, 9, PartDisque(), echelle);

    // Séparateur, puis réseau
    ID2D1SolidColorBrush* trait = Pinceau(cible, {255, 255, 255, 20});
    if (trait) {
        float y = Arrondir(348 * echelle) + 0.5f;
        cible->DrawLine(D2D1::Point2F(30 * echelle, y),
                        D2D1::Point2F(450 * echelle, y), trait, 1.0f, nullptr);
        trait->Release();
    }
    Ecrire(cible, L"Réseau", POLICE, 15, NORMAL, TEXTE, 30, 363, A_GAUCHE,
           echelle);
    WCHAR descente[24], montee[24];
    FormaterDebit(descente, ARRAYSIZE(descente), g_mesures.descenteOctetsParS);
    FormaterDebit(montee, ARRAYSIZE(montee), g_mesures.monteeOctetsParS);
    swprintf_s(valeur, L"↓ %s · ↑ %s", descente, montee);
    VirguleFrancaise(valeur);
    Ecrire(cible, valeur, POLICE, 15, NORMAL, TEXTE2, 450, 363, A_DROITE,
           echelle);

    DessinerBoutonsEntretien(cible, echelle);
}

// ---------------------------------------------------------------------------
// Bureau 1 : carte Accès rapide
// ---------------------------------------------------------------------------

// 6 dossiers sur 2 colonnes (zones 0 à 5), puis « Ouvrir l'explorateur ».
const int NB_ZONES_ACCES = 7;
const int ZONE_EXPLORATEUR = 6;

D2D1_RECT_F RectangleAcces(int zone) {
    if (zone == ZONE_EXPLORATEUR) {
        return D2D1::RectF(30, 309, 30 + 389, 309 + 65);
    }
    float x = 30.0f + (zone % 2) * 201.0f;
    float y = 75.0f + (zone / 2) * 78.0f;
    return D2D1::RectF(x, y, x + 188, y + 65);
}

int ZoneAccesRapide(float x, float y) {
    for (int i = 0; i < NB_ZONES_ACCES; i++) {
        if (DansRectangle(x, y, RectangleAcces(i))) {
            return i;
        }
    }
    return AUCUNE_ZONE;
}

void CliquerAccesRapide(int zone) {
    if (zone == ZONE_EXPLORATEUR) {
        OuvrirDossier(nullptr);
    } else if (g_dossiers[zone].chemin[0]) {
        OuvrirDossier(g_dossiers[zone].chemin);
    }
}

void DessinerAccesRapide(ID2D1RenderTarget* cible, float echelle) {
    DessinerVerre(cible, echelle);
    Ecrire(cible, L"Accès rapide", POLICE, 16.5f, DWRITE_FONT_WEIGHT_SEMI_BOLD,
           TEXTE, 30, 27, A_GAUCHE, echelle);

    for (int i = 0; i < 6; i++) {
        DessinerBoutonDossier(cible, RectangleAcces(i), RAYON_BOUTON,
                              i == g_accesRapide.survol, DOSSIER_GRAND, 2.1f,
                              g_dossiers[i].nom, 15, 54, 18, 126, echelle);
    }

    // « Ouvrir l'explorateur » : dégradé d'accent + touche « Win + E ».
    D2D1_RECT_F r = RectangleAcces(ZONE_EXPLORATEUR);
    ID2D1LinearGradientBrush* fond =
        CreerDegrade(cible, AccentTransparent(90), AccentTransparent(45),
                     r.top * echelle, r.bottom * echelle);
    ID2D1SolidColorBrush* contourBleu = Pinceau(cible, AccentTransparent(90));
    if (fond && contourBleu) {
        D2D1_ROUNDED_RECT forme = {
            D2D1::RectF(r.left * echelle, r.top * echelle, r.right * echelle,
                        r.bottom * echelle),
            RAYON_BOUTON * echelle, RAYON_BOUTON * echelle};
        cible->FillRoundedRectangle(&forme, fond);
        ContourArrondi(cible, r.left, r.top, r.right - r.left,
                       r.bottom - r.top, RAYON_BOUTON, contourBleu, echelle);
    }
    Liberer(contourBleu);
    Liberer(fond);
    Ecrire(cible, L"Ouvrir l'explorateur", POLICE, 15,
           DWRITE_FONT_WEIGHT_SEMI_BOLD, TEXTE, 48, 327, A_GAUCHE, echelle);

    ID2D1SolidColorBrush* contourTouche = Pinceau(cible, {255, 255, 255, 46});
    if (contourTouche) {
        ContourArrondi(cible, 309, 324, 95, 35, 9, contourTouche, echelle);
        contourTouche->Release();
    }
    Ecrire(cible, L"Win + E", POLICE, 13.5f, DWRITE_FONT_WEIGHT_NORMAL, TEXTE2,
           357, 330, A_CENTRE, echelle);
}

// ---------------------------------------------------------------------------
// Autres bureaux : écran d'accueil
// ---------------------------------------------------------------------------

// Zones : tuiles 0 à 25, « Ajouter » (26), puis les 6 dossiers (27 à 32).
const int ZONE_AJOUTER = NB_TUILES;
const int ZONE_PREMIER_DOSSIER = NB_TUILES + 1;

// La grille fait 9 colonnes : 88 px de tuile + 28 px d'écart, 134 px par
// rangée (88 + 46 pour le nom). « Ajouter » prend toujours la 27e place.
D2D1_RECT_F RectangleTuile(int place) {
    float x = (place % 9) * 116.0f;
    float y = 170.0f + (place / 9) * 134.0f;
    return D2D1::RectF(x, y, x + 88, y + 88);
}

D2D1_RECT_F RectangleDossierAccueil(int numero) {
    float x = 30.0f + numero * 161.0f;
    return D2D1::RectF(x, 642, x + 149, 642 + 52);
}

int ZoneAccueil(float x, float y) {
    int nombreApps = g_apps ? g_apps->nombre : 0;
    for (int i = 0; i < nombreApps; i++) {
        if (DansRectangle(x, y, RectangleTuile(i))) {
            return i;
        }
    }
    if (DansRectangle(x, y, RectangleTuile(NB_TUILES))) {
        return ZONE_AJOUTER;
    }
    for (int i = 0; i < 6; i++) {
        if (DansRectangle(x, y, RectangleDossierAccueil(i))) {
            return ZONE_PREMIER_DOSSIER + i;
        }
    }
    return AUCUNE_ZONE;
}

void CliquerAccueil(int zone) {
    if (zone >= 0 && zone < NB_TUILES) {
        if (g_apps && zone < g_apps->nombre) {
            Lancer(g_apps->apps[zone].chemin, nullptr);
        }
    } else if (zone == ZONE_AJOUTER) {
        OuvrirDossier(g_dossierApps);
    } else if (zone >= ZONE_PREMIER_DOSSIER) {
        int numero = zone - ZONE_PREMIER_DOSSIER;
        if (g_dossiers[numero].chemin[0]) {
            OuvrirDossier(g_dossiers[numero].chemin);
        }
    }
}

// Pastille CPU / RAM / Disque : petite carte avec nom, pourcentage et barre.
void DessinerPastille(ID2D1RenderTarget* cible, float x, const WCHAR* nom,
                      float valeur, float echelle) {
    const float Y = 34;
    WCHAR texte[16];
    swprintf_s(texte, L"%d %%", Arrondir(valeur * 100));
    DessinerVerreZone(cible, x, Y, 143.5f, 63.5f, 14, echelle);
    Ecrire(cible, nom, POLICE, 11, DWRITE_FONT_WEIGHT_NORMAL, TEXTE2, x + 16,
           46, A_GAUCHE, echelle);
    Ecrire(cible, texte, POLICE, 11, DWRITE_FONT_WEIGHT_SEMI_BOLD, TEXTE,
           x + 128, 46, A_DROITE, echelle);
    DessinerBarre(cible, x + 16, Y + 44, 112, 6, valeur, echelle);
}

// Dessine l'icône d'une app, centrée dans un carré de 52 px.
void DessinerIconeApp(ID2D1RenderTarget* cible, App& app, float x, float y,
                      float echelle) {
    if (!app.pixels) {
        return;  // pas d'icône : la tuile reste vide, sans planter
    }
    if (!app.bitmap) {
        D2D1_BITMAP_PROPERTIES proprietes = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED));
        if (FAILED(cible->CreateBitmap(
                D2D1::SizeU(app.largeurIcone, app.hauteurIcone), app.pixels,
                app.largeurIcone * 4, &proprietes, &app.bitmap))) {
            app.bitmap = nullptr;
            return;
        }
    }
    const float COTE = 52.0f;
    float rapport = (float)app.largeurIcone / app.hauteurIcone;
    float largeur = rapport >= 1 ? COTE : COTE * rapport;
    float hauteur = rapport >= 1 ? COTE / rapport : COTE;
    float gauche = x + (COTE - largeur) / 2, haut = y + (COTE - hauteur) / 2;
    D2D1_RECT_F destination =
        D2D1::RectF(gauche * echelle, haut * echelle,
                    (gauche + largeur) * echelle, (haut + hauteur) * echelle);
    cible->DrawBitmap(app.bitmap, &destination, 1.0f,
                      D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, nullptr);
}

void DessinerAccueil(ID2D1RenderTarget* cible, float echelle) {
    SYSTEMTIME maintenant;
    GetLocalTime(&maintenant);

    // Salutation : « Bonjour » avant 18 h, « Bonsoir » après.
    WCHAR salutation[96];
    swprintf_s(salutation, L"%s%s%s",
               maintenant.wHour < 18 ? L"Bonjour" : L"Bonsoir",
               g_prenom[0] ? L" " : L"", g_prenom);
    float hauteurSalut =
        EcrireAvecOmbre(cible, salutation, POLICE_TITRE, 56, TEXTE,
                        {0, 0, 0, 60}, 0, 0, A_GAUCHE, echelle);

    WCHAR date[64], ligne[96];
    DateEnFrancais(maintenant, date, ARRAYSIZE(date));
    swprintf_s(ligne, L"%s · %02d:%02d", date, maintenant.wHour,
               maintenant.wMinute);
    Ecrire(cible, ligne, POLICE, 15, DWRITE_FONT_WEIGHT_NORMAL, TEXTE2, 4,
           hauteurSalut, A_GAUCHE, echelle);

    DessinerPastille(cible, 560, L"CPU", g_mesures.cpu, echelle);
    DessinerPastille(cible, 716, L"RAM", PartRam(), echelle);
    DessinerPastille(cible, 872, L"Disque", PartDisque(), echelle);

    // Grille des apps
    int nombreApps = g_apps ? g_apps->nombre : 0;
    for (int i = 0; i < nombreApps; i++) {
        App& app = g_apps->apps[i];
        D2D1_RECT_F r = RectangleTuile(i);
        if (i == g_accueil.survol) {
            RemplirArrondi(cible, r.left, r.top, 88, 88, RAYON_TUILE,
                           TUILE_SURVOL, echelle);
            ID2D1SolidColorBrush* contour =
                Pinceau(cible, TUILE_SURVOL_CONTOUR);
            if (contour) {
                ContourArrondi(cible, r.left, r.top, 88, 88, RAYON_TUILE,
                               contour, echelle);
                contour->Release();
            }
        } else {
            DessinerVerreZone(cible, r.left, r.top, 88, 88, RAYON_TUILE,
                              echelle);
        }
        DessinerIconeApp(cible, app, r.left + 18, r.top + 18, echelle);
        EcrireAvecOmbre(cible, app.nom, POLICE, 10.5f, TEXTE, {0, 0, 0, 90},
                        r.left + 44, r.top + 96, A_CENTRE, echelle, 112);
    }

    // Tuile « Ajouter » : fond bleu doux, contour pointillé, grand « + ».
    D2D1_RECT_F r = RectangleTuile(NB_TUILES);
    RemplirArrondi(cible, r.left, r.top, 88, 88, RAYON_TUILE,
                   g_accueil.survol == ZONE_AJOUTER ? TUILE_SURVOL
                                                    : g_accentDoux,
                   echelle);
    ID2D1SolidColorBrush* bleu = Pinceau(cible, g_accent);
    if (bleu) {
        ContourArrondi(cible, r.left, r.top, 88, 88, RAYON_TUILE, bleu,
                       echelle, g_traitPointille);
        bleu->Release();
    }
    D2D1_POINT_2F vertical[2] = {
        D2D1::Point2F((r.left + 44) * echelle, (r.top + 30) * echelle),
        D2D1::Point2F((r.left + 44) * echelle, (r.top + 58) * echelle)};
    D2D1_POINT_2F horizontal[2] = {
        D2D1::Point2F((r.left + 30) * echelle, (r.top + 44) * echelle),
        D2D1::Point2F((r.left + 58) * echelle, (r.top + 44) * echelle)};
    DessinerLigne(cible, vertical, 2, false, 2.5f * echelle, g_accent);
    DessinerLigne(cible, horizontal, 2, false, 2.5f * echelle, g_accent);
    Ecrire(cible, L"Ajouter", POLICE, 10.5f, DWRITE_FONT_WEIGHT_NORMAL, g_accent,
           r.left + 44, r.top + 96, A_CENTRE, echelle);

    // Barre Dossiers
    DessinerVerreZone(cible, 0, 582, 1016, 132, RAYON_CARTE, echelle);
    Ecrire(cible, L"Dossiers", POLICE, 13, DWRITE_FONT_WEIGHT_SEMI_BOLD, TEXTE,
           30, 602, A_GAUCHE, echelle);
    Ecrire(cible, L"Win + E pour l'explorateur", POLICE, 11,
           DWRITE_FONT_WEIGHT_NORMAL, TEXTE3, 986, 606, A_DROITE, echelle);
    for (int i = 0; i < 6; i++) {
        DessinerBoutonDossier(
            cible, RectangleDossierAccueil(i), 14,
            g_accueil.survol == ZONE_PREMIER_DOSSIER + i, DOSSIER_PETIT, 1.8f,
            g_dossiers[i].nom, 12, 50, 15, 91, echelle);
    }
}

// ---------------------------------------------------------------------------
// Bureau 1 : barre de recherche
// ---------------------------------------------------------------------------

// Catalogue des apps installées, lu dans le dossier « Applications » de
// Windows (celui du menu Démarrer : apps classiques et apps du Store).
struct AppInstallee {
    WCHAR nom[128];
    WCHAR identifiant[400];  // à lancer par « shell:AppsFolder\identifiant »
};

struct Catalogue {
    int nombre;
    AppInstallee apps[1];  // en réalité « nombre » cases à la suite
};

Catalogue* g_catalogue;
ULONGLONG g_instantCatalogue;
bool g_lectureCatalogueEnCours;

// Index des dossiers rangés dans les dossiers de l'Accès rapide.
struct DossierIndexe {
    int profondeur;  // 1 : directement dans un dossier de l'Accès rapide
    WCHAR nom[128];
    WCHAR parent[64];  // nom du dossier qui le contient, affiché à droite
    WCHAR chemin[MAX_PATH];
};

struct IndexDossiers {
    int nombre;
    DossierIndexe dossiers[1];  // en réalité « nombre » cases à la suite
};

IndexDossiers* g_index;
ULONGLONG g_instantIndex;
bool g_indexEnCours;

const int MAX_DOSSIERS_INDEX = 8000;
const int PROFONDEUR_INDEX = 4;  // niveaux de sous-dossiers explorés

const int NB_APPS_TROUVEES = 4;
const int NB_DOSSIERS_TROUVES = 4;
enum GenreResultat {
    RESULTAT_APP,
    RESULTAT_DOSSIER,
    RESULTAT_WEB,
    RESULTAT_FICHIERS
};

struct Resultat {
    GenreResultat genre;
    WCHAR libelle[200];
    WCHAR precision[80];  // texte gris à droite : « App », « Documents »...
    WCHAR cible[2048];
};

const int NB_RESULTATS_MAX = NB_APPS_TROUVEES + NB_DOSSIERS_TROUVES + 2;
Resultat g_resultats[NB_RESULTATS_MAX];
int g_nbResultats;
int g_resultatChoisi;  // celui que lance la touche Entrée

WCHAR g_saisie[128];
bool g_barreActive;     // la barre a le clavier
bool g_curseurVisible;  // pour le clignotement du curseur
WCHAR g_moteurRecherche[512];

const float LARGEUR_BARRE = 640, HAUTEUR_BARRE = 56;
const float HAUT_LISTE = 64, HAUTEUR_LIGNE = 44;

DWORD WINAPI FilLectureCatalogue(LPVOID) {
    HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    int capacite = 512;
    Catalogue* catalogue = (Catalogue*)HeapAlloc(
        GetProcessHeap(), 0, sizeof(Catalogue) + capacite * sizeof(AppInstallee));
    IShellItem* dossier = nullptr;
    IEnumShellItems* liste = nullptr;
    if (catalogue) {
        catalogue->nombre = 0;
        if (SUCCEEDED(SHGetKnownFolderItem(FOLDERID_AppsFolder, KF_FLAG_DEFAULT,
                                           nullptr, IID_IShellItem,
                                           (void**)&dossier)) &&
            SUCCEEDED(dossier->BindToHandler(nullptr, BHID_EnumItems,
                                             IID_IEnumShellItems,
                                             (void**)&liste))) {
            IShellItem* element;
            while (liste->Next(1, &element, nullptr) == S_OK) {
                if (catalogue->nombre == capacite) {
                    // Plus de place : on double la taille du catalogue.
                    Catalogue* plusGrand = (Catalogue*)HeapReAlloc(
                        GetProcessHeap(), 0, catalogue,
                        sizeof(Catalogue) + 2 * capacite * sizeof(AppInstallee));
                    if (!plusGrand) {
                        element->Release();
                        break;
                    }
                    catalogue = plusGrand;
                    capacite *= 2;
                }
                LPWSTR nom = nullptr, identifiant = nullptr;
                if (SUCCEEDED(element->GetDisplayName(SIGDN_NORMALDISPLAY, &nom)) &&
                    SUCCEEDED(element->GetDisplayName(
                        SIGDN_PARENTRELATIVEPARSING, &identifiant))) {
                    AppInstallee& app = catalogue->apps[catalogue->nombre++];
                    lstrcpyn(app.nom, nom, ARRAYSIZE(app.nom));
                    lstrcpyn(app.identifiant, identifiant,
                             ARRAYSIZE(app.identifiant));
                }
                CoTaskMemFree(nom);
                CoTaskMemFree(identifiant);
                element->Release();
            }
        }
    }
    Liberer(liste);
    Liberer(dossier);

    if (catalogue && !(g_controleur &&
                       PostMessage(g_controleur, WM_APP_CATALOGUE, 0,
                                   (LPARAM)catalogue))) {
        HeapFree(GetProcessHeap(), 0, catalogue);  // le mod s'arrête
    }
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    return 0;
}

// Relit le catalogue dans un thread à part (environ 0,6 s pour 250 apps).
void DemanderCatalogue() {
    if (g_lectureCatalogueEnCours) {
        return;
    }
    g_lectureCatalogueEnCours = true;
    g_instantCatalogue = GetTickCount64();
    HANDLE fil =
        CreateThread(nullptr, 0, FilLectureCatalogue, nullptr, 0, nullptr);
    if (fil) {
        CloseHandle(fil);
    } else {
        g_lectureCatalogueEnCours = false;
    }
}

// « shell:Desktop » ou « %USERPROFILE%\... » devient un vrai chemin
// (« C:\Users\...\Desktop »). Faux si le dossier n'existe pas.
bool CheminReel(const WCHAR* chemin, WCHAR* reel) {
    reel[0] = L'\0';
    if (_wcsnicmp(chemin, L"shell:", 6) == 0) {
        PIDLIST_ABSOLUTE liste = nullptr;
        SFGAOF attributs;
        if (FAILED(SHParseDisplayName(chemin, nullptr, &liste, 0, &attributs))) {
            return false;
        }
        bool trouve = SHGetPathFromIDList(liste, reel);
        CoTaskMemFree(liste);
        if (!trouve) {
            return false;
        }
    } else if (!ExpandEnvironmentStrings(chemin, reel, MAX_PATH)) {
        return false;
    }
    DWORD attributs = GetFileAttributes(reel);
    return attributs != INVALID_FILE_ATTRIBUTES &&
           (attributs & FILE_ATTRIBUTE_DIRECTORY);
}

// Ajoute à l'index les sous-dossiers de « dossier », jusqu'à
// PROFONDEUR_INDEX niveaux.
void Parcourir(IndexDossiers** index, int* capacite, const WCHAR* dossier,
               int profondeur) {
    WCHAR motif[MAX_PATH];
    if (!Joindre(motif, MAX_PATH, dossier, L"*")) {
        return;
    }
    const WCHAR* nomParent = wcsrchr(dossier, L'\\');
    nomParent = nomParent ? nomParent + 1 : dossier;

    WIN32_FIND_DATA trouve;
    HANDLE recherche = FindFirstFileEx(motif, FindExInfoBasic, &trouve,
                                       FindExSearchLimitToDirectories, nullptr,
                                       FIND_FIRST_EX_LARGE_FETCH);
    if (recherche == INVALID_HANDLE_VALUE) {
        return;
    }
    do {
        DWORD attributs = trouve.dwFileAttributes;
        if (!(attributs & FILE_ATTRIBUTE_DIRECTORY) ||
            (attributs & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) ||
            trouve.cFileName[0] == L'.') {
            continue;
        }
        // Les raccourcis de dossier (jonctions, liens) peuvent faire tourner
        // l'exploration en rond : on ne les suit pas. Les dossiers OneDrive,
        // eux, sont gardés.
        if ((attributs & FILE_ATTRIBUTE_REPARSE_POINT) &&
            (trouve.dwReserved0 == IO_REPARSE_TAG_MOUNT_POINT ||
             trouve.dwReserved0 == IO_REPARSE_TAG_SYMLINK)) {
            continue;
        }
        // Dossiers techniques créés par les outils de développement : des
        // milliers de sous-dossiers sans intérêt pour une recherche.
        const WCHAR* IGNORES[] = {L"node_modules", L"__pycache__",
                                  L"site-packages", L"venv", L".venv"};
        bool ignore = false;
        for (const WCHAR* nom : IGNORES) {
            ignore = ignore || lstrcmpi(trouve.cFileName, nom) == 0;
        }
        if (ignore) {
            continue;
        }
        WCHAR chemin[MAX_PATH];
        if (!Joindre(chemin, MAX_PATH, dossier, trouve.cFileName)) {
            continue;
        }

        if ((*index)->nombre == *capacite) {
            if (*capacite >= MAX_DOSSIERS_INDEX) {
                break;
            }
            IndexDossiers* plusGrand = (IndexDossiers*)HeapReAlloc(
                GetProcessHeap(), 0, *index,
                sizeof(IndexDossiers) + 2 * *capacite * sizeof(DossierIndexe));
            if (!plusGrand) {
                break;
            }
            *index = plusGrand;
            *capacite *= 2;
        }
        DossierIndexe& entree = (*index)->dossiers[(*index)->nombre++];
        entree.profondeur = profondeur;
        lstrcpyn(entree.nom, trouve.cFileName, ARRAYSIZE(entree.nom));
        lstrcpyn(entree.parent, nomParent, ARRAYSIZE(entree.parent));
        lstrcpyn(entree.chemin, chemin, ARRAYSIZE(entree.chemin));

        if (profondeur < PROFONDEUR_INDEX) {
            Parcourir(index, capacite, chemin, profondeur + 1);
        }
    } while ((*index)->nombre < MAX_DOSSIERS_INDEX &&
             FindNextFile(recherche, &trouve));
    FindClose(recherche);
}

struct DemandeIndex {
    WCHAR racines[6][MAX_PATH];
};

DWORD WINAPI FilIndexDossiers(LPVOID parametre) {
    DemandeIndex* demande = (DemandeIndex*)parametre;
    HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    int capacite = 1024;
    IndexDossiers* index = (IndexDossiers*)HeapAlloc(
        GetProcessHeap(), 0,
        sizeof(IndexDossiers) + capacite * sizeof(DossierIndexe));
    if (index) {
        index->nombre = 0;
        for (int i = 0; i < 6; i++) {
            WCHAR racine[MAX_PATH];
            if (demande->racines[i][0] &&
                CheminReel(demande->racines[i], racine)) {
                Parcourir(&index, &capacite, racine, 1);
            }
        }
    }
    if (index && !(g_controleur && PostMessage(g_controleur, WM_APP_INDEX, 0,
                                               (LPARAM)index))) {
        HeapFree(GetProcessHeap(), 0, index);  // le mod s'arrête
    }
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    HeapFree(GetProcessHeap(), 0, demande);
    return 0;
}

// Reconstruit l'index des dossiers dans un thread à part : les dossiers
// peuvent être sur OneDrive, le thread des fenêtres ne doit pas attendre.
void DemanderIndex() {
    if (g_indexEnCours) {
        return;
    }
    DemandeIndex* demande = (DemandeIndex*)HeapAlloc(
        GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(DemandeIndex));
    if (!demande) {
        return;
    }
    for (int i = 0; i < 6; i++) {
        lstrcpyn(demande->racines[i], g_dossiers[i].chemin, MAX_PATH);
    }
    g_indexEnCours = true;
    g_instantIndex = GetTickCount64();
    HANDLE fil = CreateThread(nullptr, 0, FilIndexDossiers, demande, 0, nullptr);
    if (fil) {
        CloseHandle(fil);
    } else {
        g_indexEnCours = false;
        HeapFree(GetProcessHeap(), 0, demande);
    }
}

// 0 : le nom commence par la saisie, 1 : un mot du nom commence par la
// saisie, 2 : la saisie est ailleurs dans le nom, -1 : absente. Majuscules et
// accents ignorés (« parametres » trouve « Paramètres »).
int Correspondance(const WCHAR* nom, const WCHAR* saisie) {
    int position = FindNLSStringEx(
        L"fr-FR",
        FIND_FROMSTART | LINGUISTIC_IGNORECASE | LINGUISTIC_IGNOREDIACRITIC,
        nom, -1, saisie, -1, nullptr, nullptr, nullptr, 0);
    if (position < 0) {
        return -1;
    }
    if (position == 0) {
        return 0;
    }
    WCHAR avant = nom[position - 1];
    return (avant == L' ' || avant == L'-' || avant == L'(' || avant == L'_')
               ? 1
               : 2;
}

// Encode un texte pour une adresse web : « météo paris » devient
// « m%C3%A9t%C3%A9o%20paris ».
void EncoderUrl(const WCHAR* texte, WCHAR* sortie, int taille) {
    char utf8[512];
    int longueur = WideCharToMultiByte(CP_UTF8, 0, texte, -1, utf8,
                                       sizeof(utf8), nullptr, nullptr);
    int j = 0;
    for (int i = 0; i < longueur - 1 && j < taille - 4; i++) {
        unsigned char c = (unsigned char)utf8[i];
        bool simple = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                      (c >= '0' && c <= '9') || c == '-' || c == '_' ||
                      c == '.' || c == '~';
        if (simple) {
            sortie[j++] = c;
        } else {
            swprintf_s(sortie + j, taille - j, L"%%%02X", c);
            j += 3;
        }
    }
    sortie[j] = L'\0';
}

// Garde les « nombreMax » meilleures entrées dans « choisies », triées par
// score de Correspondance, puis par « avant » (critère de départage).
template <typename Avant>
void Classer(int* choisies, int* scores, int* nombre, int nombreMax,
             int candidat, int score, Avant avant) {
    int place = *nombre;
    while (place > 0 && (scores[place - 1] > score ||
                         (scores[place - 1] == score &&
                          avant(candidat, choisies[place - 1])))) {
        place--;
    }
    if (place >= nombreMax) {
        return;
    }
    int fin = *nombre < nombreMax ? *nombre : nombreMax - 1;
    for (int k = fin; k > place; k--) {
        choisies[k] = choisies[k - 1];
        scores[k] = scores[k - 1];
    }
    choisies[place] = candidat;
    scores[place] = score;
    if (*nombre < nombreMax) {
        (*nombre)++;
    }
}

// Calcule les lignes proposées sous la barre pour la saisie actuelle.
void ConstruireResultats() {
    g_nbResultats = 0;
    g_resultatChoisi = 0;
    if (!g_saisie[0]) {
        return;
    }

    // Les apps : les meilleures selon Correspondance, puis le nom le plus
    // court, sans doublon de nom.
    int apps[NB_APPS_TROUVEES], scoresApps[NB_APPS_TROUVEES];
    int nombreApps = 0;
    for (int i = 0; g_catalogue && i < g_catalogue->nombre; i++) {
        const WCHAR* nom = g_catalogue->apps[i].nom;
        int score = Correspondance(nom, g_saisie);
        if (score < 0) {
            continue;
        }
        bool doublon = false;
        for (int k = 0; k < nombreApps && !doublon; k++) {
            doublon = lstrcmpi(g_catalogue->apps[apps[k]].nom, nom) == 0;
        }
        if (!doublon) {
            Classer(apps, scoresApps, &nombreApps, NB_APPS_TROUVEES, i, score,
                    [](int a, int b) {
                        return wcslen(g_catalogue->apps[a].nom) <
                               wcslen(g_catalogue->apps[b].nom);
                    });
        }
    }
    for (int k = 0; k < nombreApps; k++) {
        Resultat& r = g_resultats[g_nbResultats++];
        r.genre = RESULTAT_APP;
        lstrcpyn(r.libelle, g_catalogue->apps[apps[k]].nom,
                 ARRAYSIZE(r.libelle));
        lstrcpy(r.precision, L"App");
        swprintf_s(r.cible, L"shell:AppsFolder\\%s",
                   g_catalogue->apps[apps[k]].identifiant);
    }

    // Les dossiers : les meilleurs selon Correspondance, puis les moins
    // profonds, puis par ordre alphabétique.
    int dossiers[NB_DOSSIERS_TROUVES], scoresDossiers[NB_DOSSIERS_TROUVES];
    int nombreDossiers = 0;
    for (int i = 0; g_index && i < g_index->nombre; i++) {
        int score = Correspondance(g_index->dossiers[i].nom, g_saisie);
        if (score >= 0) {
            Classer(dossiers, scoresDossiers, &nombreDossiers,
                    NB_DOSSIERS_TROUVES, i, score, [](int a, int b) {
                        const DossierIndexe& da = g_index->dossiers[a];
                        const DossierIndexe& db = g_index->dossiers[b];
                        if (da.profondeur != db.profondeur) {
                            return da.profondeur < db.profondeur;
                        }
                        return lstrcmpi(da.nom, db.nom) < 0;
                    });
        }
    }
    for (int k = 0; k < nombreDossiers; k++) {
        const DossierIndexe& d = g_index->dossiers[dossiers[k]];
        Resultat& r = g_resultats[g_nbResultats++];
        r.genre = RESULTAT_DOSSIER;
        lstrcpyn(r.libelle, d.nom, ARRAYSIZE(r.libelle));
        swprintf_s(r.precision, L"dans %s", d.parent);
        lstrcpyn(r.cible, d.chemin, ARRAYSIZE(r.cible));
    }

    WCHAR encodee[1200];
    EncoderUrl(g_saisie, encodee, ARRAYSIZE(encodee));

    // Le web : le moteur réglé, où « %s » est remplacé par la saisie.
    Resultat& web = g_resultats[g_nbResultats++];
    web.genre = RESULTAT_WEB;
    swprintf_s(web.libelle, L"Rechercher « %s » sur le web", g_saisie);
    lstrcpy(web.precision, L"Web");
    const WCHAR* trou = wcsstr(g_moteurRecherche, L"%s");
    if (trou) {
        swprintf_s(web.cible, L"%.*s%s%s", (int)(trou - g_moteurRecherche),
                   g_moteurRecherche, encodee, trou + 2);
    } else {
        swprintf_s(web.cible, L"%s%s", g_moteurRecherche, encodee);
    }

    // Les fichiers : la recherche de l'Explorateur (protocole search-ms).
    Resultat& fichiers = g_resultats[g_nbResultats++];
    fichiers.genre = RESULTAT_FICHIERS;
    swprintf_s(fichiers.libelle, L"Chercher « %s » dans les fichiers",
               g_saisie);
    lstrcpy(fichiers.precision, L"Fichiers");
    swprintf_s(fichiers.cible, L"search-ms:query=%s", encodee);
}

void LancerResultat(int numero) {
    if (numero < 0 || numero >= g_nbResultats) {
        return;
    }
    Resultat& r = g_resultats[numero];
    switch (r.genre) {
        case RESULTAT_APP:
            Lancer(L"explorer.exe", r.cible);
            break;
        case RESULTAT_DOSSIER:
            OuvrirDossier(r.cible);
            break;
        case RESULTAT_WEB:
            OuvrirDansChrome(r.cible);
            break;
        case RESULTAT_FICHIERS:
            Lancer(r.cible, nullptr);  // recherche de l'Explorateur
            break;
    }
    // On repart d'une barre vide.
    g_saisie[0] = L'\0';
    g_nbResultats = 0;
    AfficherWidget(&g_recherche);
}

// Colle la première ligne du presse-papiers à la fin de la saisie.
void CollerPressePapiers() {
    if (!OpenClipboard(g_recherche.fenetre)) {
        return;
    }
    HANDLE donnees = GetClipboardData(CF_UNICODETEXT);
    const WCHAR* texte = donnees ? (const WCHAR*)GlobalLock(donnees) : nullptr;
    if (texte) {
        size_t longueur = wcslen(g_saisie);
        for (; *texte && *texte != L'\r' && *texte != L'\n' &&
               longueur < ARRAYSIZE(g_saisie) - 1;
             texte++) {
            g_saisie[longueur++] = *texte < 32 ? L' ' : *texte;
        }
        g_saisie[longueur] = L'\0';
        GlobalUnlock(donnees);
    }
    CloseClipboard();
}

// Un caractère tapé dans la barre (WM_CHAR).
void SaisirCaractere(WCHAR caractere) {
    size_t longueur = wcslen(g_saisie);
    if (caractere == L'\r') {  // Entrée
        LancerResultat(g_resultatChoisi);
        return;
    }
    if (caractere == 27) {  // Échap : on efface tout
        g_saisie[0] = L'\0';
    } else if (caractere == L'\b') {  // Retour arrière
        if (longueur) {
            g_saisie[longueur - 1] = L'\0';
        }
    } else if (caractere == 0x16) {  // Ctrl + V
        CollerPressePapiers();
    } else if (caractere >= 32 && longueur < ARRAYSIZE(g_saisie) - 1) {
        g_saisie[longueur] = caractere;
        g_saisie[longueur + 1] = L'\0';
    } else {
        return;
    }
    g_curseurVisible = true;
    ConstruireResultats();
    AfficherWidget(&g_recherche);
}

// Flèches haut et bas : change la ligne que lancera Entrée.
void ChoisirLigne(int sens) {
    if (g_nbResultats == 0) {
        return;
    }
    g_resultatChoisi = (g_resultatChoisi + sens + g_nbResultats) % g_nbResultats;
    g_recherche.survol = AUCUNE_ZONE;  // le clavier prend le pas sur la souris
    AfficherWidget(&g_recherche);
}

D2D1_RECT_F RectangleLigne(int numero) {
    float y = HAUT_LISTE + 8 + numero * HAUTEUR_LIGNE;
    return D2D1::RectF(8, y, LARGEUR_BARRE - 8, y + HAUTEUR_LIGNE);
}

// Zone 0 : la barre. Zones 1 et suivantes : les lignes de résultats.
int ZoneRecherche(float x, float y) {
    if (DansRectangle(x, y, D2D1::RectF(0, 0, LARGEUR_BARRE, HAUTEUR_BARRE))) {
        return 0;
    }
    for (int i = 0; g_barreActive && i < g_nbResultats; i++) {
        if (DansRectangle(x, y, RectangleLigne(i))) {
            return 1 + i;
        }
    }
    return AUCUNE_ZONE;
}

void CliquerRecherche(int zone) {
    if (zone >= 1) {
        LancerResultat(zone - 1);
    }
    // Un clic sur la barre elle-même lui donne le clavier (WM_MOUSEACTIVATE).
}

void DessinerRecherche(ID2D1RenderTarget* cible, float echelle) {
    DessinerVerreZone(cible, 0, 0, LARGEUR_BARRE, HAUTEUR_BARRE, RAYON_BOUTON,
                      echelle);
    if (g_barreActive) {
        ID2D1SolidColorBrush* bleu = Pinceau(cible, AccentTransparent(140));
        if (bleu) {
            ContourArrondi(cible, 0, 0, LARGEUR_BARRE, HAUTEUR_BARRE,
                           RAYON_BOUTON, bleu, echelle);
            bleu->Release();
        }
    }

    // La loupe : un rond et un manche.
    ID2D1SolidColorBrush* gris = Pinceau(cible, TEXTE2);
    if (gris) {
        D2D1_ELLIPSE rond = {D2D1::Point2F(27 * echelle, 25 * echelle),
                             8.5f * echelle, 8.5f * echelle};
        cible->DrawEllipse(&rond, gris, 2.0f * echelle, nullptr);
        gris->Release();
    }
    D2D1_POINT_2F manche[2] = {D2D1::Point2F(33.5f * echelle, 31.5f * echelle),
                               D2D1::Point2F(39 * echelle, 37 * echelle)};
    DessinerLigne(cible, manche, 2, false, 2.0f * echelle, TEXTE2);

    // Le texte tapé, ou l'invitation, puis le curseur clignotant.
    const float X_TEXTE = 54, Y_TEXTE = 14, LARGEUR_TEXTE = 560;
    float largeur = 0;
    if (g_saisie[0]) {
        Ecrire(cible, g_saisie, POLICE, 15, DWRITE_FONT_WEIGHT_NORMAL, TEXTE,
               X_TEXTE, Y_TEXTE, A_GAUCHE, echelle, LARGEUR_TEXTE, &largeur);
    } else if (!g_barreActive) {
        Ecrire(cible, L"Rechercher une app, un dossier, un fichier ou sur le web",
               POLICE, 15, DWRITE_FONT_WEIGHT_NORMAL, TEXTE3, X_TEXTE, Y_TEXTE,
               A_GAUCHE, echelle);
    }
    if (g_barreActive && g_curseurVisible) {
        if (largeur > LARGEUR_TEXTE) {
            largeur = LARGEUR_TEXTE;
        }
        RemplirArrondi(cible, X_TEXTE + largeur + 1, 16, 2, 24, 1, g_accent,
                       echelle);
    }

    // Les résultats, seulement quand la barre a le clavier.
    if (!g_barreActive || g_nbResultats == 0) {
        return;
    }
    // La liste passe par-dessus d'autres cartes : sans vrai flou, un verre
    // transparent la rendrait illisible. Fond sombre presque opaque.
    float hauteurListe = 16 + g_nbResultats * HAUTEUR_LIGNE;
    RemplirArrondi(cible, 0, HAUT_LISTE, LARGEUR_BARRE, hauteurListe,
                   RAYON_CARTE, {20, 22, 30, 240}, echelle);
    ID2D1SolidColorBrush* bord = Pinceau(cible, BOUTON_CONTOUR);
    if (bord) {
        ContourArrondi(cible, 0, HAUT_LISTE, LARGEUR_BARRE, hauteurListe,
                       RAYON_CARTE, bord, echelle);
        bord->Release();
    }
    int surlignee =
        g_recherche.survol >= 1 ? g_recherche.survol - 1 : g_resultatChoisi;
    for (int i = 0; i < g_nbResultats; i++) {
        D2D1_RECT_F r = RectangleLigne(i);
        if (i == surlignee) {
            RemplirArrondi(cible, r.left, r.top, r.right - r.left,
                           r.bottom - r.top, 12, BOUTON_SURVOL, echelle);
        }
        Ecrire(cible, g_resultats[i].libelle, POLICE, 13.5f,
               DWRITE_FONT_WEIGHT_NORMAL, TEXTE, r.left + 16, r.top + 10,
               A_GAUCHE, echelle, 400);
        Ecrire(cible, g_resultats[i].precision, POLICE, 11,
               DWRITE_FONT_WEIGHT_NORMAL, TEXTE3, r.right - 16, r.top + 13,
               A_DROITE, echelle, 180);
    }
}

// ---------------------------------------------------------------------------
// Bureau 1 : bouton Sites (aperçus de sites qui se déroulent)
// ---------------------------------------------------------------------------

const int NB_FAVORIS = 8;

// Les aperçus sont gardés en 400 × 250 (deux fois la taille affichée, pour
// rester nets à 150 % et plus) ; la capture de Chrome fait 1280 × 800.
const int LARGEUR_CAPTURE = 400, HAUTEUR_CAPTURE = 250;

struct Favori {
    WCHAR nom[64];
    WCHAR adresse[1024];   // vide : site masqué
    WCHAR compte[128];     // compte Chrome, « choisir », ou vide
    BYTE* pixels;          // aperçu (bleu, vert, rouge, opacité prémultipliée)
    ID2D1Bitmap* bitmap;   // le même, prêt pour Direct2D
};
Favori g_favoris[NB_FAVORIS];

bool g_sitesOuverts;         // le panneau des aperçus est déroulé
float g_defilementSites;     // décalage horizontal de la bande, en pixels
LONG g_generationApercus;    // numéro de la dernière demande d'aperçus

const float LARGEUR_SITES = 480, HAUTEUR_BOUTON_SITES = 52;
const float HAUT_PANNEAU = 64, HAUTEUR_PANNEAU = 196;
const float MARGE_PANNEAU = 16;
const float LARGEUR_VIGNETTE = 200, HAUTEUR_VIGNETTE = 125, ECART_VIGNETTES = 14;

// Nom de fichier d'un aperçu, tiré de l'adresse du site (empreinte FNV-1a) :
// une même adresse donne toujours le même fichier, une autre adresse un autre.
void FichierApercu(const WCHAR* dossier, const WCHAR* adresse, WCHAR* fichier) {
    unsigned long long empreinte = 14695981039346656037ull;
    for (const WCHAR* c = adresse; *c; c++) {
        empreinte = (empreinte ^ *c) * 1099511628211ull;
    }
    WCHAR nom[32];
    swprintf_s(nom, L"%016llx.png", empreinte);
    if (!Joindre(fichier, MAX_PATH, dossier, nom)) {
        fichier[0] = L'\0';
    }
}

// Fait photographier un site par Chrome invisible. Chrome tourne dans un
// « job » Windows : s'il ne finit pas en 45 s, ou si le mod s'arrête, toutes
// ses fenêtres et sous-processus sont fermés d'un coup.
bool Photographier(const WCHAR* chrome, const WCHAR* profil,
                   const WCHAR* adresse, const WCHAR* fichier) {
    WCHAR ligne[4096];
    if (_snwprintf_s(
            ligne, ARRAYSIZE(ligne), _TRUNCATE,
            L"\"%s\" --headless=new --disable-gpu --hide-scrollbars "
            L"--mute-audio --no-first-run --no-default-browser-check "
            L"--lang=fr-FR --user-data-dir=\"%s\" --window-size=1280,800 "
            L"--virtual-time-budget=5000 --screenshot=\"%s\" \"%s\"",
            chrome, profil, fichier, adresse) < 0) {
        return false;
    }

    HANDLE travail = CreateJobObject(nullptr, nullptr);
    if (travail) {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION limites = {};
        limites.BasicLimitInformation.LimitFlags =
            JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        SetInformationJobObject(travail, JobObjectExtendedLimitInformation,
                                &limites, sizeof(limites));
    }
    STARTUPINFO demarrage = {};
    demarrage.cb = sizeof(demarrage);
    PROCESS_INFORMATION processus;
    if (!CreateProcess(nullptr, ligne, nullptr, nullptr, FALSE,
                       CREATE_NO_WINDOW | CREATE_SUSPENDED, nullptr, nullptr,
                       &demarrage, &processus)) {
        if (travail) {
            CloseHandle(travail);
        }
        return false;
    }
    if (travail) {
        AssignProcessToJobObject(travail, processus.hProcess);
    }
    ResumeThread(processus.hThread);
    bool fini = WaitForSingleObject(processus.hProcess, 45000) == WAIT_OBJECT_0;
    CloseHandle(processus.hThread);
    CloseHandle(processus.hProcess);
    if (travail) {
        CloseHandle(travail);  // ferme Chrome s'il tourne encore
    }
    return fini && GetFileAttributes(fichier) != INVALID_FILE_ATTRIBUTES;
}

struct DemandeApercus {
    LONG generation;
    WCHAR dossier[MAX_PATH];  // où ranger les aperçus
    WCHAR adresses[NB_FAVORIS][1024];
};

// Aperçus décodés, envoyés au thread des fenêtres.
struct LotApercus {
    LONG generation;
    BYTE* pixels[NB_FAVORIS];
};

// Décode les aperçus déjà photographiés et les envoie au thread des fenêtres.
void EnvoyerApercus(const DemandeApercus* demande) {
    LotApercus* lot = (LotApercus*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                             sizeof(LotApercus));
    if (!lot) {
        return;
    }
    lot->generation = demande->generation;
    for (int i = 0; i < NB_FAVORIS; i++) {
        WCHAR fichier[MAX_PATH];
        if (demande->adresses[i][0]) {
            FichierApercu(demande->dossier, demande->adresses[i], fichier);
            if (fichier[0] &&
                GetFileAttributes(fichier) != INVALID_FILE_ATTRIBUTES) {
                DecoderImage(fichier, LARGEUR_CAPTURE, HAUTEUR_CAPTURE,
                             &lot->pixels[i]);
            }
        }
    }
    if (!(g_controleur &&
          PostMessage(g_controleur, WM_APP_APERCUS, 0, (LPARAM)lot))) {
        for (BYTE* pixels : lot->pixels) {
            if (pixels) {
                HeapFree(GetProcessHeap(), 0, pixels);
            }
        }
        HeapFree(GetProcessHeap(), 0, lot);
    }
}

DWORD WINAPI FilApercus(LPVOID parametre) {
    DemandeApercus* demande = (DemandeApercus*)parametre;
    HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    // 1. Les aperçus déjà faits s'affichent tout de suite.
    EnvoyerApercus(demande);

    // 2. Les sites sans aperçu sont photographiés un par un, une seule fois.
    WCHAR chrome[MAX_PATH], profil[MAX_PATH];
    bool nouveau = false;
    if (CheminChrome(chrome, MAX_PATH) &&
        Joindre(profil, MAX_PATH, demande->dossier, L"profil-chrome")) {
        for (int i = 0; i < NB_FAVORIS; i++) {
            WCHAR fichier[MAX_PATH];
            if (!demande->adresses[i][0] ||
                demande->generation != g_generationApercus) {
                continue;  // site vide, ou réglages changés entre-temps
            }
            FichierApercu(demande->dossier, demande->adresses[i], fichier);
            if (fichier[0] &&
                GetFileAttributes(fichier) == INVALID_FILE_ATTRIBUTES &&
                Photographier(chrome, profil, demande->adresses[i], fichier)) {
                nouveau = true;
            }
        }
    }

    // 3. On renvoie la série complète si des aperçus ont été ajoutés.
    if (nouveau) {
        EnvoyerApercus(demande);
    }
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    HeapFree(GetProcessHeap(), 0, demande);
    return 0;
}

// Demande les aperçus des sites réglés (thread à part : Chrome met quelques
// secondes par site).
void DemanderApercus() {
    DemandeApercus* demande = (DemandeApercus*)HeapAlloc(
        GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(DemandeApercus));
    if (!demande) {
        return;
    }
    WCHAR stockage[MAX_PATH];
    if (!Wh_GetModStoragePath(stockage, MAX_PATH) ||
        !Joindre(demande->dossier, MAX_PATH, stockage, L"apercus")) {
        HeapFree(GetProcessHeap(), 0, demande);
        return;
    }
    SHCreateDirectoryEx(nullptr, demande->dossier, nullptr);
    demande->generation = ++g_generationApercus;
    for (int i = 0; i < NB_FAVORIS; i++) {
        lstrcpyn(demande->adresses[i], g_favoris[i].adresse, 1024);
    }
    HANDLE fil = CreateThread(nullptr, 0, FilApercus, demande, 0, nullptr);
    if (fil) {
        CloseHandle(fil);
    } else {
        HeapFree(GetProcessHeap(), 0, demande);
    }
}

void OublierBitmapsSites() {
    for (Favori& favori : g_favoris) {
        Liberer(favori.bitmap);
    }
}

void LibererApercus() {
    for (Favori& favori : g_favoris) {
        Liberer(favori.bitmap);
        if (favori.pixels) {
            HeapFree(GetProcessHeap(), 0, favori.pixels);
            favori.pixels = nullptr;
        }
    }
}

// Numéro du favori affiché à la place donnée (les sites sans adresse ne
// prennent pas de place), ou -1.
int FavoriALaPlace(int place) {
    for (int i = 0; i < NB_FAVORIS; i++) {
        if (g_favoris[i].adresse[0] && place-- == 0) {
            return i;
        }
    }
    return -1;
}

int NombreDeSites() {
    int nombre = 0;
    while (FavoriALaPlace(nombre) >= 0) {
        nombre++;
    }
    return nombre;
}

// La bande peut défiler de 0 à « fin de la dernière vignette - largeur
// visible ».
float DefilementMax() {
    int nombre = NombreDeSites();
    float bande = nombre * LARGEUR_VIGNETTE + (nombre - 1) * ECART_VIGNETTES;
    float visible = LARGEUR_SITES - 2 * MARGE_PANNEAU;
    return bande > visible ? bande - visible : 0;
}

void Defiler(float decalage) {
    g_defilementSites += decalage;
    float maximum = DefilementMax();
    if (g_defilementSites > maximum) g_defilementSites = maximum;
    if (g_defilementSites < 0) g_defilementSites = 0;
    AfficherWidget(&g_sites);
}

D2D1_RECT_F RectangleVignette(int place) {
    float x = MARGE_PANNEAU + place * (LARGEUR_VIGNETTE + ECART_VIGNETTES) -
              g_defilementSites;
    float y = HAUT_PANNEAU + MARGE_PANNEAU;
    return D2D1::RectF(x, y, x + LARGEUR_VIGNETTE, y + HAUTEUR_VIGNETTE);
}

D2D1_RECT_F ZoneVisibleBande() {
    return D2D1::RectF(MARGE_PANNEAU, HAUT_PANNEAU,
                       LARGEUR_SITES - MARGE_PANNEAU,
                       HAUT_PANNEAU + HAUTEUR_PANNEAU);
}

// --- Profils de Chrome -------------------------------------------------------

// Chrome range la liste de ses profils dans le fichier « Local State »
// (format JSON) : profile → info_cache → <dossier du profil> → name et
// user_name (le compte). Ce fichier n'est pas documenté par Google : si son
// format change, aucun profil n'est trouvé et les sites s'ouvrent dans le
// profil habituel, sans planter.
struct ProfilChrome {
    WCHAR dossier[64];   // « Profile 1 », à donner à --profile-directory
    WCHAR nom[64];       // « David Ihou »
    WCHAR compte[128];   // « nom@gmail.com »
};

const int NB_PROFILS_MAX = 5;  // ce qui tient dans le panneau
ProfilChrome g_profils[NB_PROFILS_MAX];
int g_nbProfils;
int g_choixSite = -1;  // site dont on choisit le profil, -1 : aucun

const WCHAR* SauterBlancs(const WCHAR* p) {
    while (*p == L' ' || *p == L'\t' || *p == L'\r' || *p == L'\n') {
        p++;
    }
    return p;
}

// Lit un texte JSON entre guillemets (p est sur le guillemet ouvrant) et
// renvoie la suite, ou nullptr si le texte est mal formé.
const WCHAR* LireChaineJson(const WCHAR* p, WCHAR* sortie, int taille) {
    if (*p != L'"') {
        return nullptr;
    }
    p++;
    int n = 0;
    while (*p && *p != L'"') {
        WCHAR c = *p++;
        if (c == L'\\' && *p) {
            c = *p++;
            if (c == L'u' && wcslen(p) >= 4) {  // é → é
                WCHAR hexa[5] = {p[0], p[1], p[2], p[3], L'\0'};
                c = (WCHAR)wcstoul(hexa, nullptr, 16);
                p += 4;
            } else if (c == L'n') {
                c = L'\n';
            } else if (c == L't') {
                c = L'\t';
            }  // \" \\ \/ : le caractère lui-même
        }
        if (sortie && n < taille - 1) {
            sortie[n++] = c;
        }
    }
    if (sortie) {
        sortie[n] = L'\0';
    }
    return *p == L'"' ? p + 1 : nullptr;
}

// Saute une valeur JSON entière (texte, nombre, objet, tableau...).
const WCHAR* SauterValeurJson(const WCHAR* p) {
    p = SauterBlancs(p);
    if (*p == L'"') {
        return LireChaineJson(p, nullptr, 0);
    }
    if (*p == L'{' || *p == L'[') {
        int profondeur = 0;
        while (*p) {
            if (*p == L'"') {
                p = LireChaineJson(p, nullptr, 0);
                if (!p) {
                    return nullptr;
                }
                continue;
            }
            if (*p == L'{' || *p == L'[') {
                profondeur++;
            } else if ((*p == L'}' || *p == L']') && --profondeur == 0) {
                return p + 1;
            }
            p++;
        }
        return nullptr;
    }
    while (*p && *p != L',' && *p != L'}' && *p != L']') {
        p++;  // nombre, true, false, null
    }
    return p;
}

// Parcourt les membres d'un objet JSON (p sur « { ») : pour chacun, appelle
// « membre(nom, valeur) ». S'arrête si « membre » renvoie faux.
template <typename Membre>
void ParcourirObjetJson(const WCHAR* p, Membre membre) {
    p = p ? SauterBlancs(p) : nullptr;
    if (!p || *p != L'{') {
        return;
    }
    p++;
    while (true) {
        p = SauterBlancs(p);
        WCHAR nom[128];
        if (*p != L'"' || !(p = LireChaineJson(p, nom, ARRAYSIZE(nom)))) {
            return;
        }
        p = SauterBlancs(p);
        if (*p != L':') {
            return;
        }
        const WCHAR* valeur = SauterBlancs(p + 1);
        if (!membre(nom, valeur) || !(p = SauterValeurJson(valeur))) {
            return;
        }
        p = SauterBlancs(p);
        if (*p != L',') {
            return;
        }
        p++;
    }
}

// Valeur de « cle » au premier niveau de l'objet JSON qui commence en p.
const WCHAR* ValeurJson(const WCHAR* p, const WCHAR* cle) {
    const WCHAR* trouvee = nullptr;
    ParcourirObjetJson(p, [&](const WCHAR* nom, const WCHAR* valeur) {
        if (lstrcmp(nom, cle) == 0) {
            trouvee = valeur;
            return false;
        }
        return true;
    });
    return trouvee;
}

void LireProfilsChrome() {
    g_nbProfils = 0;
    WCHAR chemin[MAX_PATH];
    if (!ExpandEnvironmentStrings(
            L"%LOCALAPPDATA%\\Google\\Chrome\\User Data\\Local State", chemin,
            MAX_PATH)) {
        return;
    }
    WCHAR* texte = LireFichierTexte(chemin);
    if (!texte) {
        return;
    }
    const WCHAR* profils = ValeurJson(ValeurJson(texte, L"profile"),
                                      L"info_cache");
    ParcourirObjetJson(profils, [](const WCHAR* dossier, const WCHAR* valeur) {
        ProfilChrome& profil = g_profils[g_nbProfils];
        lstrcpyn(profil.dossier, dossier, ARRAYSIZE(profil.dossier));
        profil.nom[0] = profil.compte[0] = L'\0';
        const WCHAR* champ = ValeurJson(valeur, L"name");
        if (champ) {
            LireChaineJson(champ, profil.nom, ARRAYSIZE(profil.nom));
        }
        champ = ValeurJson(valeur, L"user_name");
        if (champ) {
            LireChaineJson(champ, profil.compte, ARRAYSIZE(profil.compte));
        }
        g_nbProfils++;
        return g_nbProfils < NB_PROFILS_MAX;
    });
    HeapFree(GetProcessHeap(), 0, texte);
}

// Dossier du profil Chrome connecté à ce compte, ou nullptr.
const WCHAR* DossierDuCompte(const WCHAR* compte) {
    LireProfilsChrome();
    for (int i = 0; i < g_nbProfils; i++) {
        if (lstrcmpi(g_profils[i].compte, compte) == 0) {
            return g_profils[i].dossier;
        }
    }
    return nullptr;
}

const int ZONE_PREMIER_PROFIL = 100;

D2D1_RECT_F RectangleProfil(int numero) {
    float y = HAUT_PANNEAU + 44 + numero * 29;
    return D2D1::RectF(MARGE_PANNEAU, y, LARGEUR_SITES - MARGE_PANNEAU, y + 27);
}

// Le panneau, quand on choisit le profil d'un site réglé sur « choisir ».
void DessinerChoixProfil(ID2D1RenderTarget* cible, float echelle) {
    WCHAR titre[128];
    swprintf_s(titre, L"Ouvrir « %s » avec :", g_favoris[g_choixSite].nom);
    Ecrire(cible, titre, POLICE, 12, DWRITE_FONT_WEIGHT_SEMI_BOLD, TEXTE,
           MARGE_PANNEAU + 4, HAUT_PANNEAU + 14, A_GAUCHE, echelle,
           LARGEUR_SITES - 2 * MARGE_PANNEAU);
    for (int i = 0; i < g_nbProfils; i++) {
        D2D1_RECT_F r = RectangleProfil(i);
        if (g_sites.survol == ZONE_PREMIER_PROFIL + i) {
            RemplirArrondi(cible, r.left, r.top, r.right - r.left,
                           r.bottom - r.top, 9, BOUTON_SURVOL, echelle);
        }
        Ecrire(cible, g_profils[i].nom, POLICE, 12.5f,
               DWRITE_FONT_WEIGHT_NORMAL, TEXTE, r.left + 10, r.top + 3,
               A_GAUCHE, echelle, 170);
        Ecrire(cible, g_profils[i].compte, POLICE, 10.5f,
               DWRITE_FONT_WEIGHT_NORMAL, TEXTE3, r.right - 10, r.top + 6,
               A_DROITE, echelle, 250);
    }
}

// Ouvre un site, avec le compte réglé : une adresse → son profil Chrome ;
// « choisir » → le panneau propose les profils ; vide → profil habituel.
void OuvrirFavori(int numero) {
    const WCHAR* compte = g_favoris[numero].compte;
    if (lstrcmpi(compte, L"choisir") == 0) {
        LireProfilsChrome();
        if (g_nbProfils > 0) {
            g_choixSite = numero;  // le panneau reste ouvert pour choisir
            return;
        }
        compte = L"";
    }
    OuvrirDansChrome(g_favoris[numero].adresse,
                     compte[0] ? DossierDuCompte(compte) : nullptr);
    g_sitesOuverts = false;
}

// Zone 0 : le bouton. Zones 1 et suivantes : les vignettes visibles ;
// pendant le choix d'un profil, ZONE_PREMIER_PROFIL et suivantes.
int ZoneSites(float x, float y) {
    if (DansRectangle(x, y, D2D1::RectF(0, 0, LARGEUR_SITES,
                                        HAUTEUR_BOUTON_SITES))) {
        return 0;
    }
    if (!g_sitesOuverts || !DansRectangle(x, y, ZoneVisibleBande())) {
        return AUCUNE_ZONE;
    }
    if (g_choixSite >= 0) {
        for (int i = 0; i < g_nbProfils; i++) {
            if (DansRectangle(x, y, RectangleProfil(i))) {
                return ZONE_PREMIER_PROFIL + i;
            }
        }
        return AUCUNE_ZONE;
    }
    for (int place = 0; FavoriALaPlace(place) >= 0; place++) {
        if (DansRectangle(x, y, RectangleVignette(place))) {
            return 1 + place;
        }
    }
    return AUCUNE_ZONE;
}

void CliquerSites(int zone) {
    if (zone == 0) {
        g_sitesOuverts = !g_sitesOuverts;
        g_defilementSites = 0;
        g_choixSite = -1;
    } else if (zone >= ZONE_PREMIER_PROFIL) {
        int profil = zone - ZONE_PREMIER_PROFIL;
        if (g_choixSite >= 0 && profil < g_nbProfils) {
            OuvrirDansChrome(g_favoris[g_choixSite].adresse,
                             g_profils[profil].dossier);
        }
        g_choixSite = -1;
        g_sitesOuverts = false;
    } else {
        int numero = FavoriALaPlace(zone - 1);
        if (numero >= 0) {
            OuvrirFavori(numero);
        } else {
            g_sitesOuverts = false;
        }
    }
    AfficherWidget(&g_sites);
}

// Une vignette : l'aperçu en coins arrondis (ou, sans aperçu, l'initiale du
// site sur fond d'accent), son contour, et le nom dessous.
void DessinerVignette(ID2D1RenderTarget* cible, Favori& favori,
                      D2D1_RECT_F r, bool survolee, float echelle) {
    D2D1_ROUNDED_RECT forme = {
        D2D1::RectF(r.left * echelle, r.top * echelle, r.right * echelle,
                    r.bottom * echelle),
        10 * echelle, 10 * echelle};

    if (favori.pixels && !favori.bitmap) {
        D2D1_BITMAP_PROPERTIES proprietes = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED));
        if (FAILED(cible->CreateBitmap(
                D2D1::SizeU(LARGEUR_CAPTURE, HAUTEUR_CAPTURE), favori.pixels,
                LARGEUR_CAPTURE * 4, &proprietes, &favori.bitmap))) {
            favori.bitmap = nullptr;
        }
    }
    ID2D1BitmapBrush* image = nullptr;
    if (favori.bitmap &&
        SUCCEEDED(cible->CreateBitmapBrush(favori.bitmap, nullptr, nullptr,
                                           &image))) {
        // Le pinceau-image est mis à l'échelle de la vignette et posé dessus.
        image->SetTransform(
            D2D1::Matrix3x2F::Scale(
                LARGEUR_VIGNETTE * echelle / LARGEUR_CAPTURE,
                HAUTEUR_VIGNETTE * echelle / HAUTEUR_CAPTURE) *
            D2D1::Matrix3x2F::Translation(r.left * echelle, r.top * echelle));
        cible->FillRoundedRectangle(&forme, image);
        image->Release();
    } else {
        RemplirArrondi(cible, r.left, r.top, LARGEUR_VIGNETTE,
                       HAUTEUR_VIGNETTE, 10, AccentTransparent(40), echelle);
        WCHAR initiale[2] = {favori.nom[0], L'\0'};
        CharUpperBuff(initiale, 1);
        Ecrire(cible, initiale, POLICE_TITRE, 30, DWRITE_FONT_WEIGHT_NORMAL,
               g_accent, (r.left + r.right) / 2, r.top + 34, A_CENTRE,
               echelle);
    }

    ID2D1SolidColorBrush* contour =
        Pinceau(cible, survolee ? g_accent : BOUTON_CONTOUR);
    if (contour) {
        ContourArrondi(cible, r.left, r.top, LARGEUR_VIGNETTE,
                       HAUTEUR_VIGNETTE, 10, contour, echelle);
        contour->Release();
    }
    Ecrire(cible, favori.nom, POLICE, 11.5f, DWRITE_FONT_WEIGHT_NORMAL, TEXTE,
           (r.left + r.right) / 2, r.bottom + 8, A_CENTRE, echelle,
           LARGEUR_VIGNETTE);
}

void DessinerSites(ID2D1RenderTarget* cible, float echelle) {
    // Le bouton : globe, « Mes sites », nombre de sites et flèche.
    DessinerVerreZone(cible, 0, 0, LARGEUR_SITES, HAUTEUR_BOUTON_SITES,
                      RAYON_BOUTON, echelle);
    if (g_sites.survol == 0 || g_sitesOuverts) {
        RemplirArrondi(cible, 0, 0, LARGEUR_SITES, HAUTEUR_BOUTON_SITES,
                       RAYON_BOUTON, BOUTON, echelle);
    }
    Ecrire(cible, L"\xE774", POLICE_ICONES, 13.5f, DWRITE_FONT_WEIGHT_NORMAL,
           g_accent, 30, 16, A_GAUCHE, echelle);  // globe
    Ecrire(cible, L"Mes sites", POLICE, 15, DWRITE_FONT_WEIGHT_NORMAL, TEXTE,
           62, 12, A_GAUCHE, echelle);
    WCHAR nombre[24];
    int sites = NombreDeSites();
    swprintf_s(nombre, L"%d site%s", sites, sites > 1 ? L"s" : L"");
    Ecrire(cible, nombre, POLICE, 12, DWRITE_FONT_WEIGHT_NORMAL, TEXTE3, 420,
           15, A_DROITE, echelle);
    Ecrire(cible, g_sitesOuverts ? L"\xE70E" : L"\xE70D", POLICE_ICONES, 9,
           DWRITE_FONT_WEIGHT_NORMAL, TEXTE2, 450, 20, A_DROITE,
           echelle);  // flèche haut / bas

    if (!g_sitesOuverts) {
        return;
    }

    // Le panneau déroulé : une bande de vignettes qu'on fait défiler.
    DessinerVerreZone(cible, 0, HAUT_PANNEAU, LARGEUR_SITES, HAUTEUR_PANNEAU,
                      RAYON_CARTE, echelle);
    if (g_choixSite >= 0) {
        DessinerChoixProfil(cible, echelle);
        return;
    }
    D2D1_RECT_F visible = ZoneVisibleBande();
    cible->PushAxisAlignedClip(
        D2D1::RectF(visible.left * echelle, visible.top * echelle,
                    visible.right * echelle, visible.bottom * echelle),
        D2D1_ANTIALIAS_MODE_ALIASED);
    for (int place = 0; FavoriALaPlace(place) >= 0; place++) {
        D2D1_RECT_F r = RectangleVignette(place);
        if (r.right < visible.left || r.left > visible.right) {
            continue;  // hors de la partie visible
        }
        DessinerVignette(cible, g_favoris[FavoriALaPlace(place)], r,
                         g_sites.survol == 1 + place, echelle);
    }
    cible->PopAxisAlignedClip();

    // Barre de défilement fine, seulement s'il y a de quoi défiler.
    float maximum = DefilementMax();
    if (maximum > 0) {
        float piste = visible.right - visible.left;
        float largeur = piste * piste / (piste + maximum);
        float x = visible.left + (piste - largeur) * g_defilementSites / maximum;
        float y = HAUT_PANNEAU + HAUTEUR_PANNEAU - 10;
        RemplirArrondi(cible, visible.left, y, piste, 3, 1.5f, PISTE, echelle);
        RemplirArrondi(cible, x, y, largeur, 3, 1.5f, AccentTransparent(180),
                       echelle);
    }
}

// ---------------------------------------------------------------------------
// Bureau 1 : poste de pilotage (minuteur de concentration, temps d'écran)
// ---------------------------------------------------------------------------

// --- Minuteur de concentration ---------------------------------------------

enum PhaseMinuteur { PHASE_TRAVAIL, PHASE_PAUSE };

PhaseMinuteur g_phase = PHASE_TRAVAIL;
bool g_minuteurEnMarche;
ULONGLONG g_finMinuteur;     // GetTickCount64 de la fin, quand il tourne
LONGLONG g_resteMs;          // temps restant, quand il est arrêté
int g_minutesTravail = 25, g_minutesPause = 5;
int g_sessionsDuJour;        // sessions de travail terminées aujourd'hui

const int SESSIONS_PAR_CYCLE = 4;  // les pastilles sous le temps

// Aujourd'hui sous la forme 20261030 (pour repartir de zéro chaque jour).
int DateDuJour() {
    SYSTEMTIME maintenant;
    GetLocalTime(&maintenant);
    return maintenant.wYear * 10000 + maintenant.wMonth * 100 + maintenant.wDay;
}

// Le compteur de sessions est gardé par Windhawk : il survit au redémarrage
// du mod, et repart de zéro chaque jour.
void LireSessions() {
    g_sessionsDuJour = Wh_GetIntValue(L"sessionsDate", 0) == DateDuJour()
                           ? Wh_GetIntValue(L"sessionsNombre", 0)
                           : 0;
}

void AjouterSession() {
    LireSessions();
    g_sessionsDuJour++;
    Wh_SetIntValue(L"sessionsDate", DateDuJour());
    Wh_SetIntValue(L"sessionsNombre", g_sessionsDuJour);
}

LONGLONG DureePhaseMs() {
    return (g_phase == PHASE_TRAVAIL ? g_minutesTravail : g_minutesPause) *
           60000LL;
}

LONGLONG ResteMs() {
    if (!g_minuteurEnMarche) {
        return g_resteMs;
    }
    ULONGLONG maintenant = GetTickCount64();
    return g_finMinuteur > maintenant ? (LONGLONG)(g_finMinuteur - maintenant)
                                      : 0;
}

void DemarrerOuArreterMinuteur() {
    if (g_minuteurEnMarche) {
        g_resteMs = ResteMs();
        g_minuteurEnMarche = false;
    } else {
        g_finMinuteur = GetTickCount64() + g_resteMs;
        g_minuteurEnMarche = true;
    }
}

void RemettreMinuteur() {
    g_minuteurEnMarche = false;
    g_phase = PHASE_TRAVAIL;
    g_resteMs = DureePhaseMs();
}

// Appelée chaque seconde : fin d'une phase → son, puis phase suivante. Après
// le travail, la pause démarre seule ; après la pause, le minuteur attend.
void AvancerMinuteur() {
    if (!g_minuteurEnMarche || ResteMs() > 0) {
        return;
    }
    MessageBeep(MB_ICONASTERISK);
    if (g_phase == PHASE_TRAVAIL) {
        AjouterSession();
        g_phase = PHASE_PAUSE;
        g_finMinuteur = GetTickCount64() + DureePhaseMs();
    } else {
        g_phase = PHASE_TRAVAIL;
        g_minuteurEnMarche = false;
        g_resteMs = DureePhaseMs();
    }
}

// --- Temps d'écran par app --------------------------------------------------

// Chaque seconde, l'app dont la fenêtre est au premier plan gagne une
// seconde. Rien n'est compté après 5 minutes sans clavier ni souris.
const int NB_APPS_SUIVIES = 64;
const DWORD INACTIVITE_MS = 5 * 60 * 1000;

struct TempsApp {
    WCHAR nom[64];
    int secondes;
};

TempsApp g_tempsEcran[NB_APPS_SUIVIES];
int g_nbAppsSuivies;
int g_dateTempsEcran;      // jour des chiffres (20261030)
int g_secondesAvantSauvegarde;

// Nom lisible d'un programme, tel que Windows l'affiche : la « description »
// rangée dans le fichier .exe (« Google Chrome »), sinon le nom du fichier
// sans « .exe ». Les réponses sont gardées pour ne lire chaque .exe qu'une
// fois.
struct NomConnu {
    WCHAR chemin[MAX_PATH];
    WCHAR nom[64];
};
NomConnu g_nomsConnus[NB_APPS_SUIVIES];
int g_nbNomsConnus;

void NomDuProgramme(const WCHAR* chemin, WCHAR* nom, int taille) {
    for (int i = 0; i < g_nbNomsConnus; i++) {
        if (lstrcmpi(g_nomsConnus[i].chemin, chemin) == 0) {
            lstrcpyn(nom, g_nomsConnus[i].nom, taille);
            return;
        }
    }

    // Nom du fichier sans « .exe », au cas où il n'y a pas de description.
    const WCHAR* fichier = wcsrchr(chemin, L'\\');
    lstrcpyn(nom, fichier ? fichier + 1 : chemin, taille);
    WCHAR* point = wcsrchr(nom, L'.');
    if (point) {
        *point = L'\0';
    }

    DWORD ignore = 0;
    DWORD tailleInfos = GetFileVersionInfoSize(chemin, &ignore);
    BYTE* infos = tailleInfos ? (BYTE*)HeapAlloc(GetProcessHeap(), 0, tailleInfos)
                              : nullptr;
    if (infos && GetFileVersionInfo(chemin, 0, tailleInfos, infos)) {
        // La description est rangée sous une langue : on prend la première.
        struct Langue {
            WORD langue, pageDeCode;
        }* langues = nullptr;
        UINT tailleLangues = 0;
        if (VerQueryValue(infos, L"\\VarFileInfo\\Translation",
                          (void**)&langues, &tailleLangues) &&
            tailleLangues >= sizeof(Langue)) {
            WCHAR cle[64];
            swprintf_s(cle, L"\\StringFileInfo\\%04x%04x\\FileDescription",
                       langues[0].langue, langues[0].pageDeCode);
            WCHAR* description = nullptr;
            UINT longueur = 0;
            if (VerQueryValue(infos, cle, (void**)&description, &longueur) &&
                longueur > 1 && description[0]) {
                lstrcpyn(nom, description, taille);
            }
        }
    }
    if (infos) {
        HeapFree(GetProcessHeap(), 0, infos);
    }

    if (g_nbNomsConnus < NB_APPS_SUIVIES) {
        lstrcpyn(g_nomsConnus[g_nbNomsConnus].chemin, chemin, MAX_PATH);
        lstrcpyn(g_nomsConnus[g_nbNomsConnus].nom, nom, 64);
        g_nbNomsConnus++;
    }
}

// Les apps du Store sont affichées dans une fenêtre de Windows
// (ApplicationFrameHost.exe) : la vraie app est l'une de ses fenêtres
// enfants, qui appartient à un autre processus.
struct RechercheEnfant {
    DWORD processusCadre;
    DWORD trouve;
};

BOOL CALLBACK EnfantDAutreProcessus(HWND enfant, LPARAM parametre) {
    RechercheEnfant* recherche = (RechercheEnfant*)parametre;
    DWORD processus = 0;
    GetWindowThreadProcessId(enfant, &processus);
    if (processus && processus != recherche->processusCadre) {
        recherche->trouve = processus;
        return FALSE;  // trouvé, on arrête
    }
    return TRUE;
}

bool CheminDuProcessus(DWORD processus, WCHAR* chemin) {
    HANDLE ouvert =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processus);
    if (!ouvert) {
        return false;
    }
    DWORD taille = MAX_PATH;
    bool trouve = QueryFullProcessImageName(ouvert, 0, chemin, &taille);
    CloseHandle(ouvert);
    return trouve;
}

// Nom de l'app au premier plan ; faux s'il n'y a rien à compter (écran
// verrouillé, bureau, widgets du mod).
bool AppAuPremierPlan(WCHAR* nom, int taille) {
    HWND fenetre = GetForegroundWindow();
    DWORD processus = 0;
    if (!fenetre || !GetWindowThreadProcessId(fenetre, &processus) ||
        processus == GetCurrentProcessId()) {
        return false;
    }
    WCHAR classe[32];
    if (GetClassName(fenetre, classe, ARRAYSIZE(classe)) &&
        (lstrcmp(classe, L"Progman") == 0 || lstrcmp(classe, L"WorkerW") == 0 ||
         lstrcmp(classe, L"Shell_TrayWnd") == 0)) {
        return false;  // le bureau ou la barre des tâches
    }
    WCHAR chemin[MAX_PATH];
    if (!CheminDuProcessus(processus, chemin)) {
        return false;
    }
    const WCHAR* fichier = wcsrchr(chemin, L'\\');
    if (fichier && lstrcmpi(fichier + 1, L"ApplicationFrameHost.exe") == 0) {
        RechercheEnfant recherche = {processus, 0};
        EnumChildWindows(fenetre, EnfantDAutreProcessus, (LPARAM)&recherche);
        if (!recherche.trouve || !CheminDuProcessus(recherche.trouve, chemin)) {
            return false;
        }
    }
    NomDuProgramme(chemin, nom, taille);
    return true;
}

// Les chiffres du jour sont gardés par Windhawk sous la forme
// « nom=secondes|nom=secondes », avec la date à côté.
void SauverTempsEcran() {
    static WCHAR texte[NB_APPS_SUIVIES * 80];
    size_t longueur = 0;
    texte[0] = L'\0';
    for (int i = 0; i < g_nbAppsSuivies; i++) {
        WCHAR morceau[96];
        int n = swprintf_s(morceau, L"%s=%d|", g_tempsEcran[i].nom,
                           g_tempsEcran[i].secondes);
        if (n > 0 && longueur + n < ARRAYSIZE(texte)) {
            lstrcpy(texte + longueur, morceau);
            longueur += n;
        }
    }
    Wh_SetIntValue(L"tempsEcranDate", g_dateTempsEcran);
    Wh_SetStringValue(L"tempsEcran", texte);
}

void LireTempsEcran() {
    g_nbAppsSuivies = 0;
    g_dateTempsEcran = DateDuJour();
    if (Wh_GetIntValue(L"tempsEcranDate", 0) != g_dateTempsEcran) {
        return;  // chiffres d'un autre jour : on repart de zéro
    }
    static WCHAR texte[NB_APPS_SUIVIES * 80];
    if (!Wh_GetStringValue(L"tempsEcran", texte, ARRAYSIZE(texte))) {
        return;
    }
    for (WCHAR* morceau = texte; *morceau && g_nbAppsSuivies < NB_APPS_SUIVIES;) {
        WCHAR* fin = wcschr(morceau, L'|');
        if (fin) {
            *fin = L'\0';
        }
        WCHAR* egal = wcsrchr(morceau, L'=');
        if (egal) {
            *egal = L'\0';
            TempsApp& app = g_tempsEcran[g_nbAppsSuivies++];
            lstrcpyn(app.nom, morceau, ARRAYSIZE(app.nom));
            app.secondes = _wtoi(egal + 1);
        }
        if (!fin) {
            break;
        }
        morceau = fin + 1;
    }
}

// Appelée chaque seconde.
void MesurerTempsEcran() {
    if (g_dateTempsEcran != DateDuJour()) {
        g_nbAppsSuivies = 0;  // minuit : nouvelle journée
        g_dateTempsEcran = DateDuJour();
    }

    LASTINPUTINFO derniereAction = {sizeof(derniereAction)};
    WCHAR nom[64];
    if (GetLastInputInfo(&derniereAction) &&
        GetTickCount() - derniereAction.dwTime < INACTIVITE_MS &&
        AppAuPremierPlan(nom, ARRAYSIZE(nom))) {
        int i = 0;
        while (i < g_nbAppsSuivies && lstrcmp(g_tempsEcran[i].nom, nom) != 0) {
            i++;
        }
        if (i == g_nbAppsSuivies && g_nbAppsSuivies < NB_APPS_SUIVIES) {
            lstrcpyn(g_tempsEcran[i].nom, nom, ARRAYSIZE(g_tempsEcran[i].nom));
            g_tempsEcran[i].secondes = 0;
            g_nbAppsSuivies++;
        }
        if (i < g_nbAppsSuivies) {
            g_tempsEcran[i].secondes++;
        }
    }

    // Sauvegarde une fois par minute (et à l'arrêt du mod).
    if (--g_secondesAvantSauvegarde <= 0) {
        SauverTempsEcran();
        g_secondesAvantSauvegarde = 60;
    }
}

// « 2 h 10 », « 52 min », « < 1 min ».
void FormaterDuree(int secondes, WCHAR* texte, size_t taille) {
    int minutes = secondes / 60;
    if (minutes >= 60) {
        swprintf_s(texte, taille, L"%d h %02d", minutes / 60, minutes % 60);
    } else if (minutes >= 1) {
        swprintf_s(texte, taille, L"%d min", minutes);
    } else {
        swprintf_s(texte, taille, L"< 1 min");
    }
}

// --- Dessin et commandes ----------------------------------------------------

const float CENTRE_ANNEAU_X = 180, CENTRE_ANNEAU_Y = 192, RAYON_ANNEAU = 96;
const float X_ECRAN = 390, LARGEUR_ECRAN = 300;

enum ZonePilotage {
    ZONE_MARCHE,    // démarrer / mettre en pause
    ZONE_REMETTRE,  // remettre à zéro
};

D2D1_RECT_F RectangleBoutonMarche() {
    return D2D1::RectF(88, 322, 88 + 150, 322 + 40);
}

D2D1_RECT_F RectangleBoutonRemettre() {
    return D2D1::RectF(252, 322, 252 + 40, 322 + 40);
}

int ZonePilotage(float x, float y) {
    if (DansRectangle(x, y, RectangleBoutonMarche())) return ZONE_MARCHE;
    if (DansRectangle(x, y, RectangleBoutonRemettre())) return ZONE_REMETTRE;
    return AUCUNE_ZONE;
}

void CliquerPilotage(int zone) {
    if (zone == ZONE_MARCHE) {
        DemarrerOuArreterMinuteur();
    } else if (zone == ZONE_REMETTRE) {
        RemettreMinuteur();
    }
    AfficherWidget(&g_pilotage);
}

// Arc de cercle partant du haut, dans le sens des aiguilles d'une montre,
// sur « part » du tour (0 à 1).
void DessinerArc(ID2D1RenderTarget* cible, float part, Couleur couleur,
                 float epaisseur, float echelle) {
    float cx = CENTRE_ANNEAU_X * echelle, cy = CENTRE_ANNEAU_Y * echelle;
    float r = RAYON_ANNEAU * echelle;
    ID2D1SolidColorBrush* pinceau = Pinceau(cible, couleur);
    if (!pinceau || part <= 0.001f) {
        Liberer(pinceau);
        return;
    }
    if (part >= 0.999f) {
        D2D1_ELLIPSE cercle = {D2D1::Point2F(cx, cy), r, r};
        cible->DrawEllipse(&cercle, pinceau, epaisseur * echelle, nullptr);
    } else {
        const float PI = 3.14159265f;
        float angle = part * 2 * PI;
        ID2D1PathGeometry* arc = nullptr;
        ID2D1GeometrySink* crayon = nullptr;
        if (SUCCEEDED(g_fabriqueD2D->CreatePathGeometry(&arc)) &&
            SUCCEEDED(arc->Open(&crayon))) {
            crayon->BeginFigure(D2D1::Point2F(cx, cy - r),
                                D2D1_FIGURE_BEGIN_HOLLOW);
            D2D1_ARC_SEGMENT segment = {
                D2D1::Point2F(cx + r * sinf(angle), cy - r * cosf(angle)),
                D2D1::SizeF(r, r), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE,
                part > 0.5f ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL};
            crayon->AddArc(&segment);
            crayon->EndFigure(D2D1_FIGURE_END_OPEN);
            if (SUCCEEDED(crayon->Close())) {
                cible->DrawGeometry(arc, pinceau, epaisseur * echelle,
                                    g_traitRond);
            }
        }
        Liberer(crayon);
        Liberer(arc);
    }
    pinceau->Release();
}

void Rond(ID2D1RenderTarget* cible, float x, float y, float rayon,
          Couleur couleur, float echelle) {
    ID2D1SolidColorBrush* pinceau = Pinceau(cible, couleur);
    if (pinceau) {
        D2D1_ELLIPSE rond = {D2D1::Point2F(x * echelle, y * echelle),
                             rayon * echelle, rayon * echelle};
        cible->FillEllipse(&rond, pinceau);
        pinceau->Release();
    }
}

// Le minuteur : anneau fin avec une lueur, grands chiffres, phase, pastilles
// du cycle, bouton « Démarrer » en pilule et bouton pour recommencer.
void DessinerMinuteur(ID2D1RenderTarget* cible, float echelle) {
    LONGLONG reste = ResteMs();
    float part = (float)reste / DureePhaseMs();

    // Piste très discrète, puis la lueur (même arc, large et transparent),
    // puis l'arc fin par-dessus.
    DessinerArc(cible, 1.0f, {255, 255, 255, 18}, 4, echelle);
    DessinerArc(cible, part, AccentTransparent(45), 14, echelle);
    DessinerArc(cible, part, g_accent, 4, echelle);

    WCHAR temps[16];
    LONGLONG secondes = (reste + 999) / 1000;
    swprintf_s(temps, L"%02lld:%02lld", secondes / 60, secondes % 60);
    Ecrire(cible, temps, POLICE_TITRE, 34, DWRITE_FONT_WEIGHT_NORMAL, TEXTE,
           CENTRE_ANNEAU_X, CENTRE_ANNEAU_Y - 44, A_CENTRE, echelle);
    Ecrire(cible, g_phase == PHASE_TRAVAIL ? L"Concentration" : L"Pause",
           POLICE, 11.5f, DWRITE_FONT_WEIGHT_NORMAL, TEXTE2, CENTRE_ANNEAU_X,
           CENTRE_ANNEAU_Y + 10, A_CENTRE, echelle);

    // Pastilles : sessions faites dans le cycle de 4 en cours.
    int faites = g_sessionsDuJour % SESSIONS_PAR_CYCLE;
    if (faites == 0 && g_sessionsDuJour > 0 && g_phase == PHASE_PAUSE) {
        faites = SESSIONS_PAR_CYCLE;  // pause après la 4e : cycle complet
    }
    for (int i = 0; i < SESSIONS_PAR_CYCLE; i++) {
        float x = CENTRE_ANNEAU_X + (i - 1.5f) * 16;
        Rond(cible, x, CENTRE_ANNEAU_Y + 42, 4,
             i < faites ? g_accent : Couleur{255, 255, 255, 40}, echelle);
    }

    // Bouton principal en pilule.
    D2D1_RECT_F r = RectangleBoutonMarche();
    bool survole = g_pilotage.survol == ZONE_MARCHE;
    RemplirArrondi(cible, r.left, r.top, r.right - r.left, r.bottom - r.top,
                   20, AccentTransparent(survole ? 120 : 85), echelle);
    const WCHAR* texte = g_minuteurEnMarche
                             ? L"Pause"
                             : (reste < DureePhaseMs() ? L"Reprendre"
                                                       : L"Démarrer");
    Ecrire(cible, g_minuteurEnMarche ? L"\xE769" : L"\xE768", POLICE_ICONES,
           10, DWRITE_FONT_WEIGHT_NORMAL, TEXTE, r.left + 24, r.top + 13,
           A_GAUCHE, echelle);
    Ecrire(cible, texte, POLICE, 12.5f, DWRITE_FONT_WEIGHT_SEMI_BOLD, TEXTE,
           r.left + 48, r.top + 9, A_GAUCHE, echelle);

    // Bouton rond pour recommencer.
    D2D1_RECT_F rr = RectangleBoutonRemettre();
    RemplirArrondi(cible, rr.left, rr.top, 40, 40, 20,
                   g_pilotage.survol == ZONE_REMETTRE ? BOUTON_SURVOL : BOUTON,
                   echelle);
    Ecrire(cible, L"\xE72C", POLICE_ICONES, 10, DWRITE_FONT_WEIGHT_NORMAL,
           TEXTE2, (rr.left + rr.right) / 2, rr.top + 13, A_CENTRE, echelle);
}

// Le temps d'écran : total du jour, puis les 4 apps les plus utilisées avec
// une barre proportionnelle à la première.
void DessinerTempsEcran(ID2D1RenderTarget* cible, float echelle) {
    int total = 0;
    int meilleures[4] = {-1, -1, -1, -1};
    for (int i = 0; i < g_nbAppsSuivies; i++) {
        total += g_tempsEcran[i].secondes;
        for (int place = 0; place < 4; place++) {
            if (meilleures[place] < 0 ||
                g_tempsEcran[i].secondes >
                    g_tempsEcran[meilleures[place]].secondes) {
                for (int k = 3; k > place; k--) {
                    meilleures[k] = meilleures[k - 1];
                }
                meilleures[place] = i;
                break;
            }
        }
    }

    WCHAR texte[48];
    Ecrire(cible, L"Temps d'écran", POLICE, 15, DWRITE_FONT_WEIGHT_NORMAL,
           TEXTE, X_ECRAN, 80, A_GAUCHE, echelle);
    FormaterDuree(total, texte, ARRAYSIZE(texte));
    Ecrire(cible, texte, POLICE, 15, DWRITE_FONT_WEIGHT_SEMI_BOLD, TEXTE,
           X_ECRAN + LARGEUR_ECRAN, 80, A_DROITE, echelle);
    Ecrire(cible, L"aujourd'hui, sans les pauses de plus de 5 min", POLICE, 10.5f,
           DWRITE_FONT_WEIGHT_NORMAL, TEXTE3, X_ECRAN, 106, A_GAUCHE, echelle);

    if (meilleures[0] < 0) {
        Ecrire(cible, L"Les apps utilisées apparaîtront ici.", POLICE, 12,
               DWRITE_FONT_WEIGHT_NORMAL, TEXTE3, X_ECRAN, 160, A_GAUCHE,
               echelle);
        return;
    }
    int plusLong = g_tempsEcran[meilleures[0]].secondes;
    for (int place = 0; place < 4 && meilleures[place] >= 0; place++) {
        const TempsApp& app = g_tempsEcran[meilleures[place]];
        float y = 146 + place * 58;
        Ecrire(cible, app.nom, POLICE, 13, DWRITE_FONT_WEIGHT_NORMAL, TEXTE,
               X_ECRAN, y, A_GAUCHE, echelle, 210);
        FormaterDuree(app.secondes, texte, ARRAYSIZE(texte));
        Ecrire(cible, texte, POLICE, 13, DWRITE_FONT_WEIGHT_NORMAL, TEXTE2,
               X_ECRAN + LARGEUR_ECRAN, y, A_DROITE, echelle);
        DessinerBarre(cible, X_ECRAN, y + 28, LARGEUR_ECRAN, 6,
                      plusLong > 0 ? (float)app.secondes / plusLong : 0,
                      echelle);
    }
}

void DessinerPilotage(ID2D1RenderTarget* cible, float echelle) {
    DessinerVerre(cible, echelle);
    Ecrire(cible, L"Poste de pilotage", POLICE, 16.5f,
           DWRITE_FONT_WEIGHT_SEMI_BOLD, TEXTE, 30, 27, A_GAUCHE, echelle);
    WCHAR sessions[80];
    swprintf_s(sessions, L"%d session%s de concentration aujourd'hui",
               g_sessionsDuJour, g_sessionsDuJour > 1 ? L"s" : L"");
    Ecrire(cible, sessions, POLICE, 11, DWRITE_FONT_WEIGHT_NORMAL, TEXTE3, 690,
           32, A_DROITE, echelle);

    DessinerMinuteur(cible, echelle);

    // Séparateur vertical entre le minuteur et le temps d'écran.
    ID2D1SolidColorBrush* trait = Pinceau(cible, {255, 255, 255, 20});
    if (trait) {
        float x = Arrondir(350 * echelle) + 0.5f;
        cible->DrawLine(D2D1::Point2F(x, 80 * echelle),
                        D2D1::Point2F(x, 370 * echelle), trait, 1.0f, nullptr);
        trait->Release();
    }

    DessinerTempsEcran(cible, echelle);
}

// ---------------------------------------------------------------------------
// Fonds d'écran
// ---------------------------------------------------------------------------

// Les 7 fonds « Lumière » faits pour Verre Desktop, dans l'ordre du
// diaporama. Le réglage « fond » vaut l'un de ces noms, ou « aucun ».
const WCHAR* const FONDS[] = {L"06-eclipse",        L"07-orbite",
                              L"08-soie",           L"09-dunes",
                              L"10-capsules-lumiere", L"11-aurore-boreale",
                              L"12-gemini-anneaux"};

WCHAR g_fondChoisi[64];          // « aucun » : le mod ne touche pas au fond
WCHAR g_dossierFonds[MAX_PATH];
int g_minutesDiaporama;          // 0 : pas de diaporama
int g_fondDuDiaporama;           // numéro dans FONDS du fond affiché
ULONGLONG g_instantFond;         // dernier changement du diaporama
volatile bool g_changementFondEnCours;

// Chemin de l'image d'un fond : « dossier\08-soie.png » (ou .jpg).
bool FichierDuFond(const WCHAR* nom, WCHAR* fichier) {
    const WCHAR* extensions[] = {L".png", L".jpg"};
    for (const WCHAR* extension : extensions) {
        WCHAR avecExtension[96];
        swprintf_s(avecExtension, L"%s%s", nom, extension);
        if (Joindre(fichier, MAX_PATH, g_dossierFonds, avecExtension) &&
            GetFileAttributes(fichier) != INVALID_FILE_ATTRIBUTES) {
            return true;
        }
    }
    return false;
}

// Applique le fond sur le bureau virtuel actuel, s'il n'y est pas déjà.
// Windows ne propose pas de fonction documentée pour changer le fond des
// autres bureaux (mesuré : IDesktopWallpaper et SystemParametersInfo ne
// changent que le bureau actuel, et l'Explorateur ne relit pas le fond
// écrit dans le registre) : un fond fixe est donc posé à la première
// arrivée sur chaque bureau.
DWORD WINAPI FilFond(LPVOID parametre) {
    WCHAR* fichier = (WCHAR*)parametre;
    HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    IDesktopWallpaper* fond = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_DesktopWallpaper, nullptr,
                                   CLSCTX_ALL, IID_IDesktopWallpaper,
                                   (void**)&fond))) {
        // Fond actuel du premier écran : rien à faire s'il est déjà bon.
        LPWSTR ecran = nullptr, actuel = nullptr;
        bool dejaBon = false;
        if (SUCCEEDED(fond->GetMonitorDevicePathAt(0, &ecran)) &&
            SUCCEEDED(fond->GetWallpaper(ecran, &actuel)) && actuel) {
            dejaBon = lstrcmpi(actuel, fichier) == 0;
        }
        CoTaskMemFree(actuel);
        CoTaskMemFree(ecran);
        if (!dejaBon) {
            fond->SetPosition(DWPOS_FILL);  // remplit l'écran sans déformer
            if (FAILED(fond->SetWallpaper(nullptr, fichier))) {
                Wh_Log(L"Impossible de changer le fond");
            }
        }
        fond->Release();
    }
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    HeapFree(GetProcessHeap(), 0, fichier);
    g_changementFondEnCours = false;
    return 0;
}

// Nom du fond voulu en ce moment (le réglage, ou celui du diaporama).
const WCHAR* FondVoulu() {
    if (lstrcmpi(g_fondChoisi, L"aucun") == 0 || !g_fondChoisi[0]) {
        return nullptr;
    }
    return g_minutesDiaporama > 0 ? FONDS[g_fondDuDiaporama] : g_fondChoisi;
}

// Lance l'application du fond voulu dans un thread à part (décoder une image
// 4K et la poser prend un moment).
void AppliquerFond() {
    const WCHAR* nom = FondVoulu();
    WCHAR fichier[MAX_PATH];
    if (!nom || g_changementFondEnCours || !FichierDuFond(nom, fichier)) {
        return;
    }
    WCHAR* copie = (WCHAR*)HeapAlloc(GetProcessHeap(), 0,
                                     MAX_PATH * sizeof(WCHAR));
    if (!copie) {
        return;
    }
    lstrcpyn(copie, fichier, MAX_PATH);
    g_changementFondEnCours = true;
    HANDLE fil = CreateThread(nullptr, 0, FilFond, copie, 0, nullptr);
    if (fil) {
        CloseHandle(fil);
    } else {
        g_changementFondEnCours = false;
        HeapFree(GetProcessHeap(), 0, copie);
    }
}

// Appelée chaque seconde : passe au fond suivant du diaporama quand c'est
// l'heure (les fonds absents du dossier sont sautés).
void AvancerDiaporama() {
    if (g_minutesDiaporama <= 0 || !FondVoulu() ||
        GetTickCount64() - g_instantFond < g_minutesDiaporama * 60000ULL) {
        return;
    }
    g_instantFond = GetTickCount64();
    WCHAR fichier[MAX_PATH];
    for (int essai = 0; essai < (int)ARRAYSIZE(FONDS); essai++) {
        g_fondDuDiaporama = (g_fondDuDiaporama + 1) % ARRAYSIZE(FONDS);
        if (FichierDuFond(FONDS[g_fondDuDiaporama], fichier)) {
            break;
        }
    }
    AppliquerFond();
}

void LireReglagesFond() {
    CopierReglage(L"fond", g_fondChoisi, ARRAYSIZE(g_fondChoisi));
    WCHAR dossier[MAX_PATH];
    CopierReglage(L"dossierFonds", dossier, ARRAYSIZE(dossier));
    if (!ExpandEnvironmentStrings(dossier, g_dossierFonds, MAX_PATH)) {
        lstrcpyn(g_dossierFonds, dossier, MAX_PATH);
    }
    g_minutesDiaporama = Wh_GetIntSetting(L"diaporamaMinutes");
    if (g_minutesDiaporama < 0) {
        g_minutesDiaporama = 0;
    }
    // Le diaporama démarre sur le fond choisi.
    g_fondDuDiaporama = 0;
    for (int i = 0; i < (int)ARRAYSIZE(FONDS); i++) {
        if (lstrcmpi(FONDS[i], g_fondChoisi) == 0) {
            g_fondDuDiaporama = i;
        }
    }
    g_instantFond = GetTickCount64();
}

// ---------------------------------------------------------------------------
// Entretien : boutons « Libérer » (mémoire) et « Nettoyer » (disque)
// ---------------------------------------------------------------------------

enum EtatEntretien {
    ENTRETIEN_REPOS,
    ENTRETIEN_EN_COURS,     // travail en cours dans un thread
    ENTRETIEN_A_CONFIRMER,  // disque : analyse finie, on attend l'accord
    ENTRETIEN_FINI,         // résultat affiché quelques secondes
};

EtatEntretien g_etatMemoire, g_etatDisque;
ULONGLONG g_instantResultatMemoire, g_instantResultatDisque;
LONGLONG g_octetsLiberesMemoire;   // résultat de « Libérer »
LONGLONG g_octetsAnalyses;         // disque : ce qui peut être supprimé
LONGLONG g_octetsLiberesDisque;    // disque : ce qui a été libéré

const ULONGLONG DUREE_RESULTAT_MS = 6000;

// Écrit une taille : « 512 Mo », « 1,8 Go ».
void FormaterTaille(LONGLONG octets, WCHAR* texte, size_t taille) {
    if (octets < 0) {
        octets = 0;
    }
    if (octets >= 1024LL * 1024 * 1024) {
        swprintf_s(texte, taille, L"%.1f Go", octets / 1073741824.0);
        VirguleFrancaise(texte);
    } else {
        swprintf_s(texte, taille, L"%lld Mo", octets / 1048576);
    }
}

// --- Mémoire ----------------------------------------------------------------

// Demande à Windows de vider la « mémoire de travail » de chaque app (sauf
// celle au premier plan, qu'on ne veut pas ralentir). Aucune app n'est
// fermée : Windows rechargera ce dont elles ont vraiment besoin.
DWORD WINAPI FilLiberationMemoire(LPVOID) {
    MEMORYSTATUSEX avant = {sizeof(avant)};
    GlobalMemoryStatusEx(&avant);

    DWORD premierPlan = 0;
    GetWindowThreadProcessId(GetForegroundWindow(), &premierPlan);
    static DWORD processus[4096];
    DWORD octets = 0;
    if (EnumProcesses(processus, sizeof(processus), &octets)) {
        for (DWORD i = 0; i < octets / sizeof(DWORD); i++) {
            DWORD id = processus[i];
            if (id <= 4 || id == premierPlan) {
                continue;  // 0 et 4 : processus du système
            }
            HANDLE ouvert = OpenProcess(
                PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SET_QUOTA, FALSE,
                id);
            if (ouvert) {  // refusé pour les processus protégés : on passe
                EmptyWorkingSet(ouvert);
                CloseHandle(ouvert);
            }
        }
    }

    Sleep(800);  // laisse Windows faire ses comptes
    MEMORYSTATUSEX apres = {sizeof(apres)};
    GlobalMemoryStatusEx(&apres);
    LONGLONG libere =
        (LONGLONG)apres.ullAvailPhys - (LONGLONG)avant.ullAvailPhys;
    g_octetsLiberesMemoire = libere;
    if (g_controleur) {
        PostMessage(g_controleur, WM_APP_ENTRETIEN, 0, 0);
    }
    return 0;
}

// --- Disque -----------------------------------------------------------------

// Dossiers nettoyés : les fichiers temporaires de l'utilisateur et les
// rapports de plantage. On ne garde que les fichiers de plus d'un jour : un
// fichier récent peut servir à une app ouverte.
int DossiersANettoyer(WCHAR (*dossiers)[MAX_PATH]) {
    int nombre = 0;
    DWORD longueur = GetTempPath(MAX_PATH, dossiers[nombre]);
    if (longueur > 0 && longueur < MAX_PATH) {
        if (dossiers[nombre][longueur - 1] == L'\\') {
            dossiers[nombre][longueur - 1] = L'\0';
        }
        nombre++;
    }
    WCHAR local[MAX_PATH];
    if (ExpandEnvironmentStrings(L"%LOCALAPPDATA%", local, MAX_PATH) &&
        Joindre(dossiers[nombre], MAX_PATH, local, L"CrashDumps")) {
        nombre++;
    }
    return nombre;
}

bool AssezVieux(const FILETIME& modification) {
    FILETIME maintenant;
    GetSystemTimeAsFileTime(&maintenant);
    const ULONGLONG UN_JOUR = 24ULL * 3600 * 10000000;  // en 100 ns
    return VersEntier(maintenant) - VersEntier(modification) > UN_JOUR;
}

// Parcourt un dossier : additionne la taille des vieux fichiers et, si
// « supprimer », les supprime (les fichiers en cours d'utilisation refusent :
// on les laisse). Les raccourcis de dossier ne sont jamais suivis.
LONGLONG ParcourirPourNettoyer(const WCHAR* dossier, bool supprimer,
                               int profondeur) {
    WCHAR motif[MAX_PATH];
    if (profondeur > 8 || !Joindre(motif, MAX_PATH, dossier, L"*")) {
        return 0;
    }
    LONGLONG total = 0;
    WIN32_FIND_DATA trouve;
    HANDLE recherche = FindFirstFile(motif, &trouve);
    if (recherche == INVALID_HANDLE_VALUE) {
        return 0;
    }
    do {
        if (lstrcmp(trouve.cFileName, L".") == 0 ||
            lstrcmp(trouve.cFileName, L"..") == 0 ||
            (trouve.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
            continue;
        }
        WCHAR chemin[MAX_PATH];
        if (!Joindre(chemin, MAX_PATH, dossier, trouve.cFileName)) {
            continue;
        }
        if (trouve.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            total += ParcourirPourNettoyer(chemin, supprimer, profondeur + 1);
            if (supprimer) {
                RemoveDirectory(chemin);  // échoue s'il n'est pas vide : normal
            }
        } else if (AssezVieux(trouve.ftLastWriteTime)) {
            LONGLONG taille =
                ((LONGLONG)trouve.nFileSizeHigh << 32) | trouve.nFileSizeLow;
            if (!supprimer || DeleteFile(chemin)) {
                total += taille;
            }
        }
    } while (FindNextFile(recherche, &trouve));
    FindClose(recherche);
    return total;
}

LONGLONG TailleCorbeille() {
    SHQUERYRBINFO infos = {sizeof(infos)};
    return SUCCEEDED(SHQueryRecycleBin(nullptr, &infos)) ? infos.i64Size : 0;
}

LONGLONG EspaceLibreC() {
    ULARGE_INTEGER libre;
    return GetDiskFreeSpaceEx(L"C:\\", &libre, nullptr, nullptr)
               ? (LONGLONG)libre.QuadPart
               : 0;
}

// parametre = 0 : analyser ; 1 : supprimer (après confirmation).
DWORD WINAPI FilNettoyage(LPVOID parametre) {
    bool supprimer = parametre != nullptr;
    HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    WCHAR dossiers[4][MAX_PATH];
    int nombre = DossiersANettoyer(dossiers);

    if (!supprimer) {
        LONGLONG total = TailleCorbeille();
        for (int i = 0; i < nombre; i++) {
            total += ParcourirPourNettoyer(dossiers[i], false, 0);
        }
        g_octetsAnalyses = total;
    } else {
        LONGLONG avant = EspaceLibreC();
        for (int i = 0; i < nombre; i++) {
            ParcourirPourNettoyer(dossiers[i], true, 0);
        }
        // Vider la corbeille est définitif : l'utilisateur vient de le
        // confirmer en cliquant sur « Supprimer ».
        SHEmptyRecycleBin(nullptr, nullptr,
                          SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI |
                              SHERB_NOSOUND);
        g_octetsLiberesDisque = EspaceLibreC() - avant;
    }

    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    if (g_controleur) {
        PostMessage(g_controleur, WM_APP_ENTRETIEN, 1, supprimer ? 1 : 0);
    }
    return 0;
}

void LancerTravail(LPTHREAD_START_ROUTINE travail, LPVOID parametre) {
    HANDLE fil = CreateThread(nullptr, 0, travail, parametre, 0, nullptr);
    if (fil) {
        CloseHandle(fil);
    }
}

// Message WM_APP_ENTRETIEN : un travail est fini (wParam 0 : mémoire,
// 1 : disque ; lParam 1 : la suppression, 0 : l'analyse).
void FinEntretien(WPARAM quoi, LPARAM suppression) {
    if (quoi == 0) {
        g_etatMemoire = ENTRETIEN_FINI;
        g_instantResultatMemoire = GetTickCount64();
    } else if (suppression) {
        g_etatDisque = ENTRETIEN_FINI;
        g_instantResultatDisque = GetTickCount64();
        g_secondesAvantDisque = 0;  // la carte relit le disque tout de suite
    } else {
        // Rien à supprimer : on le dit, sans demander de confirmation.
        g_etatDisque =
            g_octetsAnalyses > 0 ? ENTRETIEN_A_CONFIRMER : ENTRETIEN_FINI;
        g_octetsLiberesDisque = 0;
        g_instantResultatDisque = GetTickCount64();
    }
    AfficherWidget(&g_systeme);
}

// Appelée chaque seconde : les résultats s'effacent après quelques
// secondes, et une confirmation oubliée est annulée au bout de 30 s.
void AvancerEntretien() {
    ULONGLONG maintenant = GetTickCount64();
    if (g_etatMemoire == ENTRETIEN_FINI &&
        maintenant - g_instantResultatMemoire > DUREE_RESULTAT_MS) {
        g_etatMemoire = ENTRETIEN_REPOS;
    }
    if ((g_etatDisque == ENTRETIEN_FINI &&
         maintenant - g_instantResultatDisque > DUREE_RESULTAT_MS) ||
        (g_etatDisque == ENTRETIEN_A_CONFIRMER &&
         maintenant - g_instantResultatDisque > 30000)) {
        g_etatDisque = ENTRETIEN_REPOS;
    }
}

// --- Boutons sur la carte Système --------------------------------------------

enum ZoneSysteme { ZONE_LIBERER, ZONE_NETTOYER, ZONE_ANNULER };

// Rectangles des petits boutons, mesurés au dessin (pixels de référence).
D2D1_RECT_F g_rectangleLiberer, g_rectangleNettoyer, g_rectangleAnnuler;

int ZoneSysteme(float x, float y) {
    if (DansRectangle(x, y, g_rectangleLiberer)) return ZONE_LIBERER;
    if (DansRectangle(x, y, g_rectangleNettoyer)) return ZONE_NETTOYER;
    if (g_etatDisque == ENTRETIEN_A_CONFIRMER &&
        DansRectangle(x, y, g_rectangleAnnuler)) {
        return ZONE_ANNULER;
    }
    return AUCUNE_ZONE;
}

void CliquerSysteme(int zone) {
    if (zone == ZONE_LIBERER && g_etatMemoire == ENTRETIEN_REPOS) {
        g_etatMemoire = ENTRETIEN_EN_COURS;
        LancerTravail(FilLiberationMemoire, nullptr);
    } else if (zone == ZONE_NETTOYER) {
        if (g_etatDisque == ENTRETIEN_REPOS) {
            g_etatDisque = ENTRETIEN_EN_COURS;  // analyse
            LancerTravail(FilNettoyage, nullptr);
        } else if (g_etatDisque == ENTRETIEN_A_CONFIRMER) {
            g_etatDisque = ENTRETIEN_EN_COURS;  // suppression confirmée
            LancerTravail(FilNettoyage, (LPVOID)1);
        }
    } else if (zone == ZONE_ANNULER) {
        g_etatDisque = ENTRETIEN_REPOS;
    }
    AfficherWidget(&g_systeme);
}

// Petit bouton en pilule placé juste après un texte (x = fin du texte).
// Renvoie son rectangle.
D2D1_RECT_F DessinerPetitBouton(ID2D1RenderTarget* cible, float x, float y,
                                const WCHAR* texte, bool accent, bool survole,
                                float echelle) {
    float largeurTexte = 0;
    // Première écriture « à vide » (transparente) pour mesurer le texte.
    Ecrire(cible, texte, POLICE, 10.5f, DWRITE_FONT_WEIGHT_SEMI_BOLD,
           {0, 0, 0, 0}, x, y, A_GAUCHE, echelle, 0, &largeurTexte);
    // Vrai bouton bleu, bien visible ; « Non » reste discret (gris).
    D2D1_RECT_F r = D2D1::RectF(x, y, x + largeurTexte + 22, y + 24);
    Couleur fond = accent ? AccentTransparent(survole ? 255 : 210)
                          : (survole ? BOUTON_SURVOL : Couleur{255, 255, 255, 28});
    RemplirArrondi(cible, r.left, r.top, r.right - r.left, 24, 12, fond,
                   echelle);
    Ecrire(cible, texte, POLICE, 10.5f, DWRITE_FONT_WEIGHT_SEMI_BOLD,
           accent ? Couleur{12, 18, 30, 255} : TEXTE2, x + 11, y + 3.5f,
           A_GAUCHE, echelle);
    return r;
}

// Dessine « Libérer » à côté de « Mémoire » et « Nettoyer » à côté de
// « Disque C: », selon leur état.
void DessinerBoutonsEntretien(ID2D1RenderTarget* cible, float echelle) {
    float largeur = 0;
    WCHAR texte[64], taille[24];

    // Mémoire (ligne y = 177)
    Ecrire(cible, L"Mémoire", POLICE, 15, DWRITE_FONT_WEIGHT_NORMAL,
           {0, 0, 0, 0}, 30, 177, A_GAUCHE, echelle, 0, &largeur);
    float x = 30 + largeur + 12;
    switch (g_etatMemoire) {
        case ENTRETIEN_EN_COURS:
            lstrcpy(texte, L"Libération…");
            break;
        case ENTRETIEN_FINI:
            FormaterTaille(g_octetsLiberesMemoire, taille, ARRAYSIZE(taille));
            swprintf_s(texte, L"%s libérés", taille);
            break;
        default:
            lstrcpy(texte, L"Libérer");
    }
    g_rectangleLiberer = DessinerPetitBouton(
        cible, x, 180, texte, true,
        g_systeme.survol == ZONE_LIBERER, echelle);

    // Disque (ligne y = 285)
    Ecrire(cible, L"Disque C:", POLICE, 15, DWRITE_FONT_WEIGHT_NORMAL,
           {0, 0, 0, 0}, 30, 285, A_GAUCHE, echelle, 0, &largeur);
    x = 30 + largeur + 12;
    bool accent = true;
    switch (g_etatDisque) {
        case ENTRETIEN_EN_COURS:
            lstrcpy(texte, L"Analyse…");
            break;
        case ENTRETIEN_A_CONFIRMER:
            FormaterTaille(g_octetsAnalyses, taille, ARRAYSIZE(taille));
            swprintf_s(texte, L"Supprimer %s ?", taille);
            break;
        case ENTRETIEN_FINI:
            if (g_octetsLiberesDisque > 0) {
                FormaterTaille(g_octetsLiberesDisque, taille,
                               ARRAYSIZE(taille));
                swprintf_s(texte, L"%s libérés", taille);
            } else {
                lstrcpy(texte, L"Rien à nettoyer");
            }
            break;
        default:
            lstrcpy(texte, L"Nettoyer");
    }
    g_rectangleNettoyer = DessinerPetitBouton(
        cible, x, 288, texte, accent, g_systeme.survol == ZONE_NETTOYER,
        echelle);
    if (g_etatDisque == ENTRETIEN_A_CONFIRMER) {
        g_rectangleAnnuler = DessinerPetitBouton(
            cible, g_rectangleNettoyer.right + 6, 288, L"Non", false,
            g_systeme.survol == ZONE_ANNULER, echelle);
    }
}

// ---------------------------------------------------------------------------
// Affichage d'un widget à l'écran
// ---------------------------------------------------------------------------

// Fait dessiner le widget par Direct2D dans une image en mémoire (le « DC
// mémoire »), dont chaque pixel a sa propre opacité. La « cible » de dessin
// est gardée d'une fois sur l'autre (les icônes des apps y sont rangées).
bool DessinerDansImage(Widget* widget, HDC memoire, int largeur, int hauteur) {
    if (!widget->cible) {
        D2D1_RENDER_TARGET_PROPERTIES proprietes = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED),
            // 96 ppp : une unité Direct2D = un vrai pixel de l'écran.
            96.0f, 96.0f);
        if (FAILED(g_fabriqueD2D->CreateDCRenderTarget(&proprietes,
                                                       &widget->cible))) {
            Wh_Log(L"CreateDCRenderTarget a échoué");
            widget->cible = nullptr;
            return false;
        }
        // Lissage du texte en niveaux de gris : le lissage ClearType (en
        // couleurs) ne marche pas sur une image transparente.
        widget->cible->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    }

    RECT zone = {0, 0, largeur, hauteur};
    if (FAILED(widget->cible->BindDC(memoire, &zone))) {
        return false;
    }
    widget->cible->BeginDraw();
    widget->cible->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
    widget->dessiner(widget->cible, Echelle(widget->fenetre));
    HRESULT resultat = widget->cible->EndDraw();

    if (resultat == D2DERR_RECREATE_TARGET) {
        // La carte graphique a été réinitialisée : on repartira de zéro.
        if (widget == &g_accueil) {
            OublierBitmapsApps();
        }
        if (widget == &g_sites) {
            OublierBitmapsSites();
        }
        Liberer(widget->cible);
    }
    if (FAILED(resultat)) {
        Wh_Log(L"EndDraw a échoué (0x%08X)", (unsigned)resultat);
        return false;
    }
    return true;
}

// Prépare une image 32 bits de la taille de la fenêtre, la fait dessiner,
// puis la donne à Windows avec UpdateLayeredWindow (fenêtre « layered » :
// Windows affiche l'image telle quelle, transparence comprise).
bool AfficherWidget(Widget* widget) {
    RECT rect;
    if (!widget->fenetre || !GetWindowRect(widget->fenetre, &rect)) {
        return false;
    }
    int largeur = rect.right - rect.left;
    int hauteur = rect.bottom - rect.top;

    BITMAPINFO infos = {};
    infos.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    infos.bmiHeader.biWidth = largeur;
    infos.bmiHeader.biHeight = -hauteur;  // négatif : la ligne 0 est en haut
    infos.bmiHeader.biPlanes = 1;
    infos.bmiHeader.biBitCount = 32;
    infos.bmiHeader.biCompression = BI_RGB;

    bool reussi = false;
    HDC ecran = GetDC(nullptr);
    HDC memoire = ecran ? CreateCompatibleDC(ecran) : nullptr;
    void* pixels = nullptr;
    HBITMAP image = memoire ? CreateDIBSection(memoire, &infos, DIB_RGB_COLORS,
                                               &pixels, nullptr, 0)
                            : nullptr;

    if (image) {
        HGDIOBJ ancienneImage = SelectObject(memoire, image);

        if (DessinerDansImage(widget, memoire, largeur, hauteur)) {
            POINT position = {rect.left, rect.top};
            SIZE taille = {largeur, hauteur};
            POINT origine = {0, 0};
            BLENDFUNCTION melange = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
            reussi = UpdateLayeredWindow(widget->fenetre, ecran, &position,
                                         &taille, memoire, &origine, 0,
                                         &melange, ULW_ALPHA);
            if (!reussi) {
                Wh_Log(L"UpdateLayeredWindow a échoué (%u)", GetLastError());
            }
        }

        SelectObject(memoire, ancienneImage);
        DeleteObject(image);
    } else {
        Wh_Log(L"Impossible de créer l'image en mémoire");
    }

    if (memoire) {
        DeleteDC(memoire);
    }
    if (ecran) {
        ReleaseDC(nullptr, ecran);
    }
    return reussi;
}

// Range le widget juste au-dessus du bureau (la fenêtre « Progman »), sous les
// fenêtres des applications. On évite HWND_BOTTOM : le widget passerait sous
// le bureau lui-même et deviendrait invisible.
void PoserAuFond(HWND fenetre) {
    HWND bureau = FindWindow(L"Progman", nullptr);
    if (!bureau) {
        return;
    }

    HWND justeAuDessus = GetWindow(bureau, GW_HWNDPREV);
    if (justeAuDessus == fenetre) {
        return;  // déjà à sa place
    }
    // SetWindowPos range « fenetre » juste en dessous de « justeAuDessus »,
    // donc juste au-dessus du bureau. Si rien n'est au-dessus du bureau, ou
    // seulement des fenêtres « toujours visibles » (le bureau est alors montré
    // par-dessus les apps : « Afficher le bureau »), le widget va en tête des
    // fenêtres normales, c'est-à-dire juste au-dessus du bureau lui aussi.
    if (!justeAuDessus ||
        (GetWindowLongPtr(justeAuDessus, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
        justeAuDessus = HWND_TOP;
    }
    g_changementVoulu = true;
    SetWindowPos(fenetre, justeAuDessus, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                     SWP_NOOWNERZORDER);
    g_changementVoulu = false;
}

// ---------------------------------------------------------------------------
// Bureaux virtuels
// ---------------------------------------------------------------------------

// Le bureau actuel est-il le premier ? Windows range la liste des bureaux
// (VirtualDesktopIDs : des GUID de 16 octets bout à bout, le premier est le
// bureau 1) et le bureau actuel (CurrentVirtualDesktop) dans le registre.
// En cas de doute (un seul bureau, valeur absente), on répond oui.
bool SurPremierBureau() {
    if (!g_cleBureaux) {
        return true;
    }
    BYTE liste[16 * 64];
    DWORD tailleListe = sizeof(liste);
    GUID actuel;
    DWORD tailleActuel = sizeof(actuel);
    if (RegQueryValueEx(g_cleBureaux, L"VirtualDesktopIDs", nullptr, nullptr,
                        liste, &tailleListe) != ERROR_SUCCESS ||
        tailleListe < 16 ||
        RegQueryValueEx(g_cleBureaux, L"CurrentVirtualDesktop", nullptr,
                        nullptr, (BYTE*)&actuel,
                        &tailleActuel) != ERROR_SUCCESS ||
        tailleActuel != sizeof(actuel)) {
        return true;
    }
    return memcmp(liste, &actuel, sizeof(actuel)) == 0;
}

// Demande à Windows de signaler g_changementBureaux au prochain changement
// de la clé (il faut redemander après chaque signal).
void SurveillerBureaux() {
    if (g_cleBureaux && g_changementBureaux) {
        RegNotifyChangeKeyValue(g_cleBureaux, FALSE, REG_NOTIFY_CHANGE_LAST_SET,
                                g_changementBureaux, TRUE);
    }
}

// Affiche le groupe du bureau actuel et cache l'autre. Le nouveau groupe est
// affiché avant que l'ancien soit caché, pour ne jamais laisser l'écran vide.
void AppliquerBureau(bool forcer) {
    bool premier = SurPremierBureau();
    if (premier == g_surPremierBureau && !forcer) {
        return;
    }
    g_surPremierBureau = premier;

    for (Widget* widget : g_widgets) {
        if (widget->fenetre && widget->accueil != premier) {
            AfficherWidget(widget);  // contenu à jour avant d'apparaître
            g_changementVoulu = true;
            ShowWindow(widget->fenetre, SW_SHOWNOACTIVATE);
            g_changementVoulu = false;
            PoserAuFond(widget->fenetre);
        }
    }
    for (Widget* widget : g_widgets) {
        if (widget->fenetre && widget->accueil == premier) {
            g_changementVoulu = true;
            ShowWindow(widget->fenetre, SW_HIDE);
            g_changementVoulu = false;
        }
    }

    // En arrivant sur l'accueil, on relit le dossier des apps (au plus toutes
    // les 10 s) : un raccourci ajouté apparaît sans rien faire d'autre.
    if (!premier && GetTickCount64() - g_instantLectureApps > 10000) {
        DemanderApps();
    }
}

// ---------------------------------------------------------------------------
// Réglages
// ---------------------------------------------------------------------------

void CopierReglage(PCWSTR nom, WCHAR* destination, int taille) {
    PCWSTR valeur = Wh_GetStringSetting(nom);
    lstrcpyn(destination, valeur, taille);
    Wh_FreeStringSetting(valeur);
}

// « #66BCFF » (ou « 66BCFF ») devient une couleur. Faux si le texte n'est pas
// un code hexadécimal à 6 chiffres.
bool LireCouleur(const WCHAR* texte, Couleur* couleur) {
    if (*texte == L'#') {
        texte++;
    }
    if (wcslen(texte) != 6) {
        return false;
    }
    WCHAR* fin = nullptr;
    unsigned long valeur = wcstoul(texte, &fin, 16);
    if (!fin || *fin) {
        return false;
    }
    *couleur = {(BYTE)(valeur >> 16), (BYTE)(valeur >> 8), (BYTE)valeur, 255};
    return true;
}

// Opacité du verre et couleur d'accent.
void LireApparence() {
    int opacite = Wh_GetIntSetting(L"opaciteVerre");
    if (opacite < 0) opacite = 0;
    if (opacite > 100) opacite = 100;
    BYTE haut = (BYTE)(opacite * 255 / 100);
    g_verreHaut = {VERRE_HAUT.r, VERRE_HAUT.g, VERRE_HAUT.b, haut};
    g_verreBas = {VERRE_BAS.r, VERRE_BAS.g, VERRE_BAS.b,
                  (BYTE)(haut * VERRE_BAS.a / VERRE_HAUT.a)};

    WCHAR texte[32];
    CopierReglage(L"couleurAccent", texte, ARRAYSIZE(texte));
    if (!LireCouleur(texte, &g_accent)) {
        g_accent = ACCENT_PAR_DEFAUT;
    }
    g_accentDoux = AccentTransparent(60);
}

// Positions des widgets (en pixels de référence, écran à 150 %).
void LirePositions() {
    g_horloge.x = Wh_GetIntSetting(L"horloge.x");
    g_horloge.y = Wh_GetIntSetting(L"horloge.y");
    g_systeme.x = Wh_GetIntSetting(L"systeme.x");
    g_systeme.y = Wh_GetIntSetting(L"systeme.y");
    g_accesRapide.x = Wh_GetIntSetting(L"accesRapide.marge");
    g_accesRapide.y = Wh_GetIntSetting(L"accesRapide.y");
    g_recherche.y = Wh_GetIntSetting(L"recherche.y");
    g_accueil.y = Wh_GetIntSetting(L"accueil.y");
    g_sites.x = Wh_GetIntSetting(L"sites.x");
    g_sites.y = Wh_GetIntSetting(L"sites.y");
    g_pilotage.y = Wh_GetIntSetting(L"pilotage.y");
}

// Durées du minuteur, entre 1 et 180 minutes.
void LireMinuteur() {
    int travail = Wh_GetIntSetting(L"minutesConcentration");
    int pause = Wh_GetIntSetting(L"minutesPause");
    g_minutesTravail = travail < 1 ? 1 : (travail > 180 ? 180 : travail);
    g_minutesPause = pause < 1 ? 1 : (pause > 180 ? 180 : pause);
}

void LireReglages() {
    LireApparence();
    LirePositions();
    LireMinuteur();
    LireReglagesFond();
    WCHAR nom[32];
    for (int i = 0; i < 6; i++) {
        swprintf_s(nom, L"dossier%dNom", i + 1);
        CopierReglage(nom, g_dossiers[i].nom, ARRAYSIZE(g_dossiers[i].nom));
        swprintf_s(nom, L"dossier%dChemin", i + 1);
        CopierReglage(nom, g_dossiers[i].chemin,
                      ARRAYSIZE(g_dossiers[i].chemin));
    }
    for (int i = 0; i < NB_FAVORIS; i++) {
        swprintf_s(nom, L"site%dNom", i + 1);
        CopierReglage(nom, g_favoris[i].nom, ARRAYSIZE(g_favoris[i].nom));
        swprintf_s(nom, L"site%dAdresse", i + 1);
        CopierReglage(nom, g_favoris[i].adresse,
                      ARRAYSIZE(g_favoris[i].adresse));
        swprintf_s(nom, L"site%dCompte", i + 1);
        CopierReglage(nom, g_favoris[i].compte,
                      ARRAYSIZE(g_favoris[i].compte));
    }
    CopierReglage(L"prenom", g_prenom, ARRAYSIZE(g_prenom));
    CopierReglage(L"renommages", g_renommages, ARRAYSIZE(g_renommages));
    CopierReglage(L"moteurRecherche", g_moteurRecherche,
                  ARRAYSIZE(g_moteurRecherche));

    // « %USERPROFILE%\... » devient « C:\Users\...\... ».
    WCHAR dossier[MAX_PATH];
    CopierReglage(L"dossierApps", dossier, ARRAYSIZE(dossier));
    if (!ExpandEnvironmentStrings(dossier, g_dossierApps, MAX_PATH)) {
        lstrcpyn(g_dossierApps, dossier, MAX_PATH);
    }
    // Pas de « \ » final, pour pouvoir ajouter « \nom.lnk ».
    size_t longueur = wcslen(g_dossierApps);
    if (longueur > 3 && g_dossierApps[longueur - 1] == L'\\') {
        g_dossierApps[longueur - 1] = L'\0';
    }
}

// ---------------------------------------------------------------------------
// Fenêtres
// ---------------------------------------------------------------------------

Widget* WidgetDe(HWND fenetre) {
    for (Widget* widget : g_widgets) {
        if (widget->fenetre == fenetre) {
            return widget;
        }
    }
    return nullptr;
}

LRESULT CALLBACK ProcedureWidget(HWND fenetre,
                                 UINT message,
                                 WPARAM wParam,
                                 LPARAM lParam) {
    // La barre de recherche est le seul widget qui prend le clavier.
    if (fenetre == g_recherche.fenetre && fenetre) {
        switch (message) {
            case WM_MOUSEACTIVATE:
                return MA_ACTIVATE;

            case WM_ACTIVATE:
                g_barreActive = LOWORD(wParam) != WA_INACTIVE;
                g_curseurVisible = g_barreActive;
                if (g_barreActive) {
                    SetTimer(fenetre, MINUTERIE_CURSEUR, 530, nullptr);
                    // Catalogue relu au plus toutes les 5 minutes.
                    if (!g_catalogue ||
                        GetTickCount64() - g_instantCatalogue > 300000) {
                        DemanderCatalogue();
                    }
                    // Index des dossiers relu au plus toutes les 2 minutes.
                    if (!g_index || GetTickCount64() - g_instantIndex > 120000) {
                        DemanderIndex();
                    }
                    ConstruireResultats();
                } else {
                    KillTimer(fenetre, MINUTERIE_CURSEUR);
                }
                AfficherWidget(&g_recherche);
                return 0;

            case WM_CHAR:
                SaisirCaractere((WCHAR)wParam);
                return 0;

            case WM_KEYDOWN:
                if (wParam == VK_DOWN || wParam == VK_UP) {
                    ChoisirLigne(wParam == VK_DOWN ? 1 : -1);
                    return 0;
                }
                break;
        }
    }

    // Le bouton Sites prend aussi le clavier : on sait ainsi quand on clique
    // ailleurs, et le panneau se replie.
    if (fenetre == g_sites.fenetre && fenetre) {
        switch (message) {
            case WM_MOUSEACTIVATE:
                return MA_ACTIVATE;

            case WM_ACTIVATE:
                if (LOWORD(wParam) == WA_INACTIVE && g_sitesOuverts) {
                    g_sitesOuverts = false;
                    g_choixSite = -1;
                    AfficherWidget(&g_sites);
                }
                return 0;

            case WM_MOUSEWHEEL:
            case WM_MOUSEHWHEEL:
                if (g_sitesOuverts) {
                    // Molette vers le bas, ou deux doigts vers la gauche :
                    // la bande avance.
                    float crans = (short)HIWORD(wParam) / 120.0f;
                    Defiler(message == WM_MOUSEWHEEL ? -crans * 80
                                                     : crans * 80);
                }
                return 0;
        }
    }

    switch (message) {
        case WM_MOUSEACTIVATE:
            // Un clic ne doit pas donner le focus au widget ni le faire
            // passer devant les autres fenêtres.
            return MA_NOACTIVATE;

        case WM_NCDESTROY: {
            // Un widget détruit sans que le mod l'ait voulu : le bureau auquel
            // il est rattaché a disparu (redémarrage de l'Explorateur). Il
            // sera recréé par RecreerWidgetsPerdus.
            Widget* perdu = WidgetDe(fenetre);
            if (perdu && !g_changementVoulu) {
                Wh_Log(L"Widget détruit avec le bureau, il sera recréé");
                if (perdu == &g_accueil) {
                    OublierBitmapsApps();
                }
                Liberer(perdu->cible);
                perdu->fenetre = nullptr;
                perdu->survol = AUCUNE_ZONE;
                perdu->sourisSuivie = false;
            }
            break;
        }

        case WM_WINDOWPOSCHANGING:
            if (!g_changementVoulu) {
                // Quelqu'un d'autre (« Afficher le bureau », le geste à 3
                // doigts...) veut cacher le widget ou changer sa place dans
                // la pile des fenêtres : refusé, seul le mod le fait.
                WINDOWPOS* position = (WINDOWPOS*)lParam;
                position->flags &= ~SWP_HIDEWINDOW;
                position->flags |= SWP_NOZORDER;
            }
            break;

        case WM_TIMER:
            if (wParam == MINUTERIE_SECONDE) {
                RecreerWidgetsPerdus();
                MettreAJourMesures();
                AvancerMinuteur();
                MesurerTempsEcran();
                AvancerEntretien();
                AvancerDiaporama();
                for (Widget* widget : g_widgets) {
                    if (widget->chaqueSeconde && widget->fenetre &&
                        IsWindowVisible(widget->fenetre)) {
                        AfficherWidget(widget);
                    }
                }
            } else if (wParam == MINUTERIE_CURSEUR) {
                g_curseurVisible = !g_curseurVisible;
                AfficherWidget(&g_recherche);
            }
            return 0;

        case WM_APP_ENTRETIEN:
            FinEntretien(wParam, lParam);
            return 0;

        case WM_APP_INDEX:
            if (g_index) {
                HeapFree(GetProcessHeap(), 0, g_index);
            }
            g_index = (IndexDossiers*)lParam;
            g_indexEnCours = false;
            if (g_barreActive) {
                ConstruireResultats();
                AfficherWidget(&g_recherche);
            }
            return 0;

        case WM_APP_APERCUS: {
            LotApercus* lot = (LotApercus*)lParam;
            if (lot->generation == g_generationApercus) {
                LibererApercus();
                for (int i = 0; i < NB_FAVORIS; i++) {
                    g_favoris[i].pixels = lot->pixels[i];
                    lot->pixels[i] = nullptr;
                }
                AfficherWidget(&g_sites);
            }
            for (BYTE* pixels : lot->pixels) {
                if (pixels) {
                    HeapFree(GetProcessHeap(), 0, pixels);
                }
            }
            HeapFree(GetProcessHeap(), 0, lot);
            return 0;
        }

        case WM_APP_CATALOGUE:
            if (g_catalogue) {
                HeapFree(GetProcessHeap(), 0, g_catalogue);
            }
            g_catalogue = (Catalogue*)lParam;
            g_lectureCatalogueEnCours = false;
            if (g_barreActive) {
                ConstruireResultats();
                AfficherWidget(&g_recherche);
            }
            return 0;

        case WM_APP_REGLAGES:
            LireReglages();
            AppliquerFond();
            DemanderApps();
            DemanderIndex();
            DemanderApercus();
            if (!g_minuteurEnMarche) {
                g_resteMs = DureePhaseMs();  // nouvelle durée prise en compte
            }
            for (Widget* widget : g_widgets) {
                if (widget->fenetre) {
                    PlacerWidget(widget);
                    AfficherWidget(widget);
                }
            }
            return 0;

        case WM_APP_APPS: {
            ListeApps* liste = (ListeApps*)lParam;
            if ((LONG)wParam != g_generationApps) {
                LibererApps(liste);  // une lecture plus récente est en route
                return 0;
            }
            LibererApps(g_apps);
            g_apps = liste;
            g_accueil.survol = AUCUNE_ZONE;
            AfficherWidget(&g_accueil);
            return 0;
        }
    }

    // Survol et clics des widgets qui ont des zones cliquables.
    Widget* widget = WidgetDe(fenetre);
    if (widget && widget->zoneSous) {
        switch (message) {
            case WM_MOUSEMOVE: {
                float echelle = Echelle(fenetre);
                int zone = widget->zoneSous((short)LOWORD(lParam) / echelle,
                                            (short)HIWORD(lParam) / echelle);
                if (!widget->sourisSuivie) {
                    // Demande un WM_MOUSELEAVE quand la souris sortira.
                    TRACKMOUSEEVENT suivi = {sizeof(suivi), TME_LEAVE, fenetre,
                                             0};
                    widget->sourisSuivie = TrackMouseEvent(&suivi);
                }
                if (zone != widget->survol) {
                    widget->survol = zone;
                    AfficherWidget(widget);
                }
                return 0;
            }

            case WM_MOUSELEAVE:
                widget->sourisSuivie = false;
                if (widget->survol != AUCUNE_ZONE) {
                    widget->survol = AUCUNE_ZONE;
                    AfficherWidget(widget);
                }
                return 0;

            case WM_SETCURSOR:
                if (LOWORD(lParam) == HTCLIENT &&
                    widget->survol != AUCUNE_ZONE) {
                    SetCursor(LoadCursor(nullptr, IDC_HAND));
                    return TRUE;
                }
                break;

            case WM_LBUTTONUP: {
                float echelle = Echelle(fenetre);
                int zone = widget->zoneSous((short)LOWORD(lParam) / echelle,
                                            (short)HIWORD(lParam) / echelle);
                if (zone != AUCUNE_ZONE) {
                    widget->cliquer(zone);
                }
                return 0;
            }
        }
    }

    return DefWindowProc(fenetre, message, wParam, lParam);
}

// Met la fenêtre du widget à sa place et à sa taille (en pixels réels).
void PlacerWidget(Widget* widget) {
    // Les widgets sont sur l'écran principal : son échelle est la leur.
    float echelle = Echelle(widget->fenetre);
    int largeur = Arrondir(widget->largeur * echelle);
    int hauteur = Arrondir(widget->hauteur * echelle);
    int largeurEcran = GetSystemMetrics(SM_CXSCREEN);
    int x = Arrondir(widget->x * echelle);
    if (widget->centre) {
        x = (largeurEcran - largeur) / 2;
    } else if (widget->depuisLaDroite) {
        x = largeurEcran - x - largeur;
    }
    SetWindowPos(widget->fenetre, nullptr, x, Arrondir(widget->y * echelle),
                 largeur, hauteur, SWP_NOZORDER | SWP_NOACTIVATE);
}

// Crée la fenêtre d'un widget, cachée, à sa place et à sa taille. Si un
// widget échoue, les autres continuent sans lui.
void CreerWidget(Widget* widget) {
    // WS_EX_LAYERED : transparence pixel par pixel.
    // WS_EX_TOOLWINDOW : pas de bouton dans la barre des tâches ni dans Alt+Tab.
    // WS_EX_NOACTIVATE : ne vole jamais le focus.
    // WS_POPUP : pas de bordure ni de barre de titre.
    // (Ces styles empêchent Windows d'attacher la fenêtre à un bureau
    // virtuel : c'est AppliquerBureau qui montre et cache les groupes.)
    //
    // La fenêtre est « possédée » par le bureau (Progman) : Windows la garde
    // toujours au-dessus de lui. Quand « Afficher le bureau » fait monter le
    // bureau par-dessus les apps, le widget monte avec lui (mesuré : une
    // fenêtre non possédée reste dessous, donc invisible).
    HWND bureau = FindWindow(L"Progman", nullptr);
    DWORD style = WS_EX_LAYERED | WS_EX_TOOLWINDOW;
    if (widget != &g_recherche && widget != &g_sites) {
        // Seuls la barre de recherche et le bouton Sites prennent le clavier.
        style |= WS_EX_NOACTIVATE;
    }
    widget->fenetre =
        CreateWindowEx(style, NOM_CLASSE, L"Verre Desktop", WS_POPUP, 0, 0, 1,
                       1, bureau, nullptr, g_module, nullptr);
    if (!widget->fenetre) {
        Wh_Log(L"CreateWindowEx a échoué (%u)", GetLastError());
        return;
    }

    PlacerWidget(widget);
}

// Recrée les widgets détruits avec le bureau, dès que le bureau est revenu.
// Appelée chaque seconde par la minuterie : ne fait rien si tout va bien.
void RecreerWidgetsPerdus() {
    bool manque = false;
    for (Widget* widget : g_widgets) {
        if (!widget->fenetre) {
            manque = true;
        }
    }
    if (!manque || !FindWindow(L"Progman", nullptr)) {
        return;
    }
    for (Widget* widget : g_widgets) {
        if (!widget->fenetre) {
            CreerWidget(widget);
        }
    }
    AppliquerBureau(true);
}

bool DemarrerWidgets() {
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                 __uuidof(ID2D1Factory), nullptr,
                                 (void**)&g_fabriqueD2D)) ||
        FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                                   __uuidof(IDWriteFactory),
                                   (IUnknown**)&g_fabriqueTexte))) {
        Wh_Log(L"Direct2D ou DirectWrite indisponible");
        return false;
    }

    // Traits arrondis, et pointillés (4 px de trait, 3 px de vide) pour la
    // tuile « Ajouter ». Si la création échoue, on dessine en traits pleins.
    D2D1_STROKE_STYLE_PROPERTIES rond = D2D1::StrokeStyleProperties(
        D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
        D2D1_LINE_JOIN_ROUND);
    if (FAILED(g_fabriqueD2D->CreateStrokeStyle(&rond, nullptr, 0,
                                                &g_traitRond))) {
        g_traitRond = nullptr;
    }
    const FLOAT POINTILLES[] = {4.0f, 3.0f};
    D2D1_STROKE_STYLE_PROPERTIES pointille = D2D1::StrokeStyleProperties(
        D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT,
        D2D1_LINE_JOIN_ROUND, 10.0f, D2D1_DASH_STYLE_CUSTOM);
    if (FAILED(g_fabriqueD2D->CreateStrokeStyle(&pointille, POINTILLES, 2,
                                                &g_traitPointille))) {
        g_traitPointille = nullptr;
    }

    WNDCLASSEX classe = {};
    classe.cbSize = sizeof(classe);
    classe.lpfnWndProc = ProcedureWidget;
    classe.hInstance = g_module;
    classe.hCursor = LoadCursor(nullptr, IDC_ARROW);
    classe.lpszClassName = NOM_CLASSE;
    if (!RegisterClassEx(&classe)) {
        Wh_Log(L"RegisterClassEx a échoué (%u)", GetLastError());
        return false;
    }

    LireReglages();
    DemarrerMesures();
    MettreAJourMesures();

    // Le contrôleur : fenêtre « message seulement », jamais affichée.
    g_controleur = CreateWindowEx(0, NOM_CLASSE, L"Verre Desktop", 0, 0, 0, 0,
                                  0, HWND_MESSAGE, nullptr, g_module, nullptr);
    if (!g_controleur) {
        Wh_Log(L"Contrôleur impossible à créer (%u)", GetLastError());
        return false;
    }
    if (!SetTimer(g_controleur, MINUTERIE_SECONDE, 1000, nullptr)) {
        Wh_Log(L"Pas de minuterie : affichage figé");
    }

    for (Widget* widget : g_widgets) {
        CreerWidget(widget);
    }

    // Suivi des bureaux virtuels. Sans lui, le groupe du bureau 1 reste
    // affiché partout (dégradation propre).
    if (RegOpenKeyEx(HKEY_CURRENT_USER, CLE_BUREAUX, 0,
                     KEY_QUERY_VALUE | KEY_NOTIFY,
                     &g_cleBureaux) != ERROR_SUCCESS) {
        g_cleBureaux = nullptr;
        Wh_Log(L"Clé des bureaux virtuels introuvable");
    }
    g_changementBureaux = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    SurveillerBureaux();

    LireSessions();
    RemettreMinuteur();
    LireTempsEcran();

    DemanderApps();
    DemanderCatalogue();
    DemanderIndex();
    DemanderApercus();
    AppliquerBureau(true);
    AppliquerFond();
    return true;
}

void ArreterWidgets() {
    g_changementVoulu = true;  // nos propres destructions sont voulues
    if (g_controleur) {
        DestroyWindow(g_controleur);  // détruit aussi la minuterie
        g_controleur = nullptr;
    }
    for (Widget* widget : g_widgets) {
        Liberer(widget->cible);
        if (widget->fenetre) {
            DestroyWindow(widget->fenetre);
            widget->fenetre = nullptr;
        }
    }
    UnregisterClass(NOM_CLASSE, g_module);
    LibererApps(g_apps);
    g_apps = nullptr;
    if (g_catalogue) {
        HeapFree(GetProcessHeap(), 0, g_catalogue);
        g_catalogue = nullptr;
    }
    if (g_index) {
        HeapFree(GetProcessHeap(), 0, g_index);
        g_index = nullptr;
    }
    LibererApercus();
    SauverTempsEcran();
    if (g_changementBureaux) {
        CloseHandle(g_changementBureaux);
        g_changementBureaux = nullptr;
    }
    if (g_cleBureaux) {
        RegCloseKey(g_cleBureaux);
        g_cleBureaux = nullptr;
    }
    ArreterMesures();
    Liberer(g_traitPointille);
    Liberer(g_traitRond);
    Liberer(g_fabriqueTexte);
    Liberer(g_fabriqueD2D);
}

// ---------------------------------------------------------------------------
// Thread des fenêtres
// ---------------------------------------------------------------------------

// Tout ce qui touche aux fenêtres vit ici : création, dessin, boucle de
// messages, puis destruction. Une fenêtre doit être détruite par le thread
// qui l'a créée.
DWORD WINAPI FilDesFenetres(LPVOID) {
    // Crée la file de messages tout de suite : le WM_QUIT envoyé par
    // WhTool_ModUninit ne peut donc pas se perdre.
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    // Ce thread travaille en vrais pixels, écran par écran : texte et coins
    // nets à 150 %.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    g_demarrageReussi = DemarrerWidgets();
    SetEvent(g_filPret);

    // La boucle attend deux choses à la fois : les messages des fenêtres, et
    // le signal « la clé des bureaux virtuels a changé ».
    bool continuer = g_demarrageReussi;
    while (continuer) {
        DWORD nombre = g_changementBureaux ? 1 : 0;
        DWORD reveil = MsgWaitForMultipleObjects(
            nombre, &g_changementBureaux, FALSE, INFINITE, QS_ALLINPUT);
        if (nombre && reveil == WAIT_OBJECT_0) {
            SurveillerBureaux();
            AppliquerBureau(false);
            // Fond fixe : posé sur ce bureau s'il n'y est pas encore (une
            // seule fois par bureau). Avec le diaporama, chaque bureau garde
            // son propre fond : rien ne change en arrivant, donc pas de
            // fond qui saute juste après le changement de bureau.
            if (g_minutesDiaporama == 0) {
                AppliquerFond();
            }
            continue;
        }
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                continuer = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    ArreterWidgets();
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    return 0;
}

// Demande au thread de s'arrêter et l'attend (5 s maximum, pour ne jamais
// bloquer Windhawk).
void ArreterFil() {
    if (g_fil) {
        PostThreadMessage(g_idFil, WM_QUIT, 0, 0);
        if (WaitForSingleObject(g_fil, 5000) != WAIT_OBJECT_0) {
            Wh_Log(L"Le thread des fenêtres ne s'est pas arrêté à temps");
        }
        CloseHandle(g_fil);
        g_fil = nullptr;
    }
    if (g_filPret) {
        CloseHandle(g_filPret);
        g_filPret = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Points d'entrée du mod outil (appelés par le code officiel en bas du fichier)
// ---------------------------------------------------------------------------

BOOL WhTool_ModInit() {
    Wh_Log(L">");

    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCWSTR)&ProcedureWidget, &g_module)) {
        return FALSE;
    }

    g_filPret = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_filPret) {
        return FALSE;
    }

    g_fil = CreateThread(nullptr, 0, FilDesFenetres, nullptr, 0, &g_idFil);
    if (!g_fil) {
        ArreterFil();
        return FALSE;
    }

    // On attend que les widgets soient prêts, ou que le thread s'arrête sur
    // une erreur. 10 s au maximum.
    HANDLE attente[2] = {g_filPret, g_fil};
    DWORD resultat = WaitForMultipleObjects(2, attente, FALSE, 10000);
    if (resultat != WAIT_OBJECT_0 || !g_demarrageReussi) {
        Wh_Log(L"Les widgets n'ont pas pu démarrer");
        ArreterFil();
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    // Appelé par Windhawk sur un autre thread : on prévient le thread des
    // fenêtres, qui relira les réglages lui-même.
    Wh_Log(L">");
    if (g_controleur) {
        PostMessage(g_controleur, WM_APP_REGLAGES, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L">");
    ArreterFil();
}

////////////////////////////////////////////////////////////////////////////////
// Code officiel recopié tel quel depuis le wiki de Windhawk (version 1.7.3) :
// il fait tourner le mod dans son propre processus windhawk.exe.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
