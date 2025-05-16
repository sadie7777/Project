#ifndef EVENT_H
#define EVENT_H

#include <QString>
#include <QTime>

enum class Priority {
    I = 1,
    II = 2,
    III = 3
};

enum class Category {
    Work = 0,
    Personal = 1
};

struct Event {
    QString title;
    QString description;
    Priority priority;
    Category category;
    QTime time;
};

#endif // EVENT_H
