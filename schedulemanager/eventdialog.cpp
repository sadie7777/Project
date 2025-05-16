#include "eventdialog.h"
#include "ui_eventdialog.h"

// ──────────────────────────────────────────────
// Constructor: UI 초기 설정 및 이벤트 연결
// ──────────────────────────────────────────────
EventDialog::EventDialog(const QDate &date, const QList<Event> &eventList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EventDialog)
    , currentDate(date)
    , events(eventList)
    , filteredEvents(eventList)
{
    ui->setupUi(this);
    
    setWindowTitle("Events for " + date.toString("yyyy-MM-dd"));
    
    // 날짜 표시 설정
    ui->dateLabel->setText("Date: " + date.toString("yyyy-MM-dd"));
    
    // 필터 콤보박스 초기화
    ui->filterCombo->addItem("All Events", -1);
    ui->filterCombo->addItem("Work Events", static_cast<int>(Category::Work));
    ui->filterCombo->addItem("Personal Events", static_cast<int>(Category::Personal));
    ui->filterCombo->addItem("Priority I", static_cast<int>(Priority::I));
    ui->filterCombo->addItem("Priority II", static_cast<int>(Priority::II));
    ui->filterCombo->addItem("Priority III", static_cast<int>(Priority::III));

    // 우선순위 콤보박스 초기화
    ui->priorityCombo->addItem("I (Highest)", static_cast<int>(Priority::I));
    ui->priorityCombo->addItem("II", static_cast<int>(Priority::II));
    ui->priorityCombo->addItem("III", static_cast<int>(Priority::III));

    // 카테고리 콤보박스 초기화
    ui->categoryCombo->addItem("Work", static_cast<int>(Category::Work));
    ui->categoryCombo->addItem("Personal", static_cast<int>(Category::Personal));

    // 시간 설정
    ui->timeEdit->setDisplayFormat("HH:mm");
    ui->timeEdit->setTime(QTime::currentTime());
    
    // 필터 적용 및 이벤트 리스트 업데이트
    ui->eventsListWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->filterCombo->setCurrentIndex(0);
    applyFilter(0);
    updateEventList();
    
    // 시그널-슬롯 연결
    connect(ui->addButton, &QPushButton::clicked, this, &EventDialog::addEvent);
    connect(ui->deleteButton, &QPushButton::clicked, this, &EventDialog::deleteEvent);
    connect(ui->eventsListWidget, &QListWidget::currentRowChanged, this, &EventDialog::updateEventDetails);
    connect(ui->filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EventDialog::filterEvents);
    connect(ui->editButton, &QPushButton::clicked, this, &EventDialog::editSelectedEvent);
    connect(ui->okButton, &QPushButton::clicked, this, [=]() {
        int row = ui->eventsListWidget->currentRow();
        saveModifiedEvent(row);     //수정 내용 반영
        accept();                   //팝업 닫기
    });


}

EventDialog::~EventDialog()
{
    delete ui;
}

// ──────────────────────────────────────────────
// Event 추가
// ──────────────────────────────────────────────
void EventDialog::addEvent()
{
    QString title = ui->titleEdit->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter an event title.");
        return;
    }

    Event event;
    event.title = title;
    event.description = ui->descriptionEdit->toPlainText();
    event.time = ui->timeEdit->time();
    event.priority = static_cast<Priority>(ui->priorityCombo->currentData().toInt());
    event.category = static_cast<Category>(ui->categoryCombo->currentData().toInt());

    events.append(event);
    applyFilter(ui->filterCombo->currentIndex());
    updateEventList();

    ui->titleEdit->clear();
    ui->descriptionEdit->clear();
    ui->timeEdit->setTime(QTime::currentTime());
}

// ──────────────────────────────────────────────
// Event 삭제
// ──────────────────────────────────────────────
void EventDialog::deleteEvent()
{
    QVector<Event> toDelete;

    for (int i = 0; i < ui->eventsListWidget->count(); ++i) {
        QListWidgetItem *item = ui->eventsListWidget->item(i);
        if (item->checkState() == Qt::Checked) {
            Event e = item->data(Qt::UserRole).value<Event>();
            toDelete.append(e);
        }
    }

    if (toDelete.isEmpty()) {
        QMessageBox::warning(this, "알림", "삭제할 일정을 선택해 주세요.");
        return;
    }

    // JSON 로드
    QFile file("events.json");
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "에러", "파일 열기에 실패했습니다.");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray eventArray = doc.array();

    // 삭제 대상 제거
    for (const Event &e : toDelete) {
        for (int i = 0; i < eventArray.size(); ++i) {
            QJsonObject obj = eventArray[i].toObject();
            if (obj["title"].toString() == e.title &&
                obj["time"].toString() == e.time.toString("HH:mm") &&
                obj["priority"].toInt() == static_cast<int>(e.priority) &&
                obj["category"].toString() == (e.category == Category::Work ? "Work" : "Personal")) {
                eventArray.removeAt(i);
                break;
            }
        }
    }

    // 저장
    QFile outFile("events.json");
    if (outFile.open(QIODevice::WriteOnly)) {
        QJsonDocument outDoc(eventArray);
        outFile.write(outDoc.toJson());
        outFile.close();
    } else {
        QMessageBox::critical(this, "에러", "파일 저장에 실패했습니다.");
    }

    // 메모리 리스트에서도 삭제
    for (const Event &e : toDelete) {
        for (int i = 0; i < events.size(); ++i) {
            if (events[i].title == e.title &&
                events[i].time == e.time &&
                events[i].priority == e.priority &&
                events[i].category == e.category) {
                events.removeAt(i);
                break;
            }
        }
    }

    applyFilter(ui->filterCombo->currentIndex());
    updateEventList();
}




// ──────────────────────────────────────────────
// Event 세부 정보 표시
// ──────────────────────────────────────────────
void EventDialog::updateEventDetails(int currentRow)
{
    if (currentRow >= 0 && currentRow < filteredEvents.size()) {
        originalEvent = filteredEvents[currentRow];
        const Event &event = filteredEvents[currentRow];
        ui->titleEdit->setText(filteredEvents[currentRow].title);
        ui->descriptionEdit->setText(event.description);
        ui->timeEdit->setTime(event.time);
        
        // 우선순위 설정
        int priorityIndex = static_cast<int>(event.priority) - 1;
        ui->priorityCombo->setCurrentIndex(priorityIndex);
        
        // 카테고리 설정
        ui->categoryCombo->setCurrentIndex(static_cast<int>(event.category));
    }
}

// ──────────────────────────────────────────────
// 필터링 기능 적용
// ──────────────────────────────────────────────
void EventDialog::filterEvents(int index)
{
    applyFilter(index);
    updateEventList();
}

void EventDialog::applyFilter(int filterIndex)
{
    filteredEvents.clear();
    if (filterIndex == 0) {
        filteredEvents = events;
    } else if (filterIndex == 1 || filterIndex == 2) {
        Category category = static_cast<Category>(ui->filterCombo->currentData().toInt());
        for (const Event &event : events) {
            if (event.category == category) {
                filteredEvents.append(event);
            }
        }
    } else {
        Priority priority = static_cast<Priority>(ui->filterCombo->currentData().toInt());
        for (const Event &event : events) {
            if (event.priority == priority) {
                filteredEvents.append(event);
            }
        }
    }
}

// ──────────────────────────────────────────────
// Event 리스트 업데이트 및 정렬
// ──────────────────────────────────────────────
void EventDialog::updateEventList()
{
    ui->eventsListWidget->clear();

    std::sort(filteredEvents.begin(), filteredEvents.end(), [](const Event &a, const Event &b) {
        if (static_cast<int>(a.priority) != static_cast<int>(b.priority)) {
            return static_cast<int>(a.priority) < static_cast<int>(b.priority);
        }
        if (static_cast<int>(a.category) != static_cast<int>(b.category)) {
            return static_cast<int>(a.category) < static_cast<int>(b.category);
        }
        return a.time < b.time;
    });

    for (const Event &event : filteredEvents) {
        QString priorityStr;
        switch (event.priority) {
        case Priority::I: priorityStr = "I"; break;
        case Priority::II: priorityStr = "II"; break;
        case Priority::III: priorityStr = "III"; break;
        }

        QString categoryStr = (event.category == Category::Work) ? "Work" : "Personal";

        QString displayText = QString("[%1] [%2] %3 - %4")
                                  .arg(priorityStr)
                                  .arg(categoryStr)
                                  .arg(event.time.toString("HH:mm"))
                                  .arg(event.title);

        QListWidgetItem *item = new QListWidgetItem(displayText);

        // 체크박스 추가
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);

        // 텍스트 색상 설정
        item->setForeground(event.category == Category::Work ? QColor("#f37321") : QColor("#003865"));

        // 삭제 이벤트 인식
        item->setData(Qt::UserRole, QVariant::fromValue(event));

        ui->eventsListWidget->addItem(item);
    }
}


// ──────────────────────────────────────────────
// Event 수정 기능
// ──────────────────────────────────────────────
void EventDialog::editSelectedEvent()
{
    int row = ui->eventsListWidget->currentRow();
    if (row < 0 || row >= filteredEvents.size()) {
        QMessageBox::warning(this, "Error", "수정할 일정을 선택하세요.");
        return;
    }

    Event modified;
    modified.title = ui->titleEdit->text();
    modified.description = ui->descriptionEdit->toPlainText();
    modified.time = ui->timeEdit->time();
    modified.priority = static_cast<Priority>(ui->priorityCombo->currentData().toInt());
    modified.category = static_cast<Category>(ui->categoryCombo->currentData().toInt());

    filteredEvents[row] = modified;

    // 원본에서도 찾아서 수정
    for (Event &event : events) {
        if (event.title == originalEvent.title &&
            event.time == originalEvent.time &&
            event.priority == originalEvent.priority &&
            event.category == originalEvent.category) {
            event = modified;
            break;
        }
    }

    updateEventList();
}


// ──────────────────────────────────────────────
// Getter: 수정된 이벤트 리스트 반환
// ──────────────────────────────────────────────
void EventDialog::saveModifiedEvent(int row)
{
    if (row >= 0 && row < filteredEvents.size()) {
        Event &event = filteredEvents[row];

        // ui 사용
        event.title = ui->titleEdit->text();
        event.description = ui->descriptionEdit->toPlainText();
        event.time = ui->timeEdit->time();
        event.priority = static_cast<Priority>(ui->priorityCombo->currentData().toInt());
        event.category = static_cast<Category>(ui->categoryCombo->currentData().toInt());

        // 원본 리스트에 반영
        for (int i = 0; i < events.size(); ++i) {
            if (events[i].title == event.title &&
                events[i].time == event.time &&
                events[i].priority == event.priority &&
                events[i].category == event.category) {
                events[i] = event;
                break;
            }
        }
    }
}


QList<Event> EventDialog::getEvents() const
{
    return events;
}

