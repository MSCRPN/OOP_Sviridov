#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QObject>

class DatabaseManager : public QObject {
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    bool connect(const QString &host, int port, const QString &dbname,
                 const QString &user, const QString &password);
    void disconnect();
    bool isConnected() const;
    bool createTable();
    bool createConnection(const QString &connectionName);
    QSqlDatabase& getDatabase() { return db; }

private:
    QSqlDatabase db;
    QString connectionName;
};

#endif
