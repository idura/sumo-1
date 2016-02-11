/*
 * MSPModelRemoteControlled.cpp
 *
 *  Created on: Feb 4, 2016
 *      Author: laemmel
 */
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2014-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#include "MSPModelRemoteControlled.h"


MSPModelRemoteControlled::MSPModelRemoteControlled(const OptionsCont& oc,
		MSNet* net) :
		myNet(net){
	grpcClient = new MSGRPCClient(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

	Event * e = new Event(this);
	net->getBeginOfTimestepEvents()->addEvent(e,net->getCurrentTimeStep() + DELTA_T, MSEventControl::ADAPT_AFTER_EXECUTION);
}

MSPModelRemoteControlled::~MSPModelRemoteControlled() {}


PedestrianState* MSPModelRemoteControlled::add(MSPerson* person,
		MSPerson::MSPersonStage_Walking* stage, SUMOTime now) {


	 assert(person->getCurrentStageType() == MSTransportable::MOVING_WITHOUT_VEHICLE);
	 std::cout << "MSPModelRemoteControlled::add was called" << std::endl;

	 MSPRCPState * state = new MSPRCPState(person,stage);
	 const MSEdge* edge = *(stage->getRoute().begin());;
//	 MSPRCPState * state = new MSPRCPState(person,rp);
	 if (buffers.find(edge->getID()) == buffers.end()) {
		 std::queue<MSPRCPState*> buffer;
		 buffer.push(state);


		 std::string str =  edge->getID();
		 std::cout << "current edge = " << str << std::endl;
		 buffers[edge->getID()] = buffer;
	 } else {
		 std::queue<MSPRCPState*> buffer = buffers[edge->getID()];
		 buffer.push(state);
	 }

//	 MSPRCPState * state = new MSPRCPState(person);
	 pstates[person->getID()] = state;
	 return state;
}

bool MSPModelRemoteControlled::blockedAtDist(const MSLane* lane,
		double distToCrossing, std::vector<const MSPerson*>* collectBlockers) {
	//for now no interaction btwn cars and peds
	return false;
}

SUMOTime MSPModelRemoteControlled::execute(SUMOTime currentTime) {

	std::cout << "tick" << std::endl;
	//do all the handling here

	//1. transfer agents as long as there is space (SUMO --> external sim)
	std::map<const std::string,std::queue<MSPRCPState*>>::iterator it = buffers.begin();
	while(it != buffers.end()) {
		std::queue<MSPRCPState*> buffer = (*it).second;
		handleBuffer(&buffer);
		if (buffer.empty()){
			it = buffers.erase(it);
		} else {
			it++;
		}
	}

	//2. simulate for one SUMO time step
	grpcClient->simulateTimeInterval(currentTime,currentTime+DELTA_T);

	//3. receive events, trajectories ...
	grpcClient->receiveTrajectories(pstates, currentTime);

	//4. transfer agents as long as there is space (external sim --> SUMO)
	grpcClient->retrieveAgents(pstates,myNet,currentTime);

	std::cout << "tock" << std::endl;
	if (pstates.size() == 0) {
		std::cout << "pstates empty!\n";
		return 0;
	}
	return DELTA_T;
}

void MSPModelRemoteControlled::handleBuffer(std::queue<MSPRCPState*>* buffer) {
	while(!buffer->empty()){
		MSPRCPState* st = buffer->front();
		if (transmitPedestrian(st)) {
			buffer->pop();
		} else {
			return;
		}
	}
}

bool MSPModelRemoteControlled::transmitPedestrian(MSPRCPState* st) {


	return grpcClient->transmitPedestrian(st);
}

