#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui{class MainWindow;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    const QString extensionFilters{"*.txt *.csv *.log *.ini *.md *.xml *.json *.yaml *.yml *.tsv *.bat *.sh *.plist *.html *.htm"};

    QString filePath;
    QString folderPath;

    QString outPath;

    bool isFile;

    QStringList oldWord;
    QStringList newWord;

    bool isCaseSensitive;
    char separator;

    bool isReplacingFileName;
    bool isAddingSuffix;
    QString suffix;

    bool isReplacingOriginalFile;
    bool isCreatingBackup;

private slots:
    // Input Row //
    void onInFileClicked();
    void onInFolderClicked();

    void inFileLineFocusLost();
    void inFolderLineFocusLost();

    // Replace //
    void oldWordLineFocusLost();
    void newWordLineFocusLost();

    void checkReplacementAndUpdateIfValid();

    // Option Row //
    void onCaseSensitiveClicked();

    void separatorLineFocusLost();

    // Setting Row //
    void onReplaceFileClicked();
    void onAddSuffixClicked();

    // Output Row //
    void onOpenOutputFolderClicked();
    void onCreateBackupClicked();
    void onReplaceOriginalClicked();
    void onSelectOutputFolderClicked();

    void outPathLineFocusLost();
    void suffixLineFocusLost();

    // Bottom Row //
    void onStartButtonClicked();
    void onPreviewButtonClicked();

private:
    Ui::MainWindow *ui;

private:
    void initTipsAndWarnings();
    void initConnects();
};
