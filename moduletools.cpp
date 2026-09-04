#include "moduletools.h"

#include <QComboBox>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QHash>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QPageLayout>
#include <QPageSize>
#include <QPdfWriter>
#include <QTableWidget>
#include <QUrl>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <algorithm>

QWidget *ModuleTools::createMultiCriteriaTools(QTableWidget *table,
                                               const QVector<int> &criteriaColumns,
                                               const QStringList &criteriaLabels)
{
    QWidget *container = new QWidget(table);
    QHBoxLayout *layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    QList<QLineEdit *> filters;
    for (int i = 0; i < criteriaColumns.size(); ++i) {
        auto *filter = new QLineEdit(container);
        filter->setPlaceholderText("Rechercher par " + criteriaLabels.value(i));
        filter->setClearButtonEnabled(true);
        filters << filter;
        layout->addWidget(filter, 1);
    }
    auto *sort = new QComboBox(container);
    sort->addItem("Tri : aucun", -1);
    for (int i = 0; i < criteriaColumns.size(); ++i)
        sort->addItem("Trier : " + criteriaLabels.value(i), criteriaColumns.at(i));
    auto *order = new QComboBox(container);
    order->addItems({"A-Z / croissant", "Z-A / décroissant"});
    layout->addWidget(sort);
    layout->addWidget(order);

    auto apply = [table, filters, criteriaColumns, sort, order] {
        for (int row = 0; row < table->rowCount(); ++row) {
            bool matches = true;
            for (int i = 0; i < filters.size(); ++i) {
                const QString needle = filters.at(i)->text().trimmed();
                const QTableWidgetItem *item = table->item(row, criteriaColumns.at(i));
                if (!needle.isEmpty() && (!item || !item->text().contains(needle, Qt::CaseInsensitive))) {
                    matches = false;
                    break;
                }
            }
            table->setRowHidden(row, !matches);
        }
        const int column = sort->currentData().toInt();
        if (column >= 0)
            table->sortItems(column, order->currentIndex() == 0 ? Qt::AscendingOrder : Qt::DescendingOrder);
    };
    for (QLineEdit *filter : filters)
        QObject::connect(filter, &QLineEdit::textChanged, container, apply);
    QObject::connect(sort, QOverload<int>::of(&QComboBox::currentIndexChanged), container, apply);
    QObject::connect(order, QOverload<int>::of(&QComboBox::currentIndexChanged), container, apply);
    return container;
}

void ModuleTools::updateCategoryChart(QChartView *chartView, QTableWidget *table,
                                      int categoryColumn, const QString &title)
{
    if (!chartView || !table) return;
    QHash<QString, int> counts;
    for (int row = 0; row < table->rowCount(); ++row) {
        const QTableWidgetItem *item = table->item(row, categoryColumn);
        counts[item && !item->text().isEmpty() ? item->text() : "Non défini"]++;
    }
    QList<QPair<QString, int>> categories;
    for (auto it = counts.cbegin(); it != counts.cend(); ++it)
        categories.append({it.key(), it.value()});
    std::sort(categories.begin(), categories.end(), [](const auto &a, const auto &b) {
        return a.second > b.second;
    });

    auto *series = new QPieSeries();
    series->setHoleSize(0.58);
    series->setPieSize(0.78);
    const QList<QColor> palette = { QColor("#2563EB"), QColor("#14B8A6"), QColor("#8B5CF6"),
                                    QColor("#F59E0B"), QColor("#EC4899"), QColor("#06B6D4"), QColor("#64748B") };
    int total = 0;
    for (const auto &category : categories) total += category.second;
    for (int index = 0; index < categories.size(); ++index) {
        const auto &category = categories.at(index);
        auto *slice = series->append(category.first, category.second);
        slice->setBrush(palette.at(index % palette.size()));
        slice->setBorderColor(Qt::white);
        slice->setBorderWidth(2);
        slice->setLabel(QString("%1\n%2 (%3%)").arg(category.first).arg(category.second)
                        .arg(total == 0 ? 0 : qRound(category.second * 100.0 / total)));
        slice->setLabelVisible(categories.size() <= 4);
    }
    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(title);
    chart->setTitleBrush(QColor("#0F172A"));
    QFont titleFont; titleFont.setPointSize(11); titleFont.setBold(true); chart->setTitleFont(titleFont);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setAnimationDuration(650);
    chart->setBackgroundVisible(false);
    chart->setMargins(QMargins(10, 8, 10, 8));
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setLabelColor(QColor("#475569"));
    QFont legendFont; legendFont.setPointSize(8); chart->legend()->setFont(legendFont);
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("QChartView { background: #FFFFFF; border: 1px solid #E2E8F0; border-radius: 12px; }");
}

bool ModuleTools::exportTableToPdf(QTableWidget *table, const QString &title, QString *outputPath)
{
    const QString defaultPath = QDir::homePath() + "/Documents/" + title.simplified().replace(' ', '_') + ".pdf";
    QString path = QFileDialog::getSaveFileName(table, "Exporter le rapport PDF", defaultPath, "Documents PDF (*.pdf)");
    if (path.isEmpty()) return false;
    if (!path.endsWith(".pdf", Qt::CaseInsensitive)) path += ".pdf";

    int visibleRows = 0;
    for (int row = 0; row < table->rowCount(); ++row)
        if (!table->isRowHidden(row)) ++visibleRows;

    QPdfWriter pdf(path);
    pdf.setResolution(96);
    pdf.setPageSize(QPageSize(QPageSize::A4));
    pdf.setPageOrientation(QPageLayout::Landscape);
    pdf.setPageMargins(QMarginsF(12, 12, 12, 12), QPageLayout::Millimeter);
    pdf.setTitle(title);
    pdf.setCreator("Centre de Formation");
    QPainter painter(&pdf);
    if (!painter.isActive()) {
        QMessageBox::critical(table, "Export PDF", "Le fichier PDF n'a pas pu être créé.");
        return false;
    }

    const int pageWidth = pdf.width(), pageHeight = pdf.height();
    const int left = 38, right = 38, contentWidth = pageWidth - left - right;
    const int cols = qMax(1, table->columnCount()), colWidth = contentWidth / cols;
    const int rowHeight = 34, tableHeaderHeight = 30;
    int page = 1;
    const QColor navy("#0F172A"), blue("#2563EB"), paleBlue("#EFF6FF"), slate("#64748B"), border("#DCE3EF");

    auto drawHeader = [&] {
        painter.fillRect(0, 0, pageWidth, 88, navy);
        painter.fillRect(0, 84, pageWidth, 4, blue);
        QFont brand = painter.font(); brand.setPointSize(19); brand.setBold(true); painter.setFont(brand); painter.setPen(Qt::white);
        painter.drawText(left, 40, "CENTRE DE FORMATION");
        QFont sub = painter.font(); sub.setPointSize(8); sub.setBold(false); painter.setFont(sub); painter.setPen(QColor("#BFDBFE"));
        painter.drawText(left, 60, "APPLICATION DE GESTION  |  RAPPORT OFFICIEL");
        QFont reportTitle = painter.font(); reportTitle.setPointSize(15); reportTitle.setBold(true); painter.setFont(reportTitle); painter.setPen(navy);
        painter.drawText(left, 118, title);
        QFont metadata = painter.font(); metadata.setPointSize(8); metadata.setBold(false); painter.setFont(metadata); painter.setPen(slate);
        painter.drawText(left, 136, "Généré le " + QDateTime::currentDateTime().toString("dd/MM/yyyy à HH:mm"));
        painter.fillRect(left, 148, contentWidth, 30, paleBlue);
        QFont summary = painter.font(); summary.setBold(true); painter.setFont(summary); painter.setPen(blue);
        painter.drawText(left + 10, 168, QString("%1 enregistrement(s) affiché(s)").arg(visibleRows));
        painter.setPen(slate); painter.drawText(left + contentWidth - 160, 168, QString("Total : %1").arg(table->rowCount()));
    };
    auto drawTableHeader = [&](int y) {
        painter.fillRect(left, y, contentWidth, tableHeaderHeight, blue);
        QFont font = painter.font(); font.setPointSize(8); font.setBold(true); painter.setFont(font); painter.setPen(Qt::white);
        for (int col = 0; col < cols; ++col) {
            const auto *header = table->horizontalHeaderItem(col);
            painter.drawText(QRect(left + col * colWidth + 7, y, colWidth - 12, tableHeaderHeight), Qt::AlignVCenter | Qt::TextWordWrap,
                             header ? header->text() : QString());
        }
    };
    auto drawFooter = [&] {
        painter.setPen(border); painter.drawLine(left, pageHeight - 32, pageWidth - right, pageHeight - 32);
        QFont footer = painter.font(); footer.setPointSize(7); footer.setBold(false); painter.setFont(footer); painter.setPen(slate);
        painter.drawText(left, pageHeight - 16, "Centre de Formation — document généré par l'application");
        painter.drawText(pageWidth - right - 70, pageHeight - 16, QString("Page %1").arg(page));
    };

    drawHeader();
    int y = 190;
    drawTableHeader(y); y += tableHeaderHeight;
    QFont cellFont = painter.font(); cellFont.setPointSize(8); cellFont.setBold(false); painter.setFont(cellFont);
    for (int row = 0; row < table->rowCount(); ++row) {
        if (table->isRowHidden(row)) continue;
        if (y + rowHeight > pageHeight - 48) {
            drawFooter(); pdf.newPage(); ++page; drawHeader(); y = 190; drawTableHeader(y); y += tableHeaderHeight;
            painter.setFont(cellFont);
        }
        painter.fillRect(left, y, contentWidth, rowHeight, row % 2 == 0 ? Qt::white : QColor("#F8FAFC"));
        painter.setPen(border); painter.drawRect(left, y, contentWidth, rowHeight);
        painter.setPen(navy);
        for (int col = 0; col < cols; ++col) {
            const auto *item = table->item(row, col);
            const QRect cell(left + col * colWidth + 7, y + 4, colWidth - 12, rowHeight - 8);
            painter.drawText(cell, Qt::AlignVCenter | Qt::TextSingleLine,
                             painter.fontMetrics().elidedText(item ? item->text() : QString(), Qt::ElideRight, cell.width()));
        }
        y += rowHeight;
    }
    drawFooter();
    painter.end();
    const bool created = QFileInfo(path).exists() && QFileInfo(path).size() > 0;
    if (outputPath && created) *outputPath = path;
    if (created) {
        QMessageBox::information(table, "Export PDF réussi", "Le rapport professionnel a été créé avec succès :\n" + path);
    } else {
        QMessageBox::critical(table, "Export PDF", "Le rapport PDF n'a pas pu être enregistré. Vérifiez le dossier choisi.");
    }
    return created;
}
