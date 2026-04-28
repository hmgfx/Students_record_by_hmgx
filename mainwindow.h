#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QList>

// =============================================
//  Student Data Structure
// =============================================
struct Student {
    QString name;
    QString rollNo;
    double math       = 0;
    double physics    = 0;
    double english    = 0;
    double programming= 0;
    double islamiat   = 0;

    double getTotal()      const { return math + physics + english + programming + islamiat; }
    double getPercentage() const { return (getTotal() / 500.0) * 100.0; }

    QString getGrade() const {
        double p = getPercentage();
        if (p >= 90) return "A+";
        if (p >= 80) return "A";
        if (p >= 70) return "B";
        if (p >= 60) return "C";
        if (p >= 50) return "D";
        return "F";
    }
};

// =============================================
//  Main Window Class
// =============================================
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddStudent();
    void onDeleteStudent();
    void onSearch();
    void onClearForm();
    void onSave();
    void onLoad();
    void onMarksChanged();

private:
    // UI Setup
    void     setupUI();
    QWidget* createAddTab();
    QWidget* createViewTab();
    void     applyStyle();

    // Logic Helpers
    void refreshTable(const QList<Student>& list);
    void updateStats();

    // ── Widgets ──────────────────────────────
    QTabWidget*     tabs;

    // Add-Student Tab
    QLineEdit*      nameEdit;
    QLineEdit*      rollEdit;
    QDoubleSpinBox* mathSpin;
    QDoubleSpinBox* physicsSpin;
    QDoubleSpinBox* englishSpin;
    QDoubleSpinBox* programmingSpin;
    QDoubleSpinBox* islamiatSpin;
    QLabel*         previewLbl;

    // View-Records Tab
    QTableWidget*   tableWidget;
    QLineEdit*      searchEdit;
    QLabel*         statsLbl;

    // Data Store
    QList<Student>  students;
};

#endif // MAINWINDOW_H
