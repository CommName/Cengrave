#include "tmcl.h"
#include <QObject>
#include <QtDebug>
#include<iostream>
#include<cstdio>//to pause console screen
#include <QTimer>
#include <QApplication>

Tmcl::Tmcl(QObject *parent) :
    QObject(parent)
{

    read_port_ini();

    poz_nula[0]=0;
    poz_nula[1]=0;
    poz_nula[2]=0;

    timer = new QTimer(this);
    timer->setInterval(timeout);
    PortSettings settingsx = {x_baudrate, x_databits, x_parity, x_stopbits, x_flowcontrol, x_time_interval};
    portx = new QextSerialPort(x_com_port, settingsx, QextSerialPort::Polling);

}
int Tmcl::PollComport(int port)
{
    int n ;

        if (portx == NULL || !portx->isOpen())
            return 0;

        n = portx->bytesAvailable();


    if (!n)
        return 0;
    return(n);
}

//! [4]
void Tmcl::openSerialPortx()
{

    if (!portx->isOpen()) {
        portx->open( QIODevice::ReadWrite | QIODevice::Unbuffered ) ;
               qDebug()<< "port open";
    }

    portx->setTimeout(timeout) ;

}
void Tmcl::closeSerialPortx(){
    portx->close();
}
bool Tmcl::move(float x,float y,int speed){
    if(x_dir)
        x*=-1;
    if(y_dir)
        y*=-1;
    x*=x_step;
    y*=y_step;
    SendCmd(QString("G91X"+QString::number(x)+"Y"+QString::number(y)+"F"+QString::number(speed)+"\r\n"));
    return true;
}
bool Tmcl::laserOn(bool on,int strength){
    if(on)
        SendCmd(QString("G01 F1000 "+laserOnCommand.toLocal8Bit()+'S'+QString::number(strength)+"\r\n"));
    else
        SendCmd(QString(laserOffCommand.toLocal8Bit()+"\r\n"));
    return true;
}


//Read the result that is returned by the module
//Parameters: Handle: handle of the serial port, as returned by OpenRS232
//            Address: pointer to variable to hold the reply address returned by the module
//            Status: pointer to variable to hold the status returned by the module (100 means okay)
//            Value: pointer to variable to hold the value returned by the module
//Return value: TMCL_RESULT_OK: result has been read without errors
//              TMCL_RESULT_NOT_READY: not enough bytes read so far (try again)
//              TMCL_RESULT_CHECKSUM_ERROR: checksum of reply packet wrong
int Tmcl::SendCmd(QString data)
{

    QByteArray message(data.toLocal8Bit().constData());

        if (portx->isOpen()){

            portx->write(message);
               qDebug() << "message"<<message;
        }

        return GetResult_ok(0,0);


}
int Tmcl::GetResult(int port,int check)
{
    unsigned char Checksum;
    int i;
    int n;
    QByteArray data;
    n=0;

        if (!portx->isOpen())
            return TMCL_RESULT_NOT_READY;

        while (n <9)
            n = PollComport(port);

        data = portx->readAll();


    if(data.length()==9){
        Checksum=0;
        for(i=0; i<8; i++)
            Checksum += data[i];
        TMCL_Checksum=(unsigned char)data[8];

        if((check==1) && (Checksum!=TMCL_Checksum)){
            return TMCL_RESULT_CHECKSUM_ERROR;
        }

        TMCL_Address=data[0];

        TMCL_Status=data[2];

        TMCL_Instr=data[3];
        TMCL_Byte[0]=data[4];
        TMCL_Byte[1]=data[5];
        TMCL_Byte[2]=data[6];
        TMCL_Byte[3]=data[7];
        TMCL_Value=(TMCL_Byte[0]<<24)|(TMCL_Byte[1]<<16)|(TMCL_Byte[2]<<8)|(TMCL_Byte[3]);

    }
    else{
        qDebug()<<TMCL_RESULT_NOT_READY;
        return TMCL_RESULT_NOT_READY;
    }
    return TMCL_RESULT_OK;
}
int Tmcl::GetResult_ok(int port,int check)
{
    int i;
    int n;
    QByteArray data;

    n=0;

        if (!portx->isOpen())
            return -1;
    timer->setInterval(this->timeout);

    timer->start();
        while (n <4 && timer->remainingTime()!=0){
            n = PollComport(port);
        }

    if(timer->remainingTime()==0)
        throw QString("Timeout accrued!");

    timer->stop();
    data = portx->readAll();
    qDebug() <<"Message from port" <<data;
    if(data.length()>=2){
        if(data[0]=='o' && data[1]=='k')
        return(0);
    }
    else{
        return(1);
    }
    return(-2);
}
int Tmcl::read_port_ini(void)
{

    QSettings settings("cengrave.ini",QSettings::IniFormat);
    settings.beginGroup("Serial port");
    x_com_port=                     settings.value("com_port","COM4").toString();
    x_baudrate=     (BaudRateType)  settings.value("baud",115200).toInt();
    x_databits=     (DataBitsType)  settings.value("data_bits",8).toInt();
    x_stopbits=     (StopBitsType)  settings.value("stop_bits",0).toInt();
    x_parity=       (ParityType)    settings.value("parity_type",0).toInt();
    x_flowcontrol=  (FlowType)      settings.value("flow_type",0).toInt();
    x_time_interval=                settings.value("time_interval",100).toInt();
    laserOnCommand =                settings.value("laser_on","M3").toString();
    laserOffCommand =               settings.value("laser_off","M5").toString();
    x_dir =         (bool)          settings.value("x_dir",0).toInt();
    y_dir =         (bool)          settings.value("y_dir",0).toInt();
    timeout=                        settings.value("timeout",10000).toInt();
    settings.endGroup();
    settings.beginGroup("Movement");
    x_step=                         settings.value("step_x",1).toInt();
    y_step=                         settings.value("step_y",1).toInt();
    settings.endGroup();
}

ParityType Tmcl::parity(QString  str)
{
    if(str=="PAR_NONE")return(PAR_NONE);
    else if(str=="PAR_ODD")return(PAR_ODD);
    else if(str=="PAR_EVEN")return(PAR_EVEN);
return(PAR_NONE);
}
DataBitsType Tmcl::databits(QString  str)
{
    if(str=="DATA_5")return(DATA_5);
    else if(str=="DATA_6")return(DATA_6);
    else if(str=="DATA_7")return(DATA_7);
    else if(str=="DATA_8")return(DATA_8);
return(DATA_8);
}
StopBitsType Tmcl::stopbits(QString  str)
{
    if(str=="STOP_1")return(STOP_1);
    else if(str=="STOP_1_5")return(STOP_1_5);
    else if(str=="STOP_2")return(STOP_2);
return(STOP_1);
}
FlowType Tmcl::flowcontrol(QString str)
{
    if(str=="FLOW_OFF")return(FLOW_OFF);
    else if(str=="FLOW_HARDWARE")return(FLOW_HARDWARE);
    else if(str=="FLOW_XONXOFF")return(FLOW_XONXOFF);
   return(FLOW_OFF);
}
