# Guide : mettre le mod sur GitHub

Tutoriel pas-à-pas pour publier le mod « Ressource Redirect (One UI 8.5) »
sur GitHub. Deux méthodes au choix : **l'interface web** (la plus simple,
aucune installation) ou **Git en ligne de commande** (la plus pro).

Le projet contient déjà tout ce qu'il faut pour un dépôt propre :
- `OneUI85-RessourceRedirect.wh.cpp` → le code du mod
- `README.md` → la description (GitHub l'affiche automatiquement sur la
  page du dépôt)
- `LICENSE` → la licence MIT
- `OneUI85Theme/LISEZMOI.txt` → le dossier-thème modèle

---

## Étape 0 — Créer un compte GitHub (si besoin)

1. Va sur <https://github.com/signup>
2. Choisis un nom d'utilisateur (ex. `ton-pseudo`), une adresse e-mail et un
   mot de passe.
3. Vérifie ton e-mail (lien envoyé par GitHub).

---

## Méthode A — Interface web (recommandée pour commencer)

### A1. Créer le dépôt (repository)

1. Connecte-toi sur <https://github.com>.
2. Clique sur le bouton **+** en haut à droite → **New repository**.
3. Remplis :
   - **Repository name** : `windhawk-resource-redirect-oneui85`
     (pas d'espaces, uniquement lettres/chiffres/tirets).
   - **Description** (facultatif) : « Mod Windhawk : redirige les ressources
     Windows avec un mode One UI 8.5 intégré. »
   - **Public** ou **Private** (si Private, seul toi et les gens à qui tu
     donnes accès verront le code).
   - ⚠️ Ne coche **rien** (ni « Add a README », ni « .gitignore », ni
     « license ») — on va tout importer depuis les fichiers existants.
4. Clique sur **Create repository**.

### A2. Envoyer les fichiers

1. Sur la page vide de ton nouveau dépôt, clique sur le lien
   **« uploading an existing file »**.
2. Décompresse `OneUI85-RessourceRedirect.zip` sur ton PC, puis **glisse-
   dépose** dans la page web :
   - `OneUI85-RessourceRedirect.wh.cpp`
   - `README.md`
   - `LICENSE`
   - le dossier `OneUI85Theme/` avec son `LISEZMOI.txt` (GitHub garde la
     structure des dossiers).
3. En bas, dans « Commit changes » : message conseillé
   `Initial commit : mod Ressource Redirect mode One UI 8.5` puis clique sur
   **Commit changes**.
4. C'est publié ! Ta page GitHub affiche automatiquement le README. 🎉

---

## Méthode B — En ligne de commande (Git)

### B1. Installer Git

- Télécharge et installe depuis <https://git-scm.com/download/win>
  (installation par défaut, « Next » partout).
- Ouvre « Git Bash » (menu Démarrer) après l'installation.

### B2. Créer le dépôt vide sur GitHub

1. Connecte-toi sur <https://github.com>, bouton **+** → **New repository**.
2. Nom : `windhawk-resource-redirect-oneui85`, Public ou Private, **ne coche
   rien**, puis **Create repository**.
3. Garde la page ouverte : GitHub affiche les commandes à taper (celles de la
   section « …or push an existing repository from the command line »).

### B3. Envoyer le projet

Dans Git Bash, tape (remplace `TON-PSEUDO` par ton pseudo) :

```bash
# 1. Se placer dans le dossier du projet
cd /c/Users/TON-COMPTE/Emplacement/du/dossier/OneUI85-RessourceRedirect

# 2. Initialiser le dépôt Git local
git init

# 3. Préparer tous les fichiers
git add .

# 4. Premier commit
git commit -m "Initial commit : mod Ressource Redirect mode One UI 8.5"

# 5. Renommer la branche principale en « main »
git branch -M main

# 6. Relier au dépôt distant (l'URL exacte est donnée par GitHub)
git remote add origin https://github.com/TON-PSEUDO/windhawk-resource-redirect-oneui85.git

# 7. Envoyer
git push -u origin main
```

> La première fois, Git te demandera de t'identifier :
> ```bash
> git config --global user.name "Ton pseudo"
> git config --global user.email "ton@email.com"
> ```
> Et de te connecter (fenêtre de connexion GitHub, ou jeton personnel —
> GitHub t'accompagne dans la procédure).

### B4. Après chaque modification (mise à jour)

```bash
git add .
git commit -m "Description de la modification"
git push
```

---

## Bonnes pratiques

- **Bump de version** : quand tu modifies le mod, pense à augmenter
  `@version 1.0.0` → `1.0.1` dans l'en-tête du `.wh.cpp` et à ajouter une
  section « Journal des versions » dans le README. Les utilisateurs de
  Windhawk pourront suivre les mises à jour.
- **Screenshot** : ajoute une image (ex. `preview.png`) dans le dépôt et
  affiche-la dans le README avec
  `![Aperçu](preview.png)` — un dépôt avec image attire plus de monde.
- **Répondre aux issues** : si quelqu'un ouvre un problème (« bug »), tu peux
  y répondre directement sur GitHub.
- **Publier aussi sur Windhawk** (optionnel, demande une relecture) :
  <https://windhawk.net/mods/publish> — le mod est alors installable en un
  clic par toute la communauté Windhawk, en plus de GitHub.

---

## Questions fréquentes

**Je me suis trompé dans le nom du dépôt ?**
→ GitHub → le dépôt → **Settings** → section « General » → champ
« Repository name » → **Rename**.

**Je veux passer un dépôt privé en public (ou l'inverse) ?**
→ Settings → « Danger Zone » → « Change repository visibility ».

**J'ai supprimé des fichiers par erreur ?**
→ Tant que c'est poussé sur GitHub, rien n'est perdu : l'historique Git garde
tout (onglet « Commits » → ancien commit → « Browse files »).

**Dois-je ajouter un `.gitignore` ?**
→ Pas nécessaire pour ce projet (fichier texte + thème, rien à exclure).
