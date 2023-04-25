typedef enum{

    stateStart,
    stateFlagRCV,
    stateARCV,
    stateCRCV,
    stateBCCOK,
    stateStop,
}

switch(frameState){

    case stateStart:
        if buf[0] == FLAG_RCV
            frameState = stateFlagRCV;
        break;

    case stateFlagRCV:
        if buf[0] == FLAG_RCV
            frameState = stateFlagRCV;
        if buf[0] == A_RCV
            frameState = stateARCV;
        if buf[0] == Other_RCV
            frameState = stateOther;
        break;
    case stateARCV:
        if buf[0] == FLAG_RCV
            frameState = stateFlagRCV;
        if buf[0] == C_RCV
        {
            if (C_RCV == 1) 
                frameState = stateFlagRCV;
        }
        if buf[0] == Other_RCV
            frameState = stateOther;             
        break;  

    case stateCRCV:
        if buf[0] == Other_RCV
            frameState = stateOther;
        if buf[0] == FLAG_RCV
            frameState = stateFlagRCV;
        if (A^C == buf[0])
            frameState = stateBCCOK;
        break;
    
    case stateBCCOK:
        if buf[0] == FLAG_RCV
            frameState = stateStop;
        if buf[0] == Other_RCV
            frameState = stateStart;
        break;
    
    case stateStop: 
        break;
            
} 