#ifndef CUSTOMCALENDAR_H
#define CUSTOMCALENDAR_H

#include "event.h"
#include <QPainter>
#include <QTextOption>
#include <QFont>
#include <QPalette>
#include <QDebug>
#include <QToolButton>
#include <QCalendarWidget>
#include <QMap>
#include <QDate>
#include <QList>


class CustomCalendar : public QCalendarWidget
{
    Q_OBJECT

public:
    CustomCalendar(QWidget *parent = nullptr);
    void setEvents(const QMap<QDate, QList<Event>> &eventMap);

protected:
    void paintCell(QPainter *painter, const QRect &rect, QDate date) const override;

private:
    QMap<QDate, QList<Event>> events;
};

#endif // CUSTOMCALENDAR_H
