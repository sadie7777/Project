/********************************************************************************
** Form generated from reading UI file 'eventdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EVENTDIALOG_H
#define UI_EVENTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EventDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *dateLabel;
    QSplitter *splitter;
    QWidget *leftWidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *filterLayout;
    QLabel *filterLabel;
    QComboBox *filterCombo;
    QLabel *eventsLabel;
    QListWidget *eventsListWidget;
    QWidget *rightWidget;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *detailsGroup;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *titleLayout;
    QLabel *titleLabel;
    QLineEdit *titleEdit;
    QLabel *descriptionLabel;
    QTextEdit *descriptionEdit;
    QHBoxLayout *timeLayout;
    QLabel *timeLabel;
    QTimeEdit *timeEdit;
    QHBoxLayout *priorityLayout;
    QLabel *priorityLabel;
    QComboBox *priorityCombo;
    QHBoxLayout *categoryLayout;
    QLabel *categoryLabel;
    QComboBox *categoryCombo;
    QPushButton *editButton;
    QGroupBox *horizontalGroupBox;
    QHBoxLayout *horizontalLayout;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QHBoxLayout *buttonLayout;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *EventDialog)
    {
        if (EventDialog->objectName().isEmpty())
            EventDialog->setObjectName("EventDialog");
        EventDialog->resize(700, 500);
        EventDialog->setStyleSheet(QString::fromUtf8("background-color:white;"));
        verticalLayout = new QVBoxLayout(EventDialog);
        verticalLayout->setObjectName("verticalLayout");
        dateLabel = new QLabel(EventDialog);
        dateLabel->setObjectName("dateLabel");
        QFont font;
        font.setFamilies({QString::fromUtf8("\355\225\234\354\273\264 \353\247\220\353\236\221\353\247\220\353\236\221")});
        font.setBold(true);
        dateLabel->setFont(font);

        verticalLayout->addWidget(dateLabel);

        splitter = new QSplitter(EventDialog);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Orientation::Horizontal);
        splitter->setChildrenCollapsible(false);
        leftWidget = new QWidget(splitter);
        leftWidget->setObjectName("leftWidget");
        verticalLayout_2 = new QVBoxLayout(leftWidget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        filterLayout = new QHBoxLayout();
        filterLayout->setObjectName("filterLayout");
        filterLabel = new QLabel(leftWidget);
        filterLabel->setObjectName("filterLabel");
        filterLabel->setFont(font);

        filterLayout->addWidget(filterLabel);

        filterCombo = new QComboBox(leftWidget);
        filterCombo->setObjectName("filterCombo");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\355\225\234\354\273\264 \353\247\220\353\236\221\353\247\220\353\236\221")});
        filterCombo->setFont(font1);

        filterLayout->addWidget(filterCombo);


        verticalLayout_2->addLayout(filterLayout);

        eventsLabel = new QLabel(leftWidget);
        eventsLabel->setObjectName("eventsLabel");
        eventsLabel->setFont(font);

        verticalLayout_2->addWidget(eventsLabel);

        eventsListWidget = new QListWidget(leftWidget);
        eventsListWidget->setObjectName("eventsListWidget");
        eventsListWidget->setMinimumSize(QSize(300, 0));
        eventsListWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::DefaultContextMenu);

        verticalLayout_2->addWidget(eventsListWidget);

        splitter->addWidget(leftWidget);
        rightWidget = new QWidget(splitter);
        rightWidget->setObjectName("rightWidget");
        verticalLayout_3 = new QVBoxLayout(rightWidget);
        verticalLayout_3->setObjectName("verticalLayout_3");
        detailsGroup = new QGroupBox(rightWidget);
        detailsGroup->setObjectName("detailsGroup");
        detailsGroup->setFont(font);
        verticalLayout_4 = new QVBoxLayout(detailsGroup);
        verticalLayout_4->setObjectName("verticalLayout_4");
        titleLayout = new QHBoxLayout();
        titleLayout->setObjectName("titleLayout");
        titleLabel = new QLabel(detailsGroup);
        titleLabel->setObjectName("titleLabel");
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\355\225\234\354\273\264 \353\247\220\353\236\221\353\247\220\353\236\221")});
        font2.setPointSize(10);
        font2.setBold(true);
        titleLabel->setFont(font2);

        titleLayout->addWidget(titleLabel);

        titleEdit = new QLineEdit(detailsGroup);
        titleEdit->setObjectName("titleEdit");
        QFont font3;
        font3.setPointSize(10);
        titleEdit->setFont(font3);

        titleLayout->addWidget(titleEdit);


        verticalLayout_4->addLayout(titleLayout);

        descriptionLabel = new QLabel(detailsGroup);
        descriptionLabel->setObjectName("descriptionLabel");
        descriptionLabel->setFont(font);

        verticalLayout_4->addWidget(descriptionLabel);

        descriptionEdit = new QTextEdit(detailsGroup);
        descriptionEdit->setObjectName("descriptionEdit");
        descriptionEdit->setMinimumSize(QSize(0, 100));
        descriptionEdit->setFrameShape(QFrame::Shape::Box);

        verticalLayout_4->addWidget(descriptionEdit);

        timeLayout = new QHBoxLayout();
        timeLayout->setObjectName("timeLayout");
        timeLabel = new QLabel(detailsGroup);
        timeLabel->setObjectName("timeLabel");
        timeLabel->setFont(font);

        timeLayout->addWidget(timeLabel);

        timeEdit = new QTimeEdit(detailsGroup);
        timeEdit->setObjectName("timeEdit");
        timeEdit->setFont(font1);

        timeLayout->addWidget(timeEdit);


        verticalLayout_4->addLayout(timeLayout);

        priorityLayout = new QHBoxLayout();
        priorityLayout->setObjectName("priorityLayout");
        priorityLabel = new QLabel(detailsGroup);
        priorityLabel->setObjectName("priorityLabel");
        priorityLabel->setFont(font);

        priorityLayout->addWidget(priorityLabel);

        priorityCombo = new QComboBox(detailsGroup);
        priorityCombo->setObjectName("priorityCombo");

        priorityLayout->addWidget(priorityCombo);


        verticalLayout_4->addLayout(priorityLayout);

        categoryLayout = new QHBoxLayout();
        categoryLayout->setObjectName("categoryLayout");
        categoryLabel = new QLabel(detailsGroup);
        categoryLabel->setObjectName("categoryLabel");
        categoryLabel->setFont(font);

        categoryLayout->addWidget(categoryLabel);

        categoryCombo = new QComboBox(detailsGroup);
        categoryCombo->setObjectName("categoryCombo");

        categoryLayout->addWidget(categoryCombo);


        verticalLayout_4->addLayout(categoryLayout);

        editButton = new QPushButton(detailsGroup);
        editButton->setObjectName("editButton");
        editButton->setFont(font);
        QIcon icon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties));
        editButton->setIcon(icon);

        verticalLayout_4->addWidget(editButton);

        horizontalGroupBox = new QGroupBox(detailsGroup);
        horizontalGroupBox->setObjectName("horizontalGroupBox");
        horizontalLayout = new QHBoxLayout(horizontalGroupBox);
        horizontalLayout->setObjectName("horizontalLayout");
        addButton = new QPushButton(horizontalGroupBox);
        addButton->setObjectName("addButton");
        addButton->setFont(font);
        QIcon icon1(QIcon::fromTheme(QIcon::ThemeIcon::ListAdd));
        addButton->setIcon(icon1);

        horizontalLayout->addWidget(addButton);

        deleteButton = new QPushButton(horizontalGroupBox);
        deleteButton->setObjectName("deleteButton");
        deleteButton->setFont(font);
        QIcon icon2(QIcon::fromTheme(QIcon::ThemeIcon::EditDelete));
        deleteButton->setIcon(icon2);

        horizontalLayout->addWidget(deleteButton);


        verticalLayout_4->addWidget(horizontalGroupBox);


        verticalLayout_3->addWidget(detailsGroup);

        splitter->addWidget(rightWidget);

        verticalLayout->addWidget(splitter);

        buttonLayout = new QHBoxLayout();
        buttonLayout->setObjectName("buttonLayout");
        okButton = new QPushButton(EventDialog);
        okButton->setObjectName("okButton");
        okButton->setFont(font);
        QIcon icon3(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave));
        okButton->setIcon(icon3);

        buttonLayout->addWidget(okButton);

        cancelButton = new QPushButton(EventDialog);
        cancelButton->setObjectName("cancelButton");
        cancelButton->setFont(font);
        QIcon icon4(QIcon::fromTheme(QIcon::ThemeIcon::ApplicationExit));
        cancelButton->setIcon(icon4);

        buttonLayout->addWidget(cancelButton);


        verticalLayout->addLayout(buttonLayout);


        retranslateUi(EventDialog);
        QObject::connect(okButton, &QPushButton::clicked, EventDialog, qOverload<>(&QDialog::accept));
        QObject::connect(cancelButton, &QPushButton::clicked, EventDialog, qOverload<>(&QDialog::reject));
        QObject::connect(eventsListWidget, &QListWidget::itemChanged, eventsListWidget, qOverload<>(&QListWidget::doItemsLayout));

        QMetaObject::connectSlotsByName(EventDialog);
    } // setupUi

    void retranslateUi(QDialog *EventDialog)
    {
        EventDialog->setWindowTitle(QCoreApplication::translate("EventDialog", "Dialog", nullptr));
        dateLabel->setText(QCoreApplication::translate("EventDialog", "Date: ", nullptr));
        filterLabel->setText(QCoreApplication::translate("EventDialog", "Show :", nullptr));
        filterCombo->setPlaceholderText(QCoreApplication::translate("EventDialog", "\354\235\274\354\240\225 \354\204\240\355\203\235", nullptr));
        eventsLabel->setText(QCoreApplication::translate("EventDialog", "Events :", nullptr));
        detailsGroup->setTitle(QCoreApplication::translate("EventDialog", "Event Details", nullptr));
        titleLabel->setText(QCoreApplication::translate("EventDialog", "Title :", nullptr));
        titleEdit->setPlaceholderText(QCoreApplication::translate("EventDialog", "\354\240\234\353\252\251\354\235\204 \354\236\205\353\240\245\355\225\264\354\243\274\354\204\270\354\232\224", nullptr));
        descriptionLabel->setText(QCoreApplication::translate("EventDialog", "Description :", nullptr));
        descriptionEdit->setPlaceholderText(QCoreApplication::translate("EventDialog", "\353\202\264\354\232\251\354\235\204 \354\236\205\353\240\245\355\225\264\354\243\274\354\204\270\354\232\224", nullptr));
        timeLabel->setText(QCoreApplication::translate("EventDialog", "Time :", nullptr));
        priorityLabel->setText(QCoreApplication::translate("EventDialog", "Priority :", nullptr));
        categoryLabel->setText(QCoreApplication::translate("EventDialog", "Category :", nullptr));
        editButton->setText(QCoreApplication::translate("EventDialog", "Edit", nullptr));
        addButton->setText(QCoreApplication::translate("EventDialog", "Add", nullptr));
        deleteButton->setText(QCoreApplication::translate("EventDialog", "Delete", nullptr));
        okButton->setText(QCoreApplication::translate("EventDialog", "OK", nullptr));
        cancelButton->setText(QCoreApplication::translate("EventDialog", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class EventDialog: public Ui_EventDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EVENTDIALOG_H
