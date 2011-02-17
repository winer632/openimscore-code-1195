/*
 * $Id$
 *
 * Copyright (C) 2008-2009 FhG Fokus
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
 * patents and licences may become applicable to the intended usage 
 * context.  
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * author Ancuta Corici, 
 * 	email andreea dot ancuta dot corici -at- fokus dot fraunhofer dot de
 */
/**
 * Client_Rf - implementation of the Rf interface from the CTF side, according to TS32.299 R7
 * 
 * Scope:
 * - Exports parameters and functions
 * - Initialization functions
 * 
 */
 
#ifdef CDP_FOR_SER

#include "mod.h"

#include "../../parser/msg_parser.h"
#include "../../parser/parse_uri.h"
#include "../../sr_module.h"
#include "../../socket_info.h"
#include "../../timer.h"
#include "../../locking.h"
#include "../../modules/tm/tm_load.h"
#ifdef SER_MOD_INTERFACE
	#include "../../modules_s/dialog/dlg_mod.h"
#else
	#include "../dialog/dlg_mod.h"
#endif

#include "../cdp/cdp_load.h"
#include "../cdp/diameter_code_avp.h"
#include "../cdp_avp/mod_export.h"

#include "diameter_rf.h"
#include "ims_rf.h"
#include "Rf_data.h"
#include "sip.h"

extern struct tm_binds tmb;
extern cdp_avp_bind_t *cavpb;

/**
 * Retrieves the SIP request that generated a diameter transaction
 * @param hash - the tm hash value for this request
 * @param label - the tm label value for this request
 * @returns the SIP request
 */
struct sip_msg * trans_get_request_from_current_reply()
{
	struct cell *t;
	t = tmb.t_gett();
	if (!t || t==(void*) -1){
		LOG(L_ERR,"ERR:"M_NAME":trans_get_request_from_current_reply: Reply without transaction\n");
		return 0;
	}
	if (t) return t->uas.request;
	else return 0;
}

int get_ACR_info(struct sip_msg * msg, 
		int32_t * acc_record_type,
		str * sip_method,
		str * event, uint32_t * expires,
		str * callid, str * from_uri, str * to_uri){

	sip_method->s = msg->first_line.u.request.method.s;
	sip_method->len = msg->first_line.u.request.method.len;

	if(strncmp(sip_method->s, "INVITE",6) == 0)
		*acc_record_type = AAA_ACCT_START;
	else if	(strncmp(sip_method->s, "BYE",3) == 0)
		*acc_record_type = AAA_ACCT_STOP;
	else	
		*acc_record_type = AAA_ACCT_EVENT;

	*event = cscf_get_event(msg);
	*expires = cscf_get_expires_hdr(msg, 0);
	*callid = cscf_get_call_id(msg, NULL);

	if(!cscf_get_from_uri(msg, from_uri))
		goto error;

	if(!cscf_get_to_uri(msg, to_uri))
		goto error;

	return 1;
error:
	return 0;
}

/*
 * creates the rf session for a session establishment
 * @param aor: the aor to be handled
 * @return: 0 - ok, -1 - error, -2 - out of memory
 */

Rf_ACR_t * dlg_create_rf_session(struct sip_msg * msg, 
				AAASession ** authp, 
				int dir){

	Rf_ACR_t * rf_data=0;
	AAASession * auth = NULL;
	str user_name ={0,0}, sip_method = {0,0}, event = {0,0}; 
	uint32_t expires;
	str callid = {0,0}, to_uri = {0,0}, from_uri ={0,0};

	event_type_t * event_type = 0;
	ims_information_t * ims_info = 0;
	time_stamps_t * time_stamps = 0;
	time_t req_timestamp, reply_timestamp;
	int32_t acc_record_type;

	*authp = 0;

	if(!get_ACR_info(msg, &acc_record_type, 
				&sip_method, &event, &expires, 
				&callid, &from_uri, &to_uri))
		goto error;

	if(!(event_type = new_event_type(&sip_method, &event, &expires)))
		goto error;

	if(!(time_stamps = new_time_stamps(&req_timestamp, NULL,
						&reply_timestamp, NULL)))
		goto error;

	if(!(ims_info = new_ims_information(event_type, 
					time_stamps,
					&callid, &callid,
					&from_uri, &to_uri)))
		goto error;

	event_type = 0;
	time_stamps = 0;

	rf_data = new_Rf_ACR(acc_record_type,
			&user_name, ims_info, NULL);
	if(!rf_data) {
		LOG(L_ERR,"ERR:"M_NAME":dlg_create_rf_session: no memory left for generic\n");
		goto out_of_memory;
	}
	ims_info = 0;

	LOG(L_INFO,"INFO:"M_NAME":dlg_create_rf_session: creating Rf Session\n");
	auth = cavpb->cdp->AAACreateClientAuthSession(1,NULL,(void *)rf_data);
	if (!auth) {
		LOG(L_ERR,"INFO:"M_NAME":dlg_create_rf_session: unable to create the Rf Session\n");
		goto error;
	}

	*authp = auth;
	return rf_data;

out_of_memory:
error:
	time_stamps_free(time_stamps);
	event_type_free(event_type);
	ims_information_free(ims_info);
	Rf_free_ACR(rf_data);
	return NULL;
}

int sip_create_rf_data(struct sip_msg * msg, int dir, Rf_ACR_t ** rf_data, AAASession ** auth){
	
	struct sip_msg * req;

	if(msg->first_line.type == SIP_REQUEST){
		/*end of session*/
		if(strncmp(msg->first_line.u.request.method.s, "BYE",3)==0 ){
			if(!(*rf_data = dlg_create_rf_session(msg, auth, dir)))
				goto error;
		}
	}else{

		req = trans_get_request_from_current_reply();
		if(!req) {
			LOG(L_ERR, "could not retrieve request from current transaction\n");
			return CSCF_RETURN_ERROR;
		}
		/*start of session*/
		if(msg->first_line.u.reply.statuscode == 200 &&
		   strncmp(req->first_line.u.request.method.s, INVITE, 6) == 0){

			if(!(*rf_data = dlg_create_rf_session(req, auth, dir)))
				goto error;
		}
	}

	return 1;
error:
	return 0;
}

/**
 * Send an ACR to the CDF based on the SIP message (request or reply)
 * @param msg - SIP message
 * @param str1 - not used
 * @param str2 - not used
 * @returns #CSCF_RETURN_TRUE if OK, #CSCF_RETURN_ERROR on error
 */
int Rf_Send_ACR(struct sip_msg *msg,char *str1, char *str2){
	
	AAASession * auth = 0;
	Rf_ACR_t * rf_data = 0;
//	AAAMessage * acr = 0;
	int dir =0;
	
	if(!sip_create_rf_data(msg, dir, &rf_data, &auth))
		goto error;

	if(!auth) goto error;

	//acr = Rf_new_ACR(auth);

	//sendACR(acr);

	//cdpb.AAATerminateAuthSession(auth);

	Rf_free_ACR(rf_data);

	return CSCF_RETURN_TRUE;
error:
	Rf_free_ACR(rf_data);

	return CSCF_RETURN_ERROR;
}



#endif /*CDP_FOR_SER*/
