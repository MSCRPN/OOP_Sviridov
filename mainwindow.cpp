#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QFormLayout>
#include<QLabel>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QString filePath = QCoreApplication::applicationDirPath() + "/contacts.txt";
    contactBook = new ContactBook(filePath);

    qDebug() << "Файл:" << filePath;
    bool loaded = contactBook->loadFromFile();
    qDebug() << "Загружено:" << loaded;
    setupTable();
    refreshTable();
    ui->refreshButton->setVisible(false);

}


MainWindow::~MainWindow() {
    delete ui;
    delete contactBook;
}

void MainWindow::setupTable() {
    ui->tableWidget->setColumnCount(8);
    QStringList headers = {
        "Имя", "Фамилия", "Отчество",
        "Адрес", "Дата рождения",
        "Email", "Основной телефон", "Доп. телефоны"
    };
    ui->tableWidget->setHorizontalHeaderLabels(headers);
}

void MainWindow::refreshTable() {
    ui->tableWidget->setRowCount(0);
    auto contacts = contactBook->getAllContacts();

    for (int i = 0; i < contacts.size(); ++i) {
        const auto& c = contacts[i];
        ui->tableWidget->insertRow(i);

        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(c.getFirstName())));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(c.getLastName())));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(c.getPatronymic())));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(c.getAddress())));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(c.getBirthDate())));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(c.getEmail())));

        QString mainPhone = c.getPhones().empty() ? "-" : QString::fromStdString(c.getPhones()[0]);
        ui->tableWidget->setItem(i, 6, new QTableWidgetItem(mainPhone));


        QString extraPhones;
        for (size_t j = 1; j < c.getPhones().size(); ++j) {
            if (!extraPhones.isEmpty()) extraPhones += ", ";
            extraPhones += QString::fromStdString(c.getPhones()[j]);
        }
        ui->tableWidget->setItem(i, 7, new QTableWidgetItem(extraPhones));

        ui->tableWidget->resizeColumnsToContents();
        ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

        // ПРОКРУТКА:
        ui->tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
}

void MainWindow::on_addButton_clicked() {
    showAddDialog();
}

void MainWindow::on_editButton_clicked() {

    int row = ui->tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите контакт для редактирования!");
        return;
    }


    auto contacts = contactBook->getAllContacts();
    if (row >= contacts.size()) {
        QMessageBox::warning(this, "Ошибка", "Контакт не найден!");
        return;
    }

    const Contact& currentContact = contacts[row];
    showEditDialog(currentContact);
}

void MainWindow::showAddDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("Новый контакт");
    dialog.resize(450, 420);

    QFormLayout* formLayout = new QFormLayout(&dialog);
    formLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    formLayout->setLabelAlignment(Qt::AlignRight);
    formLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    formLayout->setHorizontalSpacing(15);
    formLayout->setVerticalSpacing(10);

    int fieldWidth = 300;


    QLineEdit* firstNameEdit = new QLineEdit(&dialog);
    firstNameEdit->setFixedWidth(fieldWidth);
    firstNameEdit->setPlaceholderText("Иван");

    QLineEdit* lastNameEdit = new QLineEdit(&dialog);
    lastNameEdit->setFixedWidth(fieldWidth);
    lastNameEdit->setPlaceholderText("Иванов");

    QLineEdit* patronymicEdit = new QLineEdit(&dialog);
    patronymicEdit->setFixedWidth(fieldWidth);
    patronymicEdit->setPlaceholderText("Иванович (необязательно)");

    QLineEdit* addressEdit = new QLineEdit(&dialog);
    addressEdit->setFixedWidth(fieldWidth);
    addressEdit->setPlaceholderText("ул. Ленина, д. 5");

    QLineEdit* birthDateEdit = new QLineEdit(&dialog);
    birthDateEdit->setFixedWidth(fieldWidth);
    birthDateEdit->setPlaceholderText("2000-01-15 (YYYY-MM-DD)");

    QLineEdit* emailEdit = new QLineEdit(&dialog);
    emailEdit->setFixedWidth(fieldWidth);
    emailEdit->setPlaceholderText("ivan@example.com");


    QLineEdit* phoneEdit1 = new QLineEdit(&dialog);
    phoneEdit1->setFixedWidth(fieldWidth);
    phoneEdit1->setPlaceholderText("8 999 123-45-67 или +7 (999) 123-45-67");


    QList<QLineEdit*> phoneEdits = {phoneEdit1};
    QList<QPushButton*> phoneChecks;
    QList<QLabel*> phoneStatuses;


    QPushButton* firstNameCheck = new QPushButton("Check", &dialog);
    QPushButton* lastNameCheck = new QPushButton("Check", &dialog);
    QPushButton* patronymicCheck = new QPushButton("Check", &dialog);
    QPushButton* birthDateCheck = new QPushButton("Check", &dialog);
    QPushButton* emailCheck = new QPushButton("Check", &dialog);
    QPushButton* phoneCheck1 = new QPushButton("Check", &dialog);


    QLabel* firstNameStatus = new QLabel("", &dialog);
    QLabel* lastNameStatus = new QLabel("", &dialog);
    QLabel* patronymicStatus = new QLabel("", &dialog);
    QLabel* birthDateStatus = new QLabel("", &dialog);
    QLabel* emailStatus = new QLabel("", &dialog);
    QLabel* phoneStatus1 = new QLabel("", &dialog);


    QList<QPushButton*> checkButtons = {firstNameCheck, lastNameCheck, patronymicCheck,
                                       birthDateCheck, emailCheck, phoneCheck1};
    for (QPushButton* btn : checkButtons) {
        btn->setFixedSize(55, 28);
        btn->setStyleSheet("QPushButton { background-color: #2196F3; color: white; border: none; border-radius: 5px; font-weight: bold; font-size: 11px; }");
    }


    QList<QLabel*> statusLabels = {firstNameStatus, lastNameStatus, patronymicStatus,
                                  birthDateStatus, emailStatus, phoneStatus1};
    for (QLabel* label : statusLabels) {
        label->setFixedSize(28, 28);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("QLabel { background: transparent !important; border: none !important; padding: 0 !important; margin: 0 !important; }");
    }


    auto createPhoneRow = [&](int index) -> QFrame* {
        QFrame* phoneFrame = new QFrame(&dialog);
        phoneFrame->setFrameStyle(QFrame::StyledPanel);
        phoneFrame->setStyleSheet("QFrame { border: 1px solid #ddd; border-radius: 4px; margin: 2px; padding: 4px; }");

        QHBoxLayout* phoneLayout = new QHBoxLayout(phoneFrame);
        phoneLayout->setContentsMargins(0, 0, 0, 0);

        QLineEdit* phoneEdit = new QLineEdit(&dialog);
        phoneEdit->setFixedWidth(fieldWidth);
        phoneEdit->setPlaceholderText(QString("Доп. номер %1 (+7)").arg(index));
        phoneEdits.append(phoneEdit);

        QPushButton* phoneCheck = new QPushButton("Check", &dialog);
        phoneCheck->setFixedSize(55, 28);
        phoneCheck->setStyleSheet("QPushButton { background-color: #2196F3; color: white; border: none; border-radius: 5px; font-weight: bold; font-size: 11px; }");
        phoneChecks.append(phoneCheck);


        QLabel* phoneStatus = new QLabel("", &dialog);
        phoneStatus->setFixedSize(28, 28);
        phoneStatus->setAlignment(Qt::AlignCenter);
        phoneStatus->setStyleSheet("QLabel { background: transparent !important; border: none !important; padding: 0 !important; margin: 0 !important; }");
        phoneStatuses.append(phoneStatus);

        phoneLayout->addWidget(phoneEdit);
        phoneLayout->addSpacing(8);
        phoneLayout->addWidget(phoneCheck);
        phoneLayout->addSpacing(4);
        phoneLayout->addWidget(phoneStatus);


        connect(phoneCheck, &QPushButton::clicked, [phoneEdit, phoneStatus]() {
            bool valid = ContactValidator::validatePhone(phoneEdit->text().toStdString());
            phoneStatus->setText(valid ? "✅" : "❌");
            phoneStatus->setStyleSheet(valid ?
                "QLabel { color: green; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }" :
                "QLabel { color: red; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }");
        });

        return phoneFrame;
    };


    auto createInputRow = [&](QLineEdit* edit, QPushButton* checkBtn, QLabel* status, const QString& labelText, bool hasCheck = true) {
        QWidget* row = new QWidget();
        QHBoxLayout* hLayout = new QHBoxLayout(row);
        if (hasCheck) {
            hLayout->addWidget(edit);
            hLayout->addSpacing(8);
            hLayout->addWidget(checkBtn);
            hLayout->addSpacing(4);
            hLayout->addWidget(status);
        } else {
            hLayout->addWidget(edit);

        }
        hLayout->setContentsMargins(0, 0, 0, 0);
        hLayout->setSpacing(0);
        formLayout->addRow(labelText, row);
    };


    createInputRow(firstNameEdit, firstNameCheck, firstNameStatus, "Имя:", true);
    createInputRow(lastNameEdit, lastNameCheck, lastNameStatus, "Фамилия:", true);
    createInputRow(patronymicEdit, patronymicCheck, patronymicStatus, "Отчество:", true);
    createInputRow(addressEdit, nullptr, nullptr, "Адрес:", false);
    createInputRow(birthDateEdit, birthDateCheck, birthDateStatus, "Дата рождения:", true);
    createInputRow(emailEdit, emailCheck, emailStatus, "Email:", true);


    QWidget* firstPhoneRow = new QWidget();
    QHBoxLayout* firstPhoneLayout = new QHBoxLayout(firstPhoneRow);
    firstPhoneLayout->addWidget(phoneEdit1);
    firstPhoneLayout->addSpacing(3);
    firstPhoneLayout->addWidget(phoneCheck1);
    firstPhoneLayout->addWidget(phoneStatus1);
    firstPhoneLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->addRow("Основной телефон:", firstPhoneRow);


    QPushButton* addPhoneButton = new QPushButton("Добавить доп. номер", &dialog);
    addPhoneButton->setStyleSheet("");
    formLayout->addRow(addPhoneButton);


    int phoneIndex = 1;
    connect(addPhoneButton, &QPushButton::clicked, [&]() {
        QFrame* newPhoneFrame = createPhoneRow(++phoneIndex);
        formLayout->insertRow(formLayout->rowCount() - 1, QString("Доп. телефон %1:").arg(phoneIndex), newPhoneFrame);
        dialog.adjustSize();
    });


    QPushButton* okButton = new QPushButton("Добавить");
    okButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-size: 14px; font-weight: bold; padding: 12px 40px; border-radius: 6px; border: none; } QPushButton:hover { background-color: #45a049; }");
    formLayout->addRow(okButton);


    auto validateName = [&](const QString& text, QLabel* status) {
        bool valid = ContactValidator::validateName(text.toStdString());
        status->setText(valid ? "✅" : "❌");
        status->setStyleSheet(valid ?
            "QLabel { color: green; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }" :
            "QLabel { color: red; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }");
    };

    auto validateEmail = [&](const QString& text, QLabel* status) {
        bool valid = ContactValidator::validateEmail(text.toStdString());
        status->setText(valid ? "✅" : "❌");
        status->setStyleSheet(valid ?
            "QLabel { color: green; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }" :
            "QLabel { color: red; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }");
    };

    auto validateDate = [&](const QString& text, QLabel* status) {
        bool valid = ContactValidator::validateDate(text.toStdString());
        status->setText(valid ? "✅" : "❌");
        status->setStyleSheet(valid ?
            "QLabel { color: green; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }" :
            "QLabel { color: red; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }");
    };


    connect(firstNameCheck, &QPushButton::clicked, [firstNameEdit, firstNameStatus, validateName]() { validateName(firstNameEdit->text(), firstNameStatus); });
    connect(lastNameCheck, &QPushButton::clicked, [lastNameEdit, lastNameStatus, validateName]() { validateName(lastNameEdit->text(), lastNameStatus); });
    connect(patronymicCheck, &QPushButton::clicked, [patronymicEdit, patronymicStatus, validateName]() { validateName(patronymicEdit->text(), patronymicStatus); });
    connect(birthDateCheck, &QPushButton::clicked, [birthDateEdit, birthDateStatus, validateDate]() { validateDate(birthDateEdit->text(), birthDateStatus); });
    connect(emailCheck, &QPushButton::clicked, [emailEdit, emailStatus, validateEmail]() { validateEmail(emailEdit->text(), emailStatus); });
    connect(phoneCheck1, &QPushButton::clicked, [phoneEdit1, phoneStatus1]() {
        bool valid = ContactValidator::validatePhone(phoneEdit1->text().toStdString());
        phoneStatus1->setText(valid ? "✅" : "❌");
        phoneStatus1->setStyleSheet(valid ?
            "QLabel { color: green; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }" :
            "QLabel { color: red; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }");
    });

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);

    if (dialog.exec() == QDialog::Accepted) {

        vector<string> allPhones;
        for (QLineEdit* phoneEdit : phoneEdits) {
            QString phoneText = phoneEdit->text().trimmed();
            if (!phoneText.isEmpty()) {
                allPhones.push_back(phoneText.toStdString());
            }
        }

        if (allPhones.empty()) {
            QMessageBox::warning(this, "Ошибка", "Добавьте хотя бы один телефон!");
            return;
        }

        ContactBuilder builder;
        try {
            Contact c = builder
                .setFirstName(firstNameEdit->text().toStdString())
                .setLastName(lastNameEdit->text().toStdString())
                .setPatronymic(patronymicEdit->text().toStdString())
                .setAddress(addressEdit->text().toStdString())
                .setBirthDate(birthDateEdit->text().toStdString())
                .setEmail(emailEdit->text().toStdString())
                .setPhones(allPhones)
                .build();

            contactBook->addContact(c);
            refreshTable();
            QMessageBox::information(this, "Успех", "Контакт добавлен!");
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Ошибка", QString("Проверьте данные!\n") + QString::fromStdString(e.what()));
        }
    }
}


void MainWindow::showEditDialog(const Contact& currentContact) {
    QDialog dialog(this);
    dialog.setWindowTitle("Редактировать контакт");
    dialog.resize(450, 420);

    QFormLayout* formLayout = new QFormLayout(&dialog);
    formLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    formLayout->setLabelAlignment(Qt::AlignRight);
    formLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    formLayout->setHorizontalSpacing(15);
    formLayout->setVerticalSpacing(10);

    int fieldWidth = 300;


    QLineEdit* firstNameEdit = new QLineEdit(QString::fromStdString(currentContact.getFirstName()), &dialog);
    firstNameEdit->setFixedWidth(fieldWidth);

    QLineEdit* lastNameEdit = new QLineEdit(QString::fromStdString(currentContact.getLastName()), &dialog);
    lastNameEdit->setFixedWidth(fieldWidth);

    QLineEdit* patronymicEdit = new QLineEdit(QString::fromStdString(currentContact.getPatronymic()), &dialog);
    patronymicEdit->setFixedWidth(fieldWidth);

    QLineEdit* addressEdit = new QLineEdit(QString::fromStdString(currentContact.getAddress()), &dialog);
    addressEdit->setFixedWidth(fieldWidth);

    QLineEdit* birthDateEdit = new QLineEdit(QString::fromStdString(currentContact.getBirthDate()), &dialog);
    birthDateEdit->setFixedWidth(fieldWidth);

    QLineEdit* emailEdit = new QLineEdit(QString::fromStdString(currentContact.getEmail()), &dialog);
    emailEdit->setFixedWidth(fieldWidth);


    auto allPhones = currentContact.getPhones();
    QList<QLineEdit*> phoneEdits;
    QList<QPushButton*> phoneCheckButtons;
    QList<QLabel*> phoneStatusLabels;


    QLineEdit* phoneEdit1 = new QLineEdit(allPhones.empty() ? "" : QString::fromStdString(allPhones[0]), &dialog);
    phoneEdit1->setFixedWidth(fieldWidth);
    phoneEdit1->setPlaceholderText("8 999 123-45-67 или +7 (999) 123-45-67");
    phoneEdits.append(phoneEdit1);

    QPushButton* phoneCheck1 = new QPushButton("Check", &dialog);
    QLabel* phoneStatus1 = new QLabel("", &dialog);
    phoneCheckButtons.append(phoneCheck1);
    phoneStatusLabels.append(phoneStatus1);


    for (size_t i = 1; i < allPhones.size(); ++i) {
        QLineEdit* extraPhoneEdit = new QLineEdit(QString::fromStdString(allPhones[i]), &dialog);
        extraPhoneEdit->setFixedWidth(fieldWidth);
        extraPhoneEdit->setPlaceholderText("8 999 123-45-67 или +7 (999) 123-45-67");
        phoneEdits.append(extraPhoneEdit);

        QPushButton* extraPhoneCheck = new QPushButton("Check", &dialog);
        QLabel* extraPhoneStatus = new QLabel("", &dialog);
        phoneCheckButtons.append(extraPhoneCheck);
        phoneStatusLabels.append(extraPhoneStatus);
    }


    QPushButton* firstNameCheck = new QPushButton("Check", &dialog);
    QPushButton* lastNameCheck = new QPushButton("Check", &dialog);
    QPushButton* patronymicCheck = new QPushButton("Check", &dialog);
    QPushButton* birthDateCheck = new QPushButton("Check", &dialog);
    QPushButton* emailCheck = new QPushButton("Check", &dialog);


    QLabel* firstNameStatus = new QLabel("", &dialog);
    QLabel* lastNameStatus = new QLabel("", &dialog);
    QLabel* patronymicStatus = new QLabel("", &dialog);
    QLabel* birthDateStatus = new QLabel("", &dialog);
    QLabel* emailStatus = new QLabel("", &dialog);


    QList<QPushButton*> allCheckButtons = {
        firstNameCheck, lastNameCheck, patronymicCheck, birthDateCheck, emailCheck
    };
    allCheckButtons.append(phoneCheckButtons);

    for (QPushButton* btn : allCheckButtons) {
        btn->setFixedSize(55, 28);
        btn->setStyleSheet("QPushButton { background-color: #2196F3; color: white; border: none; border-radius: 5px; font-weight: bold; font-size: 11px; }");
    }


    QList<QLabel*> allStatusLabels = {
        firstNameStatus, lastNameStatus, patronymicStatus, birthDateStatus, emailStatus
    };
    allStatusLabels.append(phoneStatusLabels);

    for (QLabel* label : allStatusLabels) {
        label->setFixedSize(28, 28);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("QLabel { background: transparent !important; border: none !important; padding: 0 !important; margin: 0 !important; }");
    }


    auto createInputRow = [&](QLineEdit* edit, QPushButton* checkBtn, QLabel* status, const QString& labelText, bool hasCheck = true) -> QWidget* {
        QWidget* row = new QWidget();
        QHBoxLayout* hLayout = new QHBoxLayout(row);
        hLayout->setContentsMargins(0, 0, 0, 0);
        hLayout->setSpacing(0);

        if (hasCheck) {
            hLayout->addWidget(edit);
            hLayout->addSpacing(8);
            hLayout->addWidget(checkBtn);
            hLayout->addSpacing(4);
            hLayout->addWidget(status);
        } else {
            hLayout->addWidget(edit);
            hLayout->addSpacing(8 + 55 + 4 + 28);
        }
        return row;
    };


    formLayout->addRow("Имя:", createInputRow(firstNameEdit, firstNameCheck, firstNameStatus, "Имя:", true));
    formLayout->addRow("Фамилия:", createInputRow(lastNameEdit, lastNameCheck, lastNameStatus, "Фамилия:", true));
    formLayout->addRow("Отчество:", createInputRow(patronymicEdit, patronymicCheck, patronymicStatus, "Отчество:", true));
    formLayout->addRow("Адрес:", createInputRow(addressEdit, nullptr, nullptr, "Адрес:", false));
    formLayout->addRow("Дата рождения:", createInputRow(birthDateEdit, birthDateCheck, birthDateStatus, "Дата рождения:", true));
    formLayout->addRow("Email:", createInputRow(emailEdit, emailCheck, emailStatus, "Email:", true));


    for (int i = 0; i < phoneEdits.size(); ++i) {
        QString label = (i == 0) ? "Основной телефон (+7):" : QString("Доп. телефон %1:").arg(i + 1);
        formLayout->addRow(label, createInputRow(phoneEdits[i], phoneCheckButtons[i], phoneStatusLabels[i], label, true));
    }


    QPushButton* saveButton = new QPushButton("Сохранить изменения");
    saveButton->setStyleSheet("QPushButton { background-color: #FF9800; color: white; font-size: 14px; font-weight: bold; padding: 12px 40px; border-radius: 6px; border: none; } QPushButton:hover { background-color: #F57C00; }");
    formLayout->addRow(saveButton);


    auto validateName = [&](const QString& text, QLabel* status) {
        bool valid = ContactValidator::validateName(text.toStdString());
        status->setText(valid ? "✅" : "❌");
        status->setStyleSheet(valid ?
            "QLabel { color: green; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }" :
            "QLabel { color: red; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }");
    };

    auto validateEmail = [&](const QString& text, QLabel* status) {
        bool valid = ContactValidator::validateEmail(text.toStdString());
        status->setText(valid ? "✅" : "❌");
        status->setStyleSheet(valid ?
            "QLabel { color: green; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }" :
            "QLabel { color: red; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }");
    };

    auto validateDate = [&](const QString& text, QLabel* status) {
        bool valid = ContactValidator::validateDate(text.toStdString());
        status->setText(valid ? "✅" : "❌");
        status->setStyleSheet(valid ?
            "QLabel { color: green; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }" :
            "QLabel { color: red; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }");
    };

    auto validatePhone = [&](const QString& text, QLabel* status) {
        bool valid = ContactValidator::validatePhone(text.toStdString());
        status->setText(valid ? "✅" : "❌");
        status->setStyleSheet(valid ?
            "QLabel { color: green; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }" :
            "QLabel { color: red; font-size: 16px; font-weight: bold; background: transparent !important; border: none !important; }");
    };


    connect(firstNameCheck, &QPushButton::clicked, [firstNameEdit, firstNameStatus, validateName]() {
        validateName(firstNameEdit->text(), firstNameStatus);
    });
    connect(lastNameCheck, &QPushButton::clicked, [lastNameEdit, lastNameStatus, validateName]() {
        validateName(lastNameEdit->text(), lastNameStatus);
    });
    connect(patronymicCheck, &QPushButton::clicked, [patronymicEdit, patronymicStatus, validateName]() {
        validateName(patronymicEdit->text(), patronymicStatus);
    });
    connect(birthDateCheck, &QPushButton::clicked, [birthDateEdit, birthDateStatus, validateDate]() {
        validateDate(birthDateEdit->text(), birthDateStatus);
    });
    connect(emailCheck, &QPushButton::clicked, [emailEdit, emailStatus, validateEmail]() {
        validateEmail(emailEdit->text(), emailStatus);
    });


    for (int i = 0; i < phoneCheckButtons.size(); ++i) {
        connect(phoneCheckButtons[i], &QPushButton::clicked, [phoneEdits, phoneStatusLabels, i, validatePhone]() {
            validatePhone(phoneEdits[i]->text(), phoneStatusLabels[i]);
        });
    }

    connect(saveButton, &QPushButton::clicked, &dialog, &QDialog::accept);


    if (dialog.exec() == QDialog::Accepted) {
        vector<string> allPhonesUpdated;
        for (QLineEdit* phoneEdit : phoneEdits) {
            QString phoneText = phoneEdit->text().trimmed();
            if (!phoneText.isEmpty()) {
                allPhonesUpdated.push_back(phoneText.toStdString());
            }
        }

        if (allPhonesUpdated.empty()) {
            QMessageBox::warning(this, "Ошибка", "Добавьте хотя бы один телефон!");
            return;
        }

        ContactBuilder builder;
        try {
            Contact updatedContact = builder
                .setFirstName(firstNameEdit->text().toStdString())
                .setLastName(lastNameEdit->text().toStdString())
                .setPatronymic(patronymicEdit->text().toStdString())
                .setAddress(addressEdit->text().toStdString())
                .setBirthDate(birthDateEdit->text().toStdString())
                .setEmail(emailEdit->text().toStdString())
                .setPhones(allPhonesUpdated)
                .build();

            contactBook->removeContact(currentContact.getEmail());
            contactBook->addContact(updatedContact);
            refreshTable();

            QMessageBox::information(this, "Успех", "Контакт обновлен!");
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Ошибка", QString("Проверьте данные!\n") + QString::fromStdString(e.what()));
        }
    }
}

void MainWindow::on_deleteButton_clicked() {
    if (ui->tableWidget->currentRow() < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите контакт!");
        return;
    }

    int row = ui->tableWidget->currentRow();


    QString email = ui->tableWidget->item(row, 5)->text().trimmed();


    int start = email.indexOf('[');
    int end = email.indexOf(']', start);
    if (start != -1 && end != -1) {
        email = email.mid(start + 1, end - start - 1);
    }

    QString firstName = ui->tableWidget->item(row, 0)->text().trimmed();
    QString lastName = ui->tableWidget->item(row, 1)->text().trimmed();

    qDebug() << "Чистый EMAIL:" << email;

    if (QMessageBox::question(this, "Удалить?",
        firstName + " " + lastName + "\n" + email) == QMessageBox::Yes) {

        bool removed = contactBook->removeContact(email.toStdString());
        qDebug() << "Удален:" << removed;

        if (removed) {
            ui->tableWidget->removeRow(row);
            contactBook->saveToFile();
            QMessageBox::information(this, "Готово", "Удален!");
        }
    }
}

void MainWindow::on_searchButton_clicked() {
    QString searchText = QInputDialog::getText(this, "🔍 Поиск", "Поиск по всем полям:");

    if (!searchText.trimmed().isEmpty()) {
        findContacts(searchText.trimmed());
    }
}

void MainWindow::findContacts(const QString& searchText) {
    QString searchLower = searchText.toLower();
    int foundCount = 0;


    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        ui->tableWidget->setRowHidden(row, true);
    }


    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        bool found = false;


        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            QTableWidgetItem* item = ui->tableWidget->item(row, col);
            if (item && item->text().toLower().contains(searchLower)) {
                found = true;
                break;
            }
        }

        if (found) {
            ui->tableWidget->setRowHidden(row, false);
            foundCount++;
            ui->tableWidget->selectRow(row);
        }
    }

    QString message = foundCount == 0 ?
        "Контакты не найдены!" :
        QString("Найдено: %1 из %2").arg(foundCount).arg(ui->tableWidget->rowCount());

    QMessageBox::information(this, "🔍 Результат поиска", message);
    ui->refreshButton->setVisible(true);
}

void MainWindow::on_refreshButton_clicked() {
    refreshTable();

    ui->refreshButton->setVisible(false);
}

