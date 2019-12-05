#include <stdlib.h>
#include <math.h>
#include "dac.h"
#include "main.h"


float tri_values[128]={0, 0.079365f,0.158730f,0.238095f,0.317460f,0.396825f,0.476190f,0.555555f,0.634920f,0.714285f,0.793650f,
0.873015f,0.952380f,1.031746f,1.111111f,1.190476f,1.269841f,1.349206f,1.428571f,1.507936f,1.587301f,
1.666666f,1.746031f,1.825396f,1.904761f,1.984126f,2.063492f,2.142857f,2.222222f,2.301587f,2.380952f,
2.460317f,2.539682f,2.619047f,2.698412f,2.777777f,2.857142f,2.936507f,3.015873f,3.095238f,3.174603f,
3.253968f,3.333333f,3.412698f,3.492063f,3.571428f,3.650793f,3.730158f,3.809523f,3.888888f,3.968253f,
4.047619f,4.126984f,4.206349f,4.285714f,4.365079f,4.444444f,4.523809f,4.603174f,4.682539f,4.761904f,
4.841269f,4.920634f,5, 5.0f,4.920634f,4.841269f,4.761904f,4.682539f,4.603174f,4.523809f,4.444444f,4.365079f,4.285714f,4.206349f,
4.126984f,4.047619f,3.968253f,3.888888f,3.809523f,3.730158f,3.650793f,3.571428f,3.492063f,3.412698f,
3.333333f,3.253968f,3.174603f,3.095238f,3.015873f,2.936507f,2.857142f,2.777777f,2.698412f,2.619047f,
2.539682f,2.460317f,2.380952f,2.301587f,2.222222f,2.142857f,2.063492f,1.984126f,1.904761f,1.825396f,
1.746031f,1.666666f,1.587301f,1.507936f,1.428571f,1.349206f,1.269841f,1.190476f,1.111111f,1.031746f,
0.952380f,0.873015f,0.793650f,0.714285f,0.634920f,0.555555f,0.476190f,0.396825f,0.317460f,0.238095f,
0.158730f,0.079365f,0.0f};

float ramp_values[128]={5.0f,4.960629f,4.921259f,4.881889f,4.842519f,4.803149f,4.763779f,4.724409f,4.685039f,4.645669f,4.606299f, 
4.566929f,4.527559f,4.488188f,4.448818f,4.409448f,4.370078f,4.330708f,4.291338f,4.251968f,4.212598, 
4.173228f,4.133858f,4.094488f,4.055118f,4.015748f,3.976377f,3.937007f,3.897637f,3.858267f,3.818897, 
3.779527f,3.740157f,3.700787f,3.661417f,3.622047f,3.582677f,3.543307f,3.503937f,3.464566f,3.425196, 
3.385826f,3.346456f,3.307086f,3.267716f,3.228346f,3.188976f,3.149606f,3.110236f,3.070866f,3.031496, 
2.992125f,2.952755f,2.913385f,2.874015f,2.834645f,2.795275f,2.755905f,2.716535f,2.677165f,2.637795, 
2.598425f,2.559055f,2.519685f,2.480314f,2.440944f,2.401574f,2.362204f,2.322834f,2.283464f,2.244094, 
2.204724f,2.165354f,2.125984f,2.086614f,2.047244f,2.007874f,1.968503f,1.929133f,1.889763f,1.850393, 
1.811023f,1.771653f,1.732283f,1.692913f,1.653543f,1.614173f,1.574803f,1.535433f,1.496062f,1.456692, 
1.417322f,1.377952f,1.338582f,1.299212f,1.259842f,1.220472f,1.181102f,1.141732f,1.102362f,1.062992, 
1.023622f,0.984251f,0.944881f,0.905511f,0.866141f,0.826771f,0.787401f,0.748031f,0.708661f,0.669291, 
0.629921f,0.590551f,0.551181f,0.511811f,0.472440f,0.433070f,0.393700f,0.354330f,0.314960f,0.275590, 
0.236220f,0.196850f,0.157480f,0.118110f,0.078740f,0.039370f,0.0f};

float cus_values[128]={3.105517f,3.663939f,4.134686f,4.489156f,4.714245f,4.813394f,4.805059f,4.718919f,4.590555f,4.455519f,
4.343842f,4.275874f,4.260088f,4.293125f,4.361912f,4.447349f,4.528771f,4.588320f,4.614399f,4.603599f,
4.560834f,4.497746f,4.429808f,4.372778f,4.339258f,4.336076f,4.362995f,4.412990f,4.473978f,4.531617f,
4.572532f,4.587301f,4.572532f,4.531617f,4.473978f,4.412990f,4.362995f,4.336076f,4.339258f,4.372778f,
4.429808f,4.497746f,4.560834f,4.603599f,4.614399f,4.588320f,4.528771f,4.447349f,4.361912f,4.293125f,
4.260088f,4.275874f,4.343842f,4.455519f,4.590555f,4.718919f,4.805059f,4.813394f,4.714245f,4.489156f,
4.134686f,3.663939f,3.105517f,2.500000f,1.894482f,1.336060f,0.865313f,0.510843f,0.285754f,0.186605f,
0.194940f,0.281080f,0.409444f,0.544480f,0.656157f,0.724125f,0.739911f,0.706874f,0.638087f,0.552650f,
0.471228f,0.411679f,0.385600f,0.396400f,0.439165f,0.502253f,0.570191f,0.627221f,0.660741f,0.663923f,
0.637004f,0.587009f,0.526021f,0.468382f,0.427467f,0.412698f,0.427467f,0.468382f,0.526021f,0.587009f,
0.637004f,0.663923f,0.660741f,0.627221f,0.570191f,0.502253f,0.439165f,0.396400f,0.385600f,0.411679f,
0.471228f,0.552650f,0.638087f,0.706874f,0.739911f,0.724125f,0.656157f,0.544480f,0.409444f,0.281080f,
0.194940f,0.186605f,0.285754f,0.510843f,0.865313f,1.336060f,1.894482f,2.500000f};

float sine_values[128]={2.622669f,2.745042f,2.866826f,2.987725f,3.107450f,3.225711f,3.342224f,3.456708f,3.568887f,3.678491f,
3.785256f,3.888925f,3.989248f,4.085983f,4.178897f,4.267766f,4.352377f,4.432526f,4.508018f,4.578674f,
4.644321f,4.704803f,4.759973f,4.809698f,4.853860f,4.892350f,4.925078f,4.951963f,4.972941f,4.987961f,
4.996988f,5,4.996988f,4.987961f,4.972941f,4.951963f,4.925078f,4.892350f,4.853860f,4.809698f,4.759973f,4.704803f,
4.644321f,4.578674f,4.508018f,4.432526f,4.352377f,4.267766f,4.178897f,4.085983f,3.989248f,3.888925f,
3.785256f,3.678491f,3.568887f,3.456708f,3.342224f,3.225711f,3.107450f,2.987725f,2.866826f,2.745042f,
2.622669f,2.500000f,2.377330f,2.254957f,2.133173f,2.012274f,1.892549f,1.774288f,1.657775f,1.543291f,
1.431112f,1.321508f,1.214743f,1.111074f,1.010751f,0.914016f,0.821102f,0.732233f,0.647622f,0.567473f,
0.491981f,0.421325f,0.355678f,0.295196f,0.240026f,0.190301f,0.146139f,0.107649f,0.074921f,0.048036f,
0.027058f,0.012038f,0.003011f,0,0.003011f,0.012038f,0.027058f,0.048036f,0.074921f,0.107649f,0.146139f,
0.190301f,0.240026f,0.295196f,0.355678f,0.421325f,0.491981f,0.567473f,0.647622f,0.732233f,0.821102f,
0.914016f,1.010751f,1.111074f,1.214743f,1.321508f,1.431112f,1.543291f,1.657775f,1.774288f,1.892549f,
2.012274f,2.133173f,2.254957f,2.377330f,2.500000f};


extern DAC_state dac_state;

void init_dac(){
	RCC->APB1ENR1	|=RCC_APB1ENR1_DAC1EN; //enable DAC clock
	DAC1->CR|=DAC_CR_EN2;//enable DAC channel 1
}

void init_dac_GPIO(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN ; //enable clock for GPIO port A
	GPIOA->MODER &=~0x00000C00; // clear mode register for PA5
	GPIOA->MODER|=0x00000C00; //configure PA5  as analog mode
	GPIOA->PUPDR&=~0x00000C00; //set PA5 as no pullup or pulldown
}
	
void sine(void){
	DAC1->DHR12R2=(uint32_t) ((sine_values[dac_state.sample]*DIVS_PER_VOLT)*(dac_state.vref/5.0f));
}

void cus(void){
	DAC1->DHR12R2=(uint32_t)  ((cus_values[dac_state.sample]*DIVS_PER_VOLT)*(dac_state.vref/5.0f));
	volatile uint32_t x=DAC1->DHR12R2;
}

void triangle(void){
	DAC1->DHR12R2=(uint32_t)  ((tri_values[dac_state.sample]*DIVS_PER_VOLT)*(dac_state.vref/5.0f));
	
}

void ramp(void){
	DAC1->DHR12R2=(uint32_t)  ((ramp_values[dac_state.sample]*DIVS_PER_VOLT)*(dac_state.vref/5.0f));
}