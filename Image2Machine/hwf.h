#ifndef HWF_H
#define HWF_H

#include <QObject>
#include <QThread>

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <windows.h> //placer avant inpout32.h

#define serial 1

class HWF : public QObject
{
    Q_OBJECT


    //promenljive za port

    int x_step_port;
    int x_step_bit;
    int x_dir_port;
    int x_dir_bit;
    int x_enable_port;
    int x_enable_bit;

    int y_step_port;
    int y_step_bit;
    int y_dir_port;
    int y_dir_bit;
    int y_enable_port;
    int y_enable_bit;

    int motor_port;
    int motor_bit;

    int kvasenje_port;
    int kvasenje_bit;

    int granicni_prekidaci;
    int x_limit_max_port;
    int x_limit_max_bit;
    int x_limit_max_active;

    int x_limit_min_port;
    int x_limit_min_bit;
    int x_limit_min_active;

    int y_limit_max_port;
    int y_limit_max_bit;
    int y_limit_max_active;

    int y_limit_min_port;
    int y_limit_min_bit;
    int y_limit_min_active;

    long  konstanta_kasnjenja;

    float korak_x;
    float korak_y;


    int x_dir_logic;
    int y_dir_logic;

    unsigned char port1;
    unsigned char port2;
    unsigned char port3;

    HINSTANCE hLib;
    typedef short _stdcall (*inpfuncPtr)(short portaddr);
    typedef void _stdcall (*oupfuncPtr)(short portaddr, short value);

    inpfuncPtr inp32;
    oupfuncPtr oup32;
    long vxy;
    long vz;
    long va;

    //unused for this project
    int a_limit_min_port;
    int a_limit_min_bit;
    int a_limit_min_active;
    int a_limit_max_port;
    int a_limit_max_bit;
    int a_limit_max_active;
    int z_limit_min_port;
    int z_limit_min_bit;
    int z_limit_min_active;
    int z_limit_max_port;
    int z_limit_max_bit;
    int z_limit_max_active;
    int z_step_port;
    int z_step_bit;
    int z_dir_port;
    int z_dir_bit;
    int z_enable_port;
    int z_enable_bit;
    int a_step_port;
    int a_step_bit;
    int a_dir_port;
    int a_dir_bit;
    int a_enable_port;
    int a_enable_bit;
    float korak_z;
    float korak_a;
    int z_dir_logic;
    int a_dir_logic;
    int  z_delay;


public:
   int hw_type;
    long xsa,ysa,zsa;

    explicit HWF(QObject *parent = 0);
    int read_port_ini(void);
    int mainboard_speed(void);

    //turning on/off laser
    void motor_on(void);
    void motor_off(void);

    //power of the laser (not impremented)
    void kvasenje_on(void);
    void kvasenje_off(void);

    //turning on/off motors
    void enable_off();
    void enable_on();

    //movement
    int stepx(long pom, int w);
    int stepy(long pom, int w);

    void ddelay_u(long kasni);
    void ddelay_m(long kasni);
    void port_reset(void);

    int init();
    //speed
    void set_vxy(long input);

    float get_korak_x(void);
    float get_korak_y(void);

    //unused for this project
    int stepz(long pom, int w);
    int stepa(long pom, int w);
    int z_motor_direction(int dir);
    int stepz_delay(long pom);
    void set_vz(long input);
    float get_korak_z(void);
    float get_korak_a(void);

    int z_axe_type;
    long msec_delay;


signals:

    void poz_updatex(long);
    void poz_updatey(long);
    void poz_updatez(long);
    void status_led_limit_x(int);
    void status_led_limit_y(int);

    void setLivePoint(double x, double y, bool isMM);
    void setVisCurrLine(int currLine);

public slots:

};


class Sleeper : public QThread
{
public:
    static void usleep(unsigned long usecs){QThread::usleep(usecs);}
    static void msleep(unsigned long msecs){QThread::msleep(msecs);}
    static void sleep(unsigned long secs){QThread::sleep(secs);}
};

#endif // HWF_H
