/*
Title: mypredictor
Author: Conor Green
Purpose: Final Project for ECE565
Description: Header file to provide definitions for mypredictor.cc
*/

#ifndef MYPREDICTOR_H
#define MYPREDICTOR_H

#include <stdint.h>
#include <stdio.h>

// Print helper inlines.
inline void printBinary(unsigned int number)
{
    if (number >> 1) {
        printBinary(number >> 1);
    }
    putc((number & 1) ? '1' : '0', stdout);
}

inline void printBinaryNumber(unsigned int number){
    printBinary(number);
    puts("\n");
}

static const unsigned int NUM_LOADS_TO_PREDICT = 1000000000;
static const unsigned int PRINTING_MODULO = 100;

static const int APT_SIZE = 1024;
typedef uint16_t LOAD_PATH_REG_SIZE_TYPEDEF;
static const unsigned int LOAD_PATH_REG_SIZE = 16;
static const int DESIRED_PC_BIT = 0x08;  //4th bit
static const int TAG_BIT_LENGTH = 14;
static const int TAG_MASK = 0x3FFF;

static const int CONFIDENCE_THRESHOLD = 3;
static const float CONFIDENCE_TRANSITION_VECTOR[10] = {1.0 , 0.5 , 0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25};


struct predStats{
    uint64_t totalLoads = 0;
    uint64_t totalPredictions = 0;
    uint64_t numCorrect = 0;
    float coverage = 0;
    float percentCorrect = 0.0;
};

struct APTEntry{
    uint16_t tag = 0;
    uint64_t address = 0;
    uint8_t confidence = 0;
    uint8_t size = 0;
};

// extern stats myStats;
// extern APTEntry myAPT[APT_SIZE];
// extern uint32_t loadPathHistory;
// loadPathHistory = 0;

extern predStats myStats;
extern APTEntry myAPT[APT_SIZE];
extern LOAD_PATH_REG_SIZE_TYPEDEF loadPathHistory;
// loadPathHistory = 0;


void printBinary(unsigned int number);
void printBinaryNumber(unsigned int number);

void updateLoadPathHistory(uint64_t pc);

unsigned int calcAPTIndex(uint64_t pc);
uint16_t calcAPTTag(uint64_t pc);
uint16_t tagFromIndex(unsigned int index);

bool queryAPTHitMiss(unsigned int index_raw);

APTEntry getAPTEntry(unsigned int index);
void setAPTEntry(unsigned int index, APTEntry entry);

uint8_t incrementConfidence(uint8_t old_conf);

APTEntry createNew(uint64_t address, uint64_t pc);

void incrementLoadTotalStats();
void updateStats(uint64_t predictedAddr, uint64_t actualAddr, uint64_t pc);
predStats getPredStats();

void printStats();
void printStatsWithLimit();

void trainAPT(uint64_t actualAddr, uint64_t pc);

bool hasPrediction(uint64_t pc);

bool pcHasEntry(uint64_t pc);

bool getPrediction(uint64_t pc, uint64_t* predicted_address_ptr);

uint64_t getPredictionRaw(uint64_t pc);

//int main();

#endif