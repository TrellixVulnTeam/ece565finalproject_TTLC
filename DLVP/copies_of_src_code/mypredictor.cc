/*
Title: mypredictor.cc
Author: Conor Green
Purpose: Final Project for ECE565
Description: Given load instructions, a predictor is trained through trainAPT()
    and predictions are queried by getPrediction().
Usage: Call through getPrediction(uint64_t pc, uint64_t& predicted_value)
*/


#include "mypredictor.h"
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// static const int APT_SIZE = 1024;
// static const int LOAD_PATH_REG_SIZE = 32;

predStats myStats;
APTEntry myAPT[APT_SIZE];
LOAD_PATH_REG_SIZE_TYPEDEF  loadPathHistory;

void updateLoadPathHistory(uint64_t pc){
    LOAD_PATH_REG_SIZE_TYPEDEF  lsb = (LOAD_PATH_REG_SIZE_TYPEDEF ) pc;

    lsb = lsb & DESIRED_PC_BIT; //.... yyyy xXxx -> 0000 0000 0X00

    lsb = lsb << (LOAD_PATH_REG_SIZE - 3); //MSB is desired bit
    loadPathHistory = loadPathHistory >> 1; // .... yyyy xxxx -> 0... .yyy yxxx

    loadPathHistory = loadPathHistory + lsb;
}

unsigned int calcAPTIndex(uint64_t pc){
    unsigned int ind;
    //uint32_t lower_pc = (uint32_t) pc;
    ind = pc ^ ((uint64_t) loadPathHistory);

    return ind;
}

uint16_t calcAPTTag(uint64_t pc){
    uint16_t tag = (uint16_t) calcAPTIndex(pc);
    uint16_t mask = (uint16_t) TAG_MASK;
    tag = tag & mask;
    return tag;
}

uint16_t tagFromIndex(unsigned int index){
    uint16_t tag;
    uint16_t mask = (uint16_t) TAG_MASK;
    tag = (uint16_t) index & mask;
    return tag;
}

bool queryAPTHitMiss(unsigned int index){
    unsigned int tag = tagFromIndex(index);

    struct APTEntry entry = getAPTEntry(index);

    uint16_t entryTag = entry.tag;

    if(entryTag == tag){
        return true;
    }

    return false;
}

APTEntry getAPTEntry(unsigned int index){
    // Assure index in bounds
    index = index % (APT_SIZE - 1);

    APTEntry entry = myAPT[index];

    return entry;
}

void setAPTEntry(unsigned int index, APTEntry entry){
    // Assure index in bounds
    index = index % (APT_SIZE - 1);
    myAPT[index] = entry;
}

uint8_t incrementConfidence(uint8_t old_conf){

    if(old_conf >= CONFIDENCE_THRESHOLD){
        return old_conf;
    }

    //rand float between 0.0 and 1.0 inclusive
    float randNum = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

    if(randNum <= CONFIDENCE_TRANSITION_VECTOR[old_conf]){
        uint8_t new_conf;
        new_conf = old_conf + 1;

        return new_conf;
    }

    return old_conf;
}

APTEntry createNew(uint64_t address, uint64_t pc){
    APTEntry entry;

    entry.tag = calcAPTTag(pc);
    entry.address = address;

    // new confidence of 0
    entry.confidence = 0;

    // currently unused
    entry.size = 0;

    return entry;
}

void incrementLoadTotalStats(){
    myStats.totalLoads++;
}

void updateStats(uint64_t predictedAddr, uint64_t actualAddr, uint64_t pc){
    if(predictedAddr == actualAddr){
        myStats.numCorrect++;
    }
    myStats.totalPredictions++;

    float percCorr = (float) myStats.numCorrect / (float) myStats.totalPredictions;
    myStats.percentCorrect = percCorr;

    float coverage = (float) myStats.totalPredictions / (float) myStats.totalLoads;
    myStats.coverage = coverage;
}

void printStats(){
    std::cout<<myStats.numCorrect<<std::endl;
    std::cout<<myStats.totalPredictions<<std::endl;
}

void printStatsWithLimit(){
    unsigned int currTotal = myStats.totalPredictions;
    if(currTotal <= NUM_LOADS_TO_PREDICT){
        if(currTotal % PRINTING_MODULO == 0){
            std::cout<<"----------"<<std::endl;
            std::cout<<"Correct: "<<myStats.numCorrect<<std::endl;
            std::cout<<"Total: "<<myStats.totalPredictions<<std::endl;
        }
    }
}

predStats getPredStats(){
    return myStats;
}

void trainAPT(uint64_t actualAddr, uint64_t pc){

    bool entryExists = pcHasEntry(pc);

    if(entryExists){
    
        unsigned int indexAPT = calcAPTIndex(pc);
        struct APTEntry entry = getAPTEntry(indexAPT);

        // Correct prediction. Increment according to probability vector
        if(entry.address == actualAddr){
            uint8_t conf = entry.confidence;
            uint8_t new_conf = incrementConfidence(conf);
            entry.confidence = new_conf;
            setAPTEntry(indexAPT, entry);
        }
        // Incorrect prediction. Reset confidence and re-allocate entry
        else{
            if(entry.confidence == 0){
                uint64_t newAddress = actualAddr;

                APTEntry newEntry = createNew(newAddress, pc);

                setAPTEntry(indexAPT, newEntry);
            }
            else{
                entry.confidence--;
                setAPTEntry(indexAPT, entry);
            }
        }
    }
    
    //No entry matches in APT. Add this address in
    else{
        unsigned int indexAPT = calcAPTIndex(pc);
        uint64_t newAddress = actualAddr;

        APTEntry newEntry = createNew(newAddress, pc);

        setAPTEntry(indexAPT, newEntry);
    }
}

bool hasPrediction(uint64_t pc){
    bool aptHasEntry = pcHasEntry(pc);

    if(aptHasEntry){
        unsigned int indexAPT = calcAPTIndex(pc);
        APTEntry entry = getAPTEntry(indexAPT);
        if(entry.confidence >= CONFIDENCE_THRESHOLD){
            return true;
        }
    }

    return false;
}

bool pcHasEntry(uint64_t pc){
    unsigned int indexAPT = calcAPTIndex(pc);

    bool hit = queryAPTHitMiss(indexAPT);

    uint16_t desiredTag = calcAPTTag(pc);

    if(hit){
        APTEntry entry = getAPTEntry(indexAPT);
        if(entry.tag == desiredTag){
            return true;
        }
    }
    return false;   
}

bool getPrediction(uint64_t pc, uint64_t* predicted_address_ptr){
    // manually called
    // updateLoadPathHistory(pc); 

    uint indexAPT;
    indexAPT = calcAPTIndex(pc);

    bool hit = queryAPTHitMiss(indexAPT);

    if(hit){
        //std::cout<<"hit orig"<<std::endl;
        APTEntry entry = getAPTEntry(indexAPT);
        *predicted_address_ptr = entry.address;
    }
    else{
        //std::cout<<"miss orig"<<std::endl;
    }
    return hit;
}

uint64_t getPredictionRaw(uint64_t pc){
    bool hasPred = hasPrediction(pc);

    uint64_t prediction = 0;

    if(hasPred){
        unsigned int indexAPT = calcAPTIndex(pc);
        APTEntry entry = getAPTEntry(indexAPT);

        prediction = entry.address;
    }

    return prediction;
}