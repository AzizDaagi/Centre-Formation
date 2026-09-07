#ifndef COURS_H
#define COURS_H

#include <QString>
#include <QList>

class Cours
{
public:
    Cours() = default;

    Cours(int id,
          const QString& titre,
          const QString& description,
          int heuresRequises,
          int idFormateurResp)
        : m_id(id),
        m_titre(titre),
        m_description(description),
        m_heuresRequises(heuresRequises),
        m_idFormateurResp(idFormateurResp)
    {}

    // Getters
    int id() const { return m_id; }
    QString titre() const { return m_titre; }
    QString description() const { return m_description; }
    int heuresRequises() const { return m_heuresRequises; }
    int idFormateurResp() const { return m_idFormateurResp; }

    // Setters
    void setId(int id) { m_id = id; }
    void setTitre(const QString& titre) { m_titre = titre; }
    void setDescription(const QString& description) { m_description = description; }
    void setHeuresRequises(int heures) { m_heuresRequises = heures; }
    void setIdFormateurResp(int id) { m_idFormateurResp = id; }

    // Database operations
    bool ajouter();
    bool modifier();
    bool supprimer();

    static QList<Cours> listerTout();
    static Cours trouverParId(int id);
    static bool mettreAJourSignalement(int id, const QString &statut,
                                       const QString &description,
                                       const QString &auteur);
    static bool effacerSignalement(int id);

private:
    int m_id = -1;
    QString m_titre;
    QString m_description;
    int m_heuresRequises = 0;
    int m_idFormateurResp = -1;
};

#endif // COURS_H