#include "tmcl.h"
#include <QObject>
#include<iostream>
#include<cstdio>//to pause console screen

Tmcl::Tmcl(QObject *parent) :
    QObject(parent)
{

    read_port_ini();

    poz_nula[0]=0;
    poz_nula[1]=0;
    poz_nula[2]=0;

    timer = new QTimer(this);
    timer->setInterval(100);

    //PortSettings settings = {BAUD115200, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 10};
    PortSettings settingsx = {x_baudrate, x_databits, x_parity, x_stopbits, x_flowcontrol, x_time_interval};
    //   timer = new QTimer(this);
    //  timer->setInterval(40);
    portx = new QextSerialPort(x_com_port, settingsx, QextSerialPort::Polling);
    //port = new QextSerialPort(x_com_port, settings, QextSerialPort::EventDriven);
openSerialPortx();

}

//! [4]
void Tmcl::openSerialPortx()
{

    if (!portx->isOpen()) {
        //        port->setPortName(ui->portBox->currentText());
        //        port->open(QIODevice::ReadWrite);

        portx->open( QIODevice::ReadWrite | QIODevice::Unbuffered ) ;
               qDebug()<< "port otvoren";
    }

    //If using polling mode, we need a QTimer

    if (portx->isOpen() && portx->queryMode() == QextSerialPort::Polling)
        timer->start();
    else
        timer->stop();

    portx->setTimeout( 5000 ) ;

    //check presence of the board

    if (portx->isOpen()){
//uradi nesto
    }
    //   RequestTargetPositioReachedEvent();

}


int  Tmcl::Card_identify(int motor )
{


    return(0);
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



void Tmcl::SendCmd(const QByteArray &data)

{
    int i;

    QByteArray message;
    message= "G91X15Y15F1000\r\n";

        if (portx->isOpen()){

            portx->write(message);
               qDebug() << "poruka"<<message;
        }

}


//Read the result that is returned by the module
//Parameters: Handle: handle of the serial port, as returned by OpenRS232
//            Address: pointer to variable to hold the reply address returned by the module
//            Status: pointer to variable to hold the status returned by the module (100 means okay)
//            Value: pointer to variable to hold the value returned by the module
//Return value: TMCL_RESULT_OK: result has been read without errors
//              TMCL_RESULT_NOT_READY: not enough bytes read so far (try again)
//              TMCL_RESULT_CHECKSUM_ERROR: checksum of reply packet wrong



//int MainWindow::GetResult( UCHAR *Address, UCHAR *Status, int *Value)
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




    //for(i=0;i<data.length();i++)
    //   ui->terminalconsole->appendPlainText(QString::number(data[i], 16));


    if(data.length()==9){
        Checksum=0;
        for(i=0; i<8; i++)
            Checksum += data[i];
        TMCL_Checksum=(unsigned char)data[8];

        if((check==1) && (Checksum!=TMCL_Checksum)){
            //     ui->terminalconsole->appendPlainText("CHEKSUM\n");
            //    ui->terminalconsole->appendPlainText(QString::number(TMCL_Checksum, 16));
            //   ui->terminalconsole->appendPlainText(QString::number(Checksum, 16));
            return TMCL_RESULT_CHECKSUM_ERROR;
        }

        TMCL_Address=data[0];
        //ui->terminalconsole->appendPlainText("Address ");
        //ui->terminalconsole->appendPlainText(QString::number(TMCL_Address, 16));

        TMCL_Status=data[2];

        //ui->terminalconsole->appendPlainText("Status ");
        //ui->terminalconsole->appendPlainText(QString::number(TMCL_Status, 16));
        TMCL_Instr=data[3];
        TMCL_Byte[0]=data[4];
        TMCL_Byte[1]=data[5];
        TMCL_Byte[2]=data[6];
        TMCL_Byte[3]=data[7];
        TMCL_Value=(TMCL_Byte[0]<<24)|(TMCL_Byte[1]<<16)|(TMCL_Byte[2]<<8)|(TMCL_Byte[3]);
        //Value=(data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
        //ui->terminalconsole->appendPlainText("Value ");
        //ui->terminalconsole->appendPlainText(QString::number(TMCL_Value, 16));

    }
    else{
        //ui->terminalconsole->appendPlainText("NOT READY\n");
        qDebug()<<TMCL_RESULT_NOT_READY;
        return TMCL_RESULT_NOT_READY;
    }
    //ui->terminalconsole->appendPlainText("OK\n");
    return TMCL_RESULT_OK;
}





/*--------------------------------------------------------------------------*/
int Tmcl::read_port_ini(void)
{

    //default

    x_com_port="COM4";
    x_baudrate=BAUD115200;
    x_databits=DATA_8;
    x_stopbits=STOP_1;
    x_parity=PAR_NONE;
    x_flowcontrol=FLOW_OFF;
    //    x_mode=1
    x_time_interval=100;

}

/*
QString x_com_port;
BaudRateType x_baudrate;
DataBitsType x_databits;
StopBitsType x_stopbits;
ParityType x_parity;
FlowType x_flowcontrol;
*/

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
