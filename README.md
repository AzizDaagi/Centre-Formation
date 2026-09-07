# Centre de Formation

Application de bureau réalisée avec Qt et C++ pour gérer un centre de formation.

## Fonctionnalités

- Gestion des administrateurs, formateurs, cours, salles et stagiaires
- Suivi de la progression et des présences
- Réservations de salles
- Signalement et résolution d'incidents
- Export de documents PDF
- Analyse locale de la progression avec Ollama

## Technologies

- C++17 et Qt 6
- Oracle Database
- CMake
- Ollama pour la fonctionnalité d'intelligence artificielle

## Compilation

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

La base de données et les paramètres de connexion sont configurés dans `settings.ini`.

Pour utiliser l'analyse IA, lancer Ollama avec le modèle configuré :

```powershell
ollama run qwen2.5:3b
```
