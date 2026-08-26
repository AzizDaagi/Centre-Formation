#include "passwordutils.h"

#include <QCryptographicHash>

QString PasswordUtils::hashPassword(const QString& password)
{
    QByteArray hash = QCryptographicHash::hash(
        password.toUtf8(),
        QCryptographicHash::Sha256
        );

    return hash.toHex();
}