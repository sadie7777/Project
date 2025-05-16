#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "customcalendar.h"
#include "eventdialog.h"

// ──────────────────────────────────────────────
// 생성자: UI 초기화 및 signal-slot 연결
// ──────────────────────────────────────────────
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // 캘린더 설정
    calendar = new CustomCalendar(this);
    calendar->setGridVisible(true);
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    calendar->setNavigationBarVisible(false);
    calendar->setHorizontalHeaderFormat(QCalendarWidget::NoHorizontalHeader);
    calendar->setSelectedDate(QDate::currentDate());
    
    // 캘린더를 UI에 추가
    ui->calendarLayout->addWidget(calendar);
    
    // 콤보박스 초기화
    initializeComboBoxes();
    
    // 시그널-슬롯 연결
    connect(calendar, &QCalendarWidget::clicked, this, &MainWindow::dateSelected);
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::searchEvents);
    connect(ui->searchEdit, &QLineEdit::returnPressed, this, &MainWindow::searchEvents);
    connect(ui->prevMonthButton, &QPushButton::clicked, this, &MainWindow::onPrevMonthClicked);
    connect(ui->nextMonthButton, &QPushButton::clicked, this, &MainWindow::onNextMonthClicked);
    connect(ui->yearComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onDateSelectionChanged);
    connect(ui->monthComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onDateSelectionChanged);

    // 데이터 로딩 및 초기 캘린더 갱신
    loadEvents();
    updateCalendarCellFormat();
}

MainWindow::~MainWindow()
{
    saveEvents();
    delete ui;
}

// ──────────────────────────────────────────────
// 콤보박스 초기화
// ──────────────────────────────────────────────
void MainWindow::initializeComboBoxes()
{
    // 연도 콤보박스 초기화
    // int currentYear = QDate::currentDate().year();
    for (int year = 1950; year <= 2070; year++) {
        ui->yearComboBox->addItem(QString::number(year));
    }
    
    // 월 콤보박스 초기화
    for (int month = 1; month <= 12; month++) {
        ui->monthComboBox->addItem(QString::number(month));
    }
    
    // 현재 날짜로 설정
    QDate currentDate = QDate::currentDate();
    ui->yearComboBox->setCurrentText(QString::number(currentDate.year()));
    ui->monthComboBox->setCurrentIndex(currentDate.month() - 1);
}

// ──────────────────────────────────────────────
// 이전 월 버튼 클릭
// ──────────────────────────────────────────────
void MainWindow::onPrevMonthClicked()
{
    QDate newDate = calendar->selectedDate().addMonths(-1);
    newDate.setDate(newDate.year(), newDate.month(), 1);
    calendar->setSelectedDate(newDate);
    ui->yearComboBox->setCurrentText(QString::number(newDate.year()));
    ui->monthComboBox->setCurrentIndex(newDate.month() - 1);
}

// ──────────────────────────────────────────────
// 다음 월 버튼 클릭
// ──────────────────────────────────────────────
void MainWindow::onNextMonthClicked()
{
    QDate newDate = calendar->selectedDate().addMonths(1);
    newDate.setDate(newDate.year(), newDate.month(), 1);
    calendar->setSelectedDate(newDate);
    ui->yearComboBox->setCurrentText(QString::number(newDate.year()));
    ui->monthComboBox->setCurrentIndex(newDate.month() - 1);
}

// ──────────────────────────────────────────────
// 연도/월 선택 변경
// ──────────────────────────────────────────────
void MainWindow::onDateSelectionChanged()
{
    int year = ui->yearComboBox->currentText().toInt();
    int month = ui->monthComboBox->currentIndex() + 1;
    QDate newDate(year, month, 1);
    calendar->setSelectedDate(newDate);
    calendar->setCurrentPage(year, month);
}

// ──────────────────────────────────────────────
// 일정 JSON 로딩
// ──────────────────────────────────────────────
void MainWindow::loadEvents()
{
    QFile file("events.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray array = doc.array();
        for (const QJsonValue &value : array) {
            QJsonObject obj = value.toObject();
            QDate date = QDate::fromString(obj["date"].toString(), Qt::ISODate);
            Event event;
            event.title = obj["title"].toString();
            event.description = obj["description"].toString();
            event.priority = static_cast<Priority>(obj["priority"].toInt());
            event.category = static_cast<Category>(obj["category"].toInt());
            event.time = QTime::fromString(obj["time"].toString(), "HH:mm");
            events[date].append(event);
        }
        file.close();
    }
}

// ──────────────────────────────────────────────
// 일정 JSON 저장
// ──────────────────────────────────────────────
void MainWindow::saveEvents()
{
    QJsonArray eventArray;
    for (auto it = events.begin(); it != events.end(); ++it) {
        QDate date = it.key();
        const QList<Event> &list = it.value();
        for (const Event &event : list) {
            QJsonObject obj;
            obj["date"] = date.toString(Qt::ISODate);
            obj["title"] = event.title;
            obj["description"] = event.description;
            obj["priority"] = static_cast<int>(event.priority);
            obj["category"] = static_cast<int>(event.category);
            obj["time"] = event.time.toString("HH:mm");
            eventArray.append(obj);
        }
    }
    QFile file("events.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(eventArray).toJson());
        file.close();
    }
}

// ──────────────────────────────────────────────
// 우선순위/카테고리 문자열 변환
// ──────────────────────────────────────────────
QString MainWindow::priorityToString(Priority p) {
    switch (p) {
    case Priority::I: return "I";
    case Priority::II: return "II";
    case Priority::III: return "III";
    default: return "Unknown";
    }
}

QString MainWindow::categoryToString(Category c) {
    switch (c) {
    case Category::Work: return "Work";
    case Category::Personal: return "Personal";
    default: return "Unknown";
    }
}

// ──────────────────────────────────────────────
// 검색 기능
// ──────────────────────────────────────────────
void MainWindow::searchEvents()
{
    QString input = ui->searchEdit->text().trimmed();
    if (input.isEmpty()) {
        QMessageBox::information(this, "알림", "검색어를 입력하세요.");
        return;
    }

    QDate filterDate;
    QString priorityStr, categoryStr, keyword;
    QStringList tokens = input.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    for (const QString &token : tokens) {
        if (token.length() == 6 && token[0].isDigit()) {
            QDate fixed = QDate(2000 + token.mid(0, 2).toInt(), token.mid(2, 2).toInt(), token.mid(4, 2).toInt());
            if (fixed.isValid()) { filterDate = fixed; continue; }
        }

        QDate parsed;
        for (const QString &fmt : QStringList{"yyyy-MM-dd", "yy.MM.dd.", "yyMMdd"}) {
            parsed = QDate::fromString(token, fmt);
            if (parsed.isValid()) {
                if (parsed.year() < 100) parsed.setDate(parsed.year() + 2000, parsed.month(), parsed.day());
                filterDate = parsed;
                break;
            }
        }
        if (parsed.isValid()) continue;

        if (token == "I" || token == "II" || token == "III") priorityStr = token;
        else if (token.toLower() == "work" || token.toLower() == "personal") categoryStr = token.toLower();
        else keyword += token + " ";
    }

    keyword = keyword.trimmed();
    QList<QString> matchedResults;

    for (auto it = events.begin(); it != events.end(); ++it) {
        QDate date = it.key();
        for (const Event &event : it.value()) {
            bool match = true;
            if (filterDate.isValid() && date != filterDate) match = false;
            if (!priorityStr.isEmpty() && priorityToString(event.priority) != priorityStr) match = false;
            if (!categoryStr.isEmpty() && categoryToString(event.category).toLower() != categoryStr) match = false;
            if (!keyword.isEmpty() &&
                !event.title.contains(keyword, Qt::CaseInsensitive) &&
                !event.description.contains(keyword, Qt::CaseInsensitive)) match = false;

            if (match) {
                matchedResults.append(QString("%1 (%2) - %3")
                                          .arg(date.toString("yyyy-MM-dd"))
                                          .arg(event.time.toString("HH:mm"))
                                          .arg(event.title));
            }
        }
    }

    QMessageBox::information(this, "검색 결과",
                             matchedResults.isEmpty()
                                 ? "일치하는 일정이 없습니다."
                                 : "검색 결과:\n\n" + matchedResults.join("\n"));
}

// ──────────────────────────────────────────────
// 날짜 클릭 시 일정 다이얼로그 열기
// ──────────────────────────────────────────────
void MainWindow::dateSelected(const QDate &date)
{
    EventDialog dialog(date, events[date], this);
    if (dialog.exec() == QDialog::Accepted) {
        events[date] = dialog.getEvents();  //수정된 이벤트 받음
        saveEvents();                       // 파일로 저장
        updateCalendarCellFormat();         // 화면 업데이트
    }
}

// ──────────────────────────────────────────────
// 캘린더 이벤트 반영
// ──────────────────────────────────────────────
void MainWindow::updateCalendarCellFormat()
{
    calendar->setEvents(events);
}

