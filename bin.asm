SECTION TEXT
INPUT OLD_DATA; 0
LOAD OLD_DATA; 2
L1: DIV DOIS; 4
STORE NEW_DATA; 6
MUL DOIS; 8
STORE TMP_DATA; 10
LOAD OLD_DATA; 12
SUB TMP_DATA; 14  
STORE TMP_DATA; 16
OUTPUT TMP_DATA; 18 
L2: COPY NEW_DATA,OLD_DATA; 20
LOAD OLD_DATA; 23
JMPP L1; 25
STOP; 27
SECTION DATA
DOIS: CONST 2 
OLD_DATA: SPACE  
NEW_DATA: SPACE  
TMP_DATA: SPACE 








