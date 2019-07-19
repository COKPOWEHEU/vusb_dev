#ifndef __PINMACRO_H__
#define __PINMACRO_H__
//ver. 1.5 at 06.06.17 - added PORT_TEST(), DDR_TEST(), PIN_TEST()
//ver. 1.4 at 26.04.17 - added 'flags'
//ver. 1.3 at 18.01.17 - changed num()->BITx() ; bnum()->BITMx()
//ver. 1.2 at 28.11.16 - added PIN_ON / PIN_OFF
//ver. 1.1 at 24.11.16 - added PORT_ON / PORT_OFF, PORT_T
//ver. 1.0 <  23.09.16 - added all

#define _PM_ARG1(a,...)	a
#define _PM_ARG2(a,b,...)	b
#define _PM_ARG3(a,b,c,...) c
#define _PM_CONCAT(a,b)	a##b
#define _PM_PORT(a,...)	PORT##a
#define _PM_DDR(a,...)	DDR##a
#define _PM_PIN(a,...)	PIN##a
//сколько же я геморроился пока не понял что нужно обертывать вызов еще в один #define
#define arg1(x)			_PM_ARG1(x)
#define arg2(x)			_PM_ARG2(x)
#define arg3(x)         _PM_ARG3(x)
#define concat(x)		_PM_CONCAT(x)
#define PORTx(x)		_PM_PORT(x)
#define DDRx(x)			_PM_DDR(x)
#define PINx(x)			_PM_PIN(x)
#define BITx(x)			_PM_ARG2(x)
#define BITMx(x)		(1<<_PM_ARG2(x))
/* Макроопределения отдельных выводов отдельных портов для простого использования
 * 
 * пример использования:
 * (0) #define ANY_PORT B,1,args
 * (1) DDR_0(ANY_PORT);
 * (2) if(PIN_1(ANY_PORT))
 * (3)   PORT_1(ANY_PORT);
 * 
 * Описание:
 * (0) Указание буквы порта и номера вывода. Далее могут следовать прочие аргументы (args), они будут проигнорированы
 * (1) Переключение направления порта на вход. В данном случае DDRB1 = 0.
 * (2) Проверка пина на лог.1 (PIN_0 проверяет на 0 соответственно). В данном случае if( PINB0 == 1 )
 * (3) Переключение вывода в лог.1. В данном случае PORTB1 = 1
 */
#define PORT_0(x)	do{_PM_PORT(x) &=~(1<<_PM_ARG2(x));}while(0)
#define PORT_1(x)	do{_PM_PORT(x) |= (1<<_PM_ARG2(x));}while(0)
#define PORT_T(x)   do{_PM_PORT(x) ^= (1<<_PM_ARG2(x));}while(0)
#define DDR_0(x)	do{_PM_DDR(x) &=~(1<<_PM_ARG2(x));}while(0)
#define DDR_1(x)	do{_PM_DDR(x) |= (1<<_PM_ARG2(x));}while(0)
#define PIN_0(x)	(!(_PM_PIN(x) & (1<<_PM_ARG2(x))))
#define PIN_1(x)	(_PM_PIN(x) & (1<<_PM_ARG2(x)))
/* Расширение предыдущего определения, добавляет активное состояние (3-й аргумант; последующие игнорируются)
 * 
 * Пример использования:
 * (0) #define LED C,2,0
 * (1) #define BTN B,4,0
 * (2) if(PIN_ON(BTN))
 * (3)   PORT_ON(LED);
 * 
 * Описание:
 * (0) Указание буквы порта, номера вывода и "включенное состояние". В данном случае диод подключен катодом к выводу
 * (1) Указание буквы порта, номера вывода и "включенное состояние". В данном случае кнопка подтянута к питанию с включается коммутацией на землю
 * (2) Проверка "если кнопка нажата". В данном случае if( PINB4 == 0 )
 * (3) Переключение вывода во "включенное" состояние. В данном случае PORTC2 = 0
 */
#define PORT_OFF(x) do{\
    _PM_PORT(x) = (_PM_PORT(x) &~ (_PM_ARG3(x)<<_PM_ARG2(x)) ) | ((!_PM_ARG3(x))<<_PM_ARG2(x));\
  }while(0)
#define PORT_ON(x) do{\
    _PM_PORT(x) = (_PM_PORT(x) &~ ((!_PM_ARG3(x))<<_PM_ARG2(x)) ) | (_PM_ARG3(x)<<_PM_ARG2(x));\
  }while(0)
  
#define PORT_TEST(x)  ((_PM_PORT(x) & (1<<_PM_ARG2(x)))==(_PM_ARG3(x) & (1<<_PM_ARG2(x))))
#define DDR_TEST(x)   ((_PM_DDR(x) & (1<<_PM_ARG2(x)))==(_PM_ARG3(x) & (1<<_PM_ARG2(x))))
#define PIN_TEST(x)   ((_PM_PIN(x) & (1<<_PM_ARG2(x)))==(_PM_ARG3(x) & (1<<_PM_ARG2(x))))
//#define PIN_OFF(x)   ( (( _PM_PIN(x)>>_PM_ARG2(x)) & 0x01) ^ _PM_ARG3(x))
//#define PIN_ON(x)  ( ((~_PM_PIN(x)>>_PM_ARG2(x)) & 0x01) ^ _PM_ARG3(x))
  
//#define PIN_OFF(x)  ((_PM_PIN(x) & (1<<_PM_ARG2(x)))^((1-_PM_ARG3(x)) & (1<<_PM_ARG2(x))))
//#define PIN_ON(x)   ((_PM_PIN(x) & (1<<_PM_ARG2(x)))^(( _PM_ARG3(x) ) & (1<<_PM_ARG2(x))))
#define PIN_OFF(x)  ((_PM_PIN(x) & (1<<_PM_ARG2(x)))!=(_PM_ARG3(x) & (1<<_PM_ARG2(x))))
#define PIN_ON(x)   ((_PM_PIN(x) & (1<<_PM_ARG2(x)))==(_PM_ARG3(x) & (1<<_PM_ARG2(x))))

/* Работа с флагами
 * 
 * Пример использования:
 * (0) uint8_t flags = 0;
 * (1) #define F_FLAG flags,0
 * (2) #define F_ADSC ADCSRA,ADSC
 * (3) if(FLAG_TST(F_FLAG))
 * (4)   FLAG_SET(F_ADSC);
 * 
 * Описание:
 * (0) Объявление переменной
 * (1) Объявление 0-го бита переменной flags флагом F_FLAG
 * (2) Объявление бита ADSC переменной (регистра) ADCSRA именованным флагом (почему бы и нет!)
 * (3) Проверка если флаг равен лог.1
 * (4) Выставление флага в лог.1 (оно же - выставление бита ADSC регистра ADCSRA)
 */
#define FLAG_SET(x) do{_PM_ARG1(x) |= (1<<_PM_ARG2(x));}while(0)
#define FLAG_CLR(x) do{_PM_ARG1(x) &=~(1<<_PM_ARG2(x));}while(0)
#define FLAG_TST(x) (_PM_ARG1(x) & (1<<_PM_ARG2(x)))

#define NOINLINE __attribute__((__noinline__))

#endif
