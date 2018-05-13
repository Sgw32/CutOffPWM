/*******************************************************************************************
* ������ � ��������� � ������ �������                   *
* ������� ������� ��������� �� ������� 2000000/���������_�������=��������
* ����������� ���������� ������� 30,5 ��, ��� �������������
* 3% ��� �� � 4% ��� ��
* 110% ��� �������� 1144 �� == 1747 �����
                                                        *
                  ������ �� �������� ������ ��
  100% ��. �� = 325.624666 �� ��. ���� ��
  100% ��. �� = 19537.48 ��./���. ���� ��
  100% ��. �� = 83.333 �� ������� ������� �-1�
  100% ��. �� = 1039.501 �� ������� ������� ���-2500
  ����. ������������������ ��� �-1� = 1.20048
  ����. ������������������ ��� ���-2500 = 0.0962
  
                  ������ �� �������� ������ ��
  100% ��. �� = 250 �� ��. ���� C�
  100% ��. �� = 15000 ��./���. ���� C�
  100% ��. �� = 83.333 �� ������� ������� ���-1(2)
  100% ��. �� = 750.18755 �� ������� ������� ���-10
  ����. ������������������ ��� ���-1(2) = 1.20048
  ����. ������������������ ��� ���-10 = 0.1333
*******************************************************************************************/

#include <intrinsics.h>
#include "Defines.h"
#include "Types.h"

#define TIMERS_FILTER_LIMIT    8 
#define OVF_LIMIT              16


extern Sensors_str   Output_Buffer[2];    
extern volatile unsigned char writing_layer;

volatile unsigned char u8_t3_ovf_counter = 0;
volatile unsigned char t3_filter_counter = TIMERS_FILTER_LIMIT; 
unsigned long t3_filter_buffer = 0;

volatile unsigned char u8_t1_ovf_counter = 0;
volatile unsigned char t1_filter_counter = TIMERS_FILTER_LIMIT; 
unsigned long t1_filter_buffer = 0;

volatile unsigned char u8_t5_ovf_counter = 0;
volatile unsigned char t5_filter_counter = TIMERS_FILTER_LIMIT; 
unsigned long t5_filter_buffer = 0;

volatile unsigned char u8_t4_ovf_counter = 0;
volatile unsigned char t4_filter_counter = TIMERS_FILTER_LIMIT; 
unsigned long t4_filter_buffer = 0;


//******************************************************************************
// timer3 ICR interrupt service routine
#pragma vector=(0x3E*0x02)
__interrupt void t3_icr_isr(void) { // 
  static unsigned int last_sample;
  unsigned int sample, temp_result;
  
  sample = ICR3;   
 
  if(t3_filter_counter) {  // ���� ������� ������� �� ��������, ��������.
    t3_filter_counter--;  
    
    if(u8_t3_ovf_counter > 1) {   // ���� ������������ ������ ������...
      t3_filter_buffer = 0;       // ������ ������� ���������� ���� ������ � ������������� ������ ������ 0
      t3_filter_counter = 0; 
    } else {
      
      if(sample >= last_sample) { // ��������� ������ �������
        temp_result = (sample - last_sample);
      } else {
        temp_result = ((0xFFFF - last_sample) + sample);
      }
      
      t3_filter_buffer += temp_result;  // ����������� ��������� � ���������� t3_filter_buffer 
    }
  } 
  last_sample = sample;  
  u8_t3_ovf_counter = 0;                // ���������� ����� ������������
}

// timer3 ovf interrupt service routine
#pragma vector=(0x46*0x02)
__interrupt void t3_ovf_isr(void) { // 
  
    u8_t3_ovf_counter++;               // ������� ����� ������������ 
}

//***************************************************************************
// timer1 ICR interrupt service routine
#pragma vector=(0x20*0x02)
__interrupt void t1_icr_isr(void) { // 
  static unsigned int last_sample;
  unsigned int sample, temp_result;
  
  sample = ICR1;   
 
  if(t1_filter_counter) {  // ���� ������� ������� �� ��������, ��������.
    t1_filter_counter--;  
    
    if(u8_t1_ovf_counter > 1) {   // ���� ������������ ������ ������...
      t1_filter_buffer = 0;       // ������ ������� ���������� ���� ������ � ������������� ������ ������ 0
      t1_filter_counter = 0; 
    } else {
      
      if(sample >= last_sample) { // ��������� ������ �������
        temp_result = (sample - last_sample);
      } else {
        temp_result = ((0xFFFF - last_sample) + sample);
      }
      
      t1_filter_buffer += temp_result;  // ����������� ��������� � ���������� t1_filter_buffer 
    }
  } 
  last_sample = sample;  
  u8_t1_ovf_counter = 0;                // ���������� ����� ������������
}

// timer1 ovf interrupt service routine
#pragma vector=(0x28*0x02)
__interrupt void t1_ovf_isr(void) { // 
  
    u8_t1_ovf_counter++;               // ������� ����� ������������ 
}

//******************************************************************************
// timer5 ICR interrupt service routine
#pragma vector=(0x5C*0x02)
__interrupt void t5_icr_isr(void) { // 
  static unsigned int last_sample;
  unsigned int sample, temp_result;
  
  sample = ICR5;   
 
  if(t5_filter_counter) {  // ���� ������� ������� �� ��������, ��������.
    t5_filter_counter--;  
    
    if(u8_t5_ovf_counter > 1) {   // ���� ������������ ������ ������...
      t5_filter_buffer = 0;       // ������ ������� ���������� ���� ������ � ������������� ������ ������ 0
      t5_filter_counter = 0; 
    } else {
      
      if(sample >= last_sample) { // ��������� ������ �������
        temp_result = (sample - last_sample);
      } else {
        temp_result = ((0xFFFF - last_sample) + sample);
      }
      
      t5_filter_buffer += temp_result;  // ����������� ��������� � ���������� t5_filter_buffer 
    }
  } 
  last_sample = sample;  
  u8_t5_ovf_counter = 0;                // ���������� ����� ������������
}

// timer5 ovf interrupt service routine
#pragma vector=(0x64*0x02)
__interrupt void t5_ovf_isr(void) { // 
  
    u8_t5_ovf_counter++;               // ������� ����� ������������ 
}

//******************************************************************************
// timer4 ICR interrupt service routine
#pragma vector=(0x52*0x02)
__interrupt void t4_icr_isr(void) { // 
  static unsigned int last_sample;
  unsigned int sample, temp_result;
  
  sample = ICR4;   
 
  if(t4_filter_counter) {  // ���� ������� ������� �� ��������, ��������.
    t4_filter_counter--;  
    
    if(u8_t4_ovf_counter > 1) {   // ���� ������������ ������ ������...
      t4_filter_buffer = 0;       // ������ ������� ���������� ���� ������ � ������������� ������ ������ 0
      t4_filter_counter = 0; 
    } else {
      
      if(sample >= last_sample) { // ��������� ������ �������
        temp_result = (sample - last_sample);
      } else {
        temp_result = ((0xFFFF - last_sample) + sample);
      }
      
      t4_filter_buffer += temp_result;  // ����������� ��������� � ���������� t4_filter_buffer 
    }
  } 
  last_sample = sample;  
  u8_t4_ovf_counter = 0;                // ���������� ����� ������������
}

// timer4 ovf interrupt service routine
#pragma vector=(0x5A*0x02)
__interrupt void t4_ovf_isr(void) { // 
  
    u8_t4_ovf_counter++;               // ������� ����� ������������ 
}

//******************************************************************************


void mcu_timers_init(void) {
  
  DDRE  &= ~(1 << 7); // ICR3
  PORTE |=  (1 << 7); // �������� �� ����� ICR3
  
  TCCR3A = 0;
  TCCR3C = 0;
  TCNT3  = 0;
  ICR3   = 0;
  TIMSK3 = ((1 << ICIE3) | (1 << TOIE3));  // �������� ���������� ICR � �� ������������
  TCCR3B = ((1 << ICNC3) | (1 << CS31) );  // �������� ������ � ������������� 8 � ����������� ����. 
  
  DDRD  &= ~(1 << 4); // ICR1
  PORTD |=  (1 << 4); // �������� �� ����� ICR1
  
  TCCR1A = 0;
  TCCR1C = 0;
  TCNT1  = 0;
  ICR1   = 0;
  TIMSK1 = ((1 << ICIE1) | (1 << TOIE1));  // �������� ���������� ICR � �� ������������
  TCCR1B = ((1 << ICNC1) | (1 << CS11) );  // �������� ������ � ������������� 8 � ����������� ����.   
  
  DDRL  &= ~(1 << 1); // ICR5
  PORTL |=  (1 << 1); // �������� �� ����� ICR5
  
  TCCR5A = 0;
  TCCR5C = 0;
  TCNT5  = 0;
  ICR5   = 0;
  TIMSK5 = ((1 << ICIE5) | (1 << TOIE5));  // �������� ���������� ICR � �� ������������
  TCCR5B = ((1 << ICNC5) | (1 << CS51) );  // �������� ������ � ������������� 8 � ����������� ����.     
  
  DDRL  &= ~(1 << 0); // ICR4
  PORTL |=  (1 << 0); // �������� �� ����� ICR4
  
  TCCR4A = 0;
  TCCR4C = 0;
  TCNT4  = 0;
  ICR4   = 0;
  TIMSK4 = ((1 << ICIE4) | (1 << TOIE4));  // �������� ���������� ICR � �� ������������
  TCCR4B = ((1 << ICNC4) | (1 << CS41) );  // �������� ������ � ������������� 8 � ����������� ����.       
}




void mcu_timers_manager(void) {
  
    if(u8_t3_ovf_counter > OVF_LIMIT) { 
      Output_Buffer[writing_layer].timer3_result = 0;  
      t3_filter_buffer = 0;
    } else {
      if(t3_filter_counter == 0) {
        t3_filter_buffer /= TIMERS_FILTER_LIMIT;                                     // ����� ����� �� ����� ��������, ��� ����������� ���� ������� ��� �������
        Output_Buffer[writing_layer].timer3_result = (unsigned int)t3_filter_buffer; // �������� ��������� �� �����
        t3_filter_buffer = 0;
        t3_filter_counter = TIMERS_FILTER_LIMIT; 
      }
    }
    
    if(u8_t1_ovf_counter > OVF_LIMIT) { 
      Output_Buffer[writing_layer].timer1_result = 0;  
      t1_filter_buffer = 0;
    } else {    
      if(t1_filter_counter == 0) {
        t1_filter_buffer /= TIMERS_FILTER_LIMIT;                                     // ����� ����� �� ����� ��������, ��� ����������� ���� ������� ��� �������
        Output_Buffer[writing_layer].timer1_result = (unsigned int)t1_filter_buffer; // �������� ��������� �� �����
        t1_filter_buffer = 0;
        t1_filter_counter = TIMERS_FILTER_LIMIT; 
      }
    }
  
    if(u8_t5_ovf_counter > OVF_LIMIT) { 
      Output_Buffer[writing_layer].timer5_result = 0;  
      t5_filter_buffer = 0;
    } else {    
      if(t5_filter_counter == 0) {
        t5_filter_buffer /= TIMERS_FILTER_LIMIT;                                     // ����� ����� �� ����� ��������, ��� ����������� ���� ������� ��� �������
        Output_Buffer[writing_layer].timer5_result = (unsigned int)t5_filter_buffer; // �������� ��������� �� �����
        t5_filter_buffer = 0;
        t5_filter_counter = TIMERS_FILTER_LIMIT; 
      }  
    }
 
    if(u8_t4_ovf_counter > OVF_LIMIT) { 
      Output_Buffer[writing_layer].timer4_result = 0;  
      t4_filter_buffer = 0;
    } else {    
      if(t4_filter_counter == 0) {
        t4_filter_buffer /= TIMERS_FILTER_LIMIT;                                     // ����� ����� �� ����� ��������, ��� ����������� ���� ������� ��� �������
        Output_Buffer[writing_layer].timer4_result = (unsigned int)t4_filter_buffer; // �������� ��������� �� �����
        t4_filter_buffer = 0;
        t4_filter_counter = TIMERS_FILTER_LIMIT; 
      }
    }  
}




  
  
  
