# Ressource Redirect – mode One UI 8.5 (mod Windhawk)

Mod Windhawk qui **redirige le chargement des ressources Windows** (icônes,
curseurs, bitmaps, chaînes, menus, boîtes de dialogue, images GDI+) vers des
**fichiers alternatifs**, sans jamais modifier les fichiers système. Il inclut
un **mode « One UI 8.5 » intégré** pour donner à Windows un look façon Samsung
Galaxy.

> ⚠️ **Plateforme** : Windhawk ne fonctionne que sur **Windows** (10/11, PC).
> One UI 8.5 est l'interface des téléphones Samsung (Android) — ici on ne
> « tourne » pas sous One UI, on **thème Windows pour qu'il y ressemble**,
> en toute sécurité.

---

## Ce que fait le mod

Il intercepte les appels système suivants et les redirige si un fichier
alternatif existe :

| Type de ressource | Fonctions interceptées |
|---|---|
| Icônes de fichiers / EXE / DLL | `PrivateExtractIconsW` |
| Icônes, curseurs, bitmaps de modules | `LoadImageW/A`, `LoadIconW/A`, `LoadCursorW/A`, `LoadBitmapW/A` |
| Menus | `LoadMenuW/A` |
| Chaînes (textes des programmes) | `LoadStringW/A` |
| Boîtes de dialogue | `DialogBoxParamW/A`, `CreateDialogParamW/A` |
| Images PNG/JPEG intégrées aux ressources | `SHCreateStreamOnModuleResourceW` |

**Aucun fichier système n'est modifié.** Désactiver le mod (ou quitter
Windhawk) restaure instantanément l'état d'origine.

---

## Installation — aucune compilation nécessaire ✅

Windhawk compile lui-même les mods au moment de l'installation, tu n'as besoin
ni de Visual Studio, ni d'aucun outil de développement.

1. Installe **Windhawk** depuis <https://windhawk.net/>.
2. Dans Windhawk, ouvre l'onglet **Mods** (en haut à droite).
3. Clique sur **Installer un mod depuis un fichier** (« Install from file ») —
   en général via le menu ⋮ ou le bouton « Install mod from file ».
4. Sélectionne le fichier **`OneUI85-RessourceRedirect.wh.cpp`**.
5. Windhawk affiche un aperçu du code source (transparence totale) puis le
   compile et l'installe. Active le mod.

Le mod s'applique à tous les processus Windows (Explorer, programmes…).

---

## Mode One UI 8.5 — mise en route

Par défaut, le mod cherche le thème dans :

```
%LOCALAPPDATA%\Windhawk\OneUI85Theme
```

(Exemple : `C:\Users\TonNom\AppData\Local\Windhawk\OneUI85Theme`)

### Étape 1 — Récupérer un thème One UI

Le pack d'icônes **« BANAANA OneUI »** (thème façon One UI pour ce type de
mod) est disponible ici :

- Voir la page : <https://github.com/niivu/resource-redirect-icon-themes#themes>
- Téléchargement direct :
  <https://github.com/niivu/resource-redirect-icon-themes/raw/main/Resource%20Redirect%20themes/BANAANA%20OneUI.zip>

### Étape 2 — Installer le thème

1. Décompresse le ZIP.
2. Copie **le contenu** du dossier extrait (le fichier `theme.ini` s'il
   existe, les fichiers `.dll` / `.exe` / `.ico`...) directement dans
   `%LOCALAPPDATA%\Windhawk\OneUI85Theme\`.
   > Si tu préfères un autre emplacement, modifie le réglage `themePath`.
3. Dans Windhawk, sur le mod, clique sur **Redémarrer Explorer**
   (ou redémarre le PC), puis **vide le cache d'icônes** :
   - script fourni par la communauté :
     <https://github.com/niivu/resource-redirect-icon-themes/raw/main/Scripts/Clear_icon_cache.bat>
   - ou : `ie4uinit.exe -show` dans une invite de commandes, puis redémarre
     Explorer.

C'est tout. Les icônes du système (Explorer, barre des tâches, dialogues…)
sont désormais chargées depuis le thème, en mémoire uniquement.

### Comment le mode One UI 8.5 trouve les fichiers

Le mode « miroir » détecte automatiquement les fichiers du thème :

- en miroir du dossier Windows : `imageres.dll` dans le dossier système sera
  cherché dans `themePath\System32\imageres.dll` ;
- ou à la racine du dossier du thème : `themePath\imageres.dll` ;
- et/ou via un fichier `theme.ini` avec une section `[redirections]`
  (format compatible avec les packs existants du dépôt
  resource-redirect-icon-themes).

---

## Réglages

| Réglage | Type | Description |
|---|---|---|
| `oneUi85Mode` | case à cocher (activé par défaut) | Utilise le dossier One UI 8.5 par défaut quand `themePath` est vide. |
| `themePath` | texte | Chemin du dossier de thème. Vide = dossier par défaut (`%LOCALAPPDATA%\Windhawk\OneUI85Theme`). |
| `customRules` | texte (multi-lignes) | Règles supplémentaires, une par ligne : `fichier_original=fichier_alternatif`. |

### Syntaxe des règles personnalisées

```
# commentaires ignorés
C:\Windows\System32\imageres.dll=C:\MesThemes\OneUI\imageres.dll
*\shell32.dll=shell32.dll              # relatif au dossier du thème
*\imageres.dll=%LOCALAPPDATA%\My\imageres.dll
```

- Variables d'environnement (`%SystemRoot%`, `%LOCALAPPDATA%`…) autorisées
  des deux côtés du `=`.
- Jokers `*` et `?` autorisés dans le fichier d'origine
  (ex. `*\imageres.dll`).
- Le fichier alternatif peut être un chemin absolu ou relatif au dossier du
  thème.
- Après modification des réglages, redémarre Explorer pour tout appliquer.

---

## Dépannage

- **Rien ne change** → vide le cache d'icônes et redémarre Explorer
  (voir étape 2). Windows met en cache les icônes agressivement.
- **Vérifier que le mod fonctionne** → Windhawk → **Observateur d'événements**
  (Event Viewer) : le mod écrit des journaux (« Réglages chargés … », nombre de
  règles…).
- **Le thème n'apparaît pas pour les dossiers avec aperçus** → dans les
  options d'Explorer, décoche « Afficher les miniatures » ou accepte les icônes
  génériques des dossiers. (Le mod n'intercepte pas les vignettes de dossiers —
  limitation du concept, cf. ci-dessous.)
- **Fichiers ajoutés/modifiés dans le thème sans effet** → le mod met en cache
  les chemins ; recharge les réglages du mod (ou redémarre Explorer).

---

## Limites connues

- Les vignettes de dossiers (aperçus du contenu) ne sont pas redirigées.
- Les curseurs animés (`.ani`) et `LoadCursorFromFile` ne sont pas pris en
  charge.
- `SetXMLFromResource` (ressources DirectUI, export ordinal de `uxtheme.dll`)
  n'est pas hooké — certains éléments XAML modernes ne sont pas couverts.
- L'extraction multi-icônes via `PrivateExtractIconsW` fonctionne au mieux
  quand les fichiers d'origine et de thème partagent les mêmes identifiants de
  groupes d'icônes (cas standard des packs de thèmes).
- En cas de redirection, le mod ne couvre pas les ressources `.mui`/`.mun`
  multilingues.

---

## Licence & crédits

- **Code de ce mod** : réimplémentation originale, publiée sous **licence MIT**
  (voir `LICENSE`). Il ne copie pas le code du mod d'origine.
- **Concept** : inspiré du mod « Resource Redirect » de **m417z** (Ramen
  Software), publié sous GPLv3 : <https://windhawk.net/mods/icon-resource-redirect>
- **Thèmes d'icônes** : fournis par la communauté (niivu et contributeurs) —
  respecte leurs licences et crédits :
  <https://github.com/niivu/resource-redirect-icon-themes>
- **Windhawk** : outil de m417z — <https://windhawk.net/>

## Désinstallation

Désactiver le mod dans Windhawk suffit : plus aucune ressource n'est
redirigée, le système revient à l'état d'origine sans aucune modification
résiduelle.
