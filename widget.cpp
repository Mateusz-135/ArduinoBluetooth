#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , bluetooth_discovery_agent{new QBluetoothDeviceDiscoveryAgent(this)}
    , socket{nullptr}
{
    ui->setupUi(this);

    this->setFocusPolicy(Qt::FocusPolicy::ClickFocus); // this will setup focus policy,
    //so when we click on the widget window the app will react to keyEvents (clicking on other widgets, like lineEdit will not work, because it's a different widget; inside main widget)

    connect(ui->search_button, &QPushButton::clicked, this, [this](){ // -------------------------------------------------------------------- start searching
        addLog("Searching devices...");
        ui->devices_combo_box->clear();
        bluetooth_discovery_agent->start();
        ui->search_button->setDisabled(true); // disable searching button
    });

    connect(bluetooth_discovery_agent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, [this](){ // -------------------------------- device discovered
        devices_list = bluetooth_discovery_agent->discoveredDevices();
        ui->devices_combo_box->addItem(devices_list.last().name()); // the name held by the combo box will be used to locate the device on the devices list
    });

    connect(ui->stop_searching_button, &QPushButton::clicked, this, &Widget::stop_searching); // ------------------------------------------------------------ stop searching

    connect(bluetooth_discovery_agent, &QBluetoothDeviceDiscoveryAgent::finished, this, [this](){// ----------------------------------------- searching finished
        ui->search_button->setDisabled(false); // enable searching button
        addLog("Finished searching");
    });

    connect(ui->connect_button, &QPushButton::clicked, this, [this](){ // ------------------------------------------------------------------- connect
        stop_searching();
        reset_the_socket();

        QString chosen_device_name = ui->devices_combo_box->currentText(); // name from combobox is needed to find the device in the devices_list
        QBluetoothDeviceInfo chosen_device_info{get_chosen_device(chosen_device_name)}; // this variable will hold all the info about the device that we've chosen
        addLog("Attempt at connecting to device (name: " + chosen_device_name + ")");

        ui->connect_button->setDisabled(true);

        connect_chosen_device(chosen_device_info);
        check_socket_state();
    });

    connect(ui->disconnect_button, &QPushButton::clicked, this, [this](){ // ----------------------------------------------------------------- disconnect
        if(socket != nullptr){
            socket->abort();
        }
    });

    connect(ui->send_button, &QPushButton::clicked, this, [this](){ // ------------------------------------------------------------------------ send command from line edit
        if(!ui->command_line_edit->text().isEmpty()){
            QString text{ui->command_line_edit->text()}; // eventually sending a message from line edit means parsing a text into the command code known for the microcontroller
            ui->command_line_edit->clear();
            if(text == "red on")
                send_command(Command::redOn);
            else if(text == "red off")
                send_command(Command::redOff);
            else if(text == "green on")
                send_command(Command::greenOn);
            else if(text == "green off")
                send_command(Command::greenOff);
            else if(text == "blue on")
                send_command(Command::blueOn);
            else if(text == "blue off")
                send_command(Command::blueOff);
            else{
                addLog("Unknown command");
            }
        }
    });

    connect(ui->red_led_button, &QPushButton::clicked, this, [this](){
        if(red_diode_state)
            send_command(Command::redOff);
        else
            send_command(Command::redOn);
    });

    connect(ui->green_led_button, &QPushButton::clicked, this, [this](){
        if(green_diode_state)
            send_command(Command::greenOff);
        else
            send_command(Command::greenOn);
    });

    connect(ui->blue_led_button, &QPushButton::clicked, this, [this](){
        if(blue_diode_state)
            send_command(Command::blueOff);
        else
            send_command(Command::blueOn);
    });
}

Widget::~Widget()
{
    delete ui;
}

// =================================================================================================================================================== private methods
void Widget::addLog(const QString &log){
    ui->logs->addItem(log);
}

QBluetoothDeviceInfo Widget::get_chosen_device(const QString &chosen_device_name) const{ // name from combobox is needed to find the device in the devices_list
    for(const QBluetoothDeviceInfo &device_info : devices_list){
        if(device_info.name() == chosen_device_name){ // if the chosen device is found assign the info to chosen_device_info
            return device_info;
        }
    }
    return QBluetoothDeviceInfo();
}

void Widget::send_command(Command command_code){
    if(socket != nullptr && this->socket->isOpen() && this->socket->isWritable()) {
        char char_code{static_cast<char>(command_code)};
        this->socket->write(&char_code, 1);
    }
    else{
        addLog("Cannot send the message");
    }
}

// =================================================================================================================================================== private slots
void Widget::stop_searching(){
    if(bluetooth_discovery_agent->isActive()){
        bluetooth_discovery_agent->stop(); // stop discovery agent if still running
        ui->search_button->setDisabled(false); // enable searching button
    }
}

void Widget::reset_the_socket(){ // ------------------------------------------------------------- reset_the_socket
    disconnect(socket); // disconnect current socket
    delete socket; // delete current socket and create a new one
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this); // deleting and creating whole new socket is the only solution that I came up
                                                                                // with that actually solved my problem with reconnecting

    // ================================================================================================== remake connections
    connect(socket, &QBluetoothSocket::connected, this, [this](){ addLog("Device connected"); });

    connect(socket, &QBluetoothSocket::disconnected, this, [this](){ addLog("Device disconnected"); });

    connect(socket, &QBluetoothSocket::stateChanged, this, &Widget::check_socket_state);

    connect(socket, &QBluetoothSocket::errorOccurred, this, [this](){ qDebug() << "Socket error: " << socket->errorString(); });

    connect(socket, &QBluetoothSocket::readyRead, this, [this](){
        char response_code{};
        while(socket->getChar(&response_code)){ // reads one byte of data from the device, device should send either true or false
            switch(Command(response_code)){ // we need to cast in order to be able to do switch statemenst with scoped enumeration
            case Command::redOn:
                red_diode_state = true;
                ui->red_led_button->setText("Turn red off");
                addLog("Red led is on");
                break;
            case Command::redOff:
                red_diode_state = false;
                ui->red_led_button->setText("Turn red on");
                addLog("Red led is off");
                break;
            case Command::greenOn:
                green_diode_state = true;
                ui->green_led_button->setText("Turn green off");
                addLog("Green led is on");
                break;
            case Command::greenOff:
                green_diode_state = false;
                ui->green_led_button->setText("Turn green on");
                addLog("Green led is off");
                break;
            case Command::blueOn:
                blue_diode_state = true;
                ui->blue_led_button->setText("Turn blue off");
                addLog("Blue led is on");
                break;
            case Command::blueOff:
                blue_diode_state = false;
                ui->blue_led_button->setText("Turn blue on");
                addLog("Blue led is off");
                break;
            case Command::fail:
                addLog("Command failed");
                break;
            default:
                addLog("Unknowns response");
            }
        }
    });
}

void Widget::check_socket_state(){ // ------------------------------------------------------------- check_socket_state
    qDebug() << "Socket current state: " << socket->state();
    if(socket->state() != QBluetoothSocket::SocketState::ConnectedState) // if connected, disable the connect button
        ui->connect_button->setDisabled(false);
    else
        ui->connect_button->setDisabled(true);
}

void Widget::connect_chosen_device(const QBluetoothDeviceInfo &device_info){ // --------------------- connect_chosen_device
    QUuid device_uuid{}; // with HC-06 modue expect "00001101-0000-1000-8000-00805F9B34FB"
    if(!device_info.serviceUuids().empty()){ // if the list of available uuids is not empty assign the first uuid to device_uuid
        device_uuid = device_info.serviceUuids().at(0);
        addLog("Device uuid is " + device_uuid.toString());
    }
    else{
        addLog("Could not find the correct uuid");
        return;
    }
    this->socket->connectToService(device_info.address(), device_uuid, QIODevice::ReadWrite); // connect the device
}

// =================================================================================================================================================== Widget::closeEvent()
void Widget::closeEvent(QCloseEvent *event){ // makes sure that the device will be disconnected when app is closing
    if(socket != nullptr){
        socket->abort();
    }
}

void Widget::keyPressEvent(QKeyEvent *event){ // when key pressed turn on the led
    if(event->isAutoRepeat()){
        event->ignore(); // // if the event is autoRepeated ignore event and
        return; // return, this will provide occuring of releaseEvents, when the key is still pressed
    }

    switch(event->key()){
    case Qt::Key_R:
        if(!red_diode_state) // do not send the message if it's not going to change the current state
            send_command(Command::redOn);
        break;
    case Qt::Key_G:
        if(!green_diode_state)
            send_command(Command::greenOn);
        break;
    case Qt::Key_B:
        if(!blue_diode_state)
            send_command(Command::blueOn);
        break;
    }
    QWidget::keyPressEvent(event);
}

void Widget::keyReleaseEvent(QKeyEvent *event){ // when key released turn off the led
    if(event->isAutoRepeat()){
        event->ignore();
        return;
    }

    switch(event->key()){
    case Qt::Key_R:
        if(red_diode_state)
            send_command(Command::redOff);
        break;
    case Qt::Key_G:
        if(green_diode_state)
            send_command(Command::greenOff);
        break;
    case Qt::Key_B:
        if(blue_diode_state)
            send_command(Command::blueOff);
        break;
    }
    QWidget::keyReleaseEvent(event);
}
