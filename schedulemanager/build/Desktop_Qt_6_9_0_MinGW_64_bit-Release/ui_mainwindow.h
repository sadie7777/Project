/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *searchBarLayout;
    QSpacerItem *horizontalSpacer;
    QLineEdit *searchEdit;
    QPushButton *searchButton;
    QWidget *navWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *prevMonthButton;
    QComboBox *yearComboBox;
    QLabel *yearLabel;
    QComboBox *monthComboBox;
    QLabel *monthLabel;
    QPushButton *nextMonthButton;
    QWidget *daysWidget;
    QHBoxLayout *daysLayout;
    QLabel *sundayLabel;
    QLabel *mondayLabel;
    QLabel *tuesdayLabel;
    QLabel *wednesdayLabel;
    QLabel *thursdayLabel;
    QLabel *fridayLabel;
    QLabel *saturdayLabel;
    QVBoxLayout *calendarLayout;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(984, 735);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        searchBarLayout = new QHBoxLayout();
        searchBarLayout->setSpacing(3);
        searchBarLayout->setObjectName("searchBarLayout");
        searchBarLayout->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        searchBarLayout->addItem(horizontalSpacer);

        searchEdit = new QLineEdit(centralwidget);
        searchEdit->setObjectName("searchEdit");
        searchEdit->setMaximumSize(QSize(200, 16777215));
        QFont font;
        font.setFamilies({QString::fromUtf8("\355\225\234\354\273\264 \353\247\220\353\236\221\353\247\220\353\236\221")});
        font.setPointSize(11);
        searchEdit->setFont(font);

        searchBarLayout->addWidget(searchEdit);

        searchButton = new QPushButton(centralwidget);
        searchButton->setObjectName("searchButton");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\355\225\234\354\273\264 \353\247\220\353\236\221\353\247\220\353\236\221")});
        font1.setPointSize(11);
        font1.setBold(true);
        searchButton->setFont(font1);
        searchButton->setCursor(QCursor(Qt::CursorShape::ClosedHandCursor));
        searchButton->setStyleSheet(QString::fromUtf8("background-color:#ffffff;"));

        searchBarLayout->addWidget(searchButton);


        verticalLayout->addLayout(searchBarLayout);

        navWidget = new QWidget(centralwidget);
        navWidget->setObjectName("navWidget");
        navWidget->setMinimumSize(QSize(0, 50));
        navWidget->setMaximumSize(QSize(16777215, 50));
        navWidget->setStyleSheet(QString::fromUtf8("background-color:#ffffff;"));
        horizontalLayout = new QHBoxLayout(navWidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        prevMonthButton = new QPushButton(navWidget);
        prevMonthButton->setObjectName("prevMonthButton");
        prevMonthButton->setCursor(QCursor(Qt::CursorShape::ClosedHandCursor));
        prevMonthButton->setStyleSheet(QString::fromUtf8("border: none;"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/left(3).png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        prevMonthButton->setIcon(icon);
        prevMonthButton->setIconSize(QSize(50, 50));

        horizontalLayout->addWidget(prevMonthButton);

        yearComboBox = new QComboBox(navWidget);
        yearComboBox->setObjectName("yearComboBox");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(yearComboBox->sizePolicy().hasHeightForWidth());
        yearComboBox->setSizePolicy(sizePolicy);
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\355\225\234\354\273\264 \353\247\220\353\236\221\353\247\220\353\236\221")});
        font2.setPointSize(12);
        font2.setBold(true);
        yearComboBox->setFont(font2);
        yearComboBox->setCursor(QCursor(Qt::CursorShape::OpenHandCursor));
        yearComboBox->setMinimumContentsLength(0);
        yearComboBox->setIconSize(QSize(5, 16));

        horizontalLayout->addWidget(yearComboBox);

        yearLabel = new QLabel(navWidget);
        yearLabel->setObjectName("yearLabel");
        sizePolicy.setHeightForWidth(yearLabel->sizePolicy().hasHeightForWidth());
        yearLabel->setSizePolicy(sizePolicy);
        yearLabel->setMinimumSize(QSize(0, 0));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("\355\225\234\354\273\264 \353\247\220\353\236\221\353\247\220\353\236\221")});
        font3.setPointSize(11);
        font3.setBold(true);
        font3.setStyleStrategy(QFont::PreferDefault);
        yearLabel->setFont(font3);
        yearLabel->setCursor(QCursor(Qt::CursorShape::OpenHandCursor));
        yearLabel->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        yearLabel->setFrameShape(QFrame::Shape::NoFrame);
        yearLabel->setLineWidth(0);
        yearLabel->setTextFormat(Qt::TextFormat::AutoText);
        yearLabel->setMargin(10);
        yearLabel->setTextInteractionFlags(Qt::TextInteractionFlag::LinksAccessibleByMouse);

        horizontalLayout->addWidget(yearLabel);

        monthComboBox = new QComboBox(navWidget);
        monthComboBox->setObjectName("monthComboBox");
        sizePolicy.setHeightForWidth(monthComboBox->sizePolicy().hasHeightForWidth());
        monthComboBox->setSizePolicy(sizePolicy);
        monthComboBox->setFont(font2);
        monthComboBox->setCursor(QCursor(Qt::CursorShape::OpenHandCursor));
        monthComboBox->setIconSize(QSize(5, 16));

        horizontalLayout->addWidget(monthComboBox);

        monthLabel = new QLabel(navWidget);
        monthLabel->setObjectName("monthLabel");
        sizePolicy.setHeightForWidth(monthLabel->sizePolicy().hasHeightForWidth());
        monthLabel->setSizePolicy(sizePolicy);
        monthLabel->setFont(font1);
        monthLabel->setCursor(QCursor(Qt::CursorShape::OpenHandCursor));
        monthLabel->setStyleSheet(QString::fromUtf8("background-color:#ffffff;"));
        monthLabel->setWordWrap(false);
        monthLabel->setMargin(10);

        horizontalLayout->addWidget(monthLabel);

        nextMonthButton = new QPushButton(navWidget);
        nextMonthButton->setObjectName("nextMonthButton");
        nextMonthButton->setCursor(QCursor(Qt::CursorShape::ClosedHandCursor));
        nextMonthButton->setStyleSheet(QString::fromUtf8("border: none;"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/right(2).png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        nextMonthButton->setIcon(icon1);
        nextMonthButton->setIconSize(QSize(50, 50));

        horizontalLayout->addWidget(nextMonthButton);


        verticalLayout->addWidget(navWidget);

        daysWidget = new QWidget(centralwidget);
        daysWidget->setObjectName("daysWidget");
        daysWidget->setMinimumSize(QSize(0, 30));
        daysWidget->setMaximumSize(QSize(16777215, 30));
        daysLayout = new QHBoxLayout(daysWidget);
        daysLayout->setSpacing(0);
        daysLayout->setObjectName("daysLayout");
        daysLayout->setContentsMargins(0, 0, 0, 0);
        sundayLabel = new QLabel(daysWidget);
        sundayLabel->setObjectName("sundayLabel");
        QFont font4;
        font4.setFamilies({QString::fromUtf8("\355\225\234\354\273\264 \353\247\220\353\236\221\353\247\220\353\236\221")});
        font4.setPointSize(10);
        font4.setBold(true);
        sundayLabel->setFont(font4);
        sundayLabel->setStyleSheet(QString::fromUtf8("background-color: #ffffff; color: red; font-weight: bold;"));
        sundayLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        daysLayout->addWidget(sundayLabel);

        mondayLabel = new QLabel(daysWidget);
        mondayLabel->setObjectName("mondayLabel");
        mondayLabel->setFont(font4);
        mondayLabel->setStyleSheet(QString::fromUtf8("background-color: #ffffff; color: black; font-weight: bold;"));
        mondayLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        daysLayout->addWidget(mondayLabel);

        tuesdayLabel = new QLabel(daysWidget);
        tuesdayLabel->setObjectName("tuesdayLabel");
        tuesdayLabel->setFont(font4);
        tuesdayLabel->setStyleSheet(QString::fromUtf8("background-color: #ffffff; color: black; font-weight: bold;"));
        tuesdayLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        daysLayout->addWidget(tuesdayLabel);

        wednesdayLabel = new QLabel(daysWidget);
        wednesdayLabel->setObjectName("wednesdayLabel");
        wednesdayLabel->setFont(font4);
        wednesdayLabel->setStyleSheet(QString::fromUtf8("background-color: #ffffff; color: black; font-weight: bold;"));
        wednesdayLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        daysLayout->addWidget(wednesdayLabel);

        thursdayLabel = new QLabel(daysWidget);
        thursdayLabel->setObjectName("thursdayLabel");
        thursdayLabel->setFont(font4);
        thursdayLabel->setStyleSheet(QString::fromUtf8("background-color: #ffffff; color: black; font-weight: bold;"));
        thursdayLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        daysLayout->addWidget(thursdayLabel);

        fridayLabel = new QLabel(daysWidget);
        fridayLabel->setObjectName("fridayLabel");
        fridayLabel->setFont(font4);
        fridayLabel->setStyleSheet(QString::fromUtf8("background-color: #ffffff; color: black; font-weight: bold;"));
        fridayLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        daysLayout->addWidget(fridayLabel);

        saturdayLabel = new QLabel(daysWidget);
        saturdayLabel->setObjectName("saturdayLabel");
        saturdayLabel->setFont(font4);
        saturdayLabel->setStyleSheet(QString::fromUtf8("background-color:#ffffff; color: blue; font-weight: bold;"));
        saturdayLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        daysLayout->addWidget(saturdayLabel);


        verticalLayout->addWidget(daysWidget);

        calendarLayout = new QVBoxLayout();
        calendarLayout->setObjectName("calendarLayout");

        verticalLayout->addLayout(calendarLayout);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Schedule Manager", nullptr));
        searchEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\354\235\274\354\240\225 \352\262\200\354\203\211", nullptr));
        searchButton->setText(QCoreApplication::translate("MainWindow", "\352\262\200\354\203\211", nullptr));
        prevMonthButton->setText(QString());
        yearComboBox->setPlaceholderText(QString());
        yearLabel->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:14pt; color:#060b6d;\">   \353\205\204</span></p></body></html>", nullptr));
        monthLabel->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:14pt; color:#060b6d;\">\354\233\224 </span></p></body></html>", nullptr));
        nextMonthButton->setText(QString());
        sundayLabel->setText(QCoreApplication::translate("MainWindow", "\354\235\274", nullptr));
        mondayLabel->setText(QCoreApplication::translate("MainWindow", "\354\233\224", nullptr));
        tuesdayLabel->setText(QCoreApplication::translate("MainWindow", "\355\231\224", nullptr));
        wednesdayLabel->setText(QCoreApplication::translate("MainWindow", "\354\210\230", nullptr));
        thursdayLabel->setText(QCoreApplication::translate("MainWindow", "\353\252\251", nullptr));
        fridayLabel->setText(QCoreApplication::translate("MainWindow", "\352\270\210", nullptr));
        saturdayLabel->setText(QCoreApplication::translate("MainWindow", "\355\206\240", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
