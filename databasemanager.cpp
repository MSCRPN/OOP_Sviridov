#include "databasemanager.h"
#include <QDebug>
#include <QSqlQuery>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent), connectionName("PhonebookDB") {}

bool DatabaseManager::connect(const QString &host, int port, const QString &dbname,
                              const QString &user, const QString &password) {
    db = QSqlDatabase::addDatabase("QPSQL", connectionName);
    db.setHostName(host);
    db.setPort(port);
    db.setDatabaseName(dbname);
    db.setUserName(user);
    db.setPassword(password);

    if (!db.open()) {
        qDebug() << "DB Error:" << db.lastError().text();
        return false;
    }
    qDebug() << "Connected to PostgreSQL";
    return createTable();
}

bool DatabaseManager::createTable() {
    QSqlQuery query(db);
    bool ok = query.exec(R"(
        CREATE TABLE IF NOT EXISTS contacts (
            id SERIAL PRIMARY KEY,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            patronymic TEXT,
            address TEXT,
            birth_date TEXT,
            email TEXT UNIQUE NOT NULL,
            phones TEXT[]
        )
    )");
    if (!ok) qDebug() << "Table error:" << query.lastError().text();
    return ok;
}

bool DatabaseManager::isConnected() const
{
    return db.isOpen();
}
