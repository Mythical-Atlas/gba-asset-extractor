#include "window.hpp"
#include "system.hpp"
#include "ops.hpp"

uint8_t getImmediateByte(Circuit* circuit) {return circuit->getByte(circuit->pcReg + 1);}

uint8_t getA(Circuit* circuit) {return circuit->aReg;}
void setA(Circuit* circuit, uint8_t value) {circuit->aReg = value;}

void load8(uint8_t* input, uint8_t* output) {

}
void load16(uint16_t* input, uint16_t* output) {
	
}