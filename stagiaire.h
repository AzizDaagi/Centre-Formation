#ifndef STAGIAIRE_H
#define STAGIAIRE_H

#include <QString>
#include <QDate>
#include <QList>

class Stagiaire
{
public:
    Stagiaire() = default;

    Stagiaire(int id,
              const QString& nom,
              const QString& prenom,
              const QString& email,
              const QString& passwordHash,
              int idFormateur,
              int idCours,
              int idSalleAttitree,
              const QDate& dateDebut,
              const QDate& dateFinPrevue,
              double heuresValidees,
              const QString& statut)
        : m_id(id),
        m_nom(nom),
        m_prenom(prenom),
        m_email(email),
        m_passwordHash(passwordHash),
        m_idFormateur(idFormateur),
        m_idCours(idCours),
        m_idSalleAttitree(idSalleAttitree),
        m_dateDebut(dateDebut),
        m_dateFinPrevue(dateFinPrevue),
        m_heuresValidees(heuresValidees),
        m_statut(statut)
    {}

    
    int id() const { return m_id; }
    QString nom() const { return m_nom; }
    QString prenom() const { return m_prenom; }
    QString email() const { return m_email; }
    QString passwordHash() const { return m_passwordHash; }
    int idFormateur() const { return m_idFormateur; }
    int idCours() const { return m_idCours; }
    int idSalleAttitree() const { return m_idSalleAttitree; }
    QDate dateDebut() const { return m_dateDebut; }
    QDate dateFinPrevue() const { return m_dateFinPrevue; }
    double heuresValidees() const { return m_heuresValidees; }
    QString statut() const { return m_statut; }

    
    void setId(int id) { m_id = id; }
    void setNom(const QString& nom) { m_nom = nom; }
    void setPrenom(const QString& prenom) { m_prenom = prenom; }
    void setEmail(const QString& email) { m_email = email; }
    void setPasswordHash(const QString& passwordHash) { m_passwordHash = passwordHash; }
    void setIdFormateur(int id) { m_idFormateur = id; }
    void setIdCours(int id) { m_idCours = id; }
    void setIdSalleAttitree(int id) { m_idSalleAttitree = id; }
    void setDateDebut(const QDate& date) { m_dateDebut = date; }
    void setDateFinPrevue(const QDate& date) { m_dateFinPrevue = date; }
    void setHeuresValidees(double heures) { m_heuresValidees = heures; }
    void setStatut(const QString& statut) { m_statut = statut; }

    
    bool ajouter();
    bool modifier();
    bool supprimer();

    static QList<Stagiaire> listerTout();
    static Stagiaire trouverParId(int id);
    static bool ajouterHeures(int id, double heures);
    static bool mettreAJourStatut(int id, const QString &statut);

private:
    int m_id = -1;
    QString m_nom;
    QString m_prenom;
    QString m_email;
    QString m_passwordHash;

    int m_idFormateur = -1;
    int m_idCours = -1;
    int m_idSalleAttitree = -1;

    QDate m_dateDebut;
    QDate m_dateFinPrevue;

    double m_heuresValidees = 0;
    QString m_statut = "ACTIF";
};

#endif 