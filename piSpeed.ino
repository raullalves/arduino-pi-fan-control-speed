#include<PWM.h>
#include <math.h>

int sensor_pin = 2;
int pwm_pin = 3;
int potenciometro_pin = 2;
int error = 0;
double kp,ki;
int32_t frequencia = 25000; 
double rpm_anterior = 1100;
const long TEMPO_AMOSTRAGEM = 50; 
const int TAMANHO_VETOR_AMOSTRAGEM = 100;
double vetor_amostragem[TAMANHO_VETOR_AMOSTRAGEM];
double rpm_infinito = 2500;
double ts = 2, mp = 0.1;
long somatorio_erros = 0;
long ultimo_tempo,tempo_inicial,lastTime;
int SET_POINT = 2200;
const int Y_INFINITO = 3500;

//Função que determina a função de transferência após aplicar um degrau
double determinarFT(){
  
  int contador = 0;
  
  //aplicando pwm máximo, equivalente a 100% do ciclo 
  analogWrite(pwm_pin,255);

  //contador de T
  while(contador < TAMANHO_VETOR_AMOSTRAGEM){

    //Inicialização dos contadores
    unsigned long tempo_atual = millis();
    
    int variacao_tempo = (tempo_atual - ultimo_tempo);

    //assegurando a leitura no intervalo de amostragem
    if(variacao_tempo>=TEMPO_AMOSTRAGEM)
    {

      //atribui o valor do sensor ao vetor
      vetor_amostragem[contador] = lerPulso();
      
      //correção em caso de erro no sensor
      if(vetor_amostragem[contador] == -1) vetor_amostragem[contador] = vetor_amostragem[contador-1];

      //incrementa o contador
      contador++;

      //atualiza o timer
      ultimo_tempo = tempo_atual;  
      
    }
    
  }

  //Cálculo da rpm estabilizada
  rpm_infinito = calcularY_infinito();

  //retorna a constante de tempo em 63%
  return calcular_constante_tempo(rpm_infinito);
  
}

double calcularY_infinito(){

  //inicializando variável para a média do valor de rpm em ts
  double media_velocidade = 0;

  //loop com 5 valores, para ter uma medida mais precisa
  for (int i = TAMANHO_VETOR_AMOSTRAGEM-1; i > TAMANHO_VETOR_AMOSTRAGEM - 6; i--){
    media_velocidade+=vetor_amostragem[i];
  }

  //retorna a média
  return media_velocidade/5;
  
}

//calcula constante de tempo
double calcular_constante_tempo(double rpm_infinito){

  //calcula 63% do rpm no estacionário
  double rpm_63 = rpm_infinito*0.63;

  for (int i = 0; i < TAMANHO_VETOR_AMOSTRAGEM; i++)
    if (vetor_amostragem[i] >= rpm_63) return TEMPO_AMOSTRAGEM*(2*i+1)/2;
    
  return -1;
  
}

double lerPulso() {
  
  unsigned long duracao_pulso = pulseIn(sensor_pin, LOW);
  double frequencia = 1000000/duracao_pulso;
  if (((double)frequencia/4)*60 > Y_INFINITO*2)
    return -1;
  return ((double)frequencia/4)*60;
  
}

void calcularGanhosControlador(double T){
  
  double zeta = sqrt((log(mp)*log(mp))/(PI*PI+log(mp)*log(mp)));
  double wn = double(4)/(zeta*ts);
  ki = T*wn*wn;
  kp = 2*zeta*wn*T-1;
  
}

void setup() {

  //Inicialização do sensor, com delay de estabilização
  Serial.begin(9600);
  pinMode(sensor_pin, INPUT);
  digitalWrite(sensor_pin,HIGH);
  SetPinFrequencySafe(pwm_pin, frequencia);
  delay(1000);
  analogWrite(pwm_pin,80);
  delay(6000);
  tempo_inicial = millis();
  ultimo_tempo = millis();
  double T = determinarFT() - 50;
  if (T == -1)
    return;

  calcularGanhosControlador(T/1000);

  ki = (double)ki/1000;
  ki*=TEMPO_AMOSTRAGEM;

  analogWrite(pwm_pin,80);
  delay(3000);
  lastTime = millis();
  tempo_inicial = millis();
  rpm_anterior = lerPulso();
}

int atualizarPID(int rpm_target, int rpm_atual)   {      
       
    float pid = 0;                                                            
    error=0;                                  

   unsigned long now = millis();
   int timeChange = (now - lastTime);
   
   if(timeChange>=TEMPO_AMOSTRAGEM)
   {
	   
    error = rpm_target - rpm_atual;
    somatorio_erros+=error;
    pid = (kp * error) + (ki * somatorio_erros);          
    double pid_novo = (double)(1150+pid)/(calcularY_infinito());
    pid_novo*=255;
    pid_novo=constrain(int(pid_novo), 130, 255);
    lastTime = now;
    analogWrite(pwm_pin, pid_novo);
	
    return pid_novo;
      
   }
   
   return 0;
}


void loop() {
	
 double rpm_atual = lerPulso();
 
 if (rpm_atual == -1)
    rpm_atual = rpm_anterior;
 
 rpm_anterior = rpm_atual;
 
 double potenciometro = double(analogRead(potenciometro_pin))/1023;
 int set_potenciometro = rpm_infinito*potenciometro;
 set_potenciometro = constrain(set_potenciometro, 1500, rpm_infinito);
 SET_POINT = int(set_potenciometro);
 int pid = atualizarPID(SET_POINT, rpm_atual);
 
 //show values
 if (pid > 129){
  Serial.println(rpm_atual);
  Serial.println(millis()-tempo_inicial-50);
  Serial.println(set_potenciometro);
  Serial.println(pid);
  Serial.println(error);
 }

}