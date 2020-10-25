#include "RTClib.h"                                                                             // Frequências de relógio disponíveis para os temporizadores / 2, / 8, / 32, / 128
#include <SdFat.h>                                                                              // Freq. Int. = MCK / Freq. temporizador = ... MHz
#define Tw  100 /* Tw = 100ms*/                                                                 // Freq. Int. / 44.1Khz (freq. maxima da interrupçao) = 952.38 instruções por clock
#define TwSD 10 /* TwSD = 10ms */ 
#define DeltaTw  (1 * 1000) /* DeltaTw minimo = 1 s*/

/** 
 * DEFINIÇÕES DO REGISTRADOR 
 */
#define ADC_CDR_START (0x1u << 1)                                                               // Modo de leitura do registrador ativado
#define ADC_CHER_CH7 (0x1u << 7)                                                                // Definindo pino A0 para leitura
#define ADC_CHER_CH6 (0x1u << 6)                                                                // Definindo pino A1 para leitura
#define ADC_MR_LOWRES_BITS_12 (0x0u << 4)                                                       // Definindo conversão de 12 bits
#define ADC_MR_SLEEP_NORMAL (0x0u << 5)                                                         // Definindo Modo Normal: Tensão de referência são mantidos ON entre as conversões
#define ADC_MR_STARTUP_SUT8 (0x1u << 16)                                                        // Definindo periodos de clock = 8 periodos de ADCClock
#define ADC_ADVREF 3.3                                                                          // Defininição de tensoa de refecenria para conversor, vai de 0V a 3.3V
#define VALUE 64                                                                                // ADCClock = MCK / ( (PRESCAL+1) * 2 )
#define ADC_MR_PRESCAL(VALUE)                                                                   // Frequencia de amostragem amostragem de 646Khz
#define PMC_MCKR_PRESCAL(VALUE)                                                                 // Frequencia de amostragem amostragem de 646Khz - clock master

/** 
 * DEFINIÇÕES GERAIS 
 */
RTC_Millis rtc;
SdFat sd;
SdFile arquivo;                                                                                 // Arquivo de dados
int porta[4] = {8, 7, 6, 5}, estado[2] = {0, 1};                                                // Definição das portas utilizadas para ativar relés
int configuracaoAtual[4] = {0, 0, 0, 0};                                                        // Definição da configuração atual
const uint8_t SD_CHIP_SELECT_PIN = 9;                                                           // Pino de seleção para comunicação SD
boolean fimAquisicao;                                                                           // Variaveis de controle

/** 
 * SETUP 
 */
int Nw = 1;                                                                                     // Definição da quantidade de amostras
boolean transiente = true;                                                                     // Definição para obter o regime transiente

/**
 * INICIALIZAÇÃO
 */
int configuracaoInicial[4] = {0, 0, 0, 0}, configuracaoFinal[4] = {1, 1, 1, 1};                 // Definição das configurações inicias e finais
int nw = 0;                                                                                     // Definição da auxiliar para a quantidade de janelas
int tconf;                                                                                      // Definição do tempo de configuração

/**
 * Interrupção para adquirir dados
 * Registrador TC1 
 * Canal 0
 */
void TC3_Handler() {                                                                            
  TC_GetStatus(/* registrador */ TC1,/* canal */ 0);
  arquivo.print(/* corrente */ ADC->ADC_CDR[7]);
  arquivo.print(" ");
  arquivo.println(/* tensão */ ADC->ADC_CDR[6]);
}

/**
 * Interrupção para parar a aquisição de dados
 * Registrador TC2
 * Canal 1
 */
void TC7_Handler() {                                                                            
  TC_GetStatus(/* registrador */ TC2,/* canal */ 1);

  if (!fimAquisicao) {
    fimAquisicao = true;
    TC_Stop(/* registrador */ TC1,/* canal */ 0);
    NVIC_DisableIRQ(/* interrupção */ TC3_IRQn);
  }
}

/**
 * Função para iniciar as interrupções 
 * TIMER_CLOCK1: 84Mhz/2 = 42 MHz
 * TIMER_CLOCK2: 84Mhz/8 = 10.5 MHz
 * TIMER_CLOCK3: 84Mhz/32 = 2.625 MHz
 * TIMER_CLOCK4: 84Mhz/128 = 656.250 KHz
 * TIMER_CLOCK5: SLCK ( slow clock )
 */
void startTimer(/* registrador */ Tc *tc,/* canal */ uint32_t channel,/* interrupção */ IRQn_Type irq,/* frequancia */ uint32_t frequency) {                                                                                                              // TIMER_CLOCK1: 84Mhz/2 = 42.000 MHz
  pmc_set_writeprotect(false);                                                                  
  pmc_enable_periph_clk((uint32_t)irq);                                                         
  TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK2);              
  uint32_t rc =  frequency;                                                                             
  TC_SetRC(tc, channel, 400);                                                                            
  TC_Start(tc, channel);                                                                                
  tc->TC_CHANNEL[channel].TC_IER = TC_IER_CPCS;
  tc->TC_CHANNEL[channel].TC_IDR = ~TC_IER_CPCS;
  NVIC_EnableIRQ(irq);
}

/**
 * Definição de parametros
 */
void setup() {
  Serial.begin(115200);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  
  if (!sd.begin(SD_CHIP_SELECT_PIN)) {                                                        // Habilitação do cartao SD
    sd.initErrorHalt();
  }

  for (int i = 0; i < 4; i++) {                                                               // Habilitação de todas as portas como saidas
    pinMode(porta[i], OUTPUT);
  }

  /* REGISTRADORES */                                                                         // Taxa de conversão ADC de 1 MHz
  ADC->ADC_CHER = ADC_CHER_CH7;                                                               // Habilita pino A0 para leitura
  ADC->ADC_CHER = ADC_CHER_CH6;                                                               // Habilita pino A1 para leitura
  ADC->ADC_MR |= 0xFF;                                                                        // Definição do intervalo entre clocks MKC/ADC_MR
  
  DateTime now = rtc.now();
  tconf = now.second();
}

/**
 * Função loop
 */
void loop() {
  ativarPortas();
  if (!transiente) {
    delay(Tw);
    inicioAquisicao();
  }
  else {
    inicioAquisicao();
  }
}

/**
 * Função aquisição de dados
 * Interrupção para aquisição de dados
 * Interrupçao para parar aquisição de dados
 */
void inicioAquisicao() {                                                                      
  String arqStr = arquivoTxt();
  char arqChar[12];
  arqStr.toCharArray(arqChar, 12);

  arquivo.open(/* arquivo */ arqChar,/* função */ O_WRITE | O_CREAT);

  fimAquisicao = false;
  startTimer(/* registrador */ TC1,/* canal */ 0,/* irq */ TC3_IRQn,/* frequencia */ 400);    
  delay(Tw);
  startTimer(/* registrador */ TC2,/* canal */ 1,/* irq */ TC7_IRQn,/* frequencia */ 400);     
  delay(TwSD);
  arquivo.close();

  TC_Stop(/* registrador */ TC2,/* canal */ 1);                                     
  NVIC_DisableIRQ(/* interrupção */ TC7_IRQn);

  nw = nw + 1;
  if (nw >= Nw) {
    desativarPortas();
    delay(DeltaTw);
    verificaAquisicao();
    atualizaConfiguracao();
    return;
  }
  else {
    delay(DeltaTw);
    inicioAquisicao();
  }
}

/**
 * Função retorna o nome do arquivo
 */
String arquivoTxt() {                                                                        
  char aux[1], arquivo[] = {'1', '0', '0', '0', '0', '.', 't', 'x', 't', '\0'};
  String string;

  itoa(nw + 1, aux, 10);
  arquivo[0] = aux[0];
  itoa(configuracaoAtual[0], aux, 10);
  arquivo[1] = aux[0];
  itoa(configuracaoAtual[1], aux, 10);
  arquivo[2] = aux[0];
  itoa(configuracaoAtual[2], aux, 10);
  arquivo[3] = aux[0];
  itoa(configuracaoAtual[3], aux, 10);
  arquivo[4] = aux[0];

  string = arquivo;
  return string;
}

/**
 * Função para alterar a configuração das cargas
 * Implementação de um somador de 4 bits, sem o uso do carry
 */
void atualizaConfiguracao() {                                                                 
  int S0 = 0, S1 = 0, S2 = 0, S3 = 0;                                                         

  S0 = configuracaoAtual[3] ^ 1;

  int aux1 = 0, aux2 = 0;
  aux1 = configuracaoAtual[3] && 1;
  aux2 = configuracaoAtual[2] ^ 0;
  S1 = aux1 ^ aux2;

  int aux3 = 0, aux4 = 0, aux5 = 0, aux6 = 0;
  aux3 = aux1 && aux2;
  aux4 = configuracaoAtual[2] && 0;
  aux5 = aux3 || aux4;
  aux6 = configuracaoAtual[1] ^ 0;
  S2 = aux6 ^ aux5;

  int aux7 = 0, aux8 = 0, aux9 = 0, aux10 = 0;
  aux7 = aux5 && aux6;
  aux8 = configuracaoAtual[1] && 0;
  aux9 = aux7 || aux8;
  aux10 = configuracaoAtual[0] ^ 0;
  S3 = aux9 ^ aux10;

  configuracaoAtual[0] = S3;
  configuracaoAtual[1] = S2;
  configuracaoAtual[2] = S1;
  configuracaoAtual[3] = S0;
}

/**
 * Função para verificar se a aquisição de dados completou
 */
void verificaAquisicao() {                                                                   
  if ((configuracaoAtual[0] == 1) && (configuracaoAtual[1] == 1) &&
      (configuracaoAtual[2] == 1) && (configuracaoAtual[3] == 1) && (nw >= Nw)) {
    Serial.println("Finalização de aquisição de dados");
    desativarPortas();
    DateTime now = rtc.now();
    int tem = now.second();
    tconf = tem - tconf;
    Serial.print(tconf);
    while (1);
  }
  nw = 0;
  return;
}

/*
 * Função para ativar portas, habilitando os relés
 */
void ativarPortas() {                                                             
  for (int i = 0; i < 4; i++) {
    digitalWrite(porta[i], configuracaoAtual[i]);
  }
}

/**
 * Função para desabilitar porta, desligando os relés
 */
void desativarPortas() {                                                          
  for (int i = 0; i < 4; i++) {
    digitalWrite(porta[i], configuracaoInicial[i]);
  }
}
