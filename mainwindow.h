#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include "contactBook.h"
#include "contactValidator.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_addButton_clicked();
    void on_editButton_clicked();
    void on_deleteButton_clicked();
    void on_searchButton_clicked();
    void on_refreshButton_clicked();


private:
    Ui::MainWindow *ui;
    ContactBook* contactBook;
    void showAddDialog();
    void showEditDialog(const Contact& currentContact);
    void findContacts(const QString& searchText);
    void setupTable();
    void refreshTable();
};

#endif
