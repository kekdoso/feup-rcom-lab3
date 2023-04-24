switch(currentState){

    case Start:
        if buf[0] == FLAG_RCV
            currentState = stateFlagRCV;
        break;

    case stateFlagRCV:
        if buf[0] == FLAG_RCV
            currentState = stateFlagRCV;
        if buf[0] == A_RCV
            currentState = stateARCV;
        if buf[0] == Other_RCV
            currentState = stateOther;
        break;
    case stateARCV:
        if buf[0] == FLAG_RCV
            currentState = stateFlagRCV;
        if buf[0] == C_RCV
            if (C_RCV == 1) 
                currentState = stateFlagRCV;
        if buf[0] == Other_RCV
            currentState = stateOther;             
        break;  

    case stateCRCV:
        if buf[0] == Other_RCV
            currentState = stateOther;
        if buf[0] == FLAG_RCV
            currentState = stateFlagRCV;
        if (A^C == buf[0])
            currentstate = BCCOK;
        break;
    
    case BCCOK:
        if buf[0] == FLAG_RCV
            currentState = Stop;
            
} 