#ifndef USERPROFILEDIALOG_H
#define USERPROFILEDIALOG_H

#include <QDialog>

class QLineEdit;
class QLabel;

class UserProfileDialog : public QDialog {
    Q_OBJECT
public:
    enum class UserType { Admin, Formateur, Stagiaire };

    explicit UserProfileDialog(int userId, UserType type, QWidget *parent = nullptr);

signals:
    void profileUpdated(const QString &newPrenom, const QString &newNom);

private slots:
    void enregistrerModifications();

private:
    void setupUi();
    void chargerDonnees();

    int m_userId;
    UserType m_type;

    QLabel *m_lblRoleBadge;
    QLineEdit *m_editNom;
    QLineEdit *m_editPrenom;
    QLineEdit *m_editEmail;

    QLineEdit *m_editPwdActuel;
    QLineEdit *m_editPwdNouveau;
    QLineEdit *m_editPwdConfirmer;
};

#endif // USERPROFILEDIALOG_H
