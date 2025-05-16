#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include "event.h"
#include <QMessageBox>
#include <QDialog>
#include <QDate>
#include <QList>
#include <QListWidgetItem>
#include <QListWidget>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextEdit>
#include <QComboBox>
#include <QTimeEdit>

namespace Ui {
class EventDialog;
}

class EventDialog : public QDialog
{
    Q_OBJECT

public:
    EventDialog(const QDate &date, const QList<Event> &eventList, QWidget *parent = nullptr);
    ~EventDialog();
    QList<Event> getEvents() const;

private slots:
    void addEvent();
    void deleteEvent();
    void updateEventDetails(int currentRow);
    void saveModifiedEvent(int row);
    void filterEvents(int index);
    void editSelectedEvent();

private:
    Ui::EventDialog *ui;
    Event originalEvent;
    QDate currentDate;
    QList<Event> events;
    QList<Event> filteredEvents;

    QLineEdit *titleEdit;
    QTextEdit *descriptionEdit;
    QTimeEdit *timeEdit;
    QComboBox *priorityCombo;
    QComboBox *categoryCombo;

    QListWidget *eventsListWidget;
    QComboBox *filterCombo;

    QPushButton *addButton;
    QPushButton *deleteButton;
    QPushButton *editButton;

    
    void updateEventList();
    void applyFilter(int filterIndex);
};

#endif // EVENTDIALOG_H
