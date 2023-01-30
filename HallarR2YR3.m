%Programa para hallar R2 y R3 para el control de buck converter

VRef=1.235;    %Voltaje de referencia para comparacion.
PWMMin=0;      %Voltaje minimo de PWM
PWMMax=3.3;    %Voltaje maximo de PWM
VoutMax=12.96;    %Voltaje maximo del buck para cada arreglo de LEDs
VoutMin=6;     %Voltaje minimo del buck para cada arreglo de LEDs
R1=330;        %Resistencia entre tierra y nodo comun

R2= sym('R2'); %Resistencia entre feedback(vout) y nodo comun (Se quiere hallar)
R3= sym('R3'); %Resistencia entre pwm y nodo comun (Se quiere hallar)

f1 = VRef*(1+(R2/R1)+(R2/R3))-PWMMin*(R2)/(R3)==VoutMax;
f2 = VRef*(1+(R2/R1)+(R2/R3))-PWMMax*(R2)/(R3)==VoutMin;

S1= solve(f1,f2);
S1t = struct2table(S1);
S1f = table2array(S1t);
Rfinal=double(S1f);
disp(Rfinal);