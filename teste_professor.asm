INPUT OLD_DATA
LOAD OLD_DATA
L1:N2: DIV DOIS
L1:N6: COPY DOIS,TMP_DATA
STORE  NEW_DATA
N1:
aDD doiS ;Depois testar com N2:aDD doiS
MULT DOIS
STORE TMP_DATA
LOAD OLD_DATA
SUB TMP_DATA
             
STORE TMP_DATA
OUTPUT TMP_DATA
COPY NEW_DATA, OLD_DATA
LOAD OLD_DATA
JMPP L1
STOP
DOIS: CONST 2   ;33
OLD_DATA: SPACE ;34
NEW_DATA: SPACE ;35
TMP_DATA: SPACE ;36