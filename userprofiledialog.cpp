#include "userprofiledialog.h"
#include "db.h"
#include "authentification.h"
#include "moduletools.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QSqlQuery>
#include <QFrame>

UserProfileDialog::UserProfileDialog(int userId, UserType type, QWidget *parent)
    : QDialog(parent), m_userId(userId), m_type(type)
{
    setWindowTitle("Mon Profil Utilisateur — CentrePro");
    resize(480, 480);
    setupUi();
    chargerDonnees();
}

void UserProfileDialog::setupUi() {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(24, 24, 24, 24);
    root->setSpacing(16);

    // Hero Profile Header
    auto *headerCard = new QFrame(this);
    headerCard->setStyleSheet("background: #0f172a; border-radius: 14px; padding: 14px;");
    auto *headerLay = new QHBoxLayout(headerCard);

    auto *avatarLbl = new QLabel(headerCard);
    avatarLbl->setPixmap(ModuleTools::standardIcon(QStyle::SP_FileDialogInfoView).pixmap(30, 30));
    avatarLbl->setStyleSheet("background: transparent;");
    headerLay->addWidget(avatarLbl);

    auto *titleLay = new QVBoxLayout();
    auto *nameLbl = new QLabel("<b>Paramètres du Profil</b>", headerCard);
    nameLbl->setStyleSheet("color: white; font-size: 14pt; background: transparent;");

    m_lblRoleBadge = new QLabel("UTILISATEUR", headerCard);
    m_lblRoleBadge->setStyleSheet("color: #38bdf8; font-size: 9pt; font-weight: bold; background: transparent;");
    titleLay->addWidget(nameLbl);
    titleLay->addWidget(m_lblRoleBadge);

    headerLay->addLayout(titleLay);
    headerLay->addStretch();
    root->addWidget(headerCard);

    // Form 1: Identity info
    auto *boxInfo = new QFrame(this);
    boxInfo->setStyleSheet("background: #ffffff; border: 1px solid #e2e8f0; border-radius: 12px; padding: 12px;");
    auto *formInfo = new QFormLayout(boxInfo);
    formInfo->setSpacing(10);

    m_editPrenom = new QLineEdit(boxInfo);
    m_editNom = new QLineEdit(boxInfo);
    m_editEmail = new QLineEdit(boxInfo);

    formInfo->addRow("Prénom :", m_editPrenom);
    formInfo->addRow("Nom :", m_editNom);
    formInfo->addRow("Email :", m_editEmail);
    root->addWidget(boxInfo);

    // Form 2: Password change (optional)
    auto *boxPwd = new QFrame(this);
    boxPwd->setStyleSheet("background: #ffffff; border: 1px solid #e2e8f0; border-radius: 12px; padding: 12px;");
    auto *formPwd = new QFormLayout(boxPwd);
    formPwd->setSpacing(10);

    auto *lblPwdTitle = new QLabel("<b>Sécurité & Mot de passe</b> (laisser vide pour ne pas changer) :", boxPwd);
    lblPwdTitle->setStyleSheet("color: #475569; font-size: 9pt;");
    formPwd->addRow(lblPwdTitle);

    m_editPwdActuel = new QLineEdit(boxPwd);
    m_editPwdActuel->setEchoMode(QLineEdit::Password);
    m_editPwdActuel->setPlaceholderText("Requis uniquement si changement de mot de passe");

    m_editPwdNouveau = new QLineEdit(boxPwd);
    m_editPwdNouveau->setEchoMode(QLineEdit::Password);
    m_editPwdNouveau->setPlaceholderText("Nouveau mot de passe");

    m_editPwdConfirmer = new QLineEdit(boxPwd);
    m_editPwdConfirmer->setEchoMode(QLineEdit::Password);
    m_editPwdConfirmer->setPlaceholderText("Confirmer le mot de passe");

    formPwd->addRow("Mot de passe actuel :", m_editPwdActuel);
    formPwd->addRow("Nouveau mot de passe :", m_editPwdNouveau);
    formPwd->addRow("Confirmation :", m_editPwdConfirmer);
    root->addWidget(boxPwd);

    // Buttons
    auto *btnRow = new QHBoxLayout();
    auto *btnAnnuler = new QPushButton("Annuler", this);
    btnAnnuler->setObjectName("btnVider");
    btnAnnuler->setStyleSheet("padding: 9px 18px;");
    connect(btnAnnuler, &QPushButton::clicked, this, &QDialog::reject);

    auto *btnSave = new QPushButton("Enregistrer les modifications", this);
    btnSave->setIcon(ModuleTools::standardIcon(QStyle::SP_DialogSaveButton));
    btnSave->setStyleSheet("background: #0284c7; color: white; font-weight: bold; padding: 9px 20px; border-radius: 8px;");
    connect(btnSave, &QPushButton::clicked, this, &UserProfileDialog::enregistrerModifications);

    btnRow->addStretch();
    btnRow->addWidget(btnAnnuler);
    btnRow->addWidget(btnSave);
    root->addLayout(btnRow);
}

void UserProfileDialog::chargerDonnees() {
    QSqlQuery q(DB::instance().database());
    if (m_type == UserType::Admin || m_type == UserType::Formateur) {
        q.prepare("SELECT NOM, PRENOM, EMAIL, ROLE FROM FORMATEUR WHERE ID_FORMATEUR = :id");
    } else {
        q.prepare("SELECT NOM, PRENOM, EMAIL, STATUT FROM STAGIAIRE WHERE ID_STAGIAIRE = :id");
    }
    q.bindValue(":id", m_userId);

    if (q.exec() && q.next()) {
        m_editNom->setText(q.value(0).toString());
        m_editPrenom->setText(q.value(1).toString());
        m_editEmail->setText(q.value(2).toString());
        QString role = q.value(3).toString();
        m_lblRoleBadge->setText(QString("ROLE : %1").arg(role.toUpper()));
    }
}

void UserProfileDialog::enregistrerModifications() {
    QString prenom = m_editPrenom->text().trimmed();
    QString nom = m_editNom->text().trimmed();
    QString email = m_editEmail->text().trimmed();

    if (prenom.isEmpty() || nom.isEmpty() || email.isEmpty() || !ModuleTools::isValidEmail(email)) {
        QMessageBox::warning(this, "Validation", "Le nom, prénom et email doivent être renseignés avec une adresse valide.");
        return;
    }

    bool changingPassword = !m_editPwdNouveau->text().isEmpty();
    QString newPwdHash;

    if (changingPassword) {
        if (m_editPwdActuel->text().isEmpty()) {
            QMessageBox::warning(this, "Validation", "Veuillez renseigner votre mot de passe actuel.");
            return;
        }
        if (m_editPwdNouveau->text() != m_editPwdConfirmer->text()) {
            QMessageBox::warning(this, "Validation", "Les nouveaux mots de passe ne correspondent pas.");
            return;
        }

        // Verify current password
        QString hashActuel = Authentification::hashPassword(m_editPwdActuel->text());
        QSqlQuery checkQ(DB::instance().database());
        if (m_type == UserType::Admin || m_type == UserType::Formateur) {
            checkQ.prepare("SELECT COUNT(*) FROM FORMATEUR WHERE ID_FORMATEUR = :id AND LOWER(TRIM(PASSWORD_HASH)) = :pwd");
        } else {
            checkQ.prepare("SELECT COUNT(*) FROM STAGIAIRE WHERE ID_STAGIAIRE = :id AND LOWER(TRIM(PASSWORD_HASH)) = :pwd");
        }
        checkQ.bindValue(":id", m_userId);
        checkQ.bindValue(":pwd", hashActuel);

        if (!checkQ.exec() || !checkQ.next() || checkQ.value(0).toInt() == 0) {
            QMessageBox::warning(this, "Erreur", "Le mot de passe actuel est incorrect.");
            return;
        }

        newPwdHash = Authentification::hashPassword(m_editPwdNouveau->text());
    }

    QSqlQuery updateQ(DB::instance().database());
    bool ok = false;

    if (m_type == UserType::Admin || m_type == UserType::Formateur) {
        if (changingPassword) {
            updateQ.prepare("UPDATE FORMATEUR SET NOM = :nom, PRENOM = :prenom, EMAIL = :email, PASSWORD_HASH = :pwd WHERE ID_FORMATEUR = :id");
            updateQ.bindValue(":pwd", newPwdHash);
        } else {
            updateQ.prepare("UPDATE FORMATEUR SET NOM = :nom, PRENOM = :prenom, EMAIL = :email WHERE ID_FORMATEUR = :id");
        }
    } else {
        if (changingPassword) {
            updateQ.prepare("UPDATE STAGIAIRE SET NOM = :nom, PRENOM = :prenom, EMAIL = :email, PASSWORD_HASH = :pwd WHERE ID_STAGIAIRE = :id");
            updateQ.bindValue(":pwd", newPwdHash);
        } else {
            updateQ.prepare("UPDATE STAGIAIRE SET NOM = :nom, PRENOM = :prenom, EMAIL = :email WHERE ID_STAGIAIRE = :id");
        }
    }

    updateQ.bindValue(":nom", nom);
    updateQ.bindValue(":prenom", prenom);
    updateQ.bindValue(":email", email);
    updateQ.bindValue(":id", m_userId);
    ok = updateQ.exec();

    if (ok) {
        QMessageBox::information(this, "Profil mis à jour", "Vos informations de profil ont été enregistrées avec succès !");
        emit profileUpdated(prenom, nom);
        accept();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec lors de l'enregistrement dans la base Oracle.");
    }
}
