/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 *  901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 *  This product and related documentation are protected by copyright and
 *  distributed under licenses restricting its use, copying, distribution, and
 *  decompilation. No part of this product or related documentation may be
 *  reproduced in any form by any means without prior written authorization of
 *  Sun and its licensors, if any.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 *  States Government is subject to the restrictions set forth in DFARS
 *  252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 *  The product described in this manual may be protected by one or more U.S.
 *  patents, foreign patents, or pending applications.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Author:
 *
 *  Mahindra British Telecom
 *  155 , Bombay - Pune Road 
 *  Pimpri ,
 *  Pune - 411 018.
 *
 *  Module Name   : JAIN INAP API
 *  File Name     : CalledAddressAndService.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.datatype;


import java.io.*;
import java.util.*;
import java.lang.*;
import javax.jain.ss7.inap.constants.*;


/**
This class defines the CalledAddressAndService DataType
*/

public class CalledAddressAndService implements Serializable
{
	
	private DigitsGenericNumber calledAddressValue;

	private int serviceKey;

/**
Constructor For CalledAddressAndService
*/
	public CalledAddressAndService(DigitsGenericNumber calledAddressValue, int serviceKey)
	{
		setCalledAddressValue(calledAddressValue);
		setServiceKey(serviceKey);
	}
	
//---------------------------------------------	

/**
Gets Called Address Value
*/
	public DigitsGenericNumber getCalledAddressValue()
    	{
        		return calledAddressValue;
    	}
/**
Sets Called Address Value
*/
	public void setCalledAddressValue(DigitsGenericNumber calledAddressValue)
    	{
        		this.calledAddressValue = calledAddressValue;
    	}

//-----------------------

/**
Gets Service Key
*/
	public int getServiceKey()
    	{
        		return serviceKey;
    	}
/**
Sets Service Key
*/
	public void setServiceKey(int serviceKey)
    	{
        		this.serviceKey = serviceKey;
    	}

//-----------------------

}