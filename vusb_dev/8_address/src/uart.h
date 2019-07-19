#ifndef _UART_H_
#define _UART_H_
//настройка UART. принимает скорость, деленную на 100 чтобы не вылезала за int
void uart_init(unsigned int baud){
  unsigned int ubrr;
  DDRD &=~(1<<0); //UART_RX на вход
  DDRD |= (1<<1); //UART_TX на выход
  PORTD |= (1<<1);//UART_TX = 1
  
  ubrr=(F_CPU/800/baud-1);
  UBRRH=0x0F&(char)(ubrr>>8);
  UBRRL=(char)ubrr;
  UCSRA=(1<<U2X);
  UCSRB=(1<<RXEN|1<<TXEN);
}

void uart_putc(char ch){
  while(!(UCSRA & (1<<UDRE))){}
  UDR = ch;
}
void uart_puts(char *str){
  while(str[0] != '\0'){
    uart_putc(str[0]);
    str++;
  }
}

#endif
