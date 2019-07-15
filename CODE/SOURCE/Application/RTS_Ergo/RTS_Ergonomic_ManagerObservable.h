// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#pragma once
#include "RTS_Ergonomic_ManagerObserver.h"

void RTSErgonomicInterface_NotifyErgoChangeMode(ErgoMode_e NewMode);
void RTSErgonomicInterface_SendSpecialRequest(RTSErgoSpecialRequest_e Sreq);