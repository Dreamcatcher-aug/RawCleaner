#include "rawcleaner.h"
#include "./ui_rawcleaner.h"


RawCleaner::RawCleaner(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RawCleaner)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
    this->setWindowTitle("RawCleaner  Designed by Jaco_wang");
    ui->status_monitor->setReadOnly(true);
    ui->status_monitor->setTextCursor(QTextCursor());
    ui->status_monitor->setFocusPolicy(Qt::NoFocus);

    QRect oldGeom = ui->folder_choose->geometry();
    QString oldStyle = ui->folder_choose->styleSheet();
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(ui->folder_choose);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignTop);
    scrollArea->setStyleSheet("border:1px solid #bbbbbb;");
    scrollArea->setGeometry(oldGeom);
    scrollArea->show();
}

RawCleaner::~RawCleaner()
{
    delete ui;
}

void RawCleaner::dragEnterEvent(QDragEnterEvent *event)
{
    if (isScanning)
    {
        event->ignore();
    }

    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void RawCleaner::dropEvent(QDropEvent *event)
{
    if (isScanning)
    {
        QMessageBox::information(this, "提示", "请先点击【重置】后再操作！");
        return;
    }

    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
    {
        ui->folder_choose->setText("❌ 未获取到路径");
        ui->status_monitor->setPlainText("已加载文件夹个数: 0");
        return;
    }

    for (const QUrl &url : urls)
    {
        QString path = url.toLocalFile();
        QFileInfo info(path);

        if (info.isDir())
        {
            if (!allValidFolders.contains(path))
            {
                allValidFolders.append(path);
            }
        }
    }

    QString folderNames;
    for (const QString &folderPath : allValidFolders)
    {
        QDir dir(folderPath);
        QString folderName = dir.dirName();
        folderNames += QString("📁 %1\n").arg(folderName);
    }
    ui->folder_choose->setText(folderNames);
    ui->status_monitor->setPlainText
        (
            QString("✅ 已加载文件夹个数: %1").arg(allValidFolders.size())
            );
}


void RawCleaner::on_compare_btn_clicked()
{

    if (isScanning)
    {
        return;
    }

    isScanning = true;
    ui->folder_choose->setEnabled(false);
    ui->compare_btn->setEnabled(false);
    ui->compare_btn->setToolTip("请先点击【重置】后再使用");
    ui->status_monitor->clear();
    ui->status_monitor->appendPlainText("==========================================");
    ui->status_monitor->appendPlainText("🔍 正在扫描无JPG匹配的RAW文件，请稍候...");


    QStringList rawSuffixes = { "CR2", "CR3", "NEF", "ARW", "RAF", "DNG", "ORF", "RW2", "PEF" };
    int totalRawFound = 0;
    int totalNoJpg = 0;
    QStringList folderList = allValidFolders;

    if (folderList.isEmpty())
    {
        ui->status_monitor->appendPlainText("❌ 未加载任何文件夹！清点击重置按钮");
        ui->compare_btn->setEnabled(false);
        return;
    }

    for (const QString& folderPath : folderList)
    {
        QDir dir(folderPath);
        if (!dir.exists()) continue;

        QFileInfoList allFiles = dir.entryInfoList(QDir::Files);

        for (const QFileInfo& fileInfo : allFiles)
        {
            QString suffix = fileInfo.suffix().toUpper();

            if (rawSuffixes.contains(suffix))
            {
                totalRawFound++;
                QString base = fileInfo.completeBaseName();
                bool hasJpg = QFile::exists(dir.filePath(base + ".jpg")) ||
                              QFile::exists(dir.filePath(base + ".JPG")) ||
                              QFile::exists(dir.filePath(base + ".jpeg")) ||
                              QFile::exists(dir.filePath(base + ".JPEG"));
                if (!hasJpg)
                {
                    totalNoJpg++;
                    noJpgFiles.append(fileInfo.filePath());
                }
            }
        }
    }

    ui->status_monitor->clear();
    ui->status_monitor->appendPlainText("==========================================");
    ui->status_monitor->appendPlainText("📊 扫描完成！最终结果如下：");
    ui->status_monitor->appendPlainText("---------------------------------------");
    ui->status_monitor->appendPlainText(QString("📁 总检测RAW文件：%1 个").arg(totalRawFound));
    ui->status_monitor->appendPlainText(QString("🗑  无匹配JPG的RAW：%1 个").arg(totalNoJpg));
    ui->status_monitor->appendPlainText("---------------------------------------");

    if (!noJpgFiles.isEmpty())
    {
        ui->status_monitor->appendPlainText("⚠️  需要清理的文件列表：");
        for (const QString& file : noJpgFiles)
        {
            QFileInfo info(file);
            QString fileName = info.fileName();
            QString folderName = info.dir().dirName();
            ui->status_monitor->appendPlainText
                (
                QString("  📂 %1 | 📷 %2").arg(folderName).arg(fileName)
                );
        }
    }
    else
    {
        ui->status_monitor->appendPlainText("✅ 全部RAW都已匹配JPG，无需清理！");
    }

    ui->status_monitor->appendPlainText("==========================================\n");
}


void RawCleaner::on_set_again_clicked()
{
    ui->status_monitor->clear();
    ui->folder_choose->clear();
    allValidFolders.clear();
    noJpgFiles.clear();
    isScanning = false;
    ui->folder_choose->setEnabled(true);
    ui->compare_btn->setEnabled(true);
    ui->start_rawercleaner->setEnabled(true);
    ui->folder_choose->setText("请拖拽需要处理的文件夹至此");
    ui->status_monitor->setPlainText("✅ 已重置就绪！");
    ui->compare_btn->setToolTip("");

}


void RawCleaner::on_start_rawercleaner_clicked()
{
    if (noJpgFiles.isEmpty())
    {
        QMessageBox::warning(this, "提示", "❌ 无文件需处理或先点击【开始对比】，再执行清理！");
        return;
    }

    int ret = QMessageBox::question(this,
                                    "⚠️ 确认清理",
                                    QString("确定要删除 %1 个无匹配JPG的RAW文件吗？\n此操作可在回收站找回！").arg(noJpgFiles.size()),
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::No
                                    );

    if (ret != QMessageBox::Yes)
    {
        ui->status_monitor->appendPlainText("🛑 用户取消清理操作～");
        return;
    }

    ui->status_monitor->appendPlainText("==========================================");
    ui->status_monitor->appendPlainText("🗑 开始安全清理无匹配JPG的RAW文件...");

    int deleteSuccess = 0;
    int deleteFail = 0;

    for (const QString& filePath : noJpgFiles)
    {
        QFile file(filePath);
        if (file.moveToTrash())
        {
            deleteSuccess++;
        }
        else
        {
            deleteFail++;
            ui->status_monitor->appendPlainText("❌ 删除失败：" + filePath);
        }
    }

    ui->status_monitor->appendPlainText("---------------------------------------");
    ui->status_monitor->appendPlainText(QString("✅ 清理完成！成功删除：%1 个").arg(deleteSuccess));
    if (deleteFail > 0)
        ui->status_monitor->appendPlainText(QString("❌ 删除失败：%1 个").arg(deleteFail));

    ui->status_monitor->appendPlainText("==========================================\n");

    ui->start_rawercleaner->setEnabled(false);
    noJpgFiles.clear();
}

