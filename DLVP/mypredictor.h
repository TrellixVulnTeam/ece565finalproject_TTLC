/*
Title: mypredictor.h
Author: Conor Green
Purpose: Final Project for ECE565
Description: Header file to provide definitions for mypredictor.cc
*/

#include <stdint.h>

static const int APT_SIZE = 1024;
static const int LOAD_PATH_REG_SIZE = 32;
static const int TAG_BIT_LENGTH = 14;
static const float CONFIDENCE_TRANSITION_VECTOR[3] = {1.0 , 0.5 , 0.25};


static uint32_t load_path_history = 0;

struct APTEntry{
    uint16_t tag = 0;
    uint64_t address = 0;
    uint8_t confidence = 0;
    uint8_t size = 0;
}

static struct APTEntry myAPT[APT_SIZE];

void printBinary(unsigned int number);
void printBinaryNumber(unsigned int number);

void updateLoadPathHistory(uint64_t pc);

int calcAPTIndex(uint64_t pc);
int calcAPTTag(uint64_t pc);

bool queryAPTHitMiss(unsigned int index_raw);

APTEntry getAPTEntry(unsigned int index);
void setAPTEntry(unsigned int index, APTEntry entry);

uint8_t incrementConfidence(uint8_t old_conf);

APTEntry allocateNew(uint64_t address, uint64_t pc);

void trainAPT(uint64_t &predicted_val, uint64_t& actual_val, uint64_t pc);

bool getPrediction(uint64_t pc, uint64_t& predicted_value);

int main();