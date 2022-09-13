#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>
#include <QUuid>
#include <QKeyEvent>

// Using Bluetooth liraries required installing Qt connectivity package, using maintenance tool
// It also required linking the bluetooth libraries in cmake file

// For whatever reason the bluetooth library does not work with mingw compiler, altough it works fine with MSVC compiler

// For I dont know what reason when device disconnects and I attempt to reconnect program crashes

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    enum class Command : char{fail, redOn, redOff, greenOn, greenOff, blueOn, blueOff}; // fail is used mainly as a respond from the device

private:
    Ui::Widget *ui;

    QBluetoothDeviceDiscoveryAgent* bluetooth_discovery_agent;
    QList<QBluetoothDeviceInfo> devices_list;

    QBluetoothSocket *socket;

    bool red_diode_state{};
    bool green_diode_state{};
    bool blue_diode_state{};

// ============================================================================================ private methods
    void addLog(const QString &log);
    QBluetoothDeviceInfo get_chosen_device(const QString &chosen_device_name) const;

    void send_command(Command command_code);

// ============================================================================================ private slots
private slots:
    void stop_searching();
    void reset_the_socket();
    void connect_chosen_device(const QBluetoothDeviceInfo &device_info);
    void check_socket_state();

// ============================================================================================ closeEvent
protected:
    virtual void closeEvent(QCloseEvent *event);

// ============================================================================================ keyEvents
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
};



#endif // WIDGET_H
