#include "passwordutils.h"
#include "authentification.h"

QString PasswordUtils::hashPassword(const QString& password)
{
    return Authentification::hashPassword(password);
}