#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>

class NavigationInputDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NavigationInputDialog(const QStringList& placeNames, QWidget* parent = nullptr);

    QStringList getWaypoints() const;
    QString getStartName() const;
    QString getEndName() const;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    QLineEdit* startEdit;
    QLineEdit* endEdit;
    QLineEdit* waypointEdit;
    QListWidget* waypointList;
    QPushButton* addWaypointButton;
    QPushButton* removeWaypointButton;
    QPushButton* okButton;
    QPushButton* cancelButton;
    QListWidget* nodeListWidget;

    QLineEdit* lastFocusedEdit = nullptr;
};
