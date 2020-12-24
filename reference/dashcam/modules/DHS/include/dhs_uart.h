#ifndef BASE_UART_H_
#define BASE_UART_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int openPort( char *device);
int setPara(int serialfd,int speed,int databits,int stopbits,int parity);
int writeData(int fd,const char *data,int datalength);
int readData(int fd,char *data,int datalength);
void closePort(int fd);
int baudRate( int baudrate);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* BASE_UART_H_ */