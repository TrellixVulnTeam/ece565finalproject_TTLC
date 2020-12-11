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

// static uint32_t load_path_history = 0;

void MyPredictor::updateLoadPathHistory(uint64_t pc){
    uint32_t third_bit = (uint32_t) pc;

    third_bit = third_bit & 4; //.... yyyy xXxx -> 0000 0000 0X00

    third_bit = third_bit << LOAD_PATH_REG_SIZE - 3; //MSB is desired bit
    load_path_history = load_path_history >> 1; // .... yyyy xxxx -> 0... .yyy yxxx

    load_path_history = load_path_history + third_bit;
}

int MyPredictor::calcAPTIndex(uint64_t pc){
    int ind;
    //uint32_t lower_pc = (uint32_t) pc;
    ind = (uint32_t) pc ^ load_path_history;

    return ind;
}

int MyPredictor::calcAPTTag(uint64_t pc){
    int tag = calcAPTIndex(pc);
    tag = tag % TAG_BIT_LENGTH;
    return tag;
}

bool MyPredictor::queryAPTHitMiss(unsigned int index_raw){
    unsigned int tag = index_raw % TAG_BIT_LENGTH;
    unsigned int index = index_raw % APT_SIZE;

    struct APTEntry entry = getAPTEntry(index);

    uint16_t entryTag = entry.tag;

    if(entryTag == tag){
        return true;
    }

    return false;
}

APTEntry MyPredictor::getAPTEntry(unsigned int index){
    // Assure index in bounds
    index = index % APT_SIZE;
    return myAPT[index];
}

void MyPredictor::setAPTEntry(unsigned int index, APTEntry entry){
    // Assure index in bounds
    index = index % APT_SIZE;
    myAPT[index] = entry;
}

uint8_t MyPredictor::incrementConfidence(uint8_t old_conf){
    if(old_conf == 3){
        return old_conf;
    }

    uint8_t new_conf;

    //rand float between 0.0 and 1.0 inclusive
    float randNum = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

    if(randNum <= CONFIDENCE_TRANSITION_VECTOR[old_conf]){
        new_conf = old_conf + 1;
    }

    return old_conf;
}

APTEntry MyPredictor::allocateNew(uint64_t address, uint64_t pc){
    APTEntry entry;

    // new confidence of 0
    
    entry.tag = calcAPTTag(pc);
    entry.address = address;
    entry.confidence = 0;
    entry.size = 0;

    return entry;
}

bool MyPredictor::isCorrectPred(uint64_t &predictedAddr, uint64_t& actualAddr, uint64_t pc){
    int indexAPT = calcAPTIndex(pc);

    bool hit = queryAPTHitMiss(indexAPT);

    if(hit){
        // Correct prediction. Increment according to probability vector
        if(predictedAddr == actualAddr){
            return true;
        }
        // Incorrect prediction. Reset confidence and re-allocate entry
        return false;
    }
}

void MyPredictor::updateStats(uint64_t &predictedAddr, uint64_t& actualAddr, uint64_t pc){
    if(isCorrectPred(predictedAddr, actualAddr, pc)){
        myStats.numCorrect++;
    }
    myStats.total++;
}

void MyPredictor::printStats(){
    std::cout<<myStats.numCorrect<<std::endl;
    std::cout<<myStats.total<<std::endl;
}


void MyPredictor::trainAPT(uint64_t &predictedAddr, uint64_t& actualAddr, uint64_t pc){
    int indexAPT = calcAPTIndex(pc);

    bool hit = queryAPTHitMiss(indexAPT);

    // uint64_t pred = *predicted_val;
    // uint64_t actual = *actual_val;

    struct APTEntry entry = getAPTEntry(indexAPT);

    if(hit){
        // Correct prediction. Increment according to probability vector
        if(predictedAddr == actualAddr){
            uint8_t conf = entry.confidence;
            uint8_t new_conf = incrementConfidence(conf);
            entry.confidence = new_conf;
            setAPTEntry(indexAPT, entry);
        }
        // Incorrect prediction. Reset confidence and re-allocate entry
        else{
            // uint8_t new_conf = 0;
            uint64_t newAddress = actualAddr;

            // entry.tag = calcAPTTag(pc);
            // entry.address = new_address;
            // entry.confidence = new_conf;
            // entry.size = 0;

            APTEntry newEntry = allocateNew(newAddress, pc);

            setAPTEntry(indexAPT, newEntry);
        }
    }
    //miss
    else{
        // allocate new
        if(entry.confidence == 0){
            uint64_t newAddress = actualAddr;

            APTEntry newEntry = allocateNew(newAddress, pc);

            setAPTEntry(indexAPT, newEntry);
        }
        // decrement confidence
        else{
            entry.confidence--;
            setAPTEntry(indexAPT, entry);
        }
    }
    
}

bool MyPredictor::getPrediction(uint64_t pc, uint64_t* predicted_address_ptr){
    updateLoadPathHistory(pc);

    int indexAPT;
    indexAPT = calcAPTIndex(pc);

    bool hit = queryAPTHitMiss(indexAPT);

    if(hit){
        std::cout<<"hit"<<std::endl;
        APTEntry entry = getAPTEntry(indexAPT);
        *predicted_address_ptr = entry.address;
    }
    else{
        std::cout<<"miss"<<std::endl;
    }
    return hit;
}