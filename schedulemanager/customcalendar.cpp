#include "customcalendar.h"


// ──────────────────────────────────────────────
// 생성자: 초기 설정 및 네비게이션 바 커스터마이징
// ──────────────────────────────────────────────
CustomCalendar::CustomCalendar(QWidget *parent)
    : QCalendarWidget(parent)
{
    // 주 번호 숨기기
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    // 격자선 표시
    setGridVisible(true);


    // 이전/다음 월 버튼 아이콘 변경 및 스타일 적용
    QToolButton *prevButton = this->findChild<QToolButton *>("qt_calendar_prevmonth");
    QToolButton *nextButton = this->findChild<QToolButton *>("qt_calendar_nextmonth");

    if (prevButton && nextButton) {
        prevButton->setIcon(QIcon(":/icons/left.png"));
        nextButton->setIcon(QIcon(":/icons/right.png"));

        QSize iconSize(24, 24);
        prevButton->setIconSize(iconSize);
        nextButton->setIconSize(iconSize);

        prevButton->setStyleSheet("background: transparent;");
        nextButton->setStyleSheet("background: transparent;");
    }
}

// ──────────────────────────────────────────────
// 외부에서 이벤트 데이터를 전달받아 저장
// ──────────────────────────────────────────────
void CustomCalendar::setEvents(const QMap<QDate, QList<Event>> &eventMap)
{
    events = eventMap;
    updateCells();  // 캘린더 다시 그리기
}

// ──────────────────────────────────────────────
// 셀을 커스터마이징하여 날짜 및 이벤트 표시
// ──────────────────────────────────────────────
void CustomCalendar::paintCell(QPainter *painter, const QRect &rect, QDate date) const
{
    //선택 글자 highlight
    if (date == selectedDate()) {
        QPen pen(QColor("#fbb584"));  // 강조할 테두리 색
        pen.setWidth(2);              // 테두리 두께 설정
        painter->setPen(pen);         // painter에 펜 설정
        painter->setBrush(Qt::NoBrush); // 내부는 채우지 않음
        painter->drawRect(rect.adjusted(2, 2, -1, -1));  // 테두리만 그림
    } else {
        painter->fillRect(rect, palette().base());  // 기본 배경
    }

    // 테두리
    painter->setPen(palette().mid().color());
    painter->drawRect(rect);

    // 날짜 텍스트 (왼쪽 위에)
    painter->save();
    QFont dateFont = painter->font();
    dateFont.setPointSize(10);
    dateFont.setBold(true);
    painter->setFont(dateFont);

    // 날짜별 색상 설정
    int dayOfWeek = date.dayOfWeek();
    if (date.month() == monthShown()) {
        if (date == QDate::currentDate()) {
            painter->setPen(QColor("#318d87"));  // 오늘
        } else if (dayOfWeek == 7) {
            painter->setPen(Qt::red);           // 일요일
        } else if (dayOfWeek == 6) {
            painter->setPen(Qt::blue);          // 토요일
        } else {
            painter->setPen(Qt::black);         // 평일
        }
    }

    QTextOption option(Qt::AlignTop | Qt::AlignLeft);
    QRect textRect = rect.adjusted(4, 2, -2, -2);
    painter->drawText(textRect, QString::number(date.day()), option);

    //  이벤트 정보 렌더링 (Priority I만 표시)
    if (events.contains(date)) {
        const QList<Event> &dateEvents = events[date];

        // Priority I인 Work, Personal 분리
        QList<Event> workEvents, personalEvents;
        for (const Event &event : dateEvents) {
            if (event.priority == Priority::I) {
                (event.category == Category::Work ? workEvents : personalEvents).append(event);
            }
        }

        // 시간순 정렬
        std::sort(workEvents.begin(), workEvents.end(), [](const Event &a, const Event &b) {
            return a.time < b.time;
        });
        std::sort(personalEvents.begin(), personalEvents.end(), [](const Event &a, const Event &b) {
            return a.time < b.time;
        });

        int contentY = rect.top() + 20;

        //  Work 이벤트 렌더링
        if (!workEvents.isEmpty()) {
            QFont labelFont = painter->font();
            labelFont.setPointSize(8);
            labelFont.setBold(true);
            painter->setFont(labelFont);
            painter->setPen(QColor("#f37321"));
            painter->drawText(QRect(rect.left() + 4, contentY, rect.width() - 8, 14),
                              Qt::AlignLeft | Qt::AlignTop, "Work:");
            contentY += 16;

            QFont eventFont = painter->font();
            eventFont.setBold(false);
            painter->setFont(eventFont);
            for (int i = 0; i < qMin(2, workEvents.size()); ++i) {
                QString title = workEvents[i].title;
                if (title.length() > 14) title = title.left(6) + "...";
                QString text = workEvents[i].time.toString("HH:mm") + " " + title;
                painter->drawText(QRect(rect.left() + 4, contentY, rect.width() - 8, 14),
                                  Qt::AlignLeft | Qt::AlignTop, text);
                contentY += 14;
            }
            if (workEvents.size() > 2) {
                painter->drawText(QRect(rect.left() + 4, contentY, rect.width() - 8, 14),
                                  Qt::AlignLeft | Qt::AlignTop, "...");
                contentY += 14;
            }
        }

        //  Personal 이벤트 렌더링
        if (!personalEvents.isEmpty()) {
            QFont labelFont = painter->font();
            labelFont.setPointSize(8);
            labelFont.setBold(true);
            painter->setFont(labelFont);
            painter->setPen(QColor("#003865"));
            painter->drawText(QRect(rect.left() + 4, contentY, rect.width() - 8, 14),
                              Qt::AlignLeft | Qt::AlignTop, "Personal:");
            contentY += 16;

            QFont eventFont = painter->font();
            eventFont.setBold(false);
            painter->setFont(eventFont);
            for (int i = 0; i < qMin(2, personalEvents.size()); ++i) {
                QString title = personalEvents[i].title;
                if (title.length() > 14) title = title.left(6) + "...";
                QString text = personalEvents[i].time.toString("HH:mm") + " " + title;
                painter->drawText(QRect(rect.left() + 4, contentY, rect.width() - 8, 14),
                                  Qt::AlignLeft | Qt::AlignTop, text);
                contentY += 14;
            }
            if (personalEvents.size() > 2) {
                painter->drawText(QRect(rect.left() + 4, contentY, rect.width() - 8, 14),
                                  Qt::AlignLeft | Qt::AlignTop, "...");
            }
        }
    }

    painter->restore();
}
