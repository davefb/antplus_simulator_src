/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/
 


#include "StdAfx.h"
#include "CustomSensor.h"

/**************************************************************************
 * CustomSensor::ANT_eventNotification
 * 
 * Process ANT channel event
 *
 * ucEventCode_: code of ANT channel event
 * pucEventBuffer_: pointer to buffer containing data received from ANT,
 *		or a pointer to the transmit buffer in the case of an EVENT_TX
 * 
 * returns: N/A
 *
 **************************************************************************/
void CustomSensor::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
{
	switch(ucEventCode_)
	{
	case EVENT_TX:
		HandleTransmit((UCHAR*) pucEventBuffer_);
		break;
	default:
		break;
	}
}

/**************************************************************************
 * CustomSensor::InitializeSim
 * 
 * Initializes simulator variables
 * 
 * returns: N/A
 *
 **************************************************************************/
void CustomSensor::InitializeSim()
{
	ulEventCounter = 0;
	bSendHex = FALSE;
	ValidateInput();
}

/**************************************************************************
 * CustomSensor::HandleTransmit
 * 
 * Encode data generated by simulator for transmission
 *
 * pucTxBuffer_: pointer to the transmit buffer
 * 
 * returns: N/A
 *
 **************************************************************************/
void CustomSensor::HandleTransmit(UCHAR* pucTxBuffer_)
{	
	UCHAR i;

	// Copy user data into transmit buffer
	for(i=0; i<8; i++)
	{
		pucTxBuffer_[i] = arrayUserData[i];
	}
}

/**************************************************************************
 * CustomSensor::onTimerTock
 * 
 * Simulates a device event, updating simulator data based on this event
 *
 * usEventTime_: current time (ms)
 *
 * returns: N/A
 *
 **************************************************************************/
void CustomSensor::onTimerTock(USHORT usEventTime_)
{	
	++ulEventCounter;
	label_EventCount->Text = ulEventCounter.ToString();
}


/**************************************************************************
 * CustomSensor::button_UpdateData_Click
 * 
 * Validates user input and updates data to transmit
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void CustomSensor::button_UpdateData_Click(System::Object^  sender, System::EventArgs^  e) 
{
	ValidateInput();		 
}



/**************************************************************************
 * CustomSensor::radioButton_TranslateSelect_CheckedChanged
 * 
 * Selects format of input box for message to send: Hex/Char
 *
 * pucRxBuffer_: pointer to the buffer containing the received data
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void CustomSensor::radioButton_TranslateSelect_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
	if(this->radioButton_asChar->Checked){
		this->textBox_customTxData->Text = "Max8Char";
		this->textBox_customTxData->MaxLength = 8;
		bSendHex = FALSE;
	}
	else if(this->radioButton_asHex->Checked){
		this->textBox_customTxData->Text = "00,00,00,00,00,00,00,00";
		this->textBox_customTxData->MaxLength = 23;
		bSendHex = TRUE;
	}
}


/**************************************************************************
 * CustomSensor::button_SendAck_Click
 * 
 * Sends user data as an acknowledged messaage
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void CustomSensor::button_SendAck_Click(System::Object^  sender, System::EventArgs^  e) 
{
	UCHAR i;
	UCHAR aucTxBuffer[8] = {0,0,0,0,0,0,0,0};

	ValidateInput();

	// Copy user data into transmit buffer
	for(i=0; i<8; i++)
	{
		aucTxBuffer[i] = arrayUserData[i];
	}
	
	// Send acknowledged message
	requestAckMsg(aucTxBuffer);
}


/**************************************************************************
 * CustomSensor::ValidateInput
 * 
 * Validates user input (text box) and parses into an array
 * 
 * returns: N/A
 *
 **************************************************************************/
void CustomSensor::ValidateInput()
{
		UCHAR i;

	// Validate empty string
	if(System::String::IsNullOrEmpty(this->textBox_customTxData->Text))
	{
		this->label_InputError->Text = "Error: Empty String";
		return;
	}

	// Check format of char string is valid
	if(!bSendHex)
	{
		UCHAR textLength = textBox_customTxData->Text->Length;
		for(i=0; i < textLength; ++i)
			arrayUserData[i] = (UCHAR)textBox_customTxData->Text[i];
		while(i<8)
			arrayUserData[i++] = 0x00;
		this->label_InputError->Text = "";
		this->label_DataTxd->Text = this->textBox_customTxData->Text;
	}
	// Check format of hex couplets is valid
	else
	{
		array<System::String^>^ hexes;
		hexes = this->textBox_customTxData->Text->Split(',');
		try
		{
			int hexLength = hexes->Length;
			if(hexLength >8)
				throw "Too many commas";
			for(i=0; i<hexLength; ++i)
			{
				if(hexes[i]->Length >2)
					throw "Too many letters between commas";
				arrayUserData[i] = Byte::Parse(hexes[i],System::Globalization::NumberStyles::HexNumber);
			}
			while(i<8)
				arrayUserData[i++] = 0x00;
			this->label_InputError->Text = "";
			this->label_DataTxd->Text = "Hex";
		}
		catch(...)
		{
			this->label_InputError->Visible = true;
			this->label_InputError->Text = "Error: Invalid Input";
			for(i=0; i<8; ++i)
				arrayUserData[i] = 0x00;
		}
	}
}
