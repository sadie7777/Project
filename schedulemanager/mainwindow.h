#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "event.h"
#include "customcalendar.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextCharFormat>
#include <QColor>
#include <QMessageBox>
#include <QRegularExpression>
#include <QMainWindow>
#include <QMap>
#include <QDate>
#include <QList>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void dateSelected(const QDate &date);
    void searchEvents();
    void onPrevMonthClicked();
    void onNextMonthClicked();
    void onDateSelectionChanged();


private:
    Ui::MainWindow *ui;
    CustomCalendar *calendar;
    QMap<QDate, QList<Event>> events;
    
    QString priorityToString(Priority p);
    QString categoryToString(Category c);

    void initializeComboBoxes();
    void loadEvents();
    void saveEvents();
    void updateCalendarCellFormat();
};

#endif // MAINWINDOW_H
