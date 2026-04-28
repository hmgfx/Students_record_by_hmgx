#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QFont>
#include <QFrame>
#include <QStatusBar>

// ============================================================
//  Constructor / Destructor
// ============================================================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("  Student Management System  |  HaroonMughalGFX");
    setMinimumSize(950, 680);
    setupUI();
    applyStyle();
    statusBar()->showMessage("Ready  —  Students loaded: 0");
}

MainWindow::~MainWindow() {}

// ============================================================
//  UI Setup
// ============================================================
void MainWindow::setupUI()
{
    tabs = new QTabWidget(this);
    tabs->setDocumentMode(true);
    tabs->addTab(createAddTab(),  "  +  Student Add Karo  ");
    tabs->addTab(createViewTab(), "  =  Records Dekho  ");
    setCentralWidget(tabs);
}

// ─────────────────────────────────────────
//  TAB 1 : Add Student
// ─────────────────────────────────────────
QWidget* MainWindow::createAddTab()
{
    QWidget* page = new QWidget();
    QVBoxLayout* root = new QVBoxLayout(page);
    root->setSpacing(16);
    root->setContentsMargins(30, 25, 30, 25);

    // ── Header ──────────────────────────────
    QLabel* header = new QLabel("Student Information Form");
    header->setObjectName("pageHeader");
    header->setAlignment(Qt::AlignCenter);
    root->addWidget(header);

    // ── Personal Info Group ──────────────────
    QGroupBox* infoBox = new QGroupBox("  Personal Info");
    QGridLayout* infoGrid = new QGridLayout(infoBox);
    infoGrid->setSpacing(12);

    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("e.g.  Ali Hassan");

    rollEdit = new QLineEdit();
    rollEdit->setPlaceholderText("e.g.  CS-2024-001");

    infoGrid->addWidget(new QLabel("Student Name:"), 0, 0);
    infoGrid->addWidget(nameEdit,                    0, 1);
    infoGrid->addWidget(new QLabel("Roll Number:"),  0, 2);
    infoGrid->addWidget(rollEdit,                    0, 3);
    root->addWidget(infoBox);

    // ── Marks Group ─────────────────────────
    QGroupBox* marksBox = new QGroupBox("  Subject Marks  (Each out of 100)");
    QGridLayout* marksGrid = new QGridLayout(marksBox);
    marksGrid->setSpacing(12);

    auto makeSpin = [](double min, double max) -> QDoubleSpinBox* {
        QDoubleSpinBox* s = new QDoubleSpinBox();
        s->setRange(min, max);
        s->setDecimals(1);
        s->setSuffix("  pts");
        return s;
    };

    mathSpin        = makeSpin(0, 100);
    physicsSpin     = makeSpin(0, 100);
    englishSpin     = makeSpin(0, 100);
    programmingSpin = makeSpin(0, 100);
    islamiatSpin    = makeSpin(0, 100);

    marksGrid->addWidget(new QLabel("Mathematics:"),  0, 0); marksGrid->addWidget(mathSpin,        0, 1);
    marksGrid->addWidget(new QLabel("Physics:"),       0, 2); marksGrid->addWidget(physicsSpin,     0, 3);
    marksGrid->addWidget(new QLabel("English:"),       1, 0); marksGrid->addWidget(englishSpin,     1, 1);
    marksGrid->addWidget(new QLabel("Programming:"),   1, 2); marksGrid->addWidget(programmingSpin, 1, 3);
    marksGrid->addWidget(new QLabel("Islamiat:"),      2, 0); marksGrid->addWidget(islamiatSpin,    2, 1);
    root->addWidget(marksBox);

    // ── Live Preview ─────────────────────────
    previewLbl = new QLabel("  Total: 0 / 500     Percentage: 0.0%     Grade:  --");
    previewLbl->setObjectName("previewBar");
    previewLbl->setAlignment(Qt::AlignCenter);
    root->addWidget(previewLbl);

    // ── Action Buttons ───────────────────────
    QHBoxLayout* btnRow = new QHBoxLayout();
    QPushButton* addBtn   = new QPushButton("  Add Student");
    QPushButton* clearBtn = new QPushButton("  Clear Form");
    addBtn->setObjectName("btnAdd");
    clearBtn->setObjectName("btnClear");
    addBtn->setMinimumHeight(42);
    clearBtn->setMinimumHeight(42);
    btnRow->addWidget(addBtn);
    btnRow->addWidget(clearBtn);
    root->addLayout(btnRow);

    root->addStretch();

    // ── Signal Connections ───────────────────
    connect(addBtn,   &QPushButton::clicked, this, &MainWindow::onAddStudent);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::onClearForm);

    auto spinChanged = QOverload<double>::of(&QDoubleSpinBox::valueChanged);
    connect(mathSpin,        spinChanged, this, &MainWindow::onMarksChanged);
    connect(physicsSpin,     spinChanged, this, &MainWindow::onMarksChanged);
    connect(englishSpin,     spinChanged, this, &MainWindow::onMarksChanged);
    connect(programmingSpin, spinChanged, this, &MainWindow::onMarksChanged);
    connect(islamiatSpin,    spinChanged, this, &MainWindow::onMarksChanged);

    return page;
}

// ─────────────────────────────────────────
//  TAB 2 : View Records
// ─────────────────────────────────────────
QWidget* MainWindow::createViewTab()
{
    QWidget* page = new QWidget();
    QVBoxLayout* root = new QVBoxLayout(page);
    root->setSpacing(12);
    root->setContentsMargins(25, 20, 25, 20);

    // ── Search Bar ───────────────────────────
    QHBoxLayout* searchRow = new QHBoxLayout();
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("  Search by Name or Roll Number...");
    searchEdit->setMinimumHeight(38);

    QPushButton* searchBtn  = new QPushButton("  Search");
    QPushButton* showAllBtn = new QPushButton("  Show All");
    searchBtn->setObjectName("btnSearch");
    searchBtn->setMinimumHeight(38);
    showAllBtn->setMinimumHeight(38);

    searchRow->addWidget(searchEdit, 3);
    searchRow->addWidget(searchBtn,  1);
    searchRow->addWidget(showAllBtn, 1);
    root->addLayout(searchRow);

    // ── Records Table ────────────────────────
    tableWidget = new QTableWidget();
    tableWidget->setColumnCount(9);
    tableWidget->setHorizontalHeaderLabels({
        "Roll No", "Name",
        "Math", "Physics", "English", "Programming", "Islamiat",
        "Percentage", "Grade"
    });
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setShowGrid(false);
    root->addWidget(tableWidget);

    // ── Stats Bar ────────────────────────────
    statsLbl = new QLabel("  Total Students: 0     |     Class Average: 0.0%");
    statsLbl->setObjectName("statsBar");
    statsLbl->setAlignment(Qt::AlignCenter);
    root->addWidget(statsLbl);

    // ── Action Buttons ───────────────────────
    QHBoxLayout* btnRow = new QHBoxLayout();
    QPushButton* deleteBtn = new QPushButton("  Delete Selected");
    QPushButton* saveBtn   = new QPushButton("  Save to CSV");
    QPushButton* loadBtn   = new QPushButton("  Load CSV");
    deleteBtn->setObjectName("btnDelete");
    saveBtn->setObjectName("btnSave");
    loadBtn->setObjectName("btnLoad");
    deleteBtn->setMinimumHeight(40);
    saveBtn->setMinimumHeight(40);
    loadBtn->setMinimumHeight(40);

    btnRow->addWidget(deleteBtn);
    btnRow->addWidget(saveBtn);
    btnRow->addWidget(loadBtn);
    root->addLayout(btnRow);

    // ── Connections ──────────────────────────
    connect(searchBtn,  &QPushButton::clicked, this, &MainWindow::onSearch);
    connect(showAllBtn, &QPushButton::clicked, this, [this]() {
        searchEdit->clear();
        refreshTable(students);
    });
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteStudent);
    connect(saveBtn,   &QPushButton::clicked, this, &MainWindow::onSave);
    connect(loadBtn,   &QPushButton::clicked, this, &MainWindow::onLoad);

    return page;
}

// ============================================================
//  SLOT: Add Student
// ============================================================
void MainWindow::onAddStudent()
{
    // Validation
    if (nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Field Missing", "Student ka naam zaroor bharo!");
        nameEdit->setFocus();
        return;
    }
    if (rollEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Field Missing", "Roll Number zaroor bharo!");
        rollEdit->setFocus();
        return;
    }

    // Duplicate roll-no check
    QString newRoll = rollEdit->text().trimmed().toUpper();
    for (const Student& s : students) {
        if (s.rollNo.toUpper() == newRoll) {
            QMessageBox::warning(this, "Duplicate Entry",
                QString("Roll Number  \"%1\"  pehle se exist karta hai!").arg(newRoll));
            return;
        }
    }

    // Build student
    Student s;
    s.name         = nameEdit->text().trimmed();
    s.rollNo       = newRoll;
    s.math         = mathSpin->value();
    s.physics      = physicsSpin->value();
    s.english      = englishSpin->value();
    s.programming  = programmingSpin->value();
    s.islamiat     = islamiatSpin->value();

    students.append(s);
    refreshTable(students);
    updateStats();
    onClearForm();

    statusBar()->showMessage(
        QString("Student added:  %1  (%2)  —  Grade: %3")
            .arg(s.name).arg(s.rollNo).arg(s.getGrade()));

    QMessageBox::information(this, "Student Added!",
        QString(
            "Name:        %1\n"
            "Roll No:     %2\n"
            "Total:       %3 / 500\n"
            "Percentage:  %4%\n"
            "Grade:       %5"
        )
        .arg(s.name)
        .arg(s.rollNo)
        .arg(s.getTotal())
        .arg(QString::number(s.getPercentage(), 'f', 1))
        .arg(s.getGrade())
    );

    tabs->setCurrentIndex(1);  // Auto-switch to records tab
}

// ============================================================
//  SLOT: Delete Student
// ============================================================
void MainWindow::onDeleteStudent()
{
    int row = tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Nothing Selected", "Pehle koi student row select karo!");
        return;
    }

    QString selRoll = tableWidget->item(row, 0)->text();
    QString selName = tableWidget->item(row, 1)->text();

    auto reply = QMessageBox::question(
        this, "Confirm Delete",
        QString("Kya aap  \"%1\"  (%2)  ko delete karna chahte hain?")
            .arg(selName).arg(selRoll),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        for (int i = 0; i < students.size(); ++i) {
            if (students[i].rollNo == selRoll) {
                students.removeAt(i);
                break;
            }
        }
        refreshTable(students);
        updateStats();
        statusBar()->showMessage(QString("Deleted: %1").arg(selName));
    }
}

// ============================================================
//  SLOT: Search
// ============================================================
void MainWindow::onSearch()
{
    QString q = searchEdit->text().trimmed().toLower();
    if (q.isEmpty()) { refreshTable(students); return; }

    QList<Student> results;
    for (const Student& s : students) {
        if (s.name.toLower().contains(q) || s.rollNo.toLower().contains(q))
            results.append(s);
    }

    refreshTable(results);
    statusBar()->showMessage(
        results.isEmpty()
            ? "No results found."
            : QString("%1 result(s) found for: \"%2\"").arg(results.size()).arg(q));
}

// ============================================================
//  SLOT: Clear Form
// ============================================================
void MainWindow::onClearForm()
{
    nameEdit->clear();
    rollEdit->clear();
    mathSpin->setValue(0);
    physicsSpin->setValue(0);
    englishSpin->setValue(0);
    programmingSpin->setValue(0);
    islamiatSpin->setValue(0);
    previewLbl->setText("  Total: 0 / 500     Percentage: 0.0%     Grade:  --");
    nameEdit->setFocus();
}

// ============================================================
//  SLOT: Marks Changed (live preview)
// ============================================================
void MainWindow::onMarksChanged()
{
    double total = mathSpin->value()
                 + physicsSpin->value()
                 + englishSpin->value()
                 + programmingSpin->value()
                 + islamiatSpin->value();
    double pct = (total / 500.0) * 100.0;

    Student tmp;
    tmp.math = mathSpin->value(); tmp.physics = physicsSpin->value();
    tmp.english = englishSpin->value(); tmp.programming = programmingSpin->value();
    tmp.islamiat = islamiatSpin->value();

    previewLbl->setText(
        QString("  Total: %1 / 500     Percentage: %2%     Grade:  %3")
            .arg(total)
            .arg(QString::number(pct, 'f', 1))
            .arg(tmp.getGrade())
    );
}

// ============================================================
//  SLOT: Save CSV
// ============================================================
void MainWindow::onSave()
{
    if (students.isEmpty()) {
        QMessageBox::warning(this, "Empty Data", "Save karne ke liye koi data nahi hai!");
        return;
    }

    QString path = QFileDialog::getSaveFileName(
        this, "Save Records", "students.csv", "CSV Files (*.csv)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "File save nahi ho saki!");
        return;
    }

    QTextStream out(&file);
    out << "Roll No,Name,Math,Physics,English,Programming,Islamiat,Percentage,Grade\n";

    for (const Student& s : students) {
        out << s.rollNo       << ","
            << s.name         << ","
            << s.math         << ","
            << s.physics      << ","
            << s.english      << ","
            << s.programming  << ","
            << s.islamiat     << ","
            << QString::number(s.getPercentage(), 'f', 1) << ","
            << s.getGrade()   << "\n";
    }
    file.close();

    QMessageBox::information(this, "Saved!", 
        QString("%1 student records CSV mein save ho gaye!").arg(students.size()));
    statusBar()->showMessage("File saved: " + path);
}

// ============================================================
//  SLOT: Load CSV
// ============================================================
void MainWindow::onLoad()
{
    QString path = QFileDialog::getOpenFileName(
        this, "Load Records", "", "CSV Files (*.csv)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "File load nahi ho saki!");
        return;
    }

    students.clear();
    QTextStream in(&file);
    in.readLine(); // Skip header row

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        QStringList p = line.split(',');
        if (p.size() < 7) continue;

        Student s;
        s.rollNo      = p[0].trimmed();
        s.name        = p[1].trimmed();
        s.math        = p[2].toDouble();
        s.physics     = p[3].toDouble();
        s.english     = p[4].toDouble();
        s.programming = p[5].toDouble();
        s.islamiat    = p[6].toDouble();
        students.append(s);
    }
    file.close();

    refreshTable(students);
    updateStats();

    QMessageBox::information(this, "Loaded!",
        QString("%1 students load ho gaye!").arg(students.size()));
    statusBar()->showMessage(
        QString("Loaded %1 records from: %2").arg(students.size()).arg(path));
}

// ============================================================
//  Helper: Refresh Table
// ============================================================
void MainWindow::refreshTable(const QList<Student>& list)
{
    tableWidget->setRowCount(0);

    for (const Student& s : list) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);

        auto cell = [&](int col, const QString& val) {
            QTableWidgetItem* item = new QTableWidgetItem(val);
            item->setTextAlignment(Qt::AlignCenter);
            tableWidget->setItem(row, col, item);
        };

        cell(0, s.rollNo);
        cell(1, s.name);
        cell(2, QString::number(s.math,    'f', 1));
        cell(3, QString::number(s.physics, 'f', 1));
        cell(4, QString::number(s.english, 'f', 1));
        cell(5, QString::number(s.programming, 'f', 1));
        cell(6, QString::number(s.islamiat,'f', 1));

        // Percentage with color
        double pct = s.getPercentage();
        QString grade = s.getGrade();

        QTableWidgetItem* pctItem = new QTableWidgetItem(
            QString::number(pct, 'f', 1) + "%");
        pctItem->setTextAlignment(Qt::AlignCenter);
        pctItem->setFont(QFont("Segoe UI", 10, QFont::Bold));

        QColor gradeColor;
        if      (grade == "A+") gradeColor = QColor("#00e676");
        else if (grade == "A")  gradeColor = QColor("#69f0ae");
        else if (grade == "B")  gradeColor = QColor("#40c4ff");
        else if (grade == "C")  gradeColor = QColor("#ffd740");
        else if (grade == "D")  gradeColor = QColor("#ff9100");
        else                    gradeColor = QColor("#ff5252");

        pctItem->setForeground(gradeColor);
        tableWidget->setItem(row, 7, pctItem);

        QTableWidgetItem* gradeItem = new QTableWidgetItem(grade);
        gradeItem->setTextAlignment(Qt::AlignCenter);
        gradeItem->setForeground(gradeColor);
        gradeItem->setFont(QFont("Segoe UI", 11, QFont::Bold));
        tableWidget->setItem(row, 8, gradeItem);

        tableWidget->setRowHeight(row, 36);
    }
}

// ============================================================
//  Helper: Update Stats Bar
// ============================================================
void MainWindow::updateStats()
{
    int count = students.size();
    statusBar()->showMessage(QString("Total records: %1").arg(count));

    if (count == 0) {
        statsLbl->setText("  Total Students: 0     |     Class Average: 0.0%");
        return;
    }

    double sumPct = 0;
    int    passed = 0;
    for (const Student& s : students) {
        sumPct += s.getPercentage();
        if (s.getGrade() != "F") passed++;
    }

    statsLbl->setText(
        QString("  Total Students: %1     |     Class Average: %2%     |     Passed: %3  /  %4")
            .arg(count)
            .arg(QString::number(sumPct / count, 'f', 1))
            .arg(passed)
            .arg(count)
    );
}

// ============================================================
//  Stylesheet — Dark Professional Theme
// ============================================================
void MainWindow::applyStyle()
{
    setStyleSheet(R"(

/* ── Base ─────────────────────────────── */
QMainWindow, QWidget {
    background-color: #12121f;
    color: #dce2f0;
    font-family: 'Segoe UI', Arial;
    font-size: 13px;
}

/* ── Tab Widget ──────────────────────── */
QTabWidget::pane {
    border: 1px solid #1e2a4a;
    background: #181828;
    border-radius: 0 6px 6px 6px;
}
QTabBar::tab {
    background: #0f1830;
    color: #7889b0;
    padding: 11px 24px;
    font-size: 13px;
    border-radius: 6px 6px 0 0;
    margin-right: 3px;
}
QTabBar::tab:selected {
    background: #e94560;
    color: white;
    font-weight: bold;
}
QTabBar::tab:hover:!selected {
    background: #1e2a4a;
    color: #a0b4d8;
}

/* ── Group Boxes ──────────────────────── */
QGroupBox {
    border: 1px solid #1e2a4a;
    border-radius: 8px;
    margin-top: 14px;
    padding: 14px 10px 10px 10px;
    color: #7899cc;
    font-weight: bold;
    font-size: 12px;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 12px;
    padding: 0 6px;
}

/* ── Inputs ──────────────────────────── */
QLineEdit, QDoubleSpinBox {
    background: #0f1830;
    border: 1px solid #1e2a4a;
    border-radius: 6px;
    padding: 7px 10px;
    color: #dce2f0;
    min-height: 30px;
    font-size: 13px;
}
QLineEdit:focus, QDoubleSpinBox:focus {
    border: 1.5px solid #e94560;
    background: #141d35;
}
QLineEdit::placeholder { color: #445070; }

/* ── Spin Box Buttons ─────────────────── */
QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
    background: #1e2a4a;
    border: none;
    border-radius: 3px;
    width: 18px;
}
QDoubleSpinBox::up-button:hover, QDoubleSpinBox::down-button:hover {
    background: #e94560;
}

/* ── Buttons ─────────────────────────── */
QPushButton {
    background: #1e2a4a;
    color: #a0b4d8;
    border: 1px solid #2a3a60;
    border-radius: 7px;
    padding: 8px 18px;
    font-size: 13px;
    font-weight: 500;
}
QPushButton:hover {
    background: #253355;
    color: white;
}
QPushButton:pressed {
    background: #0f1830;
}

QPushButton#btnAdd {
    background: #00b894;
    color: white;
    border: none;
    font-weight: bold;
    font-size: 14px;
}
QPushButton#btnAdd:hover    { background: #00a381; }
QPushButton#btnAdd:pressed  { background: #008f70; }

QPushButton#btnDelete {
    background: #e94560;
    color: white;
    border: none;
    font-weight: bold;
}
QPushButton#btnDelete:hover { background: #c73652; }

QPushButton#btnSave {
    background: #6c5ce7;
    color: white;
    border: none;
}
QPushButton#btnSave:hover { background: #5a4bd6; }

QPushButton#btnLoad  { border: 1px solid #4a5a80; }
QPushButton#btnSearch { background: #0f3460; color: white; border: none; }
QPushButton#btnSearch:hover { background: #1a4a8a; }

/* ── Table ───────────────────────────── */
QTableWidget {
    background: #0f1830;
    alternate-background-color: #141d35;
    border: 1px solid #1e2a4a;
    border-radius: 8px;
    gridline-color: transparent;
    selection-background-color: #e94560;
    selection-color: white;
    font-size: 13px;
}
QTableWidget::item { padding: 6px; }
QTableWidget::item:hover { background: #1e2a4a; }

QHeaderView::section {
    background: #0a1020;
    color: #7889b0;
    padding: 10px 6px;
    border: none;
    border-bottom: 2px solid #e94560;
    font-weight: bold;
    font-size: 12px;
    letter-spacing: 0.5px;
}

/* ── Labels ──────────────────────────── */
QLabel#pageHeader {
    font-size: 22px;
    font-weight: bold;
    color: #e94560;
    letter-spacing: 1px;
    padding: 8px;
}
QLabel#previewBar {
    background: #0f1830;
    border: 1px solid #00b894;
    border-radius: 8px;
    padding: 14px;
    font-size: 15px;
    color: #00e5b5;
    font-weight: bold;
}
QLabel#statsBar {
    background: #0f1830;
    border: 1px solid #1e2a4a;
    border-radius: 6px;
    padding: 9px;
    color: #7899cc;
    font-size: 13px;
}

/* ── Scrollbar ───────────────────────── */
QScrollBar:vertical {
    background: #0f1830;
    width: 8px;
    border-radius: 4px;
}
QScrollBar::handle:vertical {
    background: #e94560;
    border-radius: 4px;
    min-height: 30px;
}
QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical { height: 0; }

/* ── Status Bar ──────────────────────── */
QStatusBar {
    background: #0a1020;
    color: #445070;
    font-size: 11px;
    padding: 2px 8px;
}

/* ── Message Box ─────────────────────── */
QMessageBox {
    background: #181828;
}
QMessageBox QLabel { color: #dce2f0; font-size: 13px; }

    )");
}
