#include "hwf.h"
#include <windows.h> //placer avant inpout32.h
#include <QtDebug>
#include <QThread>
#include <QFile>
#include <QSettings>
#include <QString>
#include <QTime>

#include <QCoreApplication>
#include <QElapsedTimer>

//port1 0x378
//port2 0x37a
//port3 0x379


HWF::HWF(QObject *parent) :
    QObject(parent)
{
    read_port_ini();
    int status= init();

     if( status <0){
        qApp->exit(0);
    }
}

int HWF::mainboard_speed(void)
{
    int i;
    int j;
    int difference_average;

    QTime time;
    difference_average=0;
    for(i=0;i<5;i++){
        time.start();
        for(j=0;j<1000;j++)
            ddelay_m(1000);
        int difference = time.elapsed();
        difference_average+=difference;
    }

    difference_average/=5;

    if(difference_average >= 100)
        msec_delay=1000*1000/difference_average;
    else
        msec_delay=100;

    difference_average=0;
    for(i=0;i<3;i++){
        time.start();
        for(j=0;j<1000;j++)
            ddelay_m(msec_delay);
        int difference = time.elapsed();
        difference_average+=difference;
    }


    difference_average/=3;

    QSettings settings("wport.ini", QSettings::IniFormat);

    settings.beginGroup("port");
    settings.setValue("msec_delay_const", (int)msec_delay);
    settings.endGroup();
    return(0);
}

int HWF::read_port_ini(void)
{
    xsa=0;
    ysa=0;
    zsa=0;
    //default value

    //parallel port
    hw_type=1;

    msec_delay=1000;

    //output
    x_step_port          =  0x378  ;
    x_step_bit           =  0x80   ;
    x_dir_port           =  0x378  ;
    x_dir_bit            =  0x40   ;
    x_enable_port        =  0x378  ;
    x_enable_bit         =  0x02   ;

    y_step_port          =  0x378  ;
    y_step_bit           =  0x20   ;
    y_dir_port           =  0x378  ;
    y_dir_bit            =  0x10   ;
    y_enable_port        =  0x378  ;
    y_enable_bit         =  0x01   ;

    z_step_port          =  0x378  ;
    z_step_bit           =  0x08   ;
    z_dir_port           =  0x378  ;
    z_dir_bit            =  0x04   ;

    //#only for configuration for parallel port
    //#z_axe_type=2  DC motor
    //#z_axe_type=1  stepper motor
    z_axe_type=1;
    //za DC motor
    // power ON z_enable_port
    // Z_dir dir
    // Z_enable_bit

    z_enable_port        =  0x37a  ;
    z_enable_bit         =  0x01 ;
    a_step_port          =  0x378  ;
    a_step_bit           =  0x08   ;
    a_dir_port           =  0x378  ;
    a_dir_bit            =  0x04   ;
    a_enable_port        =  0x37a  ;
    a_enable_bit         =  0x01 ;
    motor_port           =  0x37a;
    motor_bit            =  0x02;
    kvasenje_port           =  0x37a;
    kvasenje_bit            =  0x04 ;
    z_delay=10;

    //input
    granicni_prekidaci = 0;
    x_limit_max_port     =  0x379  ;
    x_limit_max_bit      =  0x40   ;
    x_limit_max_active   =  1      ;
    x_limit_min_port     =  0x379  ;
    x_limit_min_bit      =  0x02   ;
    x_limit_min_active   =  1      ;

    y_limit_max_port     =  0x379  ;
    y_limit_max_bit      =  0x04   ;
    y_limit_max_active   =  1      ;

    y_limit_min_port     =  0x379  ;
    y_limit_min_bit      =  0x06   ;
    y_limit_min_active   =  1      ;

    //unused for this project
    z_limit_max_port     =  0x379  ;
    z_limit_max_bit      =  0x08   ;
    z_limit_max_active   =  1      ;
    a_limit_max_port     =  0x379  ;
    a_limit_max_bit      =  0x08   ;
    a_limit_max_active   =  1      ;

    if (!QFile::exists("wport.ini"))
    {
        qDebug()  << "hwf: wport.ini doesn't exists";
        return(2);
    }
    else {


        //ocitaj parametre iz fajla
        bool ok;
        QString pom;

        QSettings settings("wport.ini", QSettings::IniFormat);

        settings.beginGroup("type");
        hw_type = settings.value("hw_type", "r").toInt();
        qDebug() << " hw_type" <<  hw_type;

        settings.endGroup();

        settings.beginGroup("port");

        pom = settings.value("msec_delay_const", "r").toString();
        msec_delay=pom.toLong(&ok,10);

        pom = settings.value("x_step_port", "r").toString();
        x_step_port=pom.toInt(&ok,16);
        pom = settings.value("x_step_bit", "r").toString();
        x_step_bit=pom.toInt(&ok,16);

        pom= settings.value("x_dir_port", "r").toString();
        x_dir_port=pom.toInt(&ok,16);
        pom= settings.value("x_dir_bit", "r").toString();
        x_dir_bit=pom.toInt(&ok,16);
        x_dir_logic= settings.value("x_dir_logic", "r").toInt();

        pom = settings.value("x_enable_port", "r").toString();
        x_enable_port=pom.toInt(&ok,16);
        pom= settings.value("x_enable_bit", "r").toString();
        x_enable_bit=pom.toInt(&ok,16);

        qDebug() << "X port" << x_step_port << "X bit" << x_step_bit;

        pom = settings.value("y_step_port", "r").toString();
        y_step_port=pom.toInt(&ok,16);
        pom = settings.value("y_step_bit", "r").toString();
        y_step_bit=pom.toInt(&ok,16);

        pom= settings.value("y_dir_port", "r").toString();
        y_dir_port=pom.toInt(&ok,16);
        pom= settings.value("y_dir_bit", "r").toString();
        y_dir_bit=pom.toInt(&ok,16);
        y_dir_logic= settings.value("y_dir_logic", "r").toInt();

        pom = settings.value("y_enable_port", "r").toString();
        y_enable_port=pom.toInt(&ok,16);
        pom= settings.value("y_enable_bit", "r").toString();
        y_enable_bit=pom.toInt(&ok,16);
        qDebug() << "y port" << y_step_port << "y bit" << y_step_bit;

        pom = settings.value("z_axe_type", "r").toString();
        z_axe_type=pom.toInt(&ok,10);
        qDebug() << "z_axe_type" << z_axe_type;

        pom = settings.value("z_step_port", "r").toString();
        z_step_port=pom.toInt(&ok,16);
        pom = settings.value("z_step_bit", "r").toString();
        z_step_bit=pom.toInt(&ok,16);

        pom= settings.value("z_dir_port", "r").toString();
        z_dir_port=pom.toInt(&ok,16);
        pom= settings.value("z_dir_bit", "r").toString();
        z_dir_bit=pom.toInt(&ok,16);
        z_dir_logic= settings.value("z_dir_logic", "r").toInt();

        pom = settings.value("z_enable_port", "r").toString();
        z_enable_port=pom.toInt(&ok,16);
        pom= settings.value("z_enable_bit", "r").toString();
        z_enable_bit=pom.toInt(&ok,16);
        qDebug() << "z port" << z_step_port << "z bit" << z_step_bit;

        pom = settings.value("a_step_port", "r").toString();
        a_step_port=pom.toInt(&ok,16);
        pom = settings.value("a_step_bit", "r").toString();
        a_step_bit=pom.toInt(&ok,16);

        pom= settings.value("a_dir_port", "r").toString();
        a_dir_port=pom.toInt(&ok,16);
        pom= settings.value("a_dir_bit", "r").toString();
        a_dir_bit=pom.toInt(&ok,16);
        a_dir_logic= settings.value("a_dir_logic", "r").toInt();

        pom = settings.value("a_enable_port", "r").toString();
        a_enable_port=pom.toInt(&ok,16);
        pom= settings.value("a_enable_bit", "r").toString();
        a_enable_bit=pom.toInt(&ok,16);
        qDebug() << "a port" << a_step_port << "a bit" << a_step_bit;


        pom = settings.value("motor_port", "r").toString();
        motor_port =pom.toInt(&ok,16);

        pom = settings.value("motor_bit", "r").toString();
        motor_bit =pom.toInt(&ok,16);

        pom = settings.value("kvasenje_port", "r").toString();
        kvasenje_port =pom.toInt(&ok,16);

        pom = settings.value("kvasenje_bit", "r").toString();
        kvasenje_bit =pom.toInt(&ok,16);

        qDebug() << "Kvasenje port" << kvasenje_port << " bit" << kvasenje_bit;

        pom = settings.value("granicni_prekidaci", "r").toString();
        granicni_prekidaci =pom.toInt(&ok,10);

        pom = settings.value("x_limit_max_port", "r").toString();
        x_limit_max_port =pom.toInt(&ok,16);
        pom = settings.value("x_limit_max_bit", "r").toString();
        x_limit_max_bit =pom.toInt(&ok,16);
        pom = settings.value("x_limit_max_active", "r").toString();
        x_limit_max_active =pom.toInt(&ok,16);

        pom = settings.value("x_limit_min_port", "r").toString();
        x_limit_min_port =pom.toInt(&ok,16);
        pom = settings.value("x_limit_min_bit", "r").toString();
        x_limit_min_bit =pom.toInt(&ok,16);
        pom = settings.value("x_limit_min_active", "r").toString();
        x_limit_min_active =pom.toInt(&ok,16);

        pom = settings.value("y_limit_max_port", "r").toString();
        y_limit_max_port =pom.toInt(&ok,16);
        pom = settings.value("y_limit_max_bit", "r").toString();
        y_limit_max_bit =pom.toInt(&ok,16);
        pom = settings.value("y_limit_max_active", "r").toString();
        y_limit_max_active =pom.toInt(&ok,16);

        pom = settings.value("y_limit_min_port", "r").toString();
        y_limit_min_port =pom.toInt(&ok,16);
        pom = settings.value("y_limit_min_bit", "r").toString();
        y_limit_min_bit =pom.toInt(&ok,16);
        pom = settings.value("y_limit_min_active", "r").toString();
        y_limit_min_active =pom.toInt(&ok,16);


        settings.endGroup();

        settings.beginGroup("mehanika");

        korak_x = settings.value("korak_x", "r").toFloat();
        korak_y = settings.value("korak_y", "r").toFloat();
        korak_z = settings.value("korak_z", "r").toFloat();
        korak_a = settings.value("korak_a", "r").toFloat();

        qDebug() << "X korak" << korak_x;
        qDebug() << "Y korak" << korak_y;
        qDebug() << "Z korak" << korak_z;
        settings.endGroup();
        return(0);
    }

    return(0);
}

void HWF::motor_off(void)

{
    if(motor_port==0x37a){
        port2=port2|motor_bit;
        oup32(motor_port, port2);
    }

    qDebug() << "Motor off";

    return;
}

void HWF::motor_on(void)

{
    if(motor_port==0x37a){
        port2=port2 & ~motor_bit;
        oup32(motor_port, port2);
    }

    qDebug() << "Motor on";
    return;
}

void HWF::kvasenje_off(void)

{
    if(kvasenje_port==0x37a){
        port2=port2 & ~kvasenje_bit;
        oup32(kvasenje_port, port2);
    }

    qDebug() << "Kvasenje off";
    return;
}

void HWF::kvasenje_on(void)

{
    if(kvasenje_port==0x37a){
        port2=port2 | kvasenje_bit;
        oup32(kvasenje_port, port2);
    }

    qDebug() << "Kvasenje on";
    return;
}

void HWF::enable_off()

{
    if(x_enable_port==0x378){
        port1=port1 & ~x_enable_bit;
        oup32(x_enable_port, port1);
    }
    if(y_enable_port==0x378){
        port1=port1 & ~y_enable_bit;
        oup32(y_enable_port, port1);
    }

    if(z_enable_port==0x378){
        port1=port1 & ~z_enable_bit;
        oup32(z_enable_port, port1);
    }
    else if(z_enable_port==0x37a){
        port2=port2 | z_enable_bit;
        oup32(z_enable_port, port2);
    }

    qDebug() << "Enable off";
    return;
}
void HWF::enable_on()

{

    if(x_enable_port==0x378){
        port1=port1 | x_enable_bit;
        oup32(x_enable_port, port1);
    }
    if(y_enable_port==0x378){
        port1=port1 | y_enable_bit;
        oup32(y_enable_port, port1);
    }
    if(z_enable_port==0x378){
        port1=port1 | z_enable_bit;
        oup32(z_enable_port, port1);
    }
    else if(z_enable_port==0x37a){
        port2=port2 & ~z_enable_bit;
        oup32(z_enable_port, port2);
    }

    qDebug() << "Enable on";
    return;
}


int HWF::init(){

    //short val;
    int Size = MultiByteToWideChar (CP_ACP, 0, "inpout32.dll", -1, NULL, 0);
    LPWSTR wUnicode = new WCHAR[Size];
    MultiByteToWideChar (CP_ACP, 0, "inpout32.dll", -1, wUnicode, Size);

    // LIB
    hLib = LoadLibrary(wUnicode);
    if (hLib == NULL){
        qDebug()  <<"ERR:\tinpout32 Ne mogu da ucitam.";
        return(-1);
    }

    else
        qDebug()  <<"OK :\tinpout32je ucitan.";


    //kao da trazi pointere na funkcije

    // INP32
    inp32 = (inpfuncPtr) GetProcAddress(hLib, "Inp32");
    if (inp32 == NULL){
        qDebug()  <<"ERR:\tGetProcAddress for Inp32 Failed.";
        return(-2);
    }

    else
        qDebug()  <<"OK:\tInp32 OK.";


    // OUT32
    oup32 = (oupfuncPtr) GetProcAddress(hLib, "Out32");
    if (oup32 == NULL){
        qDebug()  <<"ERR:\tGetProcAddress for Oup32 Failed.";
        return(-3);
    }
    else
        qDebug()  <<"OK:\tOup32 OK.";


    qDebug()  << "\nIskljucivanje izlaza..";
    port1=0x00;
    port2=0x03;

    oup32(0x378, port1);
    oup32(0x37a, port2);

    //  val = inp32(adr+1);
    // val=0;
    //  qDebug() << "vrednost je : " << val;

    // qDebug()  << "\nPisanje i citanje...";
    // oup32(0x378, 0x0f);
    // val = inp32(0x379);
    // qDebug() << "vrednost je : " << val;
    return(0);
}
void HWF::port_reset(void){
    port1=0x00;
    port2=0x03;
    oup32(0x378, port1);
    oup32(0x37a, port2);

}

//za L297 masinu step

/*clock  D0, D3, D6*/
//osa - po kojoj osi je pomeraj
//pom - velicina pomeraja u koracima
// v - brzina


int HWF::stepx(long pom, int w){

    QElapsedTimer timer;
    //   int counter = 1;
    //   int printEvery = 240;
    //  int yieldCounter = 0;
    //   double interval;
    //   double timeElapsed = 0.0;


    long i;
    long iz1;
    unsigned char pst;
    float kasn;
    kasn=(float)korak_x/(float)vxy;
    //interval= (double)korak_x/(double)vxy;

    //QThread::TimeCriticalPriority;

    iz1=labs(pom);

    if(pom>=0){
        if(x_dir_port==0x378){
            if(x_dir_logic==0)
                port1=port1 & ~x_dir_bit;
            else
                port1=port1 | x_dir_bit;
            oup32(x_dir_port, port1);
        }
    }
    else {
        if(x_dir_port==0x378){
            if(x_dir_logic==0)
                port1=port1 | x_dir_bit;
            else
                port1=port1 & ~x_dir_bit;
            oup32(x_dir_port, port1);
        }

    }
    //         ddelay_m(20);
    ddelay_m(msec_delay/10);

    //petlja

    for(i=0;i<iz1;i++){

        if(x_step_port==0x378){
            port1=port1 | x_step_bit;
            oup32(x_step_port, port1);
        }
        //        ddelay_m(10);
        ddelay_m(msec_delay/10);

        //Sleeper::usleep(500L);
        //Sleeper::msleep(1);
        if(x_step_port==0x378){
            port1=port1 & ~x_step_bit;
            oup32(x_step_port, port1);
        }
        //Sleeper::usleep(5000L);
        //                Sleeper::msleep(1);


        if(w!=0){
            ddelay_m(msec_delay*kasn);
        }
        //              if(w!=0)
        //            ddelay_m(vxy*8);


        /*
        if(w!=0){
             timeElapsed = 0.0;
              timer.start();
            forever {


                if( timeElapsed > interval ) break;

//            timer.start();

            // Running this just once means massive overhead from calling timer.start() so many times so quickly
  //          for( int i = 0; i < 100; i++ ) {
  //              yieldCounter++;
  //              QThread::yieldCurrentThread();
  //          }

            timeElapsed = ( double )timer.nsecsElapsed() / 1000.0 / 1000.0;

        }

             qDebug() << "timeElapsed =" << timeElapsed << "ms | interval =" << interval << "ms";
        }
*/

        pst= inp32(x_limit_max_port);
        //     qDebug() << pst << x_limit_min_bit << (pst & x_limit_min_bit);

        if(pom>0){
            xsa++;
            if(granicni_prekidaci!=0){
                if(x_limit_max_active==0)pst=~pst;
                if((pst & x_limit_max_bit)==0 ){
                    //          qDebug() << "maksimalna pozicija";
                    emit status_led_limit_x(1);
                    return(0);

                }
                else
                    emit status_led_limit_x(0);
            }

        }
        else{
            xsa--;
            if(granicni_prekidaci!=0){
                if(x_limit_min_active==0)pst=~pst;
                if((pst & x_limit_min_bit)==0 ){
                    //        qDebug() << "minimalna pozicija";
                    emit status_led_limit_x(1);
                    return(0);
                }
                else
                    emit status_led_limit_x(0);
            }
        }
        emit poz_updatex(xsa);

    }

    return(0);
}

int HWF::z_motor_direction(int dir){

    if(z_delay>0)  Sleeper::msleep(z_delay);

    if(dir==0){
        emit poz_updatez(1);

        if(z_dir_port==0x378){
            if(z_dir_logic==0)
                port1=port1 & ~z_dir_bit;
            else
                port1=port1 | z_dir_bit;
            oup32(z_dir_port, port1);
        }
    }
    else {
        emit poz_updatez(-1);

        if(z_dir_port==0x378){
            if(z_dir_logic==0)
                port1=port1 | z_dir_bit;
            else
                port1=port1 & ~z_dir_bit;
            oup32(z_dir_port, port1);
        }

    }

    return(0);
}

int HWF::stepz_delay(long pom){
    long i;
    long iz1;

    float kasn;
    kasn=(float)korak_z/(float)vz;

    iz1=labs(pom);
    for(i=0;i<iz1;i++){
        ddelay_m(msec_delay/10);
        ddelay_m(msec_delay*kasn);
    }
    return(0);
}
int HWF::stepz(long pom,int w){
    long i;
    long iz1;

    float kasn;
    kasn=(float)korak_z/(float)vz;

    iz1=labs(pom);

    if(pom>=0){
        if(z_dir_port==0x378){
            if(z_dir_logic==0)
                port1=port1 & ~z_dir_bit;
            else
                port1=port1 | z_dir_bit;
            oup32(z_dir_port, port1);
        }
    }
    else {
        if(z_dir_port==0x378){
            if(z_dir_logic==0)
                port1=port1 | z_dir_bit;
            else
                port1=port1 & ~z_dir_bit;
            oup32(z_dir_port, port1);
        }

    }

    ddelay_m(msec_delay/10);

    //ddelay_m(20);
    for(i=0;i<iz1;i++){

        if(z_step_port==0x378){
            port1=port1 | z_step_bit;
            oup32(z_step_port, port1);
        }

        //  Sleeper::usleep(500L);
        //ddelay_m(10);
        ddelay_m(msec_delay/10);

        //Sleeper::msleep(1);
        if(z_step_port==0x378){
            port1=port1 & ~z_step_bit;
            oup32(z_step_port, port1);
        }

        //                if(w!=0)
        //              ddelay_m(vz*8);

        if(w!=0){
            ddelay_m(msec_delay*kasn);
        }

        //ddelay_m(200);
        //Sleeper::usleep(10000L);
        //Sleeper::msleep(1);
        if(pom>0)
            zsa++;
        else zsa--;
        emit poz_updatez(zsa);
    }
    return(0);
}

int HWF::stepy(long pom, int w){
    long i;
    long iz1;
    float kasn;
    unsigned char pst;
    kasn=(float)korak_y/(float)vxy;

    iz1=labs(pom);

    if(pom>=0){
        if(y_dir_port==0x378){
            if(y_dir_logic==0)
                port1=port1 & ~y_dir_bit;
            else
                port1=port1 | y_dir_bit;
            oup32(y_dir_port, port1);
        }
    }
    else {
        if(y_dir_port==0x378){
            if(y_dir_logic==0)
                port1=port1 | y_dir_bit;
            else
                port1=port1 & ~y_dir_bit;

            oup32(y_dir_port, port1);
        }

    }

    ddelay_m(msec_delay/10);

    for(i=0;i<iz1;i++){

        if(y_step_port==0x378){
            port1=port1 | y_step_bit;
            oup32(y_step_port, port1);
        }
        ddelay_m(msec_delay/10);

        //           Sleeper::usleep(1L);
        //Sleeper::msleep(1);
        if(y_step_port==0x378){
            port1=port1 & ~y_step_bit;
            oup32(y_step_port, port1);
        }
        ddelay_m(msec_delay/10);
        if(w!=0){
            ddelay_m(msec_delay*kasn);
        }

        //ddelay_m(vxy*8);
        //            Sleeper::usleep(10L);
        //                Sleeper::msleep(1);

        pst= inp32(y_limit_max_port);
        //      qDebug() << pst << y_limit_max_bit << (pst & x_limit_max_bit);

        if(pom>0){
            ysa++;
            if(granicni_prekidaci!=0){
                if(y_limit_max_active==0)pst=~pst;
                if((pst & y_limit_max_bit)==0 ){
                    //     qDebug() << "maksimalna pozicija";
                    emit status_led_limit_y(1);
                    return(0);

                }
                else
                    emit status_led_limit_y(0);
            }
        }
        else{

            ysa--;
            if(granicni_prekidaci!=0){
                if(y_limit_min_active==0)pst=~pst;
                if((pst & y_limit_min_bit)==0 ){
                    //      qDebug() << "minimalna pozicija";
                    emit status_led_limit_y(1);
                    return(0);
                }
                else
                    emit status_led_limit_y(0);
            }
        }
        emit poz_updatey(ysa);
    }

    return(0);
}
int HWF::stepa(long pom, int w){
    long i;
    long iz1;
    float kasn;
    kasn=(float)korak_a/(float)vxy;

    iz1=labs(pom);

    if(pom>=0){
        if(a_dir_port==0x378){
            if(a_dir_logic==0)
                port1=port1 & ~a_dir_bit;
            else
                port1=port1 | a_dir_bit;
            oup32(a_dir_port, port1);
        }
    }
    else {
        if(a_dir_port==0x378){
            if(a_dir_logic==0)
                port1=port1 | a_dir_bit;
            else
                port1=port1 & ~a_dir_bit;

            oup32(a_dir_port, port1);
        }

    }

    ddelay_m(msec_delay/10);

    for(i=0;i<iz1;i++){

        if(y_step_port==0x378){
            port1=port1 | a_step_bit;
            oup32(a_step_port, port1);
        }
        ddelay_m(msec_delay/10);

        //           Sleeper::usleep(1L);
        //Sleeper::msleep(1);
        if(a_step_port==0x378){
            port1=port1 & ~a_step_bit;
            oup32(a_step_port, port1);
        }
        ddelay_m(msec_delay/10);
        if(w!=0){
            ddelay_m(msec_delay*kasn);
        }

        //ddelay_m(vxy*8);
        //            Sleeper::usleep(10L);
        //                Sleeper::msleep(1);

    }

    return(0);
}

void HWF::set_vxy(long input){
    vxy = input;
}
void HWF::set_vz(long input){
    vz = input;
}

void  HWF::ddelay_u(long pom)
{
    long i;
    long temp;
    char val;

    temp=0;


    for(i=0;i<pom/2;i++){
        char val;
        val = inp32(0x379);
        temp++;
    }
    for(i=0;i<pom/2;i++){
        temp--;
        val = inp32(0x379);
    }
}
void  HWF::ddelay_m(long pom)
{

    //    QThread::usleep(pom);
    //    return;
    char val;
    long i;
    for(i=0;i<pom;i++){
        //ddelay_u(10);
        val = inp32(0x379);
    }


}



float HWF::get_korak_x(void){return(korak_x);}
float HWF::get_korak_y(void){return(korak_y);}
float HWF::get_korak_z(void){return(korak_z);}
float HWF::get_korak_a(void){return(korak_a);}
