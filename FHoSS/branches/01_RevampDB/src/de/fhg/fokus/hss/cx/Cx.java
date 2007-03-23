/*
  *  Copyright (C) 2004-2007 FhG Fokus
  *
  * This file is part of Open IMS Core - an open source IMS CSCFs & HSS
  * implementation
  *
  * Open IMS Core is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 2 of the License, or
  * (at your option) any later version.
  *
  * For a license to use the Open IMS Core software under conditions
  * other than those described here, or to purchase support for this
  * software, please contact Fraunhofer FOKUS by e-mail at the following
  * addresses:
  *     info@open-ims.org
  *
  * Open IMS Core is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * It has to be noted that this Open Source IMS Core System is not
  * intended to become or act as a product in a commercial context! Its
  * sole purpose is to provide an IMS core reference implementation for
  * IMS technology testing and IMS application prototyping for research
  * purposes, typically performed in IMS test-beds.
  *
  * Users of the Open Source IMS Core System have to be aware that IMS
  * technology may be subject of patents and licence terms, as being
  * specified within the various IMS-related IETF, ITU-T, ETSI, and 3GPP
  * standards. Thus all Open IMS Core users have to take notice of this
  * fact and have to agree to check out carefully before installing,
  * using and extending the Open Source IMS Core System, if related
  * patents and licenses may become applicable to the intended usage
  * context. 
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  
  * 
  */
package de.fhg.fokus.hss.cx;

import de.fhg.fokus.diameter.DiameterPeer.DiameterPeer;
import de.fhg.fokus.diameter.DiameterPeer.data.DiameterMessage;
import de.fhg.fokus.hss.cx.op.*;
import de.fhg.fokus.hss.diam.DiameterConstants;
import de.fhg.fokus.hss.main.HSSContainer;
import de.fhg.fokus.hss.main.Task;

/**
 * @author adp dot fokus dot fraunhofer dot de 
 * Adrian Popescu / FOKUS Fraunhofer Institute
 */

public class Cx {

	public static void processCxMessage(DiameterPeer diameterPeer, String FQDN, DiameterMessage message){
		
		DiameterMessage response = null;
		
		Task task = new Task(message.commandCode, true, FQDN, diameterPeer, message);
		try{
			HSSContainer.getInstance().tasksQueue.put(task);
			System.out.println("Added task to queue!");
		}
		catch(InterruptedException e){
			e.printStackTrace();
		}
		
		
		
/*		if (message.flagRequest){
			// we have a request
			switch(message.commandCode){
		
				case DiameterConstants.Command.UAR:
					Task task = new Task(message.commandCode, true, FQDN, diameterPeer, message);
					try{
						HSSContainer.getInstance().tasksQueue.put(task);
					}
					catch(InterruptedException e){
						e.printStackTrace();
					}
					//response = UAR.processRequest(diameterPeer, message);
					break;
				
				case DiameterConstants.Command.SAR:
					//response = SAR.processRequest(diameterPeer, message);
					break;
				
				case DiameterConstants.Command.MAR:
					//response = MAR.processRequest(diameterPeer, message);
					break;
				
				case DiameterConstants.Command.LIR:
					//response = LIR.processRequest(diameterPeer, message);
					break;
			}
			// send the response
			//diameterPeer.sendMessage(FQDN, response);
		}
		else{
			// we have an answer
			switch(message.commandCode){
			
				case DiameterConstants.Command.PPA:
					PPR.processAnswer(diameterPeer, message);
					break;
			
				case DiameterConstants.Command.RTA:
					System.out.println("the answer");
//					RTR.processAnswer(diameterPeer, message);
					break;
			}
		}*/
	}
}
