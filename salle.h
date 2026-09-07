#ifndef SALLE_H
#define SALLE_H

#include <QString>
#include <QList>

class Salle {
public:
    Salle() = default;
    Salle(int id, const QString& nom, int capacite, const QString& type, const QString& statut)
        : m_id(id), m_nom(nom), m_capacite(capacite), m_type(type), m_statut(statut) {}

    
    int id() const { return m_id; }
    QString nom() const { return m_nom; }
    int capacite() const { return m_capacite; }
    QString type() const { return m_type; }
    QString statut() const { return m_statut; }

    
    void setId(int id) { m_id = id; }
    void setNom(const QString& nom) { m_nom = nom; }
    void setCapacite(int capacite) { m_capacite = capacite; }
    void setType(const QString& type) { m_type = type; }
    void setStatut(const QString& statut) { m_statut = statut; }

    
    bool ajouter();
    bool modifier();
    bool supprimer();
    static QList<Salle> listerTout();
    static Salle trouverParId(int id);
    static bool mettreAJourSignalement(int id, const QString &statut,
                                       const QString &description,
                                       const QString &auteur);
    static bool effacerSignalement(int id);

private:
    int m_id = -1;
    QString m_nom;
    int m_capacite = 0;
    QString m_type;
    QString m_statut = "DISPONIBLE";
};

#endif 