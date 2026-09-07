MERGE INTO FORMATEUR target
USING (SELECT 'owner@centrepro.tn' email, 'Daoud' nom, 'Amira' prenom, 'SUPER_ADMIN' role FROM dual UNION ALL
       SELECT 'admin.operations@centrepro.tn', 'Ben Salem', 'Karim', 'ADMIN' FROM dual UNION ALL
       SELECT 'sarah.mansour@centrepro.tn', 'Mansour', 'Sarah', 'FORMATEUR' FROM dual UNION ALL
       SELECT 'youssef.trabelsi@centrepro.tn', 'Trabelsi', 'Youssef', 'FORMATEUR' FROM dual UNION ALL
       SELECT 'lina.khelifi@centrepro.tn', 'Khelifi', 'Lina', 'FORMATEUR' FROM dual) source
ON (LOWER(target.EMAIL) = LOWER(source.email))
WHEN NOT MATCHED THEN INSERT (NOM, PRENOM, EMAIL, PASSWORD_HASH, ROLE, STATUT_COMPTE)
VALUES (source.nom, source.prenom, source.email, LOWER(STANDARD_HASH('password123', 'SHA256')), source.role, 'ACTIF');

MERGE INTO SALLE target
USING (SELECT 'Labo Digital B2' nom, 28 capacite, 'POSTE_TRAVAIL' type, 'DISPONIBLE' statut FROM dual UNION ALL
       SELECT 'Studio Creatif C1', 18, 'SALLE_REUNION', 'DISPONIBLE' FROM dual UNION ALL
       SELECT 'Amphi Innovation', 80, 'SALLE_SOUTENANCE', 'DISPONIBLE' FROM dual) source
ON (REPLACE(REPLACE(target.NOM_SALLE, 'é', 'e'), 'É', 'E') = source.nom)
WHEN NOT MATCHED THEN INSERT (NOM_SALLE, CAPACITE, TYPE_SALLE, STATUT)
VALUES (source.nom, source.capacite, source.type, source.statut);

MERGE INTO COURS target
USING (SELECT 'UX Design professionnel' titre, 'Methodes de conception centrees utilisateur', 42 heures, 'sarah.mansour@centrepro.tn' email FROM dual UNION ALL
       SELECT 'Developpement C++ avance', 'Architecture, Qt et persistance Oracle', 56, 'youssef.trabelsi@centrepro.tn' FROM dual UNION ALL
       SELECT 'Communication digitale', 'Presentation, collaboration et portfolio', 30, 'lina.khelifi@centrepro.tn' FROM dual) source
ON (REPLACE(REPLACE(target.TITRE, 'é', 'e'), 'É', 'E') = source.titre)
WHEN NOT MATCHED THEN INSERT (TITRE, DESCRIPTION, HEURES_REQUISES, ID_FORMATEUR_RESP)
VALUES (source.titre, source.titre, source.heures, (SELECT ID_FORMATEUR FROM FORMATEUR WHERE EMAIL = source.email));

MERGE INTO STAGIAIRE target
USING (SELECT 'amira.haddad@centrepro.tn' email, 'Haddad' nom, 'Amira' prenom, 'UX Design professionnel' cours, 12 heures FROM dual UNION ALL
       SELECT 'adam.benali@centrepro.tn', 'Ben Ali', 'Adam', 'Developpement C++ avance', 24 FROM dual UNION ALL
       SELECT 'maya.jaziri@centrepro.tn', 'Jaziri', 'Maya', 'Communication digitale', 9 FROM dual UNION ALL
       SELECT 'ilyes.ghannem@centrepro.tn', 'Ghannem', 'Ilyes', 'Developpement C++ avance', 18 FROM dual UNION ALL
       SELECT 'nour.chebbi@centrepro.tn', 'Chebbi', 'Nour', 'UX Design professionnel', 15 FROM dual) source
ON (LOWER(target.EMAIL) = LOWER(source.email))
WHEN NOT MATCHED THEN INSERT (NOM, PRENOM, EMAIL, PASSWORD_HASH, ID_FORMATEUR, ID_COURS, ID_SALLE_ATTITREE, DATE_DEBUT, DATE_FIN_PREVUE, HEURES_VALIDEES, STATUT)
VALUES (source.nom, source.prenom, source.email, LOWER(STANDARD_HASH('password123', 'SHA256')),
        (SELECT ID_FORMATEUR_RESP FROM COURS WHERE TITRE = source.cours),
        (SELECT ID_COURS FROM COURS WHERE TITRE = source.cours),
        (SELECT MIN(ID_SALLE) FROM SALLE WHERE STATUT = 'DISPONIBLE'),
        TRUNC(SYSDATE)-20, TRUNC(SYSDATE)+70, source.heures, 'ACTIF');

COMMIT;
