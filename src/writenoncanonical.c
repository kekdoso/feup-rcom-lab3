/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG_RCV 0x5c
#define A_RCV 0x03
#define ALT_A_RCV 0x01
#define C_SET 0x03
#define C_UA 0x07

volatile int STOP = FALSE;

typedef enum{

    stateStart,
    stateFlagRCV,
    stateARCV,
    stateCRCV,
    stateBCCOK,
    stateOther,
    stateStop,
} frameStates;

frameStates frameState = stateStart;

int C_RCV = C_UA; /* in here, we receive the UA message */

int main(int argc, char** argv)
{
    int fd, c, res, res2, flagged = 0;
    struct termios oldtio, newtio;
    char buf[255], str[255], str2[255];
    int i, sum = 0, speed = 0;

    if ( (argc < 2) ||
         ((strcmp("/dev/ttyS0", argv[1])!=0) &&
          (strcmp("/dev/ttyS1", argv[1])!=0) )) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }


    /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
    */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd < 0) { perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
        perror("tcgetattr");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 char received */


    /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) próximo(s) caracter(es)
    */


    tcflush(fd, TCIOFLUSH);

    sleep(1);
    if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");
    
    //------------------------------------
    /*printf("Enter a string : ");
    gets(str);*/

    str[0] = 0x5c;      /*   F   */
    str[1] = 0x03;      /*   A   */
    str[2] = 0x03;      /*   C   */
    str[3] = 0x03^0x03; /* BCCOK */
    str[4] = 0x5c;      /*   F   */
    
    strcpy(buf,str);    
    
    /*testing*/
    //buf[strlen(str)] = '\0';

    res = write(fd,str,5);
    printf("%d bytes written\n", res);

    /*while (STOP == FALSE) {           // loop for input 
        res = read(fd,buf,255);       // returns after 5 chars have been input
        buf[res] = 0;                 // so we can printf...
        printf(":%s:%d\n", buf, res);
        //printf("%d\n", strlen(buf));
        if (buf[0] == '\0') STOP = TRUE;
    }*/

    printf("UA State Machine has started\n"); 

    while (STOP == FALSE) {       /* loop for input */
        res = read(fd,buf,1);   /* returns after 5 (1) chars have been input */        
        //buf[res]=0;               /* so we can printf... */
        //printf(":%s:%d", buf, res);
        printf("var=0x%02x\n",(unsigned int)(buf[0] & 0xff));
        switch(frameState){

            case stateStart:
                if (buf[0] == FLAG_RCV){
                    frameState = stateFlagRCV;
                    printf("stateFlagRCV\n");
                }
                break;

            case stateFlagRCV:
                if (buf[0] == FLAG_RCV){
                    frameState = stateFlagRCV;
                    printf("stateFlagRCV\n");
                }
                else if (buf[0] == A_RCV || buf[0] == ALT_A_RCV){
                    frameState = stateARCV;
                    printf("stateARCV\n");
                }
                else{
                    frameState = stateStart; 
                    printf("stateStart\n");            
                    flagged = 0;
                }
                break;
            case stateARCV:
                if (buf[0] == FLAG_RCV){
                    frameState = stateFlagRCV;
                    printf("stateFlagRCV\n");
                }
                else if (buf[0] == C_RCV){
                    frameState = stateCRCV;
                    printf("stateCRCV\n");
                }
                else{
                    frameState = stateStart; 
                    printf("stateStart\n");            
                    flagged = 0;
                }
                break;  

            case stateCRCV:
                if (buf[0] == FLAG_RCV){
                    frameState = stateFlagRCV;
                    printf("stateFLAGRCV\n");
                }
                else if (buf[0] == A_RCV^C_RCV || buf[0] == ALT_A_RCV^C_RCV){
                    frameState = stateBCCOK;
                    printf("stateBCCOK\n");
                }
                else{
                    frameState = stateStart;
                    printf("stateStart\n");
                    flagged = 0;
                }
                break;
            
            case stateBCCOK:
                if (buf[0] == FLAG_RCV){
                    //frameState = stateStop;
                    printf("stateStop\n"); 
                }
                else{
                    frameState = stateStart;
                    printf("stateStart\n");
                    flagged = 0;
                }
                break;
                              
        } 
        
        if (buf[0]==FLAG_RCV && flagged==1)
            STOP=TRUE;
        if(buf[0] == FLAG_RCV)
            flagged = 1;
        printf("%s:%d\n", buf, res);  
    }
   

    sleep(1);
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    close(fd);
    return 0;
}

