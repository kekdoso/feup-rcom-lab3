/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG_RCV 0x5c
#define A_RCV 0x03
#define ALT_A_RCV 0x01
#define C_SET 0x03
#define C_UA 0x07

volatile int STOP=FALSE;

typedef enum{

    stateStart,
    stateFlagRCV,
    stateARCV,
    stateCRCV,
    stateBCCOK,
    stateOther,
    destuff,
    stateStop
} frameStates;

frameStates frameState = stateStart;

int C_RCV = C_SET;

int main(int argc, char** argv)
{
    // (void) signal(SIGALRM, alarmPickup);  // instala rotina que atende interrupcao


    int fd,c, res, res2;
    int SMFlag = 0, i = 0, j = 0, skip = 0, destuffFlag = 0;
    struct termios oldtio,newtio;
    char buf[255], str[255], str2[255];

    if ( (argc < 2) ||
         ((strcmp("/dev/ttyS0", argv[1])!=0) &&
          (strcmp("/dev/ttyS1", argv[1])!=0) )) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS0\n");
        exit(1);
    }


    /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
    */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd < 0) { perror(argv[1]); exit(-1); }

    if (tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
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
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

    /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) proximo(s) caracter(es)
    */


    tcflush(fd, TCIOFLUSH);

    sleep(1);
    if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");
    printf("State Machine SET started\n"); 
    i = 0;
    while (STOP == FALSE) {       /* loop for input */
        res = read(fd,buf,1);   /* returns after 5 (1) chars have been input */        
        //buf[res]=0;               /* so we can printf... */
        //printf(":%s:%d", buf, res);
        printf("Var=0x%02x\n",(unsigned int)(buf[0] & 0xff));
        switch(frameState){

            case stateStart:
                if (buf[0] == FLAG_RCV)
                {
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
                    SMFlag = 0;
                }
                break;
            case stateARCV:
                if (buf[0] == FLAG_RCV){
                    frameState = stateFlagRCV;
                    printf("stateFlagRCV\n");
                }
                else if (buf[0] == C_RCV){
                    frameState = stateCRCV;
                    buf[0] = C_UA; //UA CHANGE
                    printf("stateCRCV\n");
                }
                else{
                    frameState = stateStart; 
                    printf("stateStart\n");            
                    SMFlag = 0;
                }
                break;  

            case stateCRCV:

                if (buf[0] == FLAG_RCV){
                    frameState = stateFlagRCV;
                    printf("stateFLAGRCV\n");
                }
                else if (buf[0] == A_RCV^C_RCV || buf[0] == ALT_A_RCV^C_RCV)
                {
                    frameState = stateBCCOK;
                    printf("stateBCCOK\n");
                }
                else{
                    frameState = stateStart;
                    printf("stateStart\n");
                    SMFlag = 0;
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
                }
                break;
                  
        } 
        if (buf[0] == 0x5d)
            destuffFlag = 1;
        if (destuffFlag && buf[0] == 0x7c){
            str2[i-1] = 0x5c;
            skip = 1;
        } 
        if (destuffFlag && buf[0] == 0x7d){
            skip = 1;
        } 
        if (!skip){
            str2[i] = buf[0];
            i++;
        }

        else{
            skip = 0;
            destuffFlag = 0;
        }
        if (buf[0]==FLAG_RCV && SMFlag==1) 
            STOP=TRUE;
        if(buf[0]==FLAG_RCV)
            SMFlag = 1;
    }
    for(j=0; j<i; j++)
        printf("0x%02x:%d\n", (unsigned int)(str2[j] & 0xff), 1);  

    res2 = write(fd,str2,i);
    //printf(":%s:%d\n", str, res2);  
    printf("%d bytes written\n", res2);


    /*
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no guiao
    */
    
    sleep(1);
    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}

