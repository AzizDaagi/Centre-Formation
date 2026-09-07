#ifndef FORMATEUR_H
#define FORMATEUR_H

#include <QString>
#include <QList>

class Formateur
{
public:
    Formateur() = default;

    Formateur(int id,
              const QString& nom,
              const QString& prenom,
              const QString& email,
              const QString& passwordHash,
              const QString& role,
              const QString& statutCompte)
        : m_id(id),
        m_nom(nom),
        m_prenom(prenom),
        m_email(email),
        m_passwordHash(passwordHash),
        m_role(role),
        m_statutCompte(statutCompte)
    {}

    
    int id() const { return m_id; }
    QString nom() const { return m_nom; }
    QString prenom() const { return m_prenom; }
    QString email() const { return m_email; }
    QString passwordHash() const { return m_passwordHash; }
    QString role() const { return m_role; }
    QString statutCompte() const { return m_statutCompte; }

    
    void setId(int id) { m_id = id; }
    void setNom(const QString& nom) { m_nom = nom; }
    void setPrenom(const QString& prenom) { m_prenom = prenom; }
    void setEmail(const QString& email) { m_email = email; }
    void setPasswordHash(const QString& passwordHash) { m_passwordHash = passwordHash; }
    void setRole(const QString& role) { m_role = role; }
    void setStatutCompte(const QString& statut) { m_statutCompte = statut; }

    
    bool ajouter();
    bool modifier();
    bool supprimer();

    static QList<Formateur> listerTout();
    static Formateur trouverParId(int id);

private:
    int m_id = -1;
    QString m_nom;
    QString m_prenom;
    QString m_email;
    QString m_passwordHash;
    QString m_role = "FORMATEUR";
    QString m_statutCompte = "ACTIF";
};

#endif 