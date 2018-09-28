#ifndef TMCL_H
#define TMCL_H

#include <QObject>

//Opcodes of all TMCL commands that can be used in direct mode

#define TMCL_ROR 1
#define TMCL_ROL 2
#define TMCL_MST 3
#define TMCL_MVP 4
#define TMCL_SAP 5
#define TMCL_GAP 6
#define TMCL_STAP 7
#define TMCL_RSAP 8
#define TMCL_SGP 9
#define TMCL_GGP 10
#define TMCL_STGP 11
#define TMCL_RSGP 12
#define TMCL_RFS 13
#define TMCL_SIO 14
#define TMCL_GIO 15
#define TMCL_SCO 30
#define TMCL_GCO 31
#define TMCL_CCO 32



//Options for MVP commandds
#define MVP_ABS 0
#define MVP_REL 1
#define MVP_COORD 2

//Options for RFS command
#define RFS_START 0
#define RFS_STOP 1
#define RFS_STATUS 2



//#define FALSE 0
//#define TRUE 1

//Result codes for GetResult
#define TMCL_RESULT_OK 0
#define TMCL_RESULT_NOT_READY 1
#define TMCL_RESULT_CHECKSUM_ERROR 2


#define TMCL_GET_FRM_VER 136
#define TMCL_REQUEST_TARGET_POS_REACHED_EVENT 138


/**
 * @defgroup TypeCodes TMCL operation type codes.
 *
 * Operation type codes for TMCL commands.
 */
#define TMCL_MVP_ABS		0	/**< Moving to absolute position */
#define TMCL_MVP_REL		1	/**< Moving to relative position */
#define TMCL_MVP_COORD		2	/**< Moving to coordinate */
#define TMCL_RFS_START		0	/**< Starting reference search */
#define TMCL_RFS_STOP		1	/**< Stopping reference search */
#define TMCL_RFS_STATUS		2	/**< Checking status of reference search */
/**@} END OF TypeCodes*/



/**
 * @defgroup AxisParam Axis Parameters
 *
 * Axis parameters to be used with TMCL_SAP, TMCL_GAP, TMCL_AAP, TMCL_STAP and TMCL_RSAP
 * for TMCM-3xx/11x/109/61x modules.
 *
 * @todo These are not complete.
 */
/**@{*/
/** @defgroup RWParam	Read-Write Parameters
 *
 *  Parameters that can be read and written
 */
/**@{*/
/** Basic parameters */
#define TMCL_AP_TARGET_POS	 	  0	/**< Target (next) postition */
#define TMCL_AP_CURR_POS	 	  1	/**< Current position */
#define TMCL_AP_TARGET_SPEED 	  2	/**< Desired speed in velocity mode */
#define TMCL_AP_MAX_POS_SPEED	  4	/**< Maximum positioning speed */
#define TMCL_AP_MAX_ACCEL		  5	/**< Maximum acceleration */
#define TMCL_AP_ABS_CURRENT	 	  6	/**< Maximum absolute current */
#define TMCL_AP_STBY_CURRENT 	  7	/**< Maximum standby current */
#define TMCL_AP_DISABLE_LIMIT_R	 12 /**< Disable the right limit switch */
#define TMCL_AP_DISABLE_LIMIT_L	 13 /**< Disable the left limit switch */
#define TMCL_AP_SR_PRESC		 14 /**< @note Currently not used */

#define TMCL_AP_SET_POS	 	  129	/**< postavi zadatu poziciju */

/** Extended Parameters */
/* ... */
#define TMCL_AP_MICROSTEPS			140	/**< Microstep mode (@see TMCLMicrosteps) */
#define TMCL_AP_MAX_CURR_REST		143 /**< Maximal current at rest (Normally use #TMCL_AP_ABS_CURRENT and #TMCL_AP_STBY_CURRENT) */
#define TMCL_AP_MAX_CURR_LOW_ACCEL	144	/**< Maximal current at low acceleration (Normally use #TMCL_AP_ABS_CURRENT and #TMCL_AP_STBY_CURRENT) */
#define TMCL_AP_MAX_CURR_HIGH_ACCEL	145	/**< Maximal current at high acceleration (Normally use #TMCL_AP_ABS_CURRENT and #TMCL_AP_STBY_CURRENT) */
#define TMCL_AP_RFS_MODE			193	/**< Reference search mode */
#define TMCL_AP_RFS_SPEED			194 /**< Reference search speed mode */
#define TMCL_AP_RFS_SW_SPEED		195 /**< Reference search speed at switch position */
/** 196 and 197 reserved */
/* ... */
/**@} END OF RWParam */

/** @defgroup ROParam	Read-Only Parameters
 *
 *  Parameters that can only be read
 */
/**@{*/
/** Basic parameters */
#define TMCL_AP_CURR_SPEED	 	  3	/**< Current speed */
#define TMCL_AP_POS_REACHED 	  8	/**< Target position reached */
#define TMCL_AP_LIMIT_R		 	  9	/**< Right limit switch status */
#define TMCL_AP_LIMIT_L 		 10	/**< Left limit switch status */

#define TMCL_EXTENDED_ERROR_FLAGS 		 207	/**< extended error flags, 1-stall, 2-enkoder */
#define TMCL_DRIVER_ERROR 		 208	/**< driver error bits */

/** Extended Parameters */
/* ... */


/**@} END OF ROParam */
/**@} END OF AxisParam */






//! Motor configuration data

typedef unsigned char UCHAR;                   //!< 8 bits unsigned
typedef unsigned short USHORT;                 //!< 16 bits unsigned
typedef unsigned int UINT;                     //!< 32 bits unsigned

typedef struct
{

  UINT VMax;                   //!< maximum positioning velocity
  UINT AMax;                   //!< maximum acceleration
  UCHAR PulseDiv;              //!< pulse divisor
  UCHAR RampDiv;               //!< ramp divisor
  int microstep_res;          //!< microstep rezolution
  UCHAR IRun;                  //!< run current (0..255)
  UCHAR IStandby;              //!< stand-by current(0..255)

   //! stallguard2  configuration data
  int ST_VMin;              //!< minimum speed for stallGuard
  int ST_threshold;              //!< threshold level for stall output
  int ST_filter;              //!< Enables the stallGuard2™ filter for more precision of the measurement

  // Setovanje za jedan od motora tokom eksperimenta
  //StallVmin 80
  // Threshold = -20 SAp 174
  //IRUN 33
  //Amax 30

  UINT FreewheelingDelay;      //!< freewheeling delay time (*10ms)
  UINT SettingDelay;           //!< delay for switching to stand-by current (*10ms)

  //! coolStep configuration data
  UINT ThresholdSpeed;         //!< coolStep threshold speed
  UCHAR SlowRunCurrent;        //!< coolStep slow run current
  UCHAR HysteresisStart;       //!< coolStep hysteresis start
   int smer_rotacije;          //!< smer rotacije motora

} TMotorConfig;


#include "qextserialport.h"
#include "qextserialenumerator.h"
#include <QtCore/QtGlobal>
#include <QtCore>
#include <QThread>
#include "hwf.h"

class QTimer;
class QextSerialPort;
class QextSerialEnumerator;

using namespace std;
class Tmcl : public QObject

{
    Q_OBJECT

public: Tmcl(QWidget* parent); //.... don't forget the parent/child

public slots:

protected:

private:
     int hw_type;
public:

    Tmcl(QObject *parent = 0);
    //! TMCL reply
    unsigned char TMCL_Checksum;
    unsigned char TMCL_Address;
    unsigned char TMCL_Status;
    unsigned char TMCL_Instr;
    unsigned char  TMCL_Byte[4];   //!< reply value as 4 bytes
    long TMCL_Value;
    int motor_speed;

    long poz_nula[3];

       QTimer *timer;
       QextSerialPort *portx;
       QextSerialPort *porty;

       QextSerialEnumerator *enumerator;
       QByteArray TxBuffer;

    void openSerialPortx();
    void closeSerialPortx();

    void writeData(const QByteArray &data);
    void readData();
    int read_port_ini(void);

    void SendCmd(QString data);
    int  GetResult(int port,int check);
    int GetResult_ok(int port,int check);

    int get_status(int motor);
    int RequestTargetPositioReachedEvent(int motor );

    int  PollComport(int port);
    ParityType parity(QString str);
    DataBitsType databits(QString  str);
    StopBitsType stopbits(QString  str);
    FlowType flowcontrol(QString str);

     QString x_com_port;
     BaudRateType x_baudrate;
     DataBitsType x_databits;
     StopBitsType x_stopbits;
     ParityType x_parity;
     FlowType x_flowcontrol;
     //x_mode=1
     long x_time_interval;


    TMotorConfig MotorConfig[2];

    int minimalno_kretanje;

signals:

    void poz_updatex(long);
    void poz_updatey(long);
    void poz_updatez(long);

    void setLivePoint(double x, double y, bool isMM);
    void setVisCurrLine(int currLine);

};



#endif
