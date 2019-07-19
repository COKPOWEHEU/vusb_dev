/* Имя: usbconfig.h
 * Проект: V-USB, драйвер AVR USB	
 * Автор: Christian Starkjohann
 * Перевод: microsin.ru + XANDER
 * Дата создания: 2005-04-01
 * Табуляция: 4
 * Copyright: (c) 2005 by OBJECTIVE DEVELOPMENT Software GmbH
 * Лицензия: GNU GPL v2 (см. License.txt) или проприетарная (CommercialLicense.txt)
 * Ревизия: $Id: usbconfig-prototype.h 785 2010-05-30 17:57:07Z cs $
 */
 
#ifndef __usbconfig_h_included__
#define __usbconfig_h_included__

/* Основное описание:
 * Этот файл является примером конфигурации (со встроенной документацией) для
 * драйвера USB. Он кофигурирует V-USB для сигнала USB D+, подсоединенного к
 * порту D бит 2 (который также является входом аппаратного прерывания 0 [INT0]
 * на многих чипах) и сигнала USB D- к порту D бит 4. Вы можете подсоединить
 * эти сигналы на любой другой порт, однако D+ должен быть также подключен на
 * вход аппаратного прерывания INT0, которое имеет наивысший приоритет,
 * см. секцию в конце файла).
 */

/* ------------------------- Аппаратные настройки -------------------------- */

#define USB_CFG_IOPORTNAME      D
/* Указан порт, к которому подключена шина USB. Если Вы сконфигурируете "B",
 * будут использоваться регистры PORTB, PINB и DDRB.
 */
#define USB_CFG_DMINUS_BIT      3
/* Это номер бита в USB_CFG_IOPORT, куда подключен сигнал D-, может 
 * использоваться любой бит в порте.
 */
#define USB_CFG_DPLUS_BIT       2
/* Это номер бита в USB_CFG_IOPORTNAME, куда подключен сигнал D+, может
 * использоваться любой бит в порте. Пожалуйста, примите во внимание, что D+
 * должен быть также подсоединен к ножке прерывания INT0! [Вы можете также
 * использовать другие прерывания, см. секцию "Optional MCU Description" далее,
 * или Вы можете подсоединить D- к прерыванию, как это необходимо если Вы
 * используете опцию USB_COUNT_SOF. Если Вы используете D- для прерывания,
 * оно будет срабатывать также от маркеров Start-Of-Frame каждую
 * милисекунду.]
 */
#define USB_CFG_CLOCK_KHZ       (F_CPU/1000)
/* Тактовая частота чипа AVR в килогерцах. Допустимые величины 12000, 15000,
 * 16000, 16500 и 20000. Версия кода на 16.5 МГц не требует кварца, она
 * допускает уход частоты +/- 1% от номинальной. Все другие частоты требуют
 * точности 2000 ppm, и в этом случае обязательно нужен кварц!
 * Константа F_CPU должна быть определена в проекте, не нужно нигде писать
 * вручную.
 */ 
#define USB_CFG_CHECK_CRC       0
/* Установите 1 если хотите чтобы драйвер V-USB проверял целостность входящих
 * пакетов данных (CRC сумы). Проверка CRC требует достаточно много места для
 * кода и на данный момент поддерживается только для частоты 18 MHz. Так что
 * обязательно используйте частоту 18 MHz для включения этой опции.
 */

/* ----------------- Дополнительные Аппаратные настройки ------------------- */

#define USB_CFG_PULLUP_IOPORTNAME   D
/* Если Вы подсоединили 1.5k pullup резистор от D- к ножке порта вместо V+, 
 * Вы можете подсоединять и отсоединять устройство програмным путем вызовая
 * макрос usbDeviceConnect() и usbDeviceDisconnect() (см. usbdrv.h).
 * Эта константы задают порт, к которому подключен pullup резистор.
 */
#define USB_CFG_PULLUP_BIT          4
/* This constant defines the bit number in USB_CFG_PULLUP_IOPORT (defined
 * above) where the 1.5k pullup resistor is connected. See description
 * above for details.
 * Эта константа определяет номер бита порта в USB_CFG_PULLUP_IOPORTNAME (задан
 * више) куда подключен 1.5k pullup резистор. См. описание, указанное ранее.
 */

/* ----------------------- Функциональный диапазон ------------------------- */

#define USB_CFG_HAVE_INTRIN_ENDPOINT    1
/* Задайте здесь 1, если Вы хотите скомпилировать версию с двумя конечными
 * точками (endpoints): default control endpoint 0 и interrupt-in endpoint
 * (любой другой endpoint номер).
 */
#define USB_CFG_HAVE_INTRIN_ENDPOINT3   0
/* Задайте здесь 1, если Вы хотите скомпилировать версию с тремя конечными
 * точками (endpoints): default control endpoint 0, interrupt-in endpoint 3
 * (или номер, сконфигурированный далее) и catch-all default interrupt-in
 * endpoint как указано ранее.
 * Вы должны также задать USB_CFG_HAVE_INTRIN_ENDPOINT в 1 для этой опции.
 */
#define USB_CFG_EP3_NUMBER              3
/* Если так называемая конечная точка (endpoint) 3 используется, можно теперь
 * ее сконфигурировать на любой другой номер конечной точки (за исключением 0).
 * Если не задано, то по умолчанию 3.
 */
/* #define USB_INITIAL_DATATOKEN           USBPID_DATA1 */
/* Этот define определяет условие старта (startup condition) для переключения
 * данных (data toggling) в момент конечных точек interrupt/bulk 1 и 3.
 * По умолчанию USBPID_DATA1. Поскольку токен переключается ПЕРЕД отправкой
 * любых данных, первый пакет отправляется  c противоположной величиной
 * от этой конфигурации!
 */
#define USB_CFG_IMPLEMENT_HALT          0
/* Задайте здесь 1, если Вы хотите также реализовать возможность ENDPOINT_HALT
 * для endpoint 1 (interrupt endpoint). Несмотря на то, что Вам эта возможность
 * может не понадобиться, она требуется по стандарту. Мы ввели эту опцию в
 * конфигурацию потому что эта функция значительно раздувает код.
 */
#define USB_CFG_SUPPRESS_INTR_CODE      0
/* Задайте здесь 1, если Вы хотите определить interrupt-in endpoints, но не
 * хотите отправлять любые данные с их помощью. Если эта опция включена, то
 * функции usbSetInterrupt() и usbSetInterrupt3() будут пропущены. Полезно
 * при необходимости interrupt-in endpoints для соответсвия интерфейсу
 * (например HID), но без отправки даных с их помощью. Прилично экономит
 * флеш память для кода и оперативную память буффера передачи данных.
 */
#define USB_CFG_INTR_POLL_INTERVAL      100
/* Если Вы скомпилировали версию с endpoint 1 (interrupt-in), то здесь задается
 * интервал опроса (poll interval). Величина указана в милисекундах и не должна
 * быть меньше 10 ms для низкоскоростных устройств.
 */
#define USB_CFG_IS_SELF_POWERED         0
/* Задайте здесь 1 если устройство имеет собственный источник питания.
 * Установка в 0 соответствует устройству, запитанному от шины USB.
 */
#define USB_CFG_MAX_BUS_POWER           100
/* Установите эту переменную на max величину потребления мощности Вашего
 * устройства от шины USB. Величина указана в милиамперах.
 */
#define USB_CFG_IMPLEMENT_FN_WRITE      1
/* Задайте здесь 1, если Вы хотите, чтобы функция usbFunctionWrite()
 * вызывалась для передач control-out. Задайте 0 если Вам это не нужно,
 * что позволит сэкономить несколько байт.
 */
#define USB_CFG_IMPLEMENT_FN_READ       1
/* Задайте 1 если Вам нужно отправлять управляющие запросы ответы, которые
 * генерируются "на лету" при вызове usbFunctionRead(). Если Вам нужно только
 * отправить данные из статического буфера, задайте 0 и возвращайте данные из
 * usbFunctionSetup(). Это сэкономит некоторое количество байт.
 */
#define USB_CFG_IMPLEMENT_FN_WRITEOUT   0
/* Задайте 1 если Вы хотите использовать interrupt-out (или bulk out) 
 * endpoints. Вы должны реализовать функцию usbFunctionWriteOut(), которая
 * принимает все interrupt/bulk данные, отправленные в любую endpoint,
 * отличную от 0. Номер endpoint можно найти в 'usbRxToken'.
 */
#define USB_CFG_HAVE_FLOWCONTROL        0
/* Задайте 1, если Вы хотите реализовать управление потоком (flowcontrol)
 * через данные USB. Смотрим описание макроса usbDisableAllRequests() и
 * usbEnableAllRequests() в usbdrv.h.
 */
#define USB_CFG_DRIVER_FLASH_PAGE       0
/* Если устройство имеет больше чем 64 kBytes флеш памяти, определите это
 * в 64 k страницу где находятся константы (дескрипторы) драйвера. Другими
 * словами: установить в 1 для загрузчиков (boot loaders) на ATMega128.
 */
#define USB_CFG_LONG_TRANSFERS          0
/* Задайте 1, если Вы хотите отправлять/принимать блоки оазмером более 254 байт
 * в одиночной control-in или control-out передаче. Обратите внимание, что эта
 * возможность увеличивает размер драйвера.
 */
/* #define USB_RX_USER_HOOK(data, len)     if(usbRxToken == (uchar)USBPID_SETUP) blinkLED(); */
/* Этот macro является крюком (hook), если Вы хотите сделать нечто
 * нетрадиционное. Если макрос задан, он вставляется в начало обработки
 * принимаемого сообщения. Если Вы "съели" (обработали) сообщение и не хотите
 * включать дальнейшую обработку по умолчанию, сделайте обычный возврат после
 * Ваших действий. Одно из возможных применений (кроме отладки) - мигание
 * статусным светодиодом при каждом пакете.
 */
/* #define USB_RESET_HOOK(resetStarts)     if(!resetStarts){hadUsbReset();} */
/* Этот macro является хуком для обработки события USB RESET. Он имеет один
 * параметр, позволяющий отличать старт состояния RESET от окончания
 * состояния RESET.
 */
/* #define USB_SET_ADDRESS_HOOK()              hadAddressAssigned(); */
/* Этот macro (если задан) выполняется когда принят запрос USB SET_ADDRESS.
 */
#define USB_COUNT_SOF                   0
/* Задайте здесь 1, если Вам нужна глобальная переменная "usbSofCount",
 * в которой подсчитываются пакеты SOF. Эта возможность требует подключения
 * аппаратного прерывания к D- вместо D+.
 */
/* #ifdef __ASSEMBLER__
 * macro myAssemblerMacro
 *     in      YL, TCNT0
 *     sts     timer0Snapshot, YL
 *     endm
 * #endif
 * #define USB_SOF_HOOK                    myAssemblerMacro
 * This macro (if defined) is executed in the assembler module when a
 * Start Of Frame condition is detected. It is recommended to define it to
 * the name of an assembler macro which is defined here as well so that more
 * than one assembler instruction can be used. The macro may use the register
 * YL and modify SREG. If it lasts longer than a couple of cycles, USB messages
 * immediately after an SOF pulse may be lost and must be retried by the host.
 * What can you do with this hook? Since the SOF signal occurs exactly every
 * 1 ms (unless the host is in sleep mode), you can use it to tune OSCCAL in
 * designs running on the internal RC oscillator.
 * Please note that Start Of Frame detection works only if D- is wired to the
 * interrupt, not D+. THIS IS DIFFERENT THAN MOST EXAMPLES!
 */
#define USB_CFG_CHECK_DATA_TOGGLING     0
/* Установите в 1 если хотите фильтровать повторяющиеся (duplicate) пакеты
 * данных отправленные хостом. Повторения возникают лишь в результате ошибок
 * связи, когда хост не получил ACK. Помните, фильтровать придется самим у
 * usbFunctionWriteOut() и usbFunctionWrite(). Используйте глобальную 
 * usbCurrentDataToken и статическую переменную для каждого 
 * control- и out-endpoint для проверки на повторяющиеся пакеты.
 */
#define USB_CFG_HAVE_MEASURE_FRAME_LENGTH   0
/* Задайте здесь 1, если Вам нужна функция usbMeasureFrameLength() - тогда она
 * скомпилируется. Эта функция может использоваться для подстройки генератора
 * RC микроконтроллера AVR.
 */
#define USB_USE_FAST_CRC                0
/* Ассемблерная часть имеет две реализации для алгоритма CRC. Одна быстрее,
 * вторая меньшая по объему. Используйте меньшую лишь в передачах сообщений
 * не критичных к таймингам. Быстрая реализация требует 31 цикл на байт, а
 * меньшая требует 61 - 69 циклов. Быстрая реализация может быть на 32 байта
 * кода большей при передачах данных и выводит AVR на состояние близкое к
 * пределу возможностей.
 */

/* ---------------- Параметры, описывающие USB-устройство ------------------ */

#define  USB_CFG_VENDOR_ID       0xc0, 0x16 /* = 0x16c0 = 5824 = voti.nl */
/* USB вендор ID для устройства, младший байт идет первым. Если Вы
 * зарегистрировали свой собственный Vendor ID, укажите его здесь. В
 * противном случае используйте одну из свободно предоставляемых пар
 * VID/PID obdev. Прочитайте правила USB-IDs-for-free.txt!
 */
#define  USB_CFG_DEVICE_ID       0xdf, 0x05 /* obdev's shared PID for HIDs */
/* Это ID продукта, младший байт идет первым. Он интерпретируется в контексте
 * vendor ID. Если Вы зарегистрировали свой собственный VID на usb.org, или
 * если пользуетесь лицензией на PID от кого-нибудь еще, укажите его здесь.
 * Иначе используйте предоставленную obdev свободно распространяемую 
 * пару VID/PID. Прочитайте правила USB-IDs-for-free.txt!
 */
#define USB_CFG_DEVICE_VERSION  0x00, 0x01
/* Номер версии устройства: сначала младший номер, затем старший номер.
 */
#define USB_CFG_VENDOR_NAME     'C', 'O', 'K', 'P', 'O', 'W', 'E', 'H', 'E', 'U'
#define USB_CFG_VENDOR_NAME_LEN 10
/* Здесь указывают имя вендора (vendor name), возвращаемое устройством.
 * Имя должно предоставляться как список символов в одиночных
 * кавычках, а USB_CFG_VENDOR_NAME_LEN задает их количество. Символы
 * интерпретируются как Unicode (UTF-16). Если Вам не нужно имя вендора,
 * закомментируйте этот макрос. ВСЕГДА указывайте имя вендора, содержащее Ваше
 * доменное имя Internet, если Вы используете свободно распространяемую пару
 * obdev VID/PID. За деталями обращайтесь к файлу USB-IDs-for-free.txt.
 */
#define USB_CFG_DEVICE_NAME    'H', 'I', 'D', ' ', 'e', 'x', 'a', 'm', 'p', 'l', 'e'
#define USB_CFG_DEVICE_NAME_LEN 11
/* Здесь указывается имя устройства (device name) таким же способом, как и в
 * предыдущем параметре указывается имя вендора. Если Вам не нужно имя
 * устройства, закомментируйте этот макрос. См. файл USB-IDs-for-free.txt
 * перед назначением имени, если Вы используете свободно распространяемую
 * пару obdev VID/PID.
 */
#define USB_CFG_SERIAL_NUMBER   '0', '.', '1'
#define USB_CFG_SERIAL_NUMBER_LEN   3
/* Серийный номер (serial number). Если серийный номер Вам не нужен,
 * закомментируйте макрос. Может оказаться полезным предоставить serial number
 * по другим соображениям, отличающимся от времени компиляции. См. секцию,
 * описывающую свойства дескриптора далее, где указано как сделать точное
 * управление дескрипторами USB (USB descriptors), такими как string
 * descriptor для серийного номера.
 */
#define USB_CFG_DEVICE_CLASS        0
#define USB_CFG_DEVICE_SUBCLASS     0
/* См. спецификацию USB, если Вы хотите приспособить устройство к
 * существующему классу устройств (device class). Класс 0xff является
 * "специфическим для вендора" ("vendor specific").
 */
#define USB_CFG_INTERFACE_CLASS     3
#define USB_CFG_INTERFACE_SUBCLASS  0
#define USB_CFG_INTERFACE_PROTOCOL  0
/* См. спецификацию USB, если Вы хотите, чтобы устройство отвечало требованиям
 * существующего класса или протокола. Следующие классы должны быть установлены
 * на уровне интерфейса:
 * HID class 3, нетребуется подклассов и протоколов (может оказаться полезным!)
 * CDC class 2, используйте подкласс 2 и протокол 1 для ACM
 */
#define USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH    22
/* Задайте здесь длину HID report descriptor, если Вы реализовали HID
 * устройство. Иначе не задавайте этот параметр или установите его в 0.
 * Если используете этот параметр, то Вы должны добавить символьный массив
 * PROGMEM, названный "usbHidReportDescriptor", в код, который содержит
 * report descriptor. Не забудьте держать массив и этот параметр в
 * соответствии!
 */

/* #define USB_PUBLIC static */
/* Используйте этот define, если Вы используете директиву #include usbdrv.c
 * вместо линкования. Эта техника экономит несколько байт flash-памяти.
 */

/* --------------- Тонкое управление через дескрипторы USB ----------------- */
/* Если Вы не хотите использовать дескрипторы по умолчанию драйвера USB,
 * Вы можете предоставить Ваши собственные. Это может быть сделано 
 * как (1) статические данные фиксированной длины в памяти flash, 
 * (2) статические данные фиксированной длины в RAM, или (3) динамические во
 * время выполнения в функции usbFunctionDescriptor(). Смотрим usbdrv.h для
 * большей информации по таким дескрипторам. Поддержка дескрипторов
 * конфигурируется через свойства дескриптора. Если не заданы свойства или
 * если они равны 0, используется дескриптор по умолчанию.
 * Возможны свойства:
 *   + USB_PROP_IS_DYNAMIC: данные для дескриптора должны быть захвачены (fetched)
 *     во время выполнения через usbFunctionDescriptor(). Если используется
 *     механизм usbMsgPtr, данные по умолчанию у FLASH.
 *     Добавьте свойство USB_PROP_IS_RAM если хотите RAM указатели.
 *   + USB_PROP_IS_RAM: данные, возвращенные usbFunctionDescriptor() или
 *     найденные в статической памяти RAM, не в памяти flash.
 *   + USB_PROP_LENGTH(len): если данные в статической памяти (RAM или flash),
 *     драйвер должен знать длину дескриптора. Дескриптор сам по себе найден по адресу,
 *     известному как идентификатор (см. далее).
 * Список статических имен дескрипторов (должен быть задекларирован PROGMEM в flash):
 *   char usbDescriptorDevice[];
 *   char usbDescriptorConfiguration[];
 *   char usbDescriptorHidReport[];
 *   char usbDescriptorString0[];
 *   int usbDescriptorStringVendor[];
 *   int usbDescriptorStringDevice[];
 *   int usbDescriptorStringSerialNumber[];
 * Другие дескрипторы не могут быть предоставлены статически, они должны
 * предоставляться динамически во время выполнения.  
 *
 * Свойства дескрипторов объединяются по ИЛИ либо складываются вместе, например:
 * #define USB_CFG_DESCR_PROPS_DEVICE   (USB_PROP_IS_RAM | USB_PROP_LENGTH(18))
 *
 * Следующие дескрипторы заданы:
 *   USB_CFG_DESCR_PROPS_DEVICE
 *   USB_CFG_DESCR_PROPS_CONFIGURATION
 *   USB_CFG_DESCR_PROPS_STRINGS
 *   USB_CFG_DESCR_PROPS_STRING_0
 *   USB_CFG_DESCR_PROPS_STRING_VENDOR
 *   USB_CFG_DESCR_PROPS_STRING_PRODUCT
 *   USB_CFG_DESCR_PROPS_STRING_SERIAL_NUMBER
 *   USB_CFG_DESCR_PROPS_HID
 *   USB_CFG_DESCR_PROPS_HID_REPORT
 *   USB_CFG_DESCR_PROPS_UNKNOWN (for all descriptors not handled by the driver)
 *
 * Помните что string дескрипторы не просто строки, они являются
 * Unicode strings prefixed with a 2 byte header. Пример:
 * int  serialNumberDescriptor[] = {
 *     USB_STRING_DESCRIPTOR_HEADER(6),
 *     'S', 'e', 'r', 'i', 'a', 'l'
 * };
 */

#define USB_CFG_DESCR_PROPS_DEVICE                  0
#define USB_CFG_DESCR_PROPS_CONFIGURATION           0
#define USB_CFG_DESCR_PROPS_STRINGS                 0
#define USB_CFG_DESCR_PROPS_STRING_0                0
#define USB_CFG_DESCR_PROPS_STRING_VENDOR           0
#define USB_CFG_DESCR_PROPS_STRING_PRODUCT          0
#define USB_CFG_DESCR_PROPS_STRING_SERIAL_NUMBER    0
#define USB_CFG_DESCR_PROPS_HID                     0
#define USB_CFG_DESCR_PROPS_HID_REPORT              0
#define USB_CFG_DESCR_PROPS_UNKNOWN                 0

/* --------------------- Дополнительные настройки MCU ---------------------- */
/* Следующие конфигурации работают по умолчанию в usbdrv.h. Вам обычно не нужно
 * устанавливать их. Только если Вам нужно запустить драйвер на устройстве,
 * которое пока не обрабатывается компилятором, который не полностью
 * поддерживается (как, например, IAR C) или если используете прерывание,
 * отличное от INT0, Вы можете задать кое-что из этого.
 */
/* #define USB_INTR_CFG            MCUCR */
/* #define USB_INTR_CFG_SET        ((1 << ISC00) | (1 << ISC01)) */
/* #define USB_INTR_CFG_CLR        0 */
/* #define USB_INTR_ENABLE         GIMSK */
/* #define USB_INTR_ENABLE_BIT     INT0 */
/* #define USB_INTR_PENDING        GIFR */
/* #define USB_INTR_PENDING_BIT    INTF0 */
/* #define USB_INTR_VECTOR         INT0_vect */

#endif /* __usbconfig_h_included__ */
