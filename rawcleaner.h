#ifndef RAWCLEANER_H
#define RAWCLEANER_H

#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileInfo>
#include <QString>
#include <QMimeData>
#include <QUrl>
#include <QDir>
#include <QFileInfoList>
#include <QMessageBox>
#include <QFile>
#include <QScrollArea>

QT_BEGIN_NAMESPACE
namespace Ui
{
class RawCleaner;
}
QT_END_NAMESPACE

class RawCleaner : public QMainWindow
{
    Q_OBJECT

public:
    RawCleaner(QWidget *parent = nullptr);
    ~RawCleaner();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void on_compare_btn_clicked();

    void on_set_again_clicked();

    void on_start_rawercleaner_clicked();

private:
    Ui::RawCleaner *ui;
    bool isScanning = false;
    QStringList allValidFolders;
    QStringList noJpgFiles;
};
#endif // RAWCLEANER_H
